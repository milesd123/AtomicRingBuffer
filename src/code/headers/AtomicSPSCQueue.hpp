#pragma once
#include "headers.hpp"

// Atomic Single Producer Single Consumer Queue 
// 'to' and 'from' should be treated as 'to/from this object'
class AtomicSPSCQueue 
{
public:
    AtomicSPSCQueue(SimpleSocket* to, SimpleSocket* from, size_t buf_size, uint8_t* buf, const char* name);
    void Start();
    void WaitForStop();
private:    
    void Stop();
    inline size_t FastModulo(size_t);
    void ReadFromBuffer();
    void WriteToBuffer();

    std::thread worker_to;
    std::thread worker_from;
    SimpleSocket* const source_socket;
    SimpleSocket* const dest_socket;

    const size_t buffer_size;
    uint8_t* const buffer; 
    
    const char* name; // for realtime debugging purposes
    
    bool running_ = false; 
    bool ready_to_join = false;
    std::mutex mutex_;
    std::condition_variable cond_var_;

    std::atomic_bool running_signal{false};
    std::atomic_bool stop_initiated{false};
    std::condition_variable queue_empty_;
    std::mutex m_;

    // Place all of the writer/reader and their variables in different cache lines
    alignas(64) std::atomic_size_t writer{0};
    alignas(64) std::atomic_size_t reader{0};
};