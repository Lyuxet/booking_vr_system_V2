#pragma once

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


class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    explicit WebSocketSession(tcp::socket socket, std::set<std::shared_ptr<WebSocketSession>>& sessions);
    void start();
    void send(const std::string& message);
private:
    void on_accept(beast::error_code ec);
    void do_read();
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void on_write(beast::error_code ec, std::size_t);
    void close_session();


    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;
    std::set<std::shared_ptr<WebSocketSession>>& sessions_;
};


class WebSocketServer{
public:
    WebSocketServer(int port, std::set<std::shared_ptr<WebSocketSession>>& sessions);
    void run();

private:
    void do_accept();

    net::io_context io_context_;
    tcp::acceptor acceptor_;
    std::set<std::shared_ptr<WebSocketSession>>& sessions_; // Хранение активных WebSocket-сессий
};