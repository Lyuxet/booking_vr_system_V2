#include "booking_vr.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <iostream>

void Booking::Add_date(const Client_data& client, const Booking_data& booking) {
    clients_ = client;
    booking_ = booking;
}

void Arena::Open_arena() {
    ScopeGuard gurd(booking_, clients_);
    try {
        // Получаем соединение из пула
        std::unique_ptr<sql::Connection> conn = pool_.GetConnection();
        booking_.type_game = "OPEN";
        executeTransaction(std::move(conn));
        std::cout << "Transaction Open game completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Open arena Exception: " << e.what() << std::endl;
        throw;
    } 
    
}

void Arena::Close_arena() {
    ScopeGuard gurd(booking_, clients_);
    try
    {
        std::unique_ptr<sql::Connection> conn = pool_.GetConnection();
        booking_.type_game = "CLOSE";
        booking_.players_count = 10;
        executeTransaction(std::move(conn));
        std::cout << "Transaction Close game completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Close arena Exception: " << e.what() << std::endl;
        throw;
    }
    
}

void Arena::executeTransaction(std::unique_ptr<sql::Connection> conn) {
    try {
        conn->setAutoCommit(false); // Отключаем автокоммит

        // Добавление в таблицу клиентов
        std::unique_ptr<sql::PreparedStatement> pstmt1(conn->prepareStatement(
            "INSERT IGNORE INTO Clients(first_name, last_name, phone, email) VALUES (?,?,?,?)"));
        pstmt1->setString(1, clients_.first_name);
        pstmt1->setString(2, clients_.last_name);
        pstmt1->setString(3, clients_.phone);
        pstmt1->setString(4, clients_.email);
        pstmt1->execute();

        // Добавление в таблицу игр
        std::unique_ptr<sql::PreparedStatement> pstmt2;
        if (booking_.name_game == "ARENA SHOOTER") {
            pstmt2 = std::unique_ptr<sql::PreparedStatement>(conn->prepareStatement(
                "INSERT INTO ArenaShooterStats(date_game, time_game, players_count, comment_game, type_game) VALUES (?,?,?,?,?)"));
        } else if (booking_.name_game == "ARENA QUEST") {
            pstmt2 = std::unique_ptr<sql::PreparedStatement>(conn->prepareStatement(
                "INSERT INTO ArenaQuestStats(date_game, time_game, players_count, comment_game, type_game) VALUES (?,?,?,?,?)"));
        }

        pstmt2->setString(1, booking_.date_game);
        pstmt2->setString(2, booking_.time_game);
        pstmt2->setInt(3, booking_.players_count);
        pstmt2->setString(4, booking_.comment_game);
        pstmt2->setString(5, booking_.type_game);
        pstmt2->execute();

        // Добавление в таблицу бронирований
        std::unique_ptr<sql::PreparedStatement> pstmt3(conn->prepareStatement(
            "INSERT INTO Bookings (client_id, game_id) "
            "VALUES ((SELECT id FROM Clients WHERE phone = ? LIMIT 1), "
            "(SELECT id FROM GameSchedule WHERE name_game = ? AND date_game = ? AND time_game = ? LIMIT 1))"));
        pstmt3->setString(1, clients_.phone);
        pstmt3->setString(2, booking_.name_game);
        pstmt3->setString(3, booking_.date_game);
        pstmt3->setString(4, booking_.time_game);
        pstmt3->execute();

        conn->commit(); // Завершаем транзакцию
    } catch (const sql::SQLException& e) {
        std::cerr << "Transaction SQLException: " << e.what() << std::endl;
        std::cerr << "Transaction MySQL error code: " << e.getErrorCode() << std::endl;
        std::cerr << "Transaction SQLState: " << e.getSQLState() << std::endl;
        conn->rollback(); // Откатываем транзакцию в случае ошибки
        throw; // Пробрасываем исключение дальше
    } catch (const std::exception& e) {
        std::cerr << "Transaction Exception: " << e.what() << std::endl;
        conn->rollback(); // Откатываем транзакцию в случае ошибки
        throw; // Пробрасываем исключение дальше
    }
}
