#pragma once

#define BOOST_COROUTINES_NO_DEPRECATION_WARNING

#include <iostream>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/function.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/make_shared.hpp>

#include <biometric_scanner.hpp>

static const size_t MAX_BUFFER = 512;

template<typename T, typename F, typename Callback>
class UDP_server : public boost::noncopyable
{
public:
    using shared_udp_socket = boost::shared_ptr<boost::asio::ip::udp::socket>;
private:
    shared_udp_socket ptr_udp_sock;
    boost::asio::io_service m_service;
    boost::shared_array<T> m_buffer;
    boost::shared_ptr<biometric_scanner<T>> ptr_bioscnr;

    boost::shared_ptr<sensor> ptr_sensor;
    std::string msg;
private:
    F extractField(F b, F mask, size_t shift) {
        F tmp;
        tmp = (b & mask) << shift;
        return tmp;
    }
public:
    UDP_server() : m_buffer(new T[MAX_BUFFER]), ptr_bioscnr(boost::make_shared<biometric_scanner<T>>()), msg("HELLO CLIENT"),
                                                ptr_sensor(boost::make_shared<sensor>()){}
    ~UDP_server() {
        m_service.stop();
        ptr_udp_sock->shutdown(boost::asio::ip::udp::socket::shutdown_both);
    }

    template<typename F1>
    void send_responce(boost::asio::yield_context yield, shared_udp_socket socket, const boost::asio::ip::udp::endpoint& ep,
                      const typename boost::enable_if<std::is_same<F1, std::string>, F1>::type& request) {
        socket->async_send_to(boost::asio::buffer(request, request.length()), ep, yield);
    }
    template<typename F1>
    void send_responce(boost::asio::yield_context yield, shared_udp_socket socket, const boost::asio::ip::udp::endpoint& ep,
                      const typename boost::enable_if<std::is_same<F1, boost::shared_array<T>>, F1>::type& request, size_t byte_transfered) {
        socket->async_send_to(boost::asio::buffer(request.get(), byte_transfered), ep, yield);
    }
    void recieve_request(boost::asio::yield_context yield, boost::asio::io_service& service, size_t port_num, Callback callback) {
        ptr_udp_sock = boost::make_shared<boost::asio::ip::udp::socket>(service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port_num));
        boost::asio::ip::udp::endpoint m_remote_ep;
        boost::system::error_code m_ec;
        while (true) {
            m_buffer.reset(new T[MAX_BUFFER]);
            auto byte_recived = ptr_udp_sock->async_receive_from(boost::asio::buffer(m_buffer.get(), MAX_BUFFER), m_remote_ep, yield[m_ec]);
            if(!m_ec) {
                boost::asio::spawn(ptr_udp_sock->get_executor(), boost::bind(&UDP_server::send_responce<boost::shared_array<T>>,
                                                                             this, _1, ptr_udp_sock, m_remote_ep, m_buffer, byte_recived));
            }

            std::string number(m_buffer.get(), byte_recived);
            biometric_scanner<T> bio_scnr; //создаем экземпляр класса биометрический сканер
            bio_scnr.constract(number); //передаем ему номер
            sensor snsr;                //создаем экземпляр класса Датчик
            auto result = bio_scnr.get_biometric_result(); //получаем результат от методов биометрический сканер


            callback(byte_recived, m_ec, " Biometric scanner: ", bio_scnr.get_biometric_result(), ";\t sensor: ", snsr.get_sensor_data(), bio_scnr.uuids(), bio_scnr.print_biometric());
            boost::asio::spawn(ptr_udp_sock->get_executor(), boost::bind(&UDP_server::send_responce<std::string>,
                                                                         this, _1, ptr_udp_sock, m_remote_ep, result));
            result.erase();
        }
    }
    void server_accept_connection(boost::asio::io_service& service, size_t port_num, Callback callback) {
        boost::asio::spawn(service, boost::bind(&UDP_server::recieve_request, this, _1, boost::ref(service), port_num, boost::ref(callback)));
        service.run();
    }
    
private:
    std::string conToStr(const size_t& res) {
        std::ostringstream result;
        result << std::hex << std::uppercase << res;
        return result.str();
    }
};

