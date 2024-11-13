#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "requests_handler.h"
#include "websocket.h"
#include "https_server.h"
#include "logger.h"
#include <csignal>
#include <cstdlib>
#include <thread>
#include <chrono>

HttpServer* http_server_ptr = nullptr;
WebSocketServer* websocket_server_ptr = nullptr;

void handle_sigint(int sig) { 
    std::cout << "Пойман сигнал: " << sig << ", очищение ресурсов..." << std::endl; 
    if (http_server_ptr) {
        http_server_ptr->stop();
    }
    if (websocket_server_ptr) {
        websocket_server_ptr->stop();
    }
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Даем время завершиться всем операциям
    exit(0);
}


int main() {
    std::signal(SIGINT, handle_sigint); 
    ConnectionPool pool(10, "db_config.conf");
    std::set<std::shared_ptr<WebSocketSession>> sessions;
    try {
        HttpServer http_server(8081, pool, sessions);
        http_server_ptr = &http_server; 
        std::thread http_thread([&http_server]() { 
            try {
                http_server.run();
            } catch (const std::exception& e) {
                Logger::getInstance().log("Ошибка HTTP сервера: " + std::string(e.what()) + 
                " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
                "../logs/error_connect.log");
            }
        });
        std::cout << "HttpServer на порту 8081 запущен..." << std::endl;
        
        WebSocketServer websocket_server(8082, sessions);
        websocket_server_ptr = &websocket_server; 
        std::thread websocket_thread([&websocket_server]() { 
            try {
                websocket_server.run(); 
            } catch (const std::exception& e) {
                Logger::getInstance().log("Ошибка WebSocket сервера: " + std::string(e.what()) + 
                " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
                "../logs/error_connect.log");
            }
        });
        std::cout << "WebSocketServer на порту 8082 запущен..." << std::endl << std::endl;

        http_thread.join();
        websocket_thread.join();
   
    } catch (const std::exception& e) {
        Logger::getInstance().log("Критическая ошибка в приложении: " + std::string(e.what()) + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
            "../logs/error_connect.log");
    }

    pool.CloseAllConnections();
    return 0;
}
