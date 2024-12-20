#pragma once

#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "requests_handler.h"
#include "websocket.h"
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



class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
    HttpSession(tcp::socket socket, ConnectionPool& pool, std::set<std::shared_ptr<WebSocketSession>>& sessions)
    : socket_(std::move(socket)), pool_(pool), sessions_(sessions) {}
    void start();


private:
    void do_read();
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void handle_options();
    void handle_add_booking_open_arena();
    void handle_add_booking_cubes();
    void notify_clients(const std::string& message, 
                                const std::string& place_game, const std::string& date_game);
    void handle_availability_booking_cubes();
    void handle_availability_booking_arena();
    void handle_get_admin_booking();
    void handle_not_found();
    void set_cors_headers();
    void do_write();
    void on_write(beast::error_code ec, std::size_t);

    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    ConnectionPool& pool_;
    std::set<std::shared_ptr<WebSocketSession>>& sessions_; // Набор WebSocket-сессий

};


class HttpServer {
public:
    HttpServer(int port, ConnectionPool& pool, std::set<std::shared_ptr<WebSocketSession>>& sessions);
    void run();
    void stop();
private:
    void do_accept();
    net::io_context io_context_;
    tcp::acceptor acceptor_;
    ConnectionPool& pool_;
    std::set<std::shared_ptr<WebSocketSession>>& sessions_; // Хранение активных WebSocket-сессий
};