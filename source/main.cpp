#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "requests_handler.h"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/version.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <set>

namespace beast = boost::beast;       
namespace http = beast::http;         
namespace websocket = beast::websocket;
namespace net = boost::asio;           
namespace json = boost::json;         
using tcp = boost::asio::ip::tcp;     

// WebSocket-сессия
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    explicit WebSocketSession(tcp::socket socket, std::set<std::shared_ptr<WebSocketSession>>& sessions)
        : ws_(std::move(socket)), sessions_(sessions) {}

    void start() {
        ws_.async_accept(beast::bind_front_handler(&WebSocketSession::on_accept, shared_from_this()));
    }

    void send(const std::string& message) {
        ws_.async_write(net::buffer(message),
            beast::bind_front_handler(&WebSocketSession::on_write, shared_from_this()));
    }

private:
    void on_accept(beast::error_code ec) {
        if (ec) {
            std::cerr << "Ошибка при принятии: " << ec.message() << " (" << ec.value() << ")" << std::endl;
            return;
        }
        do_read();
    }

    void do_read() {
        ws_.async_read(buffer_,
            beast::bind_front_handler(&WebSocketSession::on_read, shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec == websocket::error::closed) {
            close_session();  // Закрываем и удаляем сессию
            return;
        } else if (ec) {
            std::cerr << "Ошибка при чтении: " << ec.message() << " (" << ec.value() << ")" << std::endl;
            close_session();  // При ошибке также удаляем сессию
            return;
        }
        do_read();
    }

    void on_write(beast::error_code ec, std::size_t) {
        if (ec) {
            std::cerr << "Ошибка при записи: " << ec.message() << " (" << ec.value() << ")" << std::endl;
        }
    }

    void close_session() {
        std::cout << "Попытка закрыть сессию..." << std::endl;

        if (ws_.is_open()) {
            beast::error_code ec;
            ws_.close(websocket::close_code::normal, ec);
            if (ec) {
                std::cerr << "Ошибка при закрытии: " << ec.message() << " (" << ec.value() << ")" << std::endl;
            }
        } else {
            std::cout << "Сокет уже закрыт." << std::endl;
        }

        sessions_.erase(shared_from_this());
        std::cout << "Сессия закрыта. Осталось активных сессий: " << sessions_.size() << std::endl << std::endl;
    }

    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;
    std::set<std::shared_ptr<WebSocketSession>>& sessions_;
};

// HTTP-сессия
class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
    HttpSession(tcp::socket socket, ConnectionPool& pool, std::set<std::shared_ptr<WebSocketSession>>& sessions)
        : socket_(std::move(socket)), pool_(pool), sessions_(sessions) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        http::async_read(socket_, buffer_, request_,
            beast::bind_front_handler(&HttpSession::on_read, shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
       if (ec) {
            std::cerr << "Error on read: " << ec.message() << " (" << ec.value() << ")" << std::endl;
            return;
        }

        // Обработка CORS preflight
        if (request_.method() == http::verb::options) {
            handle_options();
            return;
        }

        // Обработка других методов
        if (request_.method() == http::verb::get && request_.target() == "/ws") {
            // Если запрос на WebSocket, начинаем соединение
            handle_websocket_connection();
        } else if (request_.method() == http::verb::post && request_.target() == "/addBookingOpenArena") {
            handle_add_booking_open_arena();
        } else if (request_.method() == http::verb::post && request_.target() == "/addBookingCubes") {
            handle_add_booking_cubes();
        } else if (request_.method() == http::verb::get && request_.target().starts_with("/getBookingCubes")) {
            handle_get_booking_cubes();
        } else if (request_.method() == http::verb::get && request_.target().starts_with("/getBookingOpenArena")) {
            handle_get_booking_open_arena();
        } else {
            handle_not_found();
        }
    }

    void handle_websocket_connection() {
    auto ws_session = std::make_shared<WebSocketSession>(std::move(socket_), sessions_);
    ws_session->start();
    sessions_.insert(ws_session); // Добавление WebSocket-сессии
}
    void handle_options() {
        response_.result(http::status::no_content);
        response_.set(http::field::access_control_allow_origin, "*");
        response_.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
        response_.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
        do_write();
    }

    void handle_add_booking_open_arena() {
        set_cors_headers();

        auto parsed_json = json::parse(request_.body());

        vr::ArenaBookingInsert(parsed_json, response_, pool_);
        notify_clients("New booking made in open arena!");

        response_.set(http::field::content_type, "application/json");
        do_write();
    }

    void handle_add_booking_cubes() {
        set_cors_headers();

        auto parsed_json = json::parse(request_.body());

        vr::CubesBookingInsert(parsed_json, response_, pool_);
        notify_clients("New booking made in cubes!");

        response_.set(http::field::content_type, "application/json");
        
        do_write();
    }

    void notify_clients(const std::string& message) {
        for (const auto& session : sessions_) {
            session->send(message);
        }
    }

    void handle_get_booking_cubes() {
        set_cors_headers();

        vr::Availability(request_, response_, pool_);
        do_write();
    }

    void handle_get_booking_open_arena() {
        set_cors_headers();

        vr::Availability(request_, response_, pool_);
        do_write();
    }

    void handle_not_found() {
        response_.result(http::status::not_found);
        response_.set(http::field::content_type, "application/json");
        response_.body() = json::serialize(json::value{
            {"error", "Not Found"}
        });
        response_.prepare_payload();
        do_write();
    }

    void set_cors_headers() {
        response_.set(http::field::access_control_allow_origin, "*");
        response_.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
        response_.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
    }

    void do_write() {
        http::async_write(socket_, response_,
            beast::bind_front_handler(&HttpSession::on_write, shared_from_this()));
    }

    void on_write(beast::error_code ec, std::size_t) {
        if (ec) {
            std::cerr << "Error on write: " << ec.message() << " (" << ec.value() << ")" << std::endl;
            return;
        }
        socket_.shutdown(tcp::socket::shutdown_send, ec);
    }

    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    ConnectionPool& pool_;
    std::set<std::shared_ptr<WebSocketSession>>& sessions_; // Набор WebSocket-сессий
};

// HTTP-сервер
class HttpServer {
public:
    HttpServer(int port, ConnectionPool& pool, std::set<std::shared_ptr<WebSocketSession>>& sessions)
        : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)), pool_(pool), sessions_(sessions) {
        do_accept();
    }

    void run() {
        io_context_.run();
    }

private:
    void do_accept() {
        acceptor_.async_accept([this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                // Создание HTTP-сессии
                auto session = std::make_shared<HttpSession>(std::move(socket), pool_, sessions_);
                session->start();
            } else {
                std::cerr << "Error on accept: " << ec.message() << std::endl; 
            }
            do_accept();
        });
    }

    net::io_context io_context_;
    tcp::acceptor acceptor_;
    ConnectionPool& pool_;
    std::set<std::shared_ptr<WebSocketSession>>& sessions_; // Хранение активных WebSocket-сессий
};

// WebSocket-сервер
class WebSocketServer {
public:
    WebSocketServer(int port, std::set<std::shared_ptr<WebSocketSession>>& sessions)
        : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)), sessions_(sessions) {
        do_accept();
    }

    void run() {
        io_context_.run();
    }

private:
    void do_accept() {
        acceptor_.async_accept([this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                auto ws_session = std::make_shared<WebSocketSession>(std::move(socket), sessions_);
                ws_session->start();
                sessions_.insert(ws_session); // Добавление сессии в набор
                std::cout << "Добавлена новая сессия" << std::endl;
                std::cout << "Количество активных сессий: " << sessions_.size() << std::endl << std::endl;
            } else {
                std::cerr << "Error on accept: " << ec.message() << std::endl;
            }
            do_accept();
        });
    }   

    net::io_context io_context_;
    tcp::acceptor acceptor_;
    std::set<std::shared_ptr<WebSocketSession>>& sessions_; // Хранение активных WebSocket-сессий
};

// Точка входа
int main() {
    ConnectionPool pool(10, "db_config.conf"); // Инициализация пула соединений с базой данных
    std::set<std::shared_ptr<WebSocketSession>> sessions; // Набор активных WebSocket-сессий

    // Запуск HTTP сервера на порту 8080
    HttpServer http_server(8081, pool, sessions);
    std::thread http_thread([&http_server]() { http_server.run(); });

    // Запуск WebSocket сервера на порту 8081
    WebSocketServer websocket_server(8082, sessions);
    std::thread websocket_thread([&websocket_server]() { websocket_server.run(); });

    // Ждем завершения потоков
    http_thread.join();
    websocket_thread.join();

    return 0;
}
