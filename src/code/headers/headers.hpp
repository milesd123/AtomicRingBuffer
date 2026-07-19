#ifndef HEADERS_H
#define HEADERS_H

#define ASIO_STANDALONE

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <iostream>
#include <cstdint>
#include <atomic>
#include <iterator>
#include <chrono>

#include "../../external/asio-1.36.0/include/asio.hpp"

#include "SimpleSocket.hpp"
#include "AsioSocket.hpp"
#include "TestSocket.hpp"
#include "AtomicSPSCQueue.hpp"
#include "ProxyUtils.hpp"

#endif