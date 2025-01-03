#include "booking_vr.h"
#include "logger.h"
#include "domain.h"
#include "email.h"

#include <boost/json.hpp>
#include <boost/asio.hpp>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <sstream>
#include <thread>
#include <ctime>
#include <unordered_set>



std::mutex mtxtest;


namespace vr{
    
    void Booking::AddDataByInsertAndUpdate(const Client_data& client, const std::vector<Booking_data>& bookings) {
        clients_ = std::move(client);
        bookings_ = std::move(bookings);
    }
    void Booking::AddDataByDelete(const std::vector<Booking_data>& bookings){
        bookings_ = std::move(bookings);
    }

    void Booking::AddDataByCheckAvailability(const AvailabilityData& data){
        availability_ = std::move(data);
    }

   void Arena::ProcessArenaTransaction(const std::string& operationName) {
    ScopeGuard guard(bookings_, clients_);
    try {
        std::unique_ptr<sql::Connection> conn = pool_.GetConnection();
        ConnectionGuard conn_guard(std::move(conn), pool_);
        executeTransactionInsert(std::move(conn_guard.getConnection()));
        std::lock_guard<std::mutex> lock(mtxtest);
        sendToEmail();
        PrintInsertBooking();
    } catch (const std::exception& e) {
        Logger::getInstance().log(operationName + " Exception: " + std::string(e.what()) +
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../logs/error_transaction.log");
        throw;
    }
}




    void Arena::Open_arena() {
        ProcessArenaTransaction("Открытая игра");
    }

    void Arena::Close_arena() {
        ProcessArenaTransaction("Закрытая игра");
    }

    std::string Arena::initBunnot() {
        try {
            std::unique_ptr<sql::Connection> conn = pool_.GetConnection();
            ConnectionGuard conn_guard(std::move(conn), pool_);
            std::string response;
            executeTransactionInitButtonArena(std::move(conn_guard.getConnection()), response);
            return response;
        } catch (const std::exception& e) {
            Logger::getInstance().log("Avalibality Exception: " + std::string(e.what()) +
                " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
                "../logs/error_transaction.log");
            return "";
        }
    }


    std::string Cubes::initBunnot() {
        ScopeGuard guard(availability_);
        try {
            std::unique_ptr<sql::Connection> conn = pool_.GetConnection();
            ConnectionGuard conn_guard(std::move(conn), pool_);
            std::string response;
            executeTransactionInitButtonCubes(std::move(conn_guard.getConnection()), response);
            return response;
        } catch (const std::exception& e) {
            Logger::getInstance().log("Avalibality Exception: " + std::string(e.what()) +
                " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
                "../logs/error_transaction.log");
            return "";
        }
    }

    void Cubes::Open_cubes() {
        ScopeGuard guard(bookings_, clients_);
        try {
            std::unique_ptr<sql::Connection> conn = pool_.GetConnection();
            ConnectionGuard conn_guard(std::move(conn), pool_);
            executeTransactionInsert(conn_guard.getConnection());
            std::lock_guard<std::mutex> lock(mtxtest);
            sendToEmail();
            PrintInsertBooking();
        } catch (const std::exception& e) {
            Logger::getInstance().log("Кубы Исключение: " + std::string(e.what()) +
                " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
                "../logs/error_transaction.log");
            throw;
        }
    }


    void Booking::handleSQLException(const sql::SQLException& e, int attempt, int max_retries, int base_retry_delay_ms, sql::Connection* conn) {
        Logger::getInstance().log("Attempt " + std::to_string(attempt + 1) + " - SQLException: " + std::string(e.what()) +
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../logs/error_transaction.log");
        Logger::getInstance().log("Error code: " + std::to_string(e.getErrorCode()) + ", SQLState: " + e.getSQLState() +
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../logs/error_transaction.log");

        if (e.getErrorCode() == 1213) { // Deadlock
            if (attempt < max_retries - 1) {
                int delay = base_retry_delay_ms * (attempt + 1) + rand() % 1000;
                Logger::getInstance().log("Retrying after " + std::to_string(delay) + " ms", "../logs/error_transaction.log");
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            } else {
                conn->rollback();
                throw;
            }
        } else {
            conn->rollback();
            throw;
        }
    }

    void Booking::handleStdException(const std::exception& e, sql::Connection* conn) {
        Logger::getInstance().log("Исключение: " + std::string(e.what()), "../logs/error_transaction.log");
        conn->rollback();
        throw;
    }


   bool Booking::checkAvailableSlots(sql::Connection* conn, const Booking_data& booking) {
        std::unique_ptr<sql::PreparedStatement> pstmtCheck;
        if (booking.place_game == "VR Арена") {
            pstmtCheck.reset(conn->prepareStatement(
                "SELECT time_game, SUM(players_count) AS total_players, "
                "CASE "
                "    WHEN COUNT(DISTINCT name_game) = 1 AND MAX(name_game) = ? THEN (10 - SUM(players_count)) "
                "    ELSE 0 "
                "END AS available_slots "
                "FROM GameSchedule "
                "WHERE place_game = ? AND date_game = ? AND time_game = ? "
                "GROUP BY time_game"
            ));
            pstmtCheck->setString(1, booking.name_game);
            pstmtCheck->setString(2, booking.place_game);
            pstmtCheck->setString(3, booking.date_game);
            pstmtCheck->setString(4, booking.time_game);
        } else if (booking.place_game == "VR Кубы") {
            pstmtCheck.reset(conn->prepareStatement(
                "SELECT time_game, SUM(players_count) AS total_players, "
                "CASE "
                "    WHEN COUNT(DISTINCT name_game) = 1 AND MAX(name_game) = ? THEN (4 - SUM(players_count)) "
                "    ELSE 0 "
                "END AS available_slots "
                "FROM Cubes "
                "WHERE place_game = ? AND date_game = ? AND time_game = ? "
                "GROUP BY time_game"
            ));
            pstmtCheck->setString(1, booking.name_game);
            pstmtCheck->setString(2, booking.place_game);
            pstmtCheck->setString(3, booking.date_game);
            pstmtCheck->setString(4, booking.time_game);
        } else {
            throw std::runtime_error("Неизвестная площадка игры");
        }

        std::unique_ptr<sql::ResultSet> res(pstmtCheck->executeQuery());
        if (res->next()) {
            int available_slots = res->getInt("available_slots");
            if (available_slots < booking.players_count) {
                throw std::runtime_error("Недостаточно свободных мест для бронирования");
            }
        }
        return true;
    }




   void Booking::insertClient(sql::Connection* conn) {
        std::unique_ptr<sql::PreparedStatement> checkStmt(conn->prepareStatement(
            "SELECT id FROM Clients WHERE phone = ? AND email = ?"
        ));
        checkStmt->setString(1, clients_.phone);
        checkStmt->setString(2, clients_.email);
        std::unique_ptr<sql::ResultSet> res(checkStmt->executeQuery());

        if (res->next()) {
            return;
        } else {
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "INSERT INTO Clients (first_name, last_name, phone, email) VALUES (?, ?, ?, ?)"
            ));
            pstmt->setString(1, clients_.first_name);
            pstmt->setString(2, clients_.last_name);
            pstmt->setString(3, clients_.phone); 
            pstmt->setString(4, clients_.email);
            pstmt->executeUpdate();
        }
    }





    void Booking::executeTransactionInsert(sql::Connection* conn) {
        executeTransactionWithRetry(std::move(conn), [&](sql::Connection* conn) {
            conn->setAutoCommit(false); // Начинаем транзакцию
            
            std::string tableName;  
            if (gameTables.find(bookings_[0].name_game) != gameTables.end()) {
                tableName = gameTables.at(bookings_[0].name_game);
            } else {
                throw std::runtime_error("Неизвестное имя игры");
            }

            
            insertClient(std::move(conn));
        
            
            std::string queryInsert = "INSERT INTO " + tableName +
                " (client_id, place_game, name_game, type_game, date_game, time_game, players_count, price, comment_game, who_reservation, book_status) " +
                "VALUES ((SELECT id FROM Clients WHERE phone = ? LIMIT 1), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
            std::unique_ptr<sql::PreparedStatement> pstmt2(conn->prepareStatement(queryInsert));

           
            for (const auto& booking : bookings_) {
                
                if (!checkAvailableSlots(conn, booking)) {
                    throw std::runtime_error("Недостаточно свободных мест для бронирования");
                }
                pstmt2->setString(1, clients_.phone);
                pstmt2->setString(2, booking.place_game);
                pstmt2->setString(3, booking.name_game);
                pstmt2->setString(4, booking.type_game);
                pstmt2->setString(5, booking.date_game);
                pstmt2->setString(6, booking.time_game);
                pstmt2->setInt(7, booking.players_count);
                pstmt2->setInt(8, booking.price);
                pstmt2->setString(9, booking.comment_game);
                pstmt2->setString(10, booking.who_reservation);
                pstmt2->setString(11, booking.book_status);
                pstmt2->execute();
            }
        });
    }

    std::string Booking::urlEncode(const std::string &value) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (char c : value) {
            
            if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            } else {
                escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
            }
        }

        return escaped.str();
    }


    void Booking::checkButtonDataArena(sql::Connection* conn){
        std::unique_ptr<sql::PreparedStatement> pstmtIninButton;
        std::string request_init_button;


        int year = std::stoi(availability_.date.substr(0, 4));
        int month = std::stoi(availability_.date.substr(5, 2));
        int day = std::stoi(availability_.date.substr(8, 2));  

    
        if (availability_.typegame == "Открытая игра") {
            request_init_button = "SELECT * FROM OpenArenaButtonData WHERE weekday = " + std::to_string(getZellerDayOfWeek(year, month, day));
        } else if (availability_.typegame == "Закрытая игра") {
            request_init_button = "SELECT * FROM CloseArenaButtonData WHERE weekday = " + std::to_string(getZellerDayOfWeek(year, month, day));
        } else {
            throw std::runtime_error("Undefined type game");
        }

        pstmtIninButton = std::unique_ptr<sql::PreparedStatement>(conn->prepareStatement(request_init_button));
        std::unique_ptr<sql::ResultSet> res_set_data(pstmtIninButton->executeQuery());
                
        while (res_set_data->next()) {
            int price = IsHolidayOrWeekend(availability_.date)
                ? res_set_data->getInt("button_price_holidays")
                : res_set_data->getInt("button_price");

            button_data_.emplace(
                res_set_data->getString("button_id"),
                ButtonData{ res_set_data->getString("button_time"), price }
            );
        }
    }

    void Booking::checkButtonDataCubes(sql::Connection* conn){

        
        int year = std::stoi(availability_.date.substr(0, 4));
        int month = std::stoi(availability_.date.substr(5, 2));
        int day = std::stoi(availability_.date.substr(8, 2)); 


        std::unique_ptr<sql::PreparedStatement> pstmtIninButton(conn->prepareStatement("SELECT * FROM CubesButtonData WHERE weekday = " 
                                                                + std::to_string(getZellerDayOfWeek(year, month, day))));
        std::unique_ptr<sql::ResultSet> res_set_data(pstmtIninButton->executeQuery());
                
        while (res_set_data->next()) {
            int price = IsHolidayOrWeekend(availability_.date)
                ? res_set_data->getInt("button_price_holidays")
                : res_set_data->getInt("button_price");

            button_data_.emplace(
                res_set_data->getString("button_id"),
                ButtonData{ res_set_data->getString("button_time"), price }
            );
        }
    }

    std::unordered_map<std::string, int> Booking::checkAvailabilityPlaceArena(sql::Connection* conn){
        std::unique_ptr<sql::PreparedStatement> pstmtCheckAvailability(conn->prepareStatement(
            "SELECT time_game, SUM(players_count) AS total_players, "
            "CASE "
            "    WHEN COUNT(DISTINCT name_game) = 1 AND MAX(name_game) = ? THEN (10 - SUM(players_count)) "
            "    ELSE 0 "
            "END AS available_slots "
            "FROM GameSchedule "
            "WHERE place_game = ? AND date_game = ? "
            "GROUP BY time_game"
        ));
        pstmtCheckAvailability->setString(1, availability_.namegame);
        pstmtCheckAvailability->setString(2, availability_.placegame);
        pstmtCheckAvailability->setString(3, availability_.date);
        std::unique_ptr<sql::ResultSet> res_set_avaliability(pstmtCheckAvailability->executeQuery());

        std::unordered_map<std::string, int> availability_map;
        while (res_set_avaliability->next()) {
            std::string time_game = res_set_avaliability->getString("time_game");
            int available_slots = res_set_avaliability->getInt("available_slots");

            availability_map[time_game] = available_slots;
        }
        return std::move(availability_map);
    }


    std::unordered_map<std::string, int> Booking::checkAvailabilityPlaceCubes(sql::Connection* conn){
        std::unique_ptr<sql::PreparedStatement> pstmtCheckAvailability(conn->prepareStatement(
            "SELECT time_game, SUM(players_count) AS total_players, "
            "CASE "
            "    WHEN COUNT(DISTINCT name_game) = 1 AND MAX(name_game) = ? THEN (4 - SUM(players_count)) "
            "    ELSE 0 "
            "END AS available_slots "
            "FROM Cubes "
            "WHERE place_game = ? AND date_game = ? "
            "GROUP BY time_game"
        ));
        pstmtCheckAvailability->setString(1, availability_.namegame);
        pstmtCheckAvailability->setString(2, availability_.placegame);
        pstmtCheckAvailability->setString(3, availability_.date);
        std::unique_ptr<sql::ResultSet> res_set_avaliability(pstmtCheckAvailability->executeQuery());

        std::unordered_map<std::string, int> availability_map;
        while (res_set_avaliability->next()) {
            std::string time_game = res_set_avaliability->getString("time_game");
            int available_slots = res_set_avaliability->getInt("available_slots");

            availability_map[time_game] = available_slots;
        }
        return std::move(availability_map);
    }


    void Booking::executeTransactionInitButtonArena(sql::Connection* conn, std::string& response) {
       
            executeTransactionWithRetry(conn, [&](sql::Connection* conn) {
                conn->setAutoCommit(false);

                checkButtonDataArena(conn);
                
                std::unordered_map<std::string, int> availability_map = checkAvailabilityPlaceArena(conn);

                boost::json::object response_data;
                for (const auto& item : button_data_) {
                    ButtonData button = item.second;
                    if (availability_map.find(button.time) != availability_map.end()) {
                        if (availability_.typegame == "Закрытая игра"){
                            button.availability_place = 0;   
                        }
                        else{
                            button.availability_place = availability_map[button.time];
                        }
                    } else {
                        button.availability_place = 10; // Все места свободны
                    }

                    response_data[item.first] = boost::json::object{
                        {"time", button.time},
                        {"price", button.price},
                        {"availability_place", button.availability_place}
                    };
                }
                response = boost::json::serialize(response_data); 
            });
    }

   void Booking::executeTransactionInitButtonCubes(sql::Connection* conn, std::string& response) {
       
        executeTransactionWithRetry(conn, [&](sql::Connection* conn) {
                conn->setAutoCommit(false);

                checkButtonDataCubes(conn);
                
                std::unordered_map<std::string, int> availability_map = checkAvailabilityPlaceCubes(conn);
                boost::json::object response_data;
                for (const auto& item : button_data_) {
                    ButtonData button = item.second;
                    if (availability_map.find(button.time) != availability_map.end()) {
                        button.availability_place = availability_map[button.time];
                    } else {
                        button.availability_place = 4; // Все места свободны
                    }

                    // Добавление данных в response_data с использованием ключей button11, button12 и т.д.
                    response_data[item.first] = boost::json::object{
                        {"time", button.time},
                        {"price", button.price},
                        {"availability_place", button.availability_place}
                    };
                }
                response = boost::json::serialize(response_data);
            });
        
    }



    std::unordered_map<std::string, std::string> Booking::LoadGameTables(const std::string& filename){
        std::unordered_map<std::string, std::string> gameTableMap;
        std::fstream file(filename);
        if (!file.is_open()){
            throw std::runtime_error("Не удалось открыть файл чтения игр");
        }
        std::string line;
        while(std::getline(file, line)){
            std::istringstream iss(line);
            std::string gameName, tableName;
            if (std::getline(iss, gameName, ':') && std::getline(iss, tableName)) {
                gameTableMap[gameName] = tableName;
            }
        }

        file.close();
        return gameTableMap;

    }

    std::string  Booking::generate_email_body(const info_by_email& email_data) {
        std::ostringstream html;

        html << "<!DOCTYPE html>\n";
        html << "<html lang=\"ru\">\n";
        html << "<head>\n";
        html << "    <meta charset=\"UTF-8\">\n";
        html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        html << "    <title>Новое бронирование</title>\n";
        html << "    <style>\n";
        html << "        table {\n";
        html << "            width: 100%;\n";
        html << "            border-collapse: collapse;\n";
        html << "        }\n";
        html << "        table, th, td {\n";
        html << "            border: 1px solid black;\n";
        html << "        }\n";
        html << "        th, td {\n";
        html << "            padding: 10px;\n";
        html << "            text-align: left;\n";
        html << "            white-space: nowrap;\n";  // Добавлено свойство для предотвращения переноса текста
        html << "            overflow: hidden;\n";    // Добавлено свойство для предотвращения переноса текста
        html << "            text-overflow: ellipsis;\n"; // Обрезка текста с добавлением многоточия
        html << "        }\n";
        html << "        th {\n";
        html << "            background-color: #f2f2f2;\n";
        html << "        }\n";
        html << "        td {\n";
        html << "            background-color: #e6f7ff;\n";  // Добавлено цвет фона для ячеек
        html << "        }\n";
        html << "    </style>\n";
        html << "</head>\n";
        html << "<body>\n";
        html << "    <h2>Детали бронирования</h2>\n";
        html << "    <table>\n";
        html << "        <tr>\n";
        html << "            <th>Игровая площадка</th>\n";
        html << "            <td><b>" << email_data.place_game << "</b></td>\n";
        html << "        </tr>\n";  
        html << "        <tr>\n";
        html << "            <th>Тип игры</th>\n";
        html << "            <td><b>" << email_data.type_game << "</b></td>\n";
        html << "        </tr>\n";
        html << "        <tr>\n";
        html << "            <th>Название игры</th>\n";
        html << "            <td><b>" << email_data.name_game << "</b></td>\n";
        html << "        </tr>\n";
        html << "        <tr>\n";
        html << "            <th>Дата</th>\n";
        html << "            <td><b>" << convertDate(email_data.date_game) << "</b></td>\n";
        html << "        </tr>\n";
        html << "        <tr>\n";
        html << "            <th>Время игры</th>\n";
        html << "            <td><b>" << email_data.times_game_and_players_count << "</b></td>\n";
        html << "        </tr>\n";
        html << "        <tr>\n";
        html << "            <th>Стоимость</th>\n";
        html << "            <td><b>" << email_data.price << " RUB</b></td>\n";
        html << "        </tr>\n";
        html << "        <tr>\n";
        html << "            <th>Имя</th>\n";
        html << "            <td>" << email_data.client_first_name << "</td>\n";
        html << "        </tr>\n";
        html << "        <tr>\n";
        html << "            <th>Фамилия</th>\n";
        html << "            <td>" << email_data.client_last_name << "</td>\n";
        html << "        </tr>\n";
        html << "        <tr>\n";
        html << "            <th>Телефон</th>\n";
        html << "            <td>" << email_data.client_phone << "</td>\n";
        html << "        </tr>\n";
        html << "        <tr>\n";
        html << "            <th>E-mail</th>\n";
        html << "            <td>" << email_data.client_email << "</td>\n";
        html << "        </tr>\n";
        html << "        <tr>\n";
        html << "            <th>Комментарий</th>\n";
        html << "            <td>" << email_data.comment_game << "</td>\n";
        html << "        </tr>\n";
        html << "        <tr>\n";
        html << "            <th>Статус</th>\n";
        html << "            <td><b>" << email_data.status_book << "</b></td>\n";
        html << "        </tr>\n";
        html << "    </table>\n";
        html << "</body>\n";
        html << "</html>";

        return html.str();
    }


    void Booking::sendToEmail(){

        auto email_task = std::async(std::launch::async, [this]() {
            std::shared_ptr<AsyncEmailSender> email_sender;
            std::string times_game_and_players_count;
            int price = 0;
            

            for (const auto& booking : bookings_){
                times_game_and_players_count += booking.time_game + " (";
                times_game_and_players_count += booking.type_game == "Закрытая игра" ? "до 10 игроков)<br>" 
                                                : "игроков " + std::to_string(booking.players_count) + ")<br>";
                price += booking.price;
            }

            info_by_email email_data{bookings_[0].place_game, bookings_[0].type_game,
                               bookings_[0].name_game, bookings_[0].date_game,
                               times_game_and_players_count, price, 
                               clients_.first_name, clients_.last_name,
                               clients_.phone, clients_.email, 
                               bookings_[0].comment_game, bookings_[0].book_status};

            email_sender = std::make_shared<AsyncEmailSender>();

            if(!clients_.email.empty()){
                email_sender->add_recipient(clients_.email);
            }
            std::string body{generate_email_body(email_data)};
            

            std::string subject = "Уведомление о заказе https://vr-real.ru";
            email_sender->send(subject, body);
        });

    }


    bool Booking::IsHolidayOrWeekend(const std::string& date) {
        // Парсим дату из строки "ГГГГ.ММ.ДД"
        std::tm parsed_date = {};
        std::istringstream ss(date);
        ss >> std::get_time(&parsed_date, "%Y.%m.%d");

        // Проверяем корректность парсинга
        if (ss.fail()) {
            throw std::invalid_argument("Неверный формат даты. Ожидается 'ГГГГ.ММ.ДД'");
        }

        // Список праздничных дней (формат ММДД, например, 0101 для 1 января)
        std::unordered_set<int> holidays = {
            101,   // 1 января
            1225,  // 25 декабря
            // Добавьте другие праздники в формате ММДД
        };

        // Проверка на выходной день (суббота = 6, воскресенье = 0)
        if (parsed_date.tm_wday == 0 || parsed_date.tm_wday == 6) {
            return true;
        }

        // Создаем код для даты в формате ММДД
        int date_code = (parsed_date.tm_mon + 1) * 100 + parsed_date.tm_mday;

        // Проверка на праздничный день
        return holidays.count(date_code) > 0;
    }

    std::string Booking::GetAdminBooking(std::string& date, std::string& place_game) {
        ScopeGuard guard(find_data_);
        try {
            std::unique_ptr<sql::Connection> conn = pool_.GetConnection();
            ConnectionGuard conn_guard(std::move(conn), pool_);
            std::string response;
            GetAdminBooking(std::move(conn_guard.getConnection()), date, place_game, response);
            return response;
        } catch (const std::exception& e) {
            Logger::getInstance().log("Admin Booking Exception: " + std::string(e.what()) +
                " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
                "../logs/error_transaction.log");
            return "";
        }
    }

    void Booking::GetAdminBooking(sql::Connection* conn, std::string& date, std::string& place_game, std::string& response) {
        executeTransactionWithRetry(conn, [&](sql::Connection* conn) {
            conn->setAutoCommit(false);

            std::string year = date.substr(0, 4);
            std::string day = date.substr(8, 2);
            std::string month = date.substr(5, 2);

            
            std::string query = day == "00" ?
                "SELECT g.*, c.first_name, c.last_name, c.phone, c.email "
                "FROM GameSchedule g "
                "LEFT JOIN Clients c ON g.client_id = c.id "
                "WHERE g.place_game = ? AND MONTH(g.date_game) = ?" :
                "SELECT g.*, c.first_name, c.last_name, c.phone, c.email "
                "FROM GameSchedule g "
                "LEFT JOIN Clients c ON g.client_id = c.id "
                "WHERE g.place_game = ? AND DATE(g.date_game) = ?";

            std::unique_ptr<sql::PreparedStatement> get_admin_booking(conn->prepareStatement(query));
            get_admin_booking->setString(1, place_game);
            if (day == "00") {
                get_admin_booking->setString(2, month);
            } else {
                get_admin_booking->setString(2, date);  // Use full date when day is specified
            }

            std::unique_ptr<sql::ResultSet> res_set_admin_booking(get_admin_booking->executeQuery());
            
            std::unordered_map<BookingKey, boost::json::object, BookingKeyHash, BookingKeyEqual> merged_bookings;

            while (res_set_admin_booking->next()) {
                BookingKey key{
                    res_set_admin_booking->getString("date_game"),
                    res_set_admin_booking->getString("time_game"),
                    res_set_admin_booking->getInt("client_id"),
                    res_set_admin_booking->getString("place_game")
                };

                auto it = merged_bookings.find(key);
                if (it == merged_bookings.end()) {
                    // Создаем новую запись
                    boost::json::object booking_data;
                    booking_data["place_game"] = res_set_admin_booking->getString("place_game").c_str();
                    booking_data["name_game"] = res_set_admin_booking->getString("name_game").c_str();
                    booking_data["type_game"] = res_set_admin_booking->getString("type_game").c_str();
                    booking_data["date_game"] = convertDate(key.date_game.c_str());
                    booking_data["time_game"] = key.time_game.c_str();
                    booking_data["players_count"] = res_set_admin_booking->getInt("players_count");
                    booking_data["price"] = res_set_admin_booking->getInt("price");
                    booking_data["client_name"] = res_set_admin_booking->getString("first_name").c_str();
                    booking_data["client_phone"] = res_set_admin_booking->getString("phone").c_str();
                    booking_data["client_email"] = res_set_admin_booking->getString("email").c_str();
                    booking_data["book_status"] = res_set_admin_booking->getString("book_status").c_str();
                    booking_data["who_reservation"] = res_set_admin_booking->getString("who_reservation").c_str();
                    booking_data["date_add_book"] = res_set_admin_booking->getString("date_add_book").c_str();

                    merged_bookings[key] = booking_data;
                } else {
                    // Обновляем существующую запись
                    it->second["players_count"] = it->second["players_count"].as_int64() + 
                                                res_set_admin_booking->getInt("players_count");
                    it->second["price"] = it->second["price"].as_int64() + 
                                        res_set_admin_booking->getInt("price");
                }
            }

            // Преобразуем объединенные бронирования в финальный ответ
            boost::json::array bookings_array;
            for (const auto& booking : merged_bookings) {
                bookings_array.push_back(booking.second);
            }
            
            response = boost::json::serialize(bookings_array);
            conn->commit();
        });
    }

}