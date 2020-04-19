#include <set>
#include <random>
#include <iostream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

template <typename T> // создаем шаблонный класс Биометрический сканер(по умолчанию char)
class biometric_scanner
{
    private:
        std::set<std::pair<T, T>> set_compare; //создаем множество пар, где будут сравниваться
                    // поступающие на сканер от клиента биометрические данные в виде массива,
                    // и массивом биометрический эталон.
        std::unique_ptr<T[]> array; //создадим массив биометрический эталон
        std::string conToStr(const T& res) {
            std::ostringstream result;
            result << std::hex << std::uppercase << res;
            return result.str();
        }
    public:
        biometric_scanner() {
            this->set_biometric_standard();
        }
        virtual ~biometric_scanner(){} //деструктор

        void constract(const std::string& ar0) //заполняем множество парами данных
        {
            if(ar0.length() > 9 && ar0.length() < 1) throw std::out_of_range("arrays error"); //проверяем на совместимость массива
            for (size_t i = 0; i < ar0.length(); i++) //с начальными данными, если не выполняется выкидыываем исключение
            {
                set_compare.insert(std::make_pair(array[i], ar0[i])); //заполняем
            }
        }
        std::string get_biometric_result() //выводим результат,
        {
            std::random_device dre; //так как в задании надо выводить истину или лож случайным образом,
            std::mt19937 gen(dre());
            std::uniform_int_distribution<int> di(0,1); //задаем генератор случайных целых чисел 0 и 1
            return di(gen) == 0 ? "true" : "false";
        }
        std::string print_biometric() //распечатываем пару биометрические данные и биометрический эталон
        {
            std::string tmp;
            for(const auto& [x, y] : set_compare) tmp += "(scan: " + conToStr(x) + ", stand: " + conToStr(y) + "); ";
            return  tmp;
        }
        std::string  uuids() const //создаем универсальный уникальный идентификатор
        {
            boost::uuids::uuid uuid = boost::uuids::random_generator()();
            return "UUID Sensor: (" + boost::lexical_cast<std::string>(uuid) + ") ";
        }

    private:
        void set_biometric_standard() //создаем массив биометрический эталон
        {
            array = std::make_unique<T[]>(9); //так как не было огаворены начальные данные
            for (size_t i = 0; i < 9; i++)    //то я задаю их простым натуральным рядом от 0 до 9(не включительно)
            {
                std::string s = std::to_string(i); //конвертируем в строку
                array[i] = *s.c_str(); //заполняем массив char
            }                          //данная функция может ругаться при изменнение типа Т, я не стал дописывать метод по идеологии SFINAE
        }
};

class sensor //создаем структуру Датчиков
{
    public:
        sensor(){}
        std::string get_sensor_data() //выдаем случайным образом Да или Нет, как обговорено в задании
        {
            std::random_device dre;
            std::mt19937 gen(dre());
            std::uniform_int_distribution<int> di(0,1);
            return di(gen) == 0 ? "Yes" : "No";
        }
        std::string uuids() const //создаем универсальный уникальный идентификатор для датчиков
        {
            boost::uuids::uuid uuid = boost::uuids::random_generator()();
            return "UUID Sensor: (" + boost::lexical_cast<std::string>(uuid) + ") ";
        }
        virtual ~sensor(){}
};
