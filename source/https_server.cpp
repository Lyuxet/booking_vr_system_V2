#include "https_server.h"
#include "logger.h"
#include <string>


void HttpSession::start(){
    do_read();
}

void HttpSession::do_read(){
    http::async_read(socket_, buffer_, request_,
    beast::bind_front_handler(&HttpSession::on_read, shared_from_this()));
}
void HttpSession::on_read(beast::error_code ec, std::size_t bytes_transferred){
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        Logger::getInstance().log("Ошибка чтения: " + ec.message() + "(" + std::to_string(ec.value()) + ")" + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
        "../../logs/error_read.log");
        return;
    }

        // Обработка CORS preflight
    if (request_.method() == http::verb::options) {
        handle_options();
        return;
    }

    // Обработка других методов
    if (request_.method() == http::verb::post && request_.target() == "/addBookingOpenArena") {
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
void HttpSession::handle_options(){
    response_.result(http::status::no_content);
    response_.set(http::field::access_control_allow_origin, "*");
    response_.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
    response_.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
    do_write();
}
void HttpSession::handle_add_booking_open_arena(){
    set_cors_headers();

    auto parsed_json = json::parse(request_.body());

    vr::ArenaBookingInsert(parsed_json, response_, pool_);
    std::string place_game = parsed_json.at("placegame").as_string().c_str();
    std::string date_game = parsed_json.at("date").as_string().c_str();
    notify_clients("New booking made in open arena!", place_game, date_game);

    do_write();
}
void HttpSession::handle_add_booking_cubes(){
    set_cors_headers();

    auto parsed_json = json::parse(request_.body());

    vr::CubesBookingInsert(parsed_json, response_, pool_);
    std::string place_game = parsed_json.at("placegame").as_string().c_str();
    std::string date_game = parsed_json.at("date").as_string().c_str();
    notify_clients("New booking made in open arena!", place_game, date_game);

        
    do_write();
}
void HttpSession::notify_clients(const std::string& message, 
                                const std::string& place_game, const std::string& date_game){
    for (const auto& session : sessions_) {
        if (session->GetDate() == date_game && session->GetPlace() == place_game){
            session->send(message);
        }
    }
}
void HttpSession::handle_get_booking_cubes(){
    set_cors_headers();

    vr::Availability(request_, response_, pool_);
    do_write();
}
void HttpSession::handle_get_booking_open_arena(){
    set_cors_headers();

    vr::Availability(request_, response_, pool_);
    do_write();
}
void HttpSession::handle_not_found(){
    response_.result(http::status::not_found);
    response_.set(http::field::content_type, "application/json");
    response_.body() = json::serialize(json::value{
    {"error", "Not Found"}
    });
    response_.prepare_payload();
    do_write();
}
void HttpSession::set_cors_headers(){
    response_.set(http::field::access_control_allow_origin, "*");
    response_.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
    response_.set(http::field::access_control_allow_headers, "Content-Type, Authorization");

}
void HttpSession::do_write(){
    http::async_write(socket_, response_,
    beast::bind_front_handler(&HttpSession::on_write, shared_from_this()));
}
void HttpSession::on_write(beast::error_code ec, std::size_t){
    if (ec) {
        Logger::getInstance().log("Ошибка чтения: " + ec.message() + "(" + std::to_string(ec.value()) + ")" + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
        "../../logs/error_read.log");

        return;
    }
    socket_.shutdown(tcp::socket::shutdown_send, ec);

}


HttpServer::HttpServer(int port, ConnectionPool& pool, std::set<std::shared_ptr<WebSocketSession>>& sessions)
: acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)), pool_(pool), sessions_(sessions){
    do_accept();
}
void HttpServer::run() {
    io_context_.run();
}



void HttpServer::do_accept() {
    acceptor_.async_accept([this](beast::error_code ec, tcp::socket socket) {
        if (!ec) {
            // Создание HTTP-сессии
            auto session = std::make_shared<HttpSession>(std::move(socket), pool_, sessions_);
            session->start();
        } else {
            Logger::getInstance().log("Ошибка чтения: " + ec.message() + "(" + std::to_string(ec.value()) + ")" + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
            "../../logs/error_read.log"); 
        }
        do_accept();
    });
}

