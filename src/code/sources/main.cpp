#include "../headers/headers.hpp"

#define BUFFERSIZE 1024



void MinrcraftProxy(const char*, const char*);
void SocketTest();

int main(){
    // MinrcraftProxy("mc.hypixel.net", "25565");
    SocketTest();

    return 0;
}

void MinrcraftProxy(const char* hostname, const char* port)
{

    alignas(64)     uint8_t server_buffer[BUFFERSIZE];
    alignas(64)     uint8_t client_buffer[BUFFERSIZE];

    std::vector<asio::ip::tcp::endpoint> endpoints;
    asio::ip::tcp::endpoint localhost(asio::ip::make_address_v4("127.0.0.1"), asio::ip::port_type(25565));
    asio::io_context io_context_;
    asio::error_code ec;

    // Get hypixel endpoints
    ProxyUtils::resolve_endpoints(hostname, port, endpoints, io_context_, ec);

    if(endpoints.empty())
    {
        std::cout << "No Endpoints Found. Aborting..." << std::endl;
        return;
    }

    AsioSocket ClientSocket(io_context_, ec, localhost);
    AsioSocket ServerSocket(io_context_, ec, endpoints[0]);

    AtomicSPSCQueue<uint8_t> ClientToServerQueue(&ClientSocket, &ServerSocket, BUFFERSIZE, server_buffer);
    AtomicSPSCQueue<uint8_t> ServerToClientQueue(&ServerSocket, &ClientSocket, BUFFERSIZE, client_buffer);

    ClientSocket.await_connection();
    ServerSocket.connect();

    ClientToServerQueue.Start();
    ServerToClientQueue.Start();
}

void SocketTest()
{
    // create test buffers and queue buffer
    const size_t test_buffer_size = 1024 * 8; // 8kb
    alignas(64) uint8_t source_buffer[test_buffer_size];
    alignas(64) uint8_t dest_buffer[test_buffer_size];
    alignas(64) uint8_t queue_buffer[BUFFERSIZE];
    
    // fill the source buffer with random values

    TestSocket sock;

    // initiate the "socket"
    sock.SetQueueSize(1024);
    sock.SetDestination(dest_buffer);
    sock.SetSource(source_buffer, source_buffer + test_buffer_size);

    // create our worker
    AtomicSPSCQueue<uint8_t> queue(&sock, &sock, BUFFERSIZE, queue_buffer);

    sock.SetStoppingFunction([&](){queue.Stop();});


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

    std::cout << "Atomic Queue Size: " << BUFFERSIZE << std::endl;
    std::cout << "Transferred " << test_buffer_size << "bytes in " <<  seconds.count() << " seconds" << std::endl;
    std::cout << reads / seconds.count() << " Reads/Second" << std::endl;
    std::cout << writes / seconds.count() << " Writes/Second" << std::endl;

}