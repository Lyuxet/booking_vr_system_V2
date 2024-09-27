#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "requests_handler.h"

#include <iostream>
#include <httplib.h>

int main() {
    ConnectionPool pool(10, "db_config.conf");
    pool.Init_pool();
    httplib::Server server;

    // Обрабатываем основной POST-запрос с CORS-заголовками
    server.Post("/addBookingOpenArena", [&pool](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        vr::ArenaBookingInsert(req, res, pool);  // Ваша функция для обработки данных
    });

    server.Post("/addBookingCubes", [&pool](const httplib::Request& req, httplib::Response& res){
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        vr::CubesBookingInsert(req, res, pool);
    });

    server.Get("/getBookingCubes", [&pool](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        vr::Availability(req, res, pool);  // Ваша функция для обработки GET-запросов
    });

    // Обрабатываем GET-запросы с CORS-заголовками
    server.Get("/getBookingOpenArena", [&pool](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        vr::Availability(req, res, pool);  // Ваша функция для обработки GET-запросов
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
