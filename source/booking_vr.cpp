#include "booking_vr.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

std::mutex mtxtest;

void Booking::PrintInsertBooking(){
    std::cout << "Забронировано " << booking_.name_game 
    << " с типом " << booking_.type_game 
    << " на " << booking_.date_game << " " << booking_.time_game
    << " количество игроков " << booking_.players_count 
    << std::endl;
}

void Booking::PrintDeleteBooking(){
    std::cout << "Удалено бронирование " << booking_.name_game 
    << " с типом " << booking_.type_game 
    << " на " << booking_.date_game << " " << booking_.time_game
    << std::endl;
}

void Booking::PrintUpdateBooking(){
    std::cout << "Обновленно " << booking_.name_game 
    << " с типом " << booking_.type_game 
    << " с " << booking_.current_date_game << " " << booking_.current_time_game
    << " на " << booking_.date_game << " " << booking_.current_time_game
    << std::endl;
}

void Booking::AddDataByInsertAndUpdate(const Client_data& client, const Booking_data& booking) {
    clients_ = std::move(client);
    booking_ = std::move(booking);
}
void Booking::AddDataByDelete(const Booking_data& booking){
    booking_ = std::move(booking);
}

void Booking::AddDataByCheckAvailability(const AvailabilityData& data){
    availability_ = std::move(data);
}
void Arena::Open_arena() {
    ScopeGuard guard(booking_, clients_);
    try {
        std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
        ConnectionGuard conn_guard(conn, pool_);
        executeTransactionInsert(conn);
        std::lock_guard<std::mutex> lock(mtxtest);
        PrintInsertBooking();
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(mtxtest);
        std::cerr << "Open arena Exception: " << e.what() << std::endl;
        throw;
    }
}

void Arena::Close_arena() {
    ScopeGuard guard(booking_, clients_);
    try {
        std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
        ConnectionGuard conn_guard(conn, pool_);
        booking_.type_game = "CLOSE";
        booking_.players_count = 10;
        executeTransactionInsert(conn);
        std::lock_guard<std::mutex> lock(mtxtest);
        PrintInsertBooking();
    } catch (const std::exception& e) {
        std::cerr << "Close arena Exception: " << e.what() << std::endl;
        throw;
    }
}

std::string Arena::CheckAvailabilityPlace(){
    ScopeGuard guard(availability_);
    try
    {
        std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
        ConnectionGuard conn_guard(conn, pool_);
        std::string response;
        executeTransactionCheckAvailability(conn, response);
        return response;

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return "";
    }
    
}

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

void Cubes::Open_cubes() {
    ScopeGuard guard(booking_, clients_);
    try {
        std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
        ConnectionGuard conn_guard(conn, pool_);
        executeTransactionInsert(conn);
        std::lock_guard<std::mutex> lock(mtxtest);
        PrintInsertBooking();
        
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(mtxtest);
        std::cerr << "Cubes Exception: " << e.what() << std::endl;
        throw;
    }
}

void Booking::executeTransactionInsert(std::shared_ptr<sql::Connection> conn) {
    const int max_retries = 5;
    const int base_retry_delay_ms = 500;
    std::string tableName;

    // Определение таблицы в зависимости от типа игры
    if (booking_.name_game == "Combat Squad") {
        tableName = "ArenaShooterStats";
    } else if (booking_.name_game == "Medieval Journey") {
        tableName = "ArenaQuestStats";
    } else if (booking_.name_game == "CUBES") {
        tableName = "Cubes";
    } else {
        throw std::runtime_error("Unknown game type.");
    }

    for (int attempt = 0; attempt < max_retries; ++attempt) {
        try {
            conn->setAutoCommit(false);

            std::unique_ptr<sql::PreparedStatement> pstmtCheck(conn->prepareStatement(
            "SELECT time_game, SUM(players_count) AS total_players, "
            "CASE "
            "    WHEN COUNT(DISTINCT name_game) = 1 AND MAX(name_game) = ? THEN (10 - SUM(players_count)) "
            "    ELSE 0 "
            "END AS available_slots "
            "FROM gameschedule "
            "WHERE place_game = ? AND date_game = ? AND time_game = ? "
            "GROUP BY time_game"
        ));
        pstmtCheck->setString(1, booking_.name_game);
        pstmtCheck->setString(2, booking_.place_game);  // 'ARENA'
        pstmtCheck->setString(3, booking_.date_game);   // Дата игры
        pstmtCheck->setString(4, booking_.time_game);   // Время игры

        std::unique_ptr<sql::ResultSet> res(pstmtCheck->executeQuery());

        if (res->next()) {
            int available_slots = res->getInt("available_slots");
            if (available_slots < booking_.players_count) {
                throw std::runtime_error("Недостаточно свободных мест для бронирования.");
            }
        }

            // Вставка данных о клиенте
            std::unique_ptr<sql::PreparedStatement> pstmt1(conn->prepareStatement(
                "INSERT IGNORE INTO Clients (first_name, last_name, phone, email) VALUES (?, ?, ?, ?)"
                ));
            pstmt1->setString(1, clients_.first_name);
            pstmt1->setString(2, clients_.last_name);
            pstmt1->setString(3, clients_.phone);
            pstmt1->setString(4, clients_.email);
            pstmt1->execute();

            // Вставка данных о бронировании
            std::string queryInsert = "INSERT INTO " + tableName + 
                " (client_id, date_game, time_game, name_game, players_count, comment_game, type_game, price, place_game) "
                "VALUES ((SELECT id FROM Clients WHERE phone = ? LIMIT 1), ?, ?, ?, ?, ?, ?, ?, ?)";

            // Подготовка оператора SQL с использованием подготовленного выражения
            std::unique_ptr<sql::PreparedStatement> pstmt2(conn->prepareStatement(queryInsert));

            // Установка значений для подготовленного выражения
            pstmt2->setString(1, clients_.phone);               // Установка телефона клиента для подзапроса
            pstmt2->setString(2, booking_.date_game);           // Дата игры
            pstmt2->setString(3, booking_.time_game);           // Время игры
            pstmt2->setString(4, booking_.name_game);           // Название игры
            pstmt2->setInt(5, booking_.players_count);          // Количество игроков
            pstmt2->setString(6, booking_.comment_game);        // Комментарий
            pstmt2->setString(7, booking_.type_game);           // Тип игры
            pstmt2->setInt(8, booking_.price);                  // Цена
            pstmt2->setString(9, booking_.place_game);          // Место игры

            // Выполнение запроса
            pstmt2->execute();

        
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


void Booking::executeTransactionCheckAvailability(std::shared_ptr<sql::Connection> conn, std::string& response){
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
            nlohmann::json jsonResponse;

            // Перебираем результаты и добавляем их в JSON массив
            while (resSet->next()) {
                nlohmann::json gameInfo;
                gameInfo["time_game"] = resSet->getString("time_game");
                gameInfo["total_players"] = resSet->getInt("total_players");
                gameInfo["available_slots"] = resSet->getInt("available_slots");

                // Добавляем в массив
                jsonResponse.push_back(gameInfo);
            }

            // Преобразуем JSON в строку
            response = jsonResponse.dump();
            conn->commit();  // Завершаем транзакцию
            return;          // Успешный выход из функции

        }
        catch (const sql::SQLException& e) {
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