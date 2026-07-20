Atomic Ring Buffer

An atomic wait-free ring single producer single consumer buffer that reads and writes from SimpleSockets, using standalone ASIO's tcp sockets *synchronously*. In the examples folder, this is used for a minecraft proxy server to handle the connection from a single client to a provided server, as well as some simple benchmarking code. The only synchronization of the producer/consumer is via the memory barriers, so they will spin very frequently if there is nothing to read/write.
