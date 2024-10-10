#include "websocket.h"
#include "logger.h"


WebSocketSession::WebSocketSession(tcp::socket socket, std::set<std::shared_ptr<WebSocketSession>>& sessions)
: ws_(std::move(socket)), sessions_(sessions) {}

void WebSocketSession::start() {
    ws_.async_accept(beast::bind_front_handler(&WebSocketSession::on_accept, shared_from_this()));
}

void WebSocketSession::send(const std::string& message) {
    ws_.async_write(net::buffer(message),
        beast::bind_front_handler(&WebSocketSession::on_write, shared_from_this()));
}


void WebSocketSession::on_accept(beast::error_code ec) {
    if (ec) {
        Logger::getInstance().log("Ошибка при принятии: " + ec.message() + "(" + std::to_string(ec.value()) + ")" + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../../logs/error_accept.log");
        return;
    }
    do_read();
}

void WebSocketSession::do_read() {
    ws_.async_read(buffer_,
        beast::bind_front_handler(&WebSocketSession::on_read, shared_from_this()));
}

void WebSocketSession::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec == websocket::error::closed) {
        close_session();  // Закрываем и удаляем сессию
        return;
    } else if (ec) {
        Logger::getInstance().log("Ошибка при чтении: " + ec.message() + "(" + std::to_string(ec.value()) + ")" + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../../logs/error_read.log");        
        close_session();  // При ошибке также удаляем сессию
        return;
    }
    do_read();
}

void WebSocketSession::on_write(beast::error_code ec, std::size_t) {
    if (ec) {
        Logger::getInstance().log("Ошибка при записи: " + ec.message() + "(" + std::to_string(ec.value()) + ")" + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../../logs/error_write.log");    }
}

void WebSocketSession::close_session() {

    if (ws_.is_open()) {
        beast::error_code ec;
        ws_.close(websocket::close_code::normal, ec);
    } 

    sessions_.erase(shared_from_this());
}



WebSocketServer::WebSocketServer(int port, std::set<std::shared_ptr<WebSocketSession>>& sessions)
    : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)), sessions_(sessions) {
    do_accept();
}

void WebSocketServer::run() {
        io_context_.run();
    }


void WebSocketServer::do_accept() {
        acceptor_.async_accept([this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                auto ws_session = std::make_shared<WebSocketSession>(std::move(socket), sessions_);
                ws_session->start();
                sessions_.insert(ws_session); // Добавление сессии в набор
            } else {
                Logger::getInstance().log("Ошибка при принятии сесии: " + ec.message() + "(" + std::to_string(ec.value()) + ")" + 
                " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
                "../../logs/error_accept.log");            
            }
            do_accept();
        });
    }   