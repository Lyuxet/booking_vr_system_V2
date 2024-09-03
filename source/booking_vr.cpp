#include "booking_vr.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>



void Booking::AddDataByInsert(const Client_data& client, const Booking_data& booking) {
    clients_ = client;
    booking_ = booking;
}
void Booking::AddDataByDelete(const Booking_data& booking){
    booking_ = booking;
}

void Arena::Open_arena() {
    ScopeGuard guard(&booking_, &clients_);
    try {
        std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
        ConnectionGuard conn_guard(conn, pool_);
        executeTransactionInsert(conn);
        std::cout << "Transaction Open game completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Open arena Exception: " << e.what() << std::endl;
        throw;
    }
}

void Arena::Close_arena() {
    ScopeGuard guard(&booking_, &clients_);
    try {
        std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
        ConnectionGuard conn_guard(conn, pool_);
        booking_.type_game = "CLOSE";
        booking_.players_count = 10;
        executeTransactionInsert(conn);
        std::cout << "Transaction Close game completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Close arena Exception: " << e.what() << std::endl;
        throw;
    }
}

void Booking::Delete(){
    ScopeGuard guard(&booking_);
    try
    {
       std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
       ConnectionGuard conn_guard(conn, pool_);
        executeTransactionDelete(conn);
        std::cout << "Transaction Delete game completed successfully." << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Delete arena Exception: " << e.what() << std::endl;
        throw;
    }
    
}

void Cubes::Open_cubes() {
    ScopeGuard guard(&booking_, &clients_);
    try {
        std::shared_ptr<sql::Connection> conn = pool_.GetConnection();
        ConnectionGuard conn_guard(conn, pool_);
        executeTransactionInsert(conn);
        std::cout << "Transaction Cubes completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Cubes Exception: " << e.what() << std::endl;
        throw;
    }
}

void Booking::executeTransactionInsert(std::shared_ptr<sql::Connection> conn) {
    const int max_retries = 5;
    const int base_retry_delay_ms = 500;
    std::string tableName;

    // Определение таблицы в зависимости от типа игры
    if (booking_.name_game == "ARENA SHOOTER") {
        tableName = "ArenaShooterStats";
    } else if (booking_.name_game == "ARENA QUEST") {
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

            if (res->next()) {
                throw std::runtime_error("Selected time slot is already booked.");
            }

            // Вставка данных о клиенте
            std::unique_ptr<sql::PreparedStatement> pstmt1(conn->prepareStatement(
                "INSERT INTO Clients (first_name, last_name, phone, email) "
                "VALUES (?, ?, ?, ?) "
                "ON DUPLICATE KEY UPDATE first_name=VALUES(first_name), last_name=VALUES(last_name), email=VALUES(email)"
            ));
            pstmt1->setString(1, clients_.first_name);
            pstmt1->setString(2, clients_.last_name);
            pstmt1->setString(3, clients_.phone);
            pstmt1->setString(4, clients_.email);
            pstmt1->execute();

            // Вставка данных о бронировании
            std::string queryInsert = "INSERT INTO " + tableName + " (date_game, time_game, players_count, comment_game, type_game) "
                                      "VALUES (?, ?, ?, ?, ?)";
            std::unique_ptr<sql::PreparedStatement> pstmt2(conn->prepareStatement(queryInsert));
            pstmt2->setString(1, booking_.date_game);
            pstmt2->setString(2, booking_.time_game);
            pstmt2->setInt(3, booking_.players_count);
            pstmt2->setString(4, booking_.comment_game);
            pstmt2->setString(5, booking_.type_game);
            pstmt2->execute();

            // Вставка данных о бронировании в таблицу Bookings
            std::unique_ptr<sql::PreparedStatement> pstmt3(conn->prepareStatement(
                "INSERT INTO Bookings (client_id, game_id) "
                "VALUES ((SELECT id FROM Clients WHERE phone = ? LIMIT 1), "
                "(SELECT id FROM GameSchedule WHERE name_game = ? AND date_game = ? AND time_game = ? LIMIT 1))"
            ));
            pstmt3->setString(1, clients_.phone);
            pstmt3->setString(2, booking_.name_game);
            pstmt3->setString(3, booking_.date_game);
            pstmt3->setString(4, booking_.time_game);
            pstmt3->execute();

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
    if (booking_.name_game == "ARENA SHOOTER") {
        tableName = "ArenaShooterStats";
    } else if (booking_.name_game == "ARENA QUEST") {
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
                throw std::runtime_error("Record not found for deletion.");
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