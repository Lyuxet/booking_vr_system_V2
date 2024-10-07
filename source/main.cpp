#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "requests_handler.h"
#include "websocket.h"
#include "https_server.h"

int main() {

    ConnectionPool pool(10, "db_config.conf"); // Инициализация пула соединений с базой данных
    std::set<std::shared_ptr<WebSocketSession>> sessions; // Набор активных WebSocket-сессий

    // Запуск HTTP сервера на порту 8080
    HttpServer http_server(8081, pool, sessions);
    std::thread http_thread([&http_server]() { http_server.run(); });
    std::cout << "HttpServer на порту 8081 запущен..." << std::endl;
        
    // Запуск WebSocket сервера на порту 8081
    WebSocketServer websocket_server(8082, sessions);
    std::thread websocket_thread([&websocket_server]() { websocket_server.run(); });
    std::cout << "WebSocketServer на порту 8082 запущен..." << std::endl << std::endl;


    // Ждем завершения потоков
        http_thread.join();
        websocket_thread.join();
   
}
