#pragma once
#include "headers.hpp"

#define FastModulo(x) (x & 1023)

// Atomic Single Producer Single Consumer Queue 
// 'to' and 'from' should be treated as 'to/from this object'
template<typename BufferType>
class AtomicSPSCQueue 
{
public:
    AtomicSPSCQueue
    (
        SimpleSocket* to, SimpleSocket* from, size_t buf_size, BufferType* buf
    )
    :   socket_to(to), socket_from(from), buffer(buf), buffer_size(buf_size) {}

    void Start()
    {
        running.store(true, std::memory_order_release);
        worker_from = std::thread(&AtomicSPSCQueue::ReadFromBuffer, this);
        worker_to = std::thread(&AtomicSPSCQueue::WriteToBuffer, this);
    }

    void WaitForStop()
    {
        if(running.load() == false) return;
        worker_from.join();
        worker_to.join();
    }

    void Stop()
    {
        // Does nothing for now
        running.store(false);
    }

private:    
    void ReadFromBuffer() noexcept
    {
        // assuming that nothing is added inbetween these variables(and 64-byte cache lines) 
        // they should be on the same cache page;
        alignas(64) size_t start_inclusive_r = 0;
        size_t end_exclusive_r = 0;
        size_t amount_r = 0;

        while (running.load()) 
        {
            // get our indexes
            start_inclusive_r = FastModulo(reader.load(std::memory_order_relaxed));
            end_exclusive_r = FastModulo(writer.load(std::memory_order_acquire));

            // determine where to stop reading
            if(start_inclusive_r > end_exclusive_r) end_exclusive_r = buffer_size;
            amount_r = end_exclusive_r - start_inclusive_r;

            // atomically increment read_in_index AND write to the buffer
            reader.store(
                start_inclusive_r + socket_to->write(buffer + start_inclusive_r, amount_r), 
                std::memory_order_release
            );
        }
    }

    // Write to the buffer from the socket
    void WriteToBuffer() noexcept
    {
        alignas(64) size_t start_inclusive_w = 0;
        size_t end_exclusive_w = 0;
        size_t available_bytes_w = 0;
        size_t amount_w = 0;

        while (running.load())
        {
            start_inclusive_w = FastModulo(writer.load(std::memory_order_relaxed));
            end_exclusive_w = FastModulo(reader.load(std::memory_order_acquire));

            if(start_inclusive_w > end_exclusive_w) end_exclusive_w = buffer_size;

            amount_w = end_exclusive_w - start_inclusive_w;

            // determine how many bytes we CAN read
            available_bytes_w = socket_from->available();

            // compare with amount of our buffer we can currently fill
            if(amount_w > available_bytes_w) amount_w = available_bytes_w;

            // Increment by the amount read returned by the reader (may differ from the amount_w)
            writer.store(
                start_inclusive_w + socket_from->read(buffer + start_inclusive_w, amount_w),
                std::memory_order_release
            );
        }
    }

    const size_t buffer_size;
    BufferType* const buffer; 
    SimpleSocket* socket_to; // leaving non-const for the future
    SimpleSocket* socket_from;
    std::thread worker_to;
    std::thread worker_from;
    std::atomic<bool> running{false};

    // Place all of the writer/reader and their variables in the same cache line
    alignas(64) std::atomic_size_t writer;
    alignas(64) std::atomic_size_t reader;
};