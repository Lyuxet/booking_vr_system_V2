#include "requests_handler.h"
#include "booking_vr.h"
#include "parseInpudData.h"


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

            // Получаем массивы из JSON
            std::vector<std::string> times;
            for (const auto& time : jsonData.at("times").as_array()) {
                times.push_back(time.as_string().c_str());
            }

            std::vector<int> playerCount;
            for (const auto& count : jsonData.at("playerCount").as_array()) {
                playerCount.push_back(count.as_int64());
            }

            std::vector<int> price;
            for (const auto& p : jsonData.at("price").as_array()) {
                price.push_back(p.as_int64());
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

            // Успешный ответ
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"status": "success", "message": "Booking inserted successfully"})";

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            res.result(http::status::internal_server_error);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"status": "error", "message": ")" + std::string(e.what()) + R"("})";
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

            AvailabilityData data = {date, namegame, placegame};
            std::string response;

            if (placegame == "ARENA") {
                Arena arena(pool);
                arena.AddDataByCheckAvailability(data);
                response = arena.CheckAvailabilityPlace();
            } else if (placegame == "CUBES") {
                Cubes cubes(pool);
                cubes.AddDataByCheckAvailability(data);
                response = cubes.CheckAvailabilityPlace();
            }

            // Устанавливаем содержимое ответа
            res.set(http::field::content_type, "application/json"); // Установка заголовка Content-Type
            res.body() = response; // Установка тела ответа
            res.result(http::status::ok); // Установите статус ответа на 200 OK

        } catch (const std::exception& e) {
            // Логируем ошибки
            std::cerr << "Error: " << e.what() << std::endl;
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

            // Получаем массивы из JSON
            std::vector<std::string> times;
            for (const auto& time : jsonData.at("times").as_array()) {
                times.push_back(time.as_string().c_str());
            }

            std::vector<int> playerCount;
            for (const auto& count : jsonData.at("playerCount").as_array()) {
                playerCount.push_back(count.as_int64());
            }

            std::vector<int> price;
            for (const auto& p : jsonData.at("price").as_array()) {
                price.push_back(p.as_int64());
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
                bookings.push_back({placegame, typegame, namegame, date, times[i], playerCount[i], price[i], comment});
            }

            // Вставка данных
            cubes.AddDataByInsertAndUpdate(client, bookings);
            cubes.Open_cubes();

            // Успешный ответ
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"status": "success", "message": "Booking inserted successfully"})";

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            res.result(http::status::internal_server_error);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"status": "error", "message": ")" + std::string(e.what()) + R"("})";
        }
    }





}