#include "requests_handler.h"
#include "booking_vr.h"
#include "parseInpudData.h"


namespace vr{
    
    void ArenaBookingInsert(const httplib::Request& req, httplib::Response& res, ConnectionPool& pool) {
        try {
            // Проверяем заголовок Content-Type
            if (req.get_header_value("Content-Type") != "application/x-www-form-urlencoded") {
                throw std::runtime_error("Content-Type must be application/x-www-form-urlencoded");
            }

            // Парсим данные из строки запроса
            auto formData = parse_form_data(req.body);

            // Получаем значения из данных
            std::string firstname = formData.at("firstname");
            std::string lastname = formData.at("lastname");
            std::string phone = formData.at("phone");
            std::string email = formData.at("email");
            std::string placegame = formData.at("placegame");
            std::string typegame = formData.at("typegame");
            std::string namegame = formData.at("namegame");
            std::string date = formData.at("date");

            // Получаем массивы
            std::vector<std::string> times = split_string(formData.at("times"), ',');
            std::vector<int> playerCount = parse_int_list(formData.at("playerCount"));
            std::vector<int> price = parse_int_list(formData.at("price"));

            // Проверяем, что массивы имеют одинаковую длину
            if (times.size() != playerCount.size() || times.size() != price.size()) {
                throw std::runtime_error("Arrays times, playerCount, and price must be of the same length");
            }

            // Получаем комментарий
            std::string comment = formData.at("comment");

            // Обработка данных
            Client_data client = {firstname, lastname, phone, email};
            Arena arena(pool);

            // Формируем массив данных для вставки
            std::vector<Booking_data> bookings;
            for (size_t i = 0; i < times.size(); ++i) {
                bookings.push_back({placegame, typegame, namegame, date, times[i], playerCount[i], price[i], comment});
            }

            // Вставка данных
            arena.AddDataByInsertAndUpdate(client, bookings);

            // Открываем или закрываем арену в зависимости от типа игры
            if (typegame == "OPEN") {
                arena.Open_arena();
            } else if (typegame == "CLOSE") {
                arena.Close_arena();
            }

        } catch (const std::exception& e) {
            // Логируем ошибки
            std::cerr << "Error: " << e.what() << std::endl;
            res.status = 500;
            res.set_content(std::string(e.what()), "text/plain");
        }
    }
    void Availability(const httplib::Request& req, httplib::Response& res, ConnectionPool& pool){
        try {
            // Парсим параметры из строки запроса
            auto queryParams = req.params;
            
            // Пример получения параметров
            auto itDate = queryParams.find("date");
            auto itNameGame = queryParams.find("namegame");
            auto itPlaceGame = queryParams.find("placegame");
            std::string date, namegame, placegame;

            if (itDate != queryParams.end() && itNameGame != queryParams.end() && itPlaceGame != queryParams.end()){
                date = itDate->second;
                namegame = itNameGame->second;
                placegame = itPlaceGame->second;
            }
            else {
                throw std::runtime_error("data isn't correctly");
            }

            AvailabilityData data = {date, namegame, placegame};
            std::string response;
            if (placegame == "ARENA"){
                Arena arena(pool);
                arena.AddDataByCheckAvailability(data);
                response = arena.CheckAvailabilityPlace();
            }
            else if(placegame == "CUBES"){
                Cubes cubes(pool);
                cubes.AddDataByCheckAvailability(data);
                response = cubes.CheckAvailabilityPlace();
            }

            res.set_content(response, "application/x-www-form-urlencoded");

        } catch (const std::exception& e) {
            // Логируем ошибки
            std::cerr << "Error: " << e.what() << std::endl;
            res.status = 500;
            res.set_content(std::string(e.what()), "text/plain");
        }
    }   


    void CubesBookingInsert(const httplib::Request& req, httplib::Response& res, ConnectionPool& pool) {
        try {
            // Проверяем заголовок Content-Type
            if (req.get_header_value("Content-Type") != "application/x-www-form-urlencoded") {
                throw std::runtime_error("Content-Type must be application/x-www-form-urlencoded");
            }

            // Парсим данные из строки запроса
            auto formData = parse_form_data(req.body);

            // Получаем значения из данных
            std::string firstname = std::move(formData.at("firstname"));
            std::string lastname = std::move(formData.at("lastname"));
            std::string phone = std::move(formData.at("phone"));
            std::string email = std::move(formData.at("email"));
            std::string placegame = std::move(formData.at("placegame"));
            std::string typegame = std::move(formData.at("typegame"));
            std::string namegame = std::move(formData.at("namegame"));
            std::string date = std::move(formData.at("date"));

            // Получаем массивы
            std::vector<std::string> times = split_string(formData.at("times"), ',');
            std::vector<int> playerCount = parse_int_list(formData.at("playerCount"));
            std::vector<int> price = parse_int_list(formData.at("price"));

            // Проверяем, что массивы имеют одинаковую длину
            if (times.size() != playerCount.size() || times.size() != price.size()) {
                throw std::runtime_error("Arrays times, playerCount, and price must be of the same length");
            }

            // Получаем комментарий
            std::string comment = std::move(formData.at("comment"));

            // Обработка данных
            Client_data client = {std::move(firstname), std::move(lastname), std::move(phone), std::move(email)};
            Cubes cubes(pool);

            // Формируем массив данных для вставки
            std::vector<Booking_data> bookings;
            bookings.reserve(times.size()); // Резервируем память для массива, чтобы избежать лишних аллокаций
            for (size_t i = 0; i < times.size(); ++i) {
                bookings.emplace_back(Booking_data{
                    std::move(placegame),
                    std::move(typegame),
                    std::move(namegame),
                    std::move(date),
                    std::move(times[i]),
                    playerCount[i],
                    price[i],
                    std::move(comment)
                });
            }

            // Вставка данных
            cubes.AddDataByInsertAndUpdate(std::move(client), std::move(bookings));
            cubes.Open_cubes();

        } catch (const std::exception& e) {
            // Логируем ошибки
            std::cerr << "Error: " << e.what() << std::endl;
            res.status = 500;
            res.set_content(std::string(e.what()), "text/plain");
        }
    }




}