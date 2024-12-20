#include "requests_handler.h"
#include "booking_vr.h"
#include "parseInpudData.h"
#include "logger.h"


namespace vr{
    
    namespace json = boost::json;

    void ArenaBookingInsert(const json::value& jsonData, http::response<http::string_body>& res, ConnectionPool& pool) {
        try {
            // Получаем значения из JSON
            std::string firstname = jsonData.at("firstname").as_string().c_str();
            std::string lastname = jsonData.at("lastname").as_string().c_str();
            std::string phone = jsonData.at("phone").as_string().c_str();
            std::string email = jsonData.at("email").as_string().c_str();
            std::string placegame = jsonData.at("placegame").as_string().c_str();
            std::string typegame = jsonData.at("typegame").as_string().c_str();
            std::string namegame = jsonData.at("namegame").as_string().c_str();
            std::string date = jsonData.at("date").as_string().c_str();
            std::string who_reservation = jsonData.at("lastname_admin").as_string().c_str();
            std::string book_status = jsonData.at("book_status").as_string().c_str();
            
            // Получаем массивы из JSON
            std::vector<std::string> times;
            for (const auto& time : jsonData.at("times").as_array()) {
                times.push_back(time.as_string().c_str());
            }

            std::vector<int> playerCount;
            for (const auto& count : jsonData.at("playerCount").as_array()) {
                playerCount.push_back(static_cast<int>(count.as_int64()));
            }

            std::vector<int> price;
            for (const auto& p : jsonData.at("price").as_array()) {
                price.push_back(static_cast<int>(p.as_int64()));
            }

            // Проверяем, что массивы имеют одинаковую длину
            if (times.size() != playerCount.size() || times.size() != price.size()) {
                throw std::runtime_error("Arrays times, playerCount, and price must be of the same length");
            }

            // Получаем комментарий
            std::string comment = jsonData.at("comment").as_string().c_str();

            // Обработка данных клиента
            Client_data client = {firstname, lastname, phone, email};
            Arena arena(pool);

            // Формируем массив данных для вставки
            std::vector<Booking_data> bookings;
            for (size_t i = 0; i < times.size(); ++i) {
                bookings.push_back({placegame, typegame, namegame, date, times[i], playerCount[i], price[i], comment, who_reservation, book_status});
            }

            // Вставка данных
            arena.AddDataByInsertAndUpdate(client, bookings);

            // Открываем или закрываем арену в зависимости от типа игры
            if (typegame == "Открытая игра") {
                arena.Open_arena();
            } else if (typegame == "Закрытая игра") {
                arena.Close_arena();
            }

            // Успешный ответ
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"status": "success", "message": "Booking inserted successfully"})";

        } catch (const std::exception& e) {
            Logger::getInstance().log("Error: " + std::string(e.what()) + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
            "../logs/error_transaction.log");
            res.result(http::status::bad_request);
            res.set(http::field::content_type, "application/json");
            res.body() = std::string(e.what());
        }
    }
    void Availability(const http::request<http::string_body>& req, http::response<http::string_body>& res, ConnectionPool& pool) {
        try {
            // Парсим параметры из строки запроса
            auto formData = parse_form_data(req.target());
            // Пример получения параметров
            std::string date = formData.at("date");
            std::string namegame = formData.at("namegame");
            std::string placegame = formData.at("placegame");
            std::string typegame = formData.at("typegame") == "undefined" ? "" : (formData.at("typegame") == "false" ? "Открытая игра" : "Закрытая игра");
            //Дальше по коду дополнять 
            AvailabilityData data = {date, namegame, placegame, typegame};
            std::string response;

            if (placegame == "VR Арена") {
                Arena arena(pool);
                arena.AddDataByCheckAvailability(data);
                response = arena.initBunnot();
            } else if (placegame == "VR Кубы") {
                Cubes cubes(pool);
                cubes.AddDataByCheckAvailability(data);
                response = cubes.initBunnot();
            }

            // Устанавливаем содержимое ответа
            res.set(http::field::content_type, "application/json"); // Установка заголовка Content-Type
            res.body() = response; // Установка тела ответа
            res.result(http::status::ok); // Установите статус ответа на 200 OK

        } catch (const std::exception& e) {
            // Логируем ошибки
            Logger::getInstance().log("Error: " + std::string(e.what()) + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
            "../logs/error_transaction.log");
            res.result(http::status::internal_server_error);
            res.set(http::field::content_type, "text/plain");
            res.body() = e.what(); // Устанавливаем тело ответа в сообщении об ошибке
        }
    }

    void CubesBookingInsert(const json::value& jsonData, http::response<http::string_body>& res, ConnectionPool& pool) {
        try {
            // Получаем значения из JSON
            std::string firstname = jsonData.at("firstname").as_string().c_str();
            std::string lastname = jsonData.at("lastname").as_string().c_str();
            std::string phone = jsonData.at("phone").as_string().c_str();
            std::string email = jsonData.at("email").as_string().c_str();
            std::string placegame = jsonData.at("placegame").as_string().c_str();
            std::string typegame = jsonData.at("typegame").as_string().c_str();
            std::string namegame = jsonData.at("namegame").as_string().c_str();
            std::string date = jsonData.at("date").as_string().c_str();
            std::string who_reservation = jsonData.at("lastname_admin").as_string().c_str();
            std::string book_status = jsonData.at("book_status").as_string().c_str();
            
            // Получаем массивы из JSON
            std::vector<std::string> times;
            for (const auto& time : jsonData.at("times").as_array()) {
                times.push_back(time.as_string().c_str());
            }

            std::vector<int> playerCount;
            for (const auto& count : jsonData.at("playerCount").as_array()) {
                playerCount.push_back(static_cast<int>(count.as_int64()));
            }

            std::vector<int> price;
            for (const auto& p : jsonData.at("price").as_array()) {
                price.push_back(static_cast<int>(p.as_int64()));
            }

            // Проверяем, что массивы имеют одинаковую длину
            if (times.size() != playerCount.size() || times.size() != price.size()) {
                throw std::runtime_error("Arrays times, playerCount, and price must be of the same length");
            }

            // Получаем комментарий
            std::string comment = jsonData.at("comment").as_string().c_str();

            // Обработка данных клиента
            Client_data client = {firstname, lastname, phone, email};
            Cubes cubes(pool);

            // Формируем массив данных для вставки
            std::vector<Booking_data> bookings;
            for (size_t i = 0; i < times.size(); ++i) {
                bookings.push_back({placegame, typegame, namegame, date, times[i], playerCount[i], price[i], comment, who_reservation, book_status});
            }

            // Вставка данных
            cubes.AddDataByInsertAndUpdate(client, bookings);
            cubes.Open_cubes();

            // Успешный ответ
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"status": "success", "message": "Booking inserted successfully"})";

        } catch (const std::exception& e) {
            Logger::getInstance().log("Error: " + std::string(e.what()) + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
            "../logs/error_transaction.log");
            res.result(http::status::bad_request);
            res.set(http::field::content_type, "application/json");
            res.body() = std::string(e.what());
        }
    }


    void GetAdminBooking(const http::request<http::string_body>& req, http::response<http::string_body>& res, ConnectionPool& pool) {
        auto formData = parse_form_data(req.target());

        std::string date = formData.at("date");
        std::string place_game = formData.at("place_game");
        std::string response;
        Arena arena(pool);
        response = arena.GetAdminBooking(date, place_game);
        res.set(http::field::content_type, "application/json");
        res.body() = response;
        res.result(http::status::ok);
        
    }





}