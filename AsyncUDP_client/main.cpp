
#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include <string>
#include <boost/asio.hpp>

using namespace boost::asio;

int main(int argc, char const *argv[])
{

        io_service service;
        try
        {
            ip::udp::resolver::query query(ip::udp::v4(), argv[1], argv[2]);
            ip::udp::resolver resolver(service);
            auto iter = resolver.resolve(query);
            ip::udp::endpoint endpoint = iter->endpoint();
            ip::udp::socket socket(service, ip::udp::v4());

            while (true)
            {
                std::cout << "biometric number or string [1-9]: "; //предлогаем ввести номера или строку не более 9 байт.
                std::string line;
                std::cin >> line;                               //считываем
                if(std::cin.fail() || line.length() < 1 || line.length() > 9) break; //проверяем на условия задачи

                // auto request = line;
                socket.send_to(buffer(line, line.length()), endpoint);  //записываем в буфер и отправляем

                std::array<char, 9> reply;
                auto reply_length = socket.receive(buffer(reply, reply.size())); //получаем ответ от сервера

                std::cout << "biometric scaner reply is: ";
                std::cout.write(reply.data(), reply_length); //распечатываем ответ из сервера
                std::cout << std::endl;
            }
        }
        catch(const std::exception& e) //если что ловим исключение
        {
            std::cerr << e.what() << '\n';
        }
        return 0;
}
