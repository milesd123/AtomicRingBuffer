#include "../headers/AsioSocket.hpp"

AsioSocket::AsioSocket
(
    asio::io_context& ctx, asio::error_code ec, asio::ip::tcp::endpoint ep
)
:   acceptor(ctx), endpoint(ep), socket(ctx), ec(ec) {}

// write to the socket from a buffer
size_t AsioSocket::write(void* buffer, size_t amount)
{
    return asio::write(socket, asio::mutable_buffer(buffer, amount));
}

// read from the socket into a buffer
size_t AsioSocket::read(void* buffer, size_t amount)
{
    return asio::read(socket, asio::mutable_buffer(buffer, amount));
}

// return the available bytes in the socket
size_t AsioSocket::available()
{
    return socket.available();
}

// bind to the endpoint, open on ipv4, listen and accept
void AsioSocket::await_connection()
{

    acceptor.open(endpoint.protocol());
    acceptor.bind(endpoint);
    acceptor.listen();

    std::cout << "["<< endpoint.address().to_string() <<":"<< endpoint.port()<< "] Awaiting connection..." << std::endl;
    acceptor.accept(socket, recieving_endpoint);

    std::cout << "Accepted Connection from: " << recieving_endpoint.address() << std::endl;
}

// connect to the endpoint
void AsioSocket::connect()
{
    socket.connect(endpoint, ec);

    if(ec){
        std::cout << "Error connecting to the endpoint " << endpoint <<": "<<ec.message()<<std::endl;
    } else{
        std::cout << "Connected to " << endpoint.address() << std::endl;
    }
}

// block until ready to read
void AsioSocket::WaitRead()
{
    socket.wait(asio::ip::tcp::socket::wait_read);
    // while(!socket.is_open());
}


// block until ready to write
void AsioSocket::WaitWrite()
{
    socket.wait(asio::ip::tcp::socket::wait_write);
    // while(!socket.is_open());
}

// private:
//     asio::ip::tcp::socket socket;
//     asio::ip::tcp::acceptor acceptor;
//     asio::ip::tcp::endpoint endpoint;