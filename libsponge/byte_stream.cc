#include "byte_stream.hh"

#include <cassert>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in
// `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */)
{
}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : _buffer(capacity, 0) {}

size_t ByteStream::write(const string &data)
{
   assert(buffer_size() <= _buffer.size());
   if (input_ended()) return 0;
   size_t canWrite  = _buffer.size() - buffer_size();
   size_t realWrite = min(canWrite, data.size());

   for (size_t i = 0; i < realWrite; i++)
   {
      _buffer[_writeIdx++ % _buffer.size()] = data[i];
   }
   return realWrite;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const
{
   assert(buffer_size() <= _buffer.size());
   size_t canPeek = min(buffer_size(), len);
   string ret;
   for (size_t i = 0; i < canPeek; i++)
   {
      ret += _buffer[(_readIdx + i) % _buffer.size()];
   }
   return ret;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len)
{
   assert(buffer_size() <= _buffer.size());
   if (len > buffer_size())
   {
      set_error();
      return;
   }
   _readIdx += len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len)
{
   assert(buffer_size() <= _buffer.size());
   auto ret = peek_output(len);
   pop_output(len);
   if (_error) return {};
   return ret;
}

void ByteStream::end_input() { _eof = true; }

bool ByteStream::input_ended() const { return _eof; }

size_t ByteStream::buffer_size() const
{
   assert(_writeIdx >= _readIdx);
   return _writeIdx - _readIdx;
}

bool ByteStream::buffer_empty() const { return _writeIdx == _readIdx; }

bool ByteStream::eof() const { return _eof && buffer_empty(); }

size_t ByteStream::bytes_written() const { return _writeIdx; }

size_t ByteStream::bytes_read() const { return _readIdx; }

size_t ByteStream::remaining_capacity() const
{
   return _buffer.size() - buffer_size();
}
