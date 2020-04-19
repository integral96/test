#include <iostream>

#include <UDP_server.hpp>


#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/function.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/make_shared.hpp>

static size_t server1_port_name = 3333;

static size_t server2_port_name = 3334;

template <typename T1>
void handler1(T1 messaga) {
    std::cout << messaga << ". ";
}

template <typename T1, typename... Types>
void handler1(T1 messaga, Types&&... args) {
     handler1(messaga);
     handler1(args...);
}

static auto handler([](size_t bytes_transfered, const boost::system::error_code& ec, auto&&... param){
    if (!ec) {
        std::cout << "Обработано: " << bytes_transfered << std::endl;
        handler1(std::forward<decltype(param)>(param)...);
        std::cout << std::endl << std::endl;
    } else if (ec == boost::asio::error::operation_aborted) {
        std::cout << "Zaniato." << std::endl;
    } else {
        std::cout << "failed! code = {" << ec.value() << "}. Messaga: " << ec.message() << std::endl;
    }
});

int main()
{
    try {
        boost::asio::io_service service;
        UDP_server<char, uint8_t, decltype (handler)> server;
        server.server_accept_connection(service, server2_port_name, handler);

    } catch (boost::system::system_error& ec) {
        std::cout << "Error ocured! # " << ec.code() << " Messaga "
                  << ec.what() << std::endl;
    }
    return 0;
}
