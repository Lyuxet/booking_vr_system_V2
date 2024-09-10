#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "test_insert_data.h"
#include "test_delete_data.h"
#include "test_update_data.h"

#include <iostream>
#include <thread>
#include <vector>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <sstream>
#include <unordered_map>

// Функция для разбиения строки на вектор строк по разделителю
std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::istringstream stream(str);
    std::string item;

    while (std::getline(stream, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}

// Функция для парсинга строки чисел в вектор целых чисел
std::vector<int> parse_int_list(const std::string& str) {
    std::vector<int> result;
    std::vector<std::string> tokens = split_string(str, ',');

    for (const auto& token : tokens) {
        try {
            result.push_back(std::stoi(token));
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid integer in list");
        }
    }

    return result;
}

// Функция для парсинга строки запроса в формате x-www-form-urlencoded
std::unordered_map<std::string, std::string> parse_form_data(const std::string& data) {
    std::unordered_map<std::string, std::string> result;
    std::istringstream stream(data);
    std::string key_value;
    
    while (std::getline(stream, key_value, '&')) {
        auto equal_pos = key_value.find('=');
        if (equal_pos != std::string::npos) {
            std::string key = key_value.substr(0, equal_pos);
            std::string value = key_value.substr(equal_pos + 1);
            
            // Декодирование URL-элементов
            std::replace(key.begin(), key.end(), '+', ' ');
            std::replace(value.begin(), value.end(), '+', ' ');
            // Обработка кодировок
            std::string::size_type pos = 0;
            while ((pos = value.find('%', pos)) != std::string::npos) {
                if (pos + 2 < value.size()) {
                    int code;
                    std::istringstream hex_stream(value.substr(pos + 1, 2));
                    hex_stream >> std::hex >> code;
                    value[pos] = static_cast<char>(code);
                    value.erase(pos + 1, 2);
                }
            }
            result[std::string(key)] = std::string(value);
        }
    }
    
    return result;
}

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
        for (size_t i = 0; i < times.size(); ++i) {
            Booking_data booking = {typegame, namegame, date, times[i], playerCount[i], price[i], comment};
            arena.AddDataByInsertAndUpdate(client, booking);
            arena.Open_arena();
        }

        // Успешный ответ
        nlohmann::json response;
        response["status"] = "success";
        response["message"] = "Data successfully added";
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        // Логируем ошибки
        std::cerr << "Error: " << e.what() << std::endl;

        // Обработка ошибок
        nlohmann::json response;
        response["status"] = "error";
        response["message"] = e.what();
        res.set_content(response.dump(), "application/json");
    }
}



static void add_cors_headers(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

int main() {
    ConnectionPool pool(10, "db_config.conf");
    pool.Init_pool();
    httplib::Server server;

    // Обрабатываем основной POST-запрос с CORS-заголовками
    server.Post("/addBooking", [&pool](const httplib::Request& req, httplib::Response& res) {
        add_cors_headers(res);  // Добавляем CORS-заголовки в ответ
        add(req, res, pool);  // Ваша функция для обработки данных
    });

    // Обрабатываем preflight-запросы методом OPTIONS
    server.Options("/*", [](const httplib::Request& req, httplib::Response& res) {
        add_cors_headers(res);  // Добавляем CORS-заголовки для preflight
        res.set_content("", "text/plain");  // Возвращаем пустое тело ответа
        res.status = 204;  // Статус "No Content"
    });

    std::cout << "Starting server on port 8080..." << std::endl; 
    server.listen("0.0.0.0", 8080);  // Запускаем сервер
}
