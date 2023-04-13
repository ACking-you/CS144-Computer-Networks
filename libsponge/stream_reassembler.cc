#include "stream_reassembler.hh"

#include <cstring>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in
// `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */)
{
}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
  : _unass_base(0),
    _unass_size(0),
    _eof(false),
    _buffer(capacity, 0),
    _bitmap(capacity, false),
    _output(capacity),
    _capacity(capacity)
{
}

//! \details This functions calls just after pushing a substring into the
//! _output stream. It aims to check if there exists any contiguous substrings
//! recorded earlier can be push into the stream.
void StreamReassembler::check_contiguous()
{
   size_t len = 0;
   if (_bitmap.front())
   {
      while (len < _capacity && _bitmap[len]) ++len;
   }

   if (len > 0)
   {
      _unass_base += len;
      _unass_size -= len;
      _output.write(_buffer.substr(0, len));
      // 把流往前推进
      for (size_t i = 0; i < _capacity; i++)
      {
         if (i + len < _capacity)
         {
            _bitmap[i] = _bitmap[i + len];
            _buffer[i] = _buffer[i + len];
         }
         else { _bitmap[i] = false; }
      }
   }
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index,
                                       const bool eof)
{
   if (eof) { _eof = true; }
   size_t len = data.length();
   if (len == 0 && _eof && _unass_size == 0)
   {
      _output.end_input();
      return;
   }
   // ignore invalid index
   if (index >= _unass_base + _capacity) return;
   // 未重排数据的序号大于等于已经重排数据的下一个序号
   if (index >= _unass_base)
   {
      int    offset   = index - _unass_base;
      // 减去_output.buffer_size()是为了确保滑动窗口的大小(如果已经重排好的数据没有被读取,那么不一直接收更多的未重排数据)
      size_t real_len = min(len, _capacity - _output.buffer_size() - offset);
      if (real_len < len) { _eof = false; }
      for (size_t i = 0; i < real_len; i++)
      {
         if (_bitmap[i + offset]) continue;
         _buffer[i + offset] = data[i];
         _bitmap[i + offset] = true;
         _unass_size++;
      }
   }
   // 如果未重排数据的序号包含了已经重排的情况,但是整体数据有部分数据未被重排
   else if (index + len > _unass_base)
   {
      int    offset   = _unass_base - index;
      size_t real_len = min(len - offset, _capacity - _output.buffer_size());
      if (real_len < len - offset) { _eof = false; }
      for (size_t i = 0; i < real_len; i++)
      {
         if (_bitmap[i]) continue;
         _buffer[i] = data[i + offset];
         _bitmap[i] = true;
         _unass_size++;
      }
   }
   check_contiguous();
   if (_eof && _unass_size == 0) { _output.end_input(); }
}

size_t StreamReassembler::unassembled_bytes() const { return _unass_size; }

bool StreamReassembler::empty() const { return _unass_size == 0; }

size_t StreamReassembler::ack_index() const { return _unass_base; }