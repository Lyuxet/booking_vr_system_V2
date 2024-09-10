#pragma once
#include "Connect_to_DB.h"
#include <memory>
#include <string>

// Структура для хранения данных клиента
struct Client_data {
    std::string first_name;
    std::string last_name;
    std::string phone;
    std::string email;
    std::string current_phone;
    

};

// Структура для хранения данных бронирования
struct Booking_data {
    std::string type_game;
    std::string name_game;
    std::string date_game;
    std::string time_game;
    int players_count = 0;
    int price = 0;
    std::string comment_game;
    std::string current_date_game;
    std::string current_time_game;
};

// Класс для управления ресурсами соединения
class ConnectionGuard {
public:
    ConnectionGuard(std::shared_ptr<sql::Connection> conn, ConnectionPool& pool)
        : conn_(std::move(conn)), pool_(pool) {}

    ~ConnectionGuard() {
        if (conn_) {
            pool_.ReleaseConnection(std::move(conn_));
        }
    }

private:
    std::shared_ptr<sql::Connection> conn_;
    ConnectionPool& pool_;
};

// Класс для управления ресурсами бронирования и клиента
class ScopeGuard {
public:
    // Конструктор для сброса как Booking_data, так и Client_data
    ScopeGuard(Booking_data* booking, Client_data* clients = nullptr)
        : booking_(booking), clients_(clients) {}

    ~ScopeGuard() {
        if (booking_) {
            // Сброс данных booking
            booking_->comment_game.clear();
            booking_->date_game.clear();
            booking_->name_game.clear();
            booking_->players_count = 0;
            booking_->time_game.clear();
            booking_->type_game.clear();
        }

        if (clients_) {
            // Сброс данных clients
            clients_->first_name.clear();
            clients_->last_name.clear();
            clients_->phone.clear();
            clients_->email.clear();
        }
    }

private:
    Booking_data* booking_;
    Client_data* clients_;
};

// Базовый класс для управления бронированиями
class Booking {
public:
    Booking(ConnectionPool& pool) : pool_(pool) {}
    virtual ~Booking() = default;

    void AddDataByInsertAndUpdate(const Client_data& client, const Booking_data& booking);
    void AddDataByDelete(const Booking_data& booking);
    void Update();
    void Delete();

protected:
    void executeTransactionInsert(std::shared_ptr<sql::Connection> conn);
    void executeTransactionDelete(std::shared_ptr<sql::Connection> conn);
    void executeTransactionUpdate(std::shared_ptr<sql::Connection> conn);
    void PrintInsertBooking();
    void PrintDeleteBooking();
    void PrintUpdateBooking();

    Client_data clients_;
    Booking_data booking_;
    ConnectionPool& pool_;
};

// Класс для управления ареной
class Arena : public Booking {
public:
    Arena(ConnectionPool& pool) : Booking(pool) {}
    void Open_arena();
    void Close_arena();
    
};

// Класс для управления кубами
class Cubes : public Booking {
public:
    Cubes(ConnectionPool& pool) : Booking(pool) {}
    void Open_cubes();
};
