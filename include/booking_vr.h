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
};

// Структура для хранения данных бронирования
struct Booking_data {
    std::string type_game;
    std::string name_game;
    std::string date_game;
    std::string time_game;
    int players_count = 0;
    std::string comment_game;
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
    ScopeGuard(Booking_data& booking, Client_data& clients)
        : booking_(booking), clients_(clients) {}

    ~ScopeGuard() {
        // Сброс данных
        booking_.comment_game.clear();
        booking_.date_game.clear();
        booking_.name_game.clear();
        booking_.players_count = 0;
        booking_.time_game.clear();
        booking_.type_game.clear();

        clients_.first_name.clear();
        clients_.last_name.clear();
        clients_.phone.clear();
        clients_.email.clear();
    }

private:
    Booking_data& booking_;
    Client_data& clients_;
};

// Базовый класс для управления бронированиями
class Booking {
public:
    Booking(ConnectionPool& pool) : pool_(pool) {}
    virtual ~Booking() = default;

    void Add_date(const Client_data& client, const Booking_data& booking);

protected:
    void executeTransaction(std::shared_ptr<sql::Connection> conn);

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
