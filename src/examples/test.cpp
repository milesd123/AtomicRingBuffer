#include "../code/headers/headers.hpp"

size_t BUFFERSIZE = 1024;

struct Benchmark{
    size_t reads = 0;
    size_t writes = 0;
    size_t buffer_size = 0;
    std::chrono::duration<double> seconds;
};


void SocketTest(int n, Benchmark* b);

int main(int c, char* argv[])
{

    if(c != 2)
    {
        std::cout << "usage: ./program <mb_test_amount>" << std::endl;
        return 0;
    }




    // BENCHMARK: 1024
    Benchmark benchmark{0, 0, BUFFERSIZE, std::chrono::duration<double>{}};

    int amt = atoi(argv[1]);
    for(int i = 0; i < amt; i++)
    {
        // 1 GB * 256
        SocketTest(1048576, &benchmark);
    }

    std::cout << "Atomic Queue Size: " << BUFFERSIZE << std::endl;
    std::cout << "Transferred "<< amt <<" Mb in " <<  benchmark.seconds.count() <<"("<<amt/benchmark.seconds.count() <<"Mbps)"<< std::endl;
    std::cout << benchmark.reads / benchmark.seconds.count() << " Reads/Second" << std::endl;
    std::cout << benchmark.writes / benchmark.seconds.count() << " Writes/Second" << std::endl;




    // BENCHMARK: 2048
    BUFFERSIZE = 2048;
    benchmark = {0, 0, BUFFERSIZE, std::chrono::duration<double>{}};

    for(int i = 0; i < amt; i++)
    {
        SocketTest(1048576, &benchmark);
    }

    std::cout << "Atomic Queue Size: " << BUFFERSIZE << std::endl;
    std::cout << "Transferred "<< amt <<" Mb in " <<  benchmark.seconds.count() <<"("<<amt/benchmark.seconds.count() <<"Mbps)"<< std::endl;
    std::cout << benchmark.reads / benchmark.seconds.count() << " Reads/Second" << std::endl;
    std::cout << benchmark.writes / benchmark.seconds.count() << " Writes/Second" << std::endl;




    // BENCHMARK: 4096
    BUFFERSIZE = 4096;
    benchmark = {0, 0, BUFFERSIZE, std::chrono::duration<double>{}};

    for(int i = 0; i < amt; i++)
    {
        SocketTest(1048576, &benchmark);
    }

    std::cout << "Atomic Queue Size: " << BUFFERSIZE << std::endl;
    std::cout << "Transferred "<< amt <<" Mb in " <<  benchmark.seconds.count() <<"("<<amt/benchmark.seconds.count() <<"Mbps)"<< std::endl;
    std::cout << benchmark.reads / benchmark.seconds.count() << " Reads/Second" << std::endl;
    std::cout << benchmark.writes / benchmark.seconds.count() << " Writes/Second" << std::endl;

    return 0;
}


void SocketTest(int n, Benchmark* b)
{
    // create test buffers and queue buffer
    const size_t test_buffer_size = n; // n MBs
    alignas(64) uint8_t* source_buffer = new uint8_t[test_buffer_size];
    alignas(64) uint8_t* dest_buffer = new uint8_t[test_buffer_size];
    alignas(64) uint8_t* queue_buffer = new uint8_t[BUFFERSIZE];
    
    // fill the source buffer with random values

    TestSocket sock;

    // initiate the "socket"
    sock.SetQueueSize(1024);
    sock.SetDestination(dest_buffer);
    sock.SetSource(source_buffer, source_buffer + test_buffer_size);

    // create our worker
    AtomicSPSCQueue queue(&sock, &sock, BUFFERSIZE, queue_buffer, "A");


    // Start timer
    auto start = std::chrono::steady_clock::now();

    queue.Start(); 

    queue.WaitForStop();

    // end timer after queue has finished
    auto end = std::chrono::steady_clock::now();
    

    // performance
    size_t reads = sock.GetReadOperations();
    size_t writes = sock.GetWriteOperations();
    std::chrono::duration<double> seconds = end - start;

    // std::cout << "Atomic Queue Size: " << BUFFERSIZE << std::endl;
    // std::cout << "Transferred " << test_buffer_size << " bytes in " <<  seconds.count() << " seconds("<<(test_buffer_size / 1e6) / seconds.count() <<" Mbps)"<< std::endl;
    // std::cout << reads / seconds.count() << " Reads/Second" << std::endl;
    // std::cout << writes / seconds.count() << " Writes/Second" << std::endl;

    b->reads += reads;
    b->writes += writes;
    b->seconds += seconds;

    delete[] source_buffer;
    delete[] dest_buffer;
}