#include "../code/headers/headers.hpp"

#define BUFFERSIZE 1024

void MinrcraftProxy(const char*, const char*);

int main(int argc, char* argv[]){
    try{
        if(argc == 2)
        {
            MinrcraftProxy(argv[1], "25565");
        }else{
            MinrcraftProxy("na.hypixel.net", "25565");
        }

        // MinrcraftProxy("minemen.club", "25565");
    } catch(std::system_error error)
    {
        std::cout << error.what() << std::endl;
    }
    return 0;
}

void MinrcraftProxy(const char* hostname, const char* port)
{
    alignas(64)     uint8_t server_buffer[BUFFERSIZE];
    alignas(64)     uint8_t client_buffer[BUFFERSIZE];

    std::vector<asio::ip::tcp::endpoint> endpoints;
    asio::ip::tcp::endpoint localhost(asio::ip::make_address_v4("127.0.0.1"), 25565);
    asio::io_context io_context_;
    asio::error_code ec;

    // Get hypixel endpoints
    ProxyUtils::resolve_endpoints(hostname, port, endpoints, io_context_, ec);
    
    for(const auto& ep : endpoints)
    {
        std::cout << "Endpoint: " << ep.address().to_string() << std::endl;
    }

    if(endpoints.empty())
    {
        std::cout << "No Endpoints Found. Aborting..." << std::endl;
        return;
    }

    // Acceptor for localhost
    asio::ip::tcp::acceptor acceptor(io_context_);

    acceptor.open(localhost.protocol()); // create OS socket descriptor
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.set_option(asio::ip::tcp::no_delay(true));

    acceptor.bind(localhost);
    acceptor.listen();


    int i = 0;
    while(true){
        std::cout << "iteration " << i++ << std::endl;
        AsioSocket ClientSocket(io_context_);
        AsioSocket ServerSocket(io_context_);
        
        AtomicSPSCQueue ClientToServerQueue(&ClientSocket, &ServerSocket, BUFFERSIZE, server_buffer, "C->S");
        AtomicSPSCQueue ServerToClientQueue(&ServerSocket, &ClientSocket, BUFFERSIZE, client_buffer, "S->C");
        
        ClientSocket.await_connection(acceptor);
        ServerSocket.connect(endpoints[0]);
        
        ClientToServerQueue.Start();
        ServerToClientQueue.Start();
        
        ClientToServerQueue.WaitForStop();
        ServerToClientQueue.WaitForStop();
    }
}
