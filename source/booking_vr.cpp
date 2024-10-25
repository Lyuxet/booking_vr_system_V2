#include "booking_vr.h"
#include "logger.h"
#include <boost/json.hpp>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <sstream>

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
            std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
            ConnectionGuard conn_guard(conn, pool_);
            
            executeTransactionInsert(conn);

            std::lock_guard<std::mutex> lock(mtxtest);
            PrintInsertBooking();
        } catch (const std::exception& e) {
            std::lock_guard<std::mutex> lock(mtxtest);
            std::cerr << operationName << " Exception: " << e.what() << std::endl;
            Logger::getInstance().log(operationName + " Exeption: " + std::string(e.what()) + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
            "../../logs/error_transaction.log");
            throw;
        }
    }
    void Arena::Open_arena() {
        ProcessArenaTransaction("Open_Arena");
    }

    void Arena::Close_arena() {
        ProcessArenaTransaction("Close_Arena");
    }

    std::string Arena::CheckAvailabilityPlace(){
        ScopeGuard guard(availability_);
        try
        {
            std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
            ConnectionGuard conn_guard(conn, pool_);
            std::string response;
            executeTransactionCheckAvailabilityArena(conn, response);
            return response;

        }
        catch(const std::exception& e)
        {
            Logger::getInstance().log(" Avalibality Exeption: " + std::string(e.what()) + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
            "../../logs/error_transaction.log");
            return "";
        }
        
    }

    std::string Cubes::CheckAvailabilityPlace(){
        ScopeGuard guard(availability_);
        try
        {
            std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
            ConnectionGuard conn_guard(conn, pool_);
            std::string response;
            executeTransactionCheckAvailabilityCubes(conn, response);
            return response;

        }
        catch(const std::exception& e)
        {
            Logger::getInstance().log("Avalibality Exeption: " + std::string(e.what()) + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
            "../../logs/error_transaction.log");
            return "";
        }
        
    }
    /*
    void Booking::Update(){
        ScopeGuard guard(booking_);
        try
        {
        std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
        ConnectionGuard conn_guard(conn, pool_);
            executeTransactionUpdate(conn);
            std::lock_guard<std::mutex> lock(mtxtest);
            PrintUpdateBooking();
        }
        catch(const std::exception& e)
        {
            std::lock_guard<std::mutex> lock(mtxtest);
            std::cerr << "Update arena Exception: " << e.what() << std::endl;
            throw;
        }
    }

    void Booking::Delete(){
        ScopeGuard guard(booking_);
        try
        {
        std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
        ConnectionGuard conn_guard(conn, pool_);
            executeTransactionDelete(conn);
            std::lock_guard<std::mutex> lock(mtxtest);
            PrintDeleteBooking();
        }
        catch(const std::exception& e)
        {
            std::lock_guard<std::mutex> lock(mtxtest);
            std::cerr << "Delete Exception: " << e.what() << std::endl;
            throw;
        }
        
    }
    */
    void Cubes::Open_cubes() {
        ScopeGuard guard(bookings_, clients_);
        try {
            std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
            ConnectionGuard conn_guard(conn, pool_);
            executeTransactionInsert(conn);
            std::lock_guard<std::mutex> lock(mtxtest);
            PrintInsertBooking();
            
        } catch (const std::exception& e) {
            std::lock_guard<std::mutex> lock(mtxtest);
            Logger::getInstance().log("Cubes Exeption: " + std::string(e.what()) + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__), 
            "../../logs/error_transaction.log");
            throw;
        }
    }

    void Booking::handleSQLException(const sql::SQLException& e, int attempt, int max_retries, int base_retry_delay_ms, std::shared_ptr<sql::Connection> conn) {
        Logger::getInstance().log("Attempt " + std::to_string(attempt + 1) + " - SQLException: " + std::string(e.what()) + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../../logs/error_transaction.log");
        Logger::getInstance().log("Error code: " + std::to_string(e.getErrorCode()) + ", SQLState: " + e.getSQLState() + 
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../../logs/error_transaction.log");

        if (e.getErrorCode() == 1213) { // Deadlock
            if (attempt < max_retries - 1) {
                int delay = base_retry_delay_ms * (attempt + 1) + rand() % 1000;
                Logger::getInstance().log("Retrying after " + std::to_string(delay) + " ms", "../../logs/error_transaction.log");
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

    void Booking::handleStdException(const std::exception& e, std::shared_ptr<sql::Connection> conn) {
        std::cerr << "Exception: " << e.what() << std::endl;
        Logger::getInstance().log("Exception: " + std::string(e.what()), "../../logs/error_transaction.log");
        conn->rollback();
        throw;
    }

   bool Booking::checkAvailableSlots(std::shared_ptr<sql::Connection> conn, const Booking_data& booking) {
        std::unique_ptr<sql::PreparedStatement> pstmtCheck;

        if (booking.place_game == "ARENA") {
            pstmtCheck.reset(conn->prepareStatement(
                "SELECT time_game, SUM(players_count) AS total_players, "
                "CASE "
                "    WHEN COUNT(DISTINCT name_game) = 1 AND MAX(name_game) = ? THEN (10 - SUM(players_count)) "
                "    ELSE 0 "
                "END AS available_slots "
                "FROM gameschedule "
                "WHERE place_game = ? AND date_game = ? AND time_game = ? "
                "GROUP BY time_game"
            ));
            pstmtCheck->setString(1, booking.name_game);
            pstmtCheck->setString(2, booking.place_game);  // 'ARENA'
            pstmtCheck->setString(3, booking.date_game);   // Дата игры
            pstmtCheck->setString(4, booking.time_game);   // Время игры
        } 
        else if (booking.place_game == "CUBES") {
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
            pstmtCheck->setString(2, booking.place_game);  // 'CUBES'
            pstmtCheck->setString(3, booking.date_game);   // Дата игры
            pstmtCheck->setString(4, booking.time_game);   // Время игры
        } 
        else {
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


    void Booking::insertClient(std::shared_ptr<sql::Connection> conn) {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT IGNORE INTO Clients (first_name, last_name, phone, email) VALUES (?, ?, ?, ?)"
        ));
        pstmt->setString(1, clients_.first_name);
        pstmt->setString(2, clients_.last_name);
        pstmt->setString(3, clients_.phone);
        pstmt->setString(4, clients_.email);
        pstmt->execute();
    }



    void Booking::executeTransactionInsert(std::shared_ptr<sql::Connection> conn) {
    executeTransactionWithRetry(conn, [&](std::shared_ptr<sql::Connection> conn) {
        
        conn->setAutoCommit(false); // Начинаем транзакцию

        // Определение таблицы в зависимости от типа игры
        std::string tableName;
        if (gameTables.find(bookings_[0].name_game) != gameTables.end()){
            tableName = gameTables.at(bookings_[0].name_game);
        }
        else{
            throw std::runtime_error("Неизвестный тип игры");
        }

        // Добавление клиента
        insertClient(conn);

        // Подготовка запроса на вставку данных о бронировании
        std::string queryInsert = "INSERT INTO " + tableName +
            " (client_id, place_game, name_game, type_game, date_game, time_game, players_count, price, comment_game, who_reservation, book_status) "
            "VALUES ((SELECT id FROM Clients WHERE phone = ? LIMIT 1), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

        std::unique_ptr<sql::PreparedStatement> pstmt2(conn->prepareStatement(queryInsert));

            // Вставка всех данных из массива
        for (const auto& booking : bookings_) {
            // Проверка наличия доступных мест
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



    /*
    void Booking::executeTransactionDelete(std::shared_ptr<sql::Connection> conn) {
        const int max_retries = 5;
        const int base_retry_delay_ms = 500;
        std::string tableName;

        // Определение таблицы в зависимости от типа игры
        if (booking_.name_game == "SHOOTER") {
            tableName = "ArenaShooterStats";
        } else if (booking_.name_game == "QUEST") {
            tableName = "ArenaQuestStats";
        } else if (booking_.name_game == "CUBES") {
            tableName = "Cubes";
        } else {
            throw std::runtime_error("Unknown game type.");
        }

        for (int attempt = 0; attempt < max_retries; ++attempt) {
            try {
                conn->setAutoCommit(false);

                // Проверка на наличие уже забронированного слота
                std::string queryCheck = "SELECT 1 FROM " + tableName + " WHERE name_game = ? AND date_game = ? AND time_game = ?";
                std::unique_ptr<sql::PreparedStatement> pstmtCheck(conn->prepareStatement(queryCheck));
                pstmtCheck->setString(1, booking_.name_game);
                pstmtCheck->setString(2, booking_.date_game);
                pstmtCheck->setString(3, booking_.time_game);
                std::unique_ptr<sql::ResultSet> res(pstmtCheck->executeQuery());

                if (!res->next()) {
                    std::string error_message = booking_.name_game + " with time " + booking_.time_game + " not found"; 
                    throw std::runtime_error(error_message);
                }

                // Удаление данных из соответствующей таблицы
                std::string queryDelete = "DELETE FROM " + tableName + " WHERE name_game = ? AND date_game = ? AND time_game = ?";
                std::unique_ptr<sql::PreparedStatement> pstmtDelete(conn->prepareStatement(queryDelete));
                pstmtDelete->setString(1, booking_.name_game);
                pstmtDelete->setString(2, booking_.date_game);
                pstmtDelete->setString(3, booking_.time_game);
                pstmtDelete->execute();

                // Удаление данных о бронировании из таблицы Bookings
                std::unique_ptr<sql::PreparedStatement> pstmtBookings(conn->prepareStatement(
                    "DELETE FROM Bookings "
                    "WHERE client_id = (SELECT id FROM Clients WHERE phone = ? LIMIT 1) "
                    "AND game_id = (SELECT id FROM GameSchedule WHERE name_game = ? AND date_game = ? AND time_game = ? LIMIT 1)"
                ));
                pstmtBookings->setString(1, clients_.phone);
                pstmtBookings->setString(2, booking_.name_game);
                pstmtBookings->setString(3, booking_.date_game);
                pstmtBookings->setString(4, booking_.time_game);
                pstmtBookings->execute();

                conn->commit();
                return; 
            } catch (const sql::SQLException& e) {
                std::cerr << "Attempt " << (attempt + 1) << " - SQLException: " << e.what() << std::endl;
                std::cerr << "Error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << std::endl;

                if (e.getErrorCode() == 1213) { // Deadlock
                    if (attempt < max_retries - 1) {
                        int delay = base_retry_delay_ms * (attempt + 1) + rand() % 1000;
                        std::cerr << "Retrying after " << delay << " ms" << std::endl;
                        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                    } else {
                        conn->rollback();
                        throw;
                    }
                } else {
                    conn->rollback();
                    throw;
                }
            } catch (const std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
                conn->rollback();
                throw;
            }
        }
    }

    void Booking::executeTransactionUpdate(std::shared_ptr<sql::Connection> conn){
        const int max_retries = 5;
        const int base_retry_delay_ms = 500;
        std::string tableName;

        // Определение таблицы в зависимости от типа игры
        if (booking_.name_game == "SHOOTER") {
            tableName = "ArenaShooterStats";
        } else if (booking_.name_game == "QUEST") {
            tableName = "ArenaQuestStats";
        } else if (booking_.name_game == "CUBES") {
            tableName = "Cubes";
        } else {
            throw std::runtime_error("Unknown game type.");
        }

        for (int attempt = 0; attempt < max_retries; ++attempt) {
            try {
                conn->setAutoCommit(false);

                // Проверка на наличие записи для обновления по текущим времени и дате
                std::string queryCheck = "SELECT 1 FROM " + tableName + " WHERE name_game = ? AND date_game = ? AND time_game = ?";
                std::unique_ptr<sql::PreparedStatement> pstmtCheck(conn->prepareStatement(queryCheck));
                pstmtCheck->setString(1, booking_.name_game);
                pstmtCheck->setString(2, booking_.current_date_game);  // Текущая дата
                pstmtCheck->setString(3, booking_.current_time_game);  // Текущее время
                std::unique_ptr<sql::ResultSet> res(pstmtCheck->executeQuery());

                if (!res->next()) {
                    std::string error_message = booking_.name_game + " with time " + booking_.current_time_game + " and date " + booking_.current_date_game + " not found";
                    throw std::runtime_error(error_message);
                }

                if (clients_.current_phone != clients_.phone){
                    // Обновление данных о клиенте (если нужно обновить данные клиента)
                    std::unique_ptr<sql::PreparedStatement> pstmtUpdateClient(conn->prepareStatement(
                        "UPDATE Clients SET first_name = ?, last_name = ?, email = ?, phone = ? WHERE phone = ?"
                    ));
                    pstmtUpdateClient->setString(1, clients_.first_name);
                    pstmtUpdateClient->setString(2, clients_.last_name);
                    pstmtUpdateClient->setString(3, clients_.email);
                    pstmtUpdateClient->setString(4, clients_.phone);
                    pstmtUpdateClient->setString(5, clients_.current_phone);
                    pstmtUpdateClient->execute();
                    std::cout << "Клиент обновлен" << std::endl;
                }

                
                // Обновление данных о бронировании (включая дату и время)
                std::string queryUpdate = "UPDATE " + tableName + " SET date_game = ?, time_game = ?, players_count = ?, comment_game = ?, type_game = ?, price = ?, place_game = ?"
                                        "WHERE name_game = ? AND date_game = ? AND time_game = ?";
                std::unique_ptr<sql::PreparedStatement> pstmtUpdateBooking(conn->prepareStatement(queryUpdate));
                pstmtUpdateBooking->setString(1, booking_.date_game);  // Новая дата
                pstmtUpdateBooking->setString(2, booking_.time_game);  // Новое время
                pstmtUpdateBooking->setInt(3, booking_.players_count);
                pstmtUpdateBooking->setString(4, booking_.comment_game);
                pstmtUpdateBooking->setString(5, booking_.type_game);
                pstmtUpdateBooking->setInt(6, booking_.price);
                pstmtUpdateBooking->setString(7, booking_.place_game);
                pstmtUpdateBooking->setString(8, booking_.name_game);
                pstmtUpdateBooking->setString(9, booking_.current_date_game);  // Текущая дата
                pstmtUpdateBooking->setString(10, booking_.current_time_game);  // Текущее время
                pstmtUpdateBooking->execute();

                // Обновление данных в таблице Bookings
                std::unique_ptr<sql::PreparedStatement> pstmtUpdateBookings(conn->prepareStatement(
                    "UPDATE Bookings SET game_id = (SELECT id FROM GameSchedule WHERE name_game = ? AND date_game = ? AND time_game = ?) "
                    "WHERE client_id = (SELECT id FROM Clients WHERE phone = ? LIMIT 1) "
                    "AND game_id = (SELECT id FROM GameSchedule WHERE name_game = ? AND date_game = ? AND time_game = ? LIMIT 1)"
                ));
                pstmtUpdateBookings->setString(1, booking_.name_game);
                pstmtUpdateBookings->setString(2, booking_.date_game);  // Новая дата
                pstmtUpdateBookings->setString(3, booking_.time_game);  // Новое время
                pstmtUpdateBookings->setString(4, clients_.phone);
                pstmtUpdateBookings->setString(5, booking_.name_game);
                pstmtUpdateBookings->setString(6, booking_.current_date_game);  // Текущая дата
                pstmtUpdateBookings->setString(7, booking_.current_time_game);  // Текущее время
                pstmtUpdateBookings->execute();

                conn->commit();
                return;
            } catch (const sql::SQLException& e) {
                std::cerr << "Attempt " << (attempt + 1) << " - SQLException: " << e.what() << std::endl;
                std::cerr << "Error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << std::endl;

                if (e.getErrorCode() == 1213) { // Deadlock
                    if (attempt < max_retries - 1) {
                        int delay = base_retry_delay_ms * (attempt + 1) + rand() % 1000;
                        std::cerr << "Retrying after " << delay << " ms" << std::endl;
                        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                    } else {
                        conn->rollback();
                        throw;
                    }
                } else {
                    conn->rollback();
                    throw;
                }
            } catch (const std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
                conn->rollback();
                throw;
            }
        }


    }
    */

    std::string Booking::urlEncode(const std::string &value) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (char c : value) {
            // URL-кодирование для спецсимволов
            if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            } else {
                escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
            }
        }

        return escaped.str();
    }


    void Booking::executeTransactionCheckAvailabilityArena(std::shared_ptr<sql::Connection> conn, std::string& response){
        const int max_retries = 5;
        const int base_retry_delay_ms = 500;

        for (int attempt = 0; attempt < max_retries; ++attempt) {
            try {
                conn->setAutoCommit(false);
                std::unique_ptr<sql::PreparedStatement> pstmtCheckAvailability(conn->prepareStatement(
                "SELECT time_game, SUM(players_count) AS total_players, "
                "CASE "
                "    WHEN COUNT(DISTINCT name_game) = 1 AND MAX(name_game) = ? THEN (10 - SUM(players_count)) "
                "    ELSE 0 "
                "END AS available_slots "
                "FROM gameschedule "
                "WHERE place_game = ? AND date_game = ? "
                "GROUP BY time_game"
            ));


                pstmtCheckAvailability->setString(1, availability_.namegame);
                pstmtCheckAvailability->setString(2, availability_.placegame);
                pstmtCheckAvailability->setString(3, availability_.date);

                std::unique_ptr<sql::ResultSet> resSet(pstmtCheckAvailability->executeQuery());

                // Создаем JSON объект для хранения результатов
                boost::json::array jsonResponse;

                // Перебираем результаты и добавляем их в JSON массив
                while (resSet->next()) {
                    boost::json::object gameInfo;
                    gameInfo["time_game"] = std::string(resSet->getString("time_game"));
                    gameInfo["total_players"] = resSet->getInt("total_players");
                    gameInfo["available_slots"] = resSet->getInt("available_slots");

                    // Добавляем в массив
                    jsonResponse.push_back(gameInfo);
                }

                // Преобразуем JSON в строку
                 response = boost::json::serialize(jsonResponse);
                conn->commit();  // Завершаем транзакцию
                return;          // Успешный выход из функции

            }
            catch (const sql::SQLException& e) {
                handleSQLException(e, attempt, max_retries, base_retry_delay_ms, conn);
            } 
            catch (const std::exception& e) {
                handleStdException(e, conn);
            }
            

        }
    }

    void Booking::executeTransactionCheckAvailabilityCubes(std::shared_ptr<sql::Connection> conn, std::string& response){
        const int max_retries = 5;
        const int base_retry_delay_ms = 500;

        for (int attempt = 0; attempt < max_retries; ++attempt) {
            try {
                conn->setAutoCommit(false);
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

                std::unique_ptr<sql::ResultSet> resSet(pstmtCheckAvailability->executeQuery());

                // Создаем JSON объект для хранения результатов
                boost::json::array jsonResponse;

                // Перебираем результаты и добавляем их в JSON массив
                while (resSet->next()) {
                    boost::json::object gameInfo;
                    gameInfo["time_game"] = std::string(resSet->getString("time_game"));
                    gameInfo["total_players"] = resSet->getInt("total_players");
                    gameInfo["available_slots"] = resSet->getInt("available_slots");

                    // Добавляем в массив
                    jsonResponse.push_back(gameInfo);
                }

                // Преобразуем JSON в строку
                response = boost::json::serialize(jsonResponse);
                conn->commit();  // Завершаем транзакцию
                return;          // Успешный выход из функции

            }
            catch (const sql::SQLException& e) {
                handleSQLException(e, attempt, max_retries, base_retry_delay_ms, conn);
            } 
            catch (const std::exception& e) {
                handleStdException(e, conn);
            }
            

        }
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

}