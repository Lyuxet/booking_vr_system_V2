#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "test_insert_data.h"
#include "test_delete_data.h"
#include "test_update_data.h"
#include "parseInpudData.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>
#include <vector>
#include <httplib.h>


static void add(const httplib::Request& req, httplib::Response& res, ConnectionPool& pool) {
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
        if (formData.at("typegame") == "OPEN"){
            for (size_t i = 0; i < times.size(); ++i) {
                Booking_data booking = {placegame, typegame, namegame, date, times[i], playerCount[i], price[i], comment};
                arena.AddDataByInsertAndUpdate(client, booking);
                arena.Open_arena();
            }
        }
        else if (formData.at("typegame") == "CLOSE"){
            for (size_t i = 0; i < times.size(); ++i) {
                Booking_data booking = {placegame, typegame, namegame, date, times[i], playerCount[i], price[i], comment};
                arena.AddDataByInsertAndUpdate(client, booking);
                arena.Close_arena();
            }
        }
        

        
    } catch (const std::exception& e) {
        // Логируем ошибки
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content(std::string(e.what()), "text/plain");

        
    }
}

// Обработка GET-запроса
static void get(const httplib::Request& req, httplib::Response& res, ConnectionPool& pool) {
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
        Arena arena(pool);
        arena.AddDataByCheckAvailability(data);
        std::string response = arena.CheckAvailabilityPlace();

        res.set_content(response, "application/x-www-form-urlencoded");

    } catch (const std::exception& e) {
        // Логируем ошибки
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content(std::string(e.what()), "text/plain");
    }
}



void add_cors_headers(httplib::Response& res) {
}

int main() {
    ConnectionPool pool(10, "db_config.conf");
    pool.Init_pool();
    httplib::Server server;

    // Обрабатываем основной POST-запрос с CORS-заголовками
    server.Post("/addBookingOpenArena", [&pool](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        add(req, res, pool);  // Ваша функция для обработки данных
    });

    // Обрабатываем GET-запросы с CORS-заголовками
    server.Get("/getBookingOpenArena", [&pool](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        get(req, res, pool);  // Ваша функция для обработки GET-запросов
    });

    // Обрабатываем preflight-запросы методом OPTIONS
    server.Options("/*", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.set_content("", "text/plain");  // Возвращаем пустое тело ответа
        res.status = 204;  // Статус "No Content"
    });

    std::cout << "Starting server on port 8080..." << std::endl; 
    server.listen("0.0.0.0", 8080);  // Запускаем сервер
}
