#include "../headers/TestSocket.hpp"

TestSocket::TestSocket() {}

// write to a "socket" from the buffer
size_t TestSocket::write(void* queue_buffer, size_t buffer_size, asio::error_code& unused)
{

    // TODO: copy stuff into our destination buffer from this given one
    memcpy(dest_buffer, queue_buffer, buffer_size);

    dest_buffer = (uint8_t*) dest_buffer + buffer_size;

    write_operations++;

    return buffer_size; // in reality this may not be the case, asio may end up writing less
}

size_t TestSocket::write(void* queue_buffer, size_t buffer_size)
{

    // TODO: copy stuff into our destination buffer from this given one
    memcpy(dest_buffer, queue_buffer, buffer_size);

    dest_buffer = (uint8_t*) dest_buffer + buffer_size;

    write_operations++;

    return buffer_size; // in reality this may not be the case, asio may end up writing less
}

size_t TestSocket::read(void* queue_buffer, size_t buffer_size, asio::error_code& ec)
{
    if(buffer_size == 0) return 0;

    // fill the queue's buffer from our source buffer
    void* a = (uint8_t*) source_buffer + buffer_size;

    if(a > source_buffer_end)
    {
        ec = asio::error::eof;
        return 0;
    }

    memcpy(queue_buffer, source_buffer, buffer_size);

    source_buffer = a;

    read_operations++;
    
    return buffer_size; // asio might return less
}

size_t TestSocket::read(void* queue_buffer, size_t buffer_size)
{
    if(buffer_size == 0) return 0;

    // fill the queue's buffer from our source buffer
    void* a = (uint8_t*) source_buffer + buffer_size;

    if(a > source_buffer_end)
    {
        return 0;
    }

    memcpy(queue_buffer, source_buffer, buffer_size);

    source_buffer = a;

    read_operations++;
    
    return buffer_size; // asio might return less
}

// May be past the available amount in the buffer - handled by read's error code
size_t TestSocket::available()
{
    current_available = (rng_mult * current_available + rng_incr) & mod;
    return current_available;
}

void TestSocket::WaitRead()
{

}

void TestSocket::Close()
{

}


// block until ready to write
void TestSocket::WaitWrite()
{

}



//
//      Getters And Setters
//
void TestSocket::SetQueueSize(size_t s)
{
    this->mod = s - 1;
}
size_t TestSocket::GetReadOperations()
{
    return this->read_operations;
}
size_t TestSocket::GetWriteOperations()
{
    return this->write_operations;
}
const void* TestSocket::GetSource()
{
    return this->source_buffer;
}
const void* TestSocket::GetDestination()
{
    return this->dest_buffer;
}
void TestSocket::SetSource(void* s, const void* t)
{
    this->source_buffer = s;
    this->source_buffer_end = t;
}
void TestSocket::SetDestination(void* d)
{
    this->dest_buffer = d;
}

// private:
// size_t counter;