#pragma once
#include "Connect_to_DB.h"

struct Client_data {
    std::string first_name;
    std::string last_name;
    std::string phone;
    std::string email;
};

struct Booking_data {
    std::string name_game;
    std::string date_game;
    std::string time_game;
    std::string players_count;
    std::string comment_game;
};

class Booking {
public:
    Booking(ConnectionPool& pool) : pool_(pool) {}
    virtual ~Booking() = default;
    void Add_date(const Client_data& client, const Booking_data& booking);

protected:
    Client_data clients_;
    Booking_data booking_;
    ConnectionPool& pool_;
};

class Arena : public Booking {
public:
    Arena(ConnectionPool& pool) : Booking(pool) {}
    void Open_arena();
    void Close_arena();

private:
    void executeTransaction(std::unique_ptr<sql::Connection> conn);
};
