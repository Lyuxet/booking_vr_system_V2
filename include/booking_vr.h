#include <string>
#include "Connect_to_DB.h"


struct Client_data{
    std::string first_name;
    std::string last_name;
    std::string phone;
    std::string email;
};

struct Booking_data{
    std::string date_game;
    std::string time_game;
    std::string players_count;
    std::string comment_game;
};



class Booking {
public:
    Booking(Connection& conn) : conn_(conn) {};
    void Add_date(Client_data client, Booking_data booking);

protected:
    Client_data clients_;
    Booking_data booking_;
    Connection& conn_;

};

class Arena : public Booking{
public:
    void Open_arena() const;
    void Close_arena();

};