#pragma once
#include "Connect_to_DB.h"

struct Client_data {
    std::string first_name;
    std::string last_name;
    std::string phone;
    std::string email;
};

struct Booking_data {
    std::string type_game;
    std::string name_game;
    std::string date_game;
    std::string time_game;
    int players_count = 0;
    std::string comment_game;
};

class ConnectionGuard {
public:
    ConnectionGuard(std::unique_ptr<sql::Connection>& conn, ConnectionPool& pool)
        : conn_(conn), pool_(pool) {}
    ~ConnectionGuard() {
        if (conn_) {
            pool_.ReleaseConnection(std::move(conn_));
        }
    }
private:
    std::unique_ptr<sql::Connection>& conn_;
    ConnectionPool& pool_;
};

class ScopeGuard {
public:
    ScopeGuard(Booking_data& booking, Client_data& clients) : booking_(booking), clients_(clients) {}
    ~ScopeGuard() {
        booking_.comment_game = "";
        booking_.date_game = "";
        booking_.name_game = "";
        booking_.players_count = 0;
        booking_.time_game = "";
        booking_.type_game = "";

        clients_.first_name = "";
        clients_.last_name = "";
        clients_.phone = "";
        clients_.email = "";

    }
private:
    Booking_data& booking_;
    Client_data& clients_;
};

class Booking {
public:
    Booking(ConnectionPool& pool) : pool_(pool) {}
    virtual ~Booking() = default;
    void Add_date(const Client_data& client, const Booking_data& booking);

protected:
    void executeTransaction(std::unique_ptr<sql::Connection> conn);
    Client_data clients_;
    Booking_data booking_;
    ConnectionPool& pool_;
};

class Arena : public Booking {
public:
    Arena(ConnectionPool& pool) : Booking(pool) {}
    void Open_arena();
    void Close_arena();
};

class Cubes : public Booking{
public:
    Cubes(ConnectionPool& pool) : Booking(pool) {}
    void Open_cubes();
};
