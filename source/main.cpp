#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "requests_handler.h"
#include "websocket.h"
#include "https_server.h"
#include "logger.h"  // Подключаем логгер

int main() {

    ConnectionPool pool(10, "db_config.conf"); // Инициализация пула соединений с базой данных
    std::set<std::shared_ptr<WebSocketSession>> sessions; // Набор активных WebSocket-сессий

    try {
        // Запуск HTTP сервера на порту 8080
        HttpServer http_server(8081, pool, sessions);
        std::thread http_thread([&http_server]() { 
            try {
                http_server.run();
            } catch (const std::exception& e) {
                Logger::getInstance().log("Ошибка HTTP сервера: " + std::string(e.what()), "../../logs/error_connect.log");
            }
        });
        std::cout << "HttpServer на порту 8081 запущен..." << std::endl;
        
        // Запуск WebSocket сервера на порту 8082
        WebSocketServer websocket_server(8082, sessions);
        std::thread websocket_thread([&websocket_server]() { 
            try {
                websocket_server.run(); 
            } catch (const std::exception& e) {
                Logger::getInstance().log("Ошибка WebSocket сервера: " + std::string(e.what()), "../../logs/error_connect.log");
            }
        });
        std::cout << "WebSocketServer на порту 8082 запущен..." << std::endl << std::endl;

        // Ждем завершения потоков
        http_thread.join();
        websocket_thread.join();
   
    } catch (const std::exception& e) {
        // Логирование критической ошибки
        Logger::getInstance().log("Критическая ошибка в приложении: " + std::string(e.what()), "../logs/error_connect.log");
    }
}
