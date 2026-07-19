#include "../headers/AsioSocket.hpp"

AsioSocket::AsioSocket
(
    asio::io_context& ctx
)
: socket(ctx) {}

// write to the socket from a buffer
size_t AsioSocket::write(void* buffer, size_t amount, asio::error_code& ec)
{
    // TODO: add error code to the function arguments so the main thread only makes 1 check.
    return socket.write_some(asio::mutable_buffer(buffer, amount), ec);
}

// read from the socket into a buffer
size_t AsioSocket::read(void* buffer, size_t amount, asio::error_code& ec)
{
    return socket.read_some(asio::mutable_buffer(buffer, amount), ec);
}

// return the available bytes in the socket
size_t AsioSocket::available()
{
    return socket.available();
}

// bind to the endpoint, open on ipv4, listen and accept
void AsioSocket::await_connection(asio::ip::tcp::acceptor& acceptor)
{
    std::cout << " Awaiting connection..." << std::endl;
    acceptor.accept(socket, receiving_endpoint);

    std::cout << "Accepted Connection from: " << receiving_endpoint.address() << std::endl;
}

// connect to the endpoint
void AsioSocket::connect(asio::ip::tcp::endpoint& endpoint)
{
    asio::error_code ec;
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

void AsioSocket::Close()
{
    asio::error_code ec;
    socket.cancel(ec);
    socket.close(ec);
}

// private:
//     asio::ip::tcp::socket socket;
//     asio::ip::tcp::acceptor acceptor;
//     asio::ip::tcp::endpoint endpoint;