#pragma once
#include "Connect_to_DB.h"
#include <memory>
#include <string>
#include <unordered_map>


namespace vr{

     // Структура для хранения уникального ключа бронирования
    struct BookingKey {
        std::string date_game;
        std::string time_game;
        int client_id;
        std::string place_game;
    };

    // Хэш-функция для структуры BookingKey
    struct BookingKeyHash {
        std::size_t operator()(const BookingKey& k) const {
            // Используем комбинаторный подход для создания уникального хэша
            std::size_t h1 = std::hash<std::string>{}(k.date_game);
            std::size_t h2 = std::hash<std::string>{}(k.time_game);
            std::size_t h3 = std::hash<std::string>{}(k.place_game);
             
            // Используем FNV-1a подход для комбинирования хэшей
            std::size_t hash = 14695981039346656037ULL; // FNV offset basis
            hash = (hash ^ h1) * 1099511628211ULL;  // FNV prime
            hash = (hash ^ h2) * 1099511628211ULL;
            hash = (hash ^ h3) * 1099511628211ULL;
                
                
            return hash;
            }
        };

    // Оператор сравнения для структуры BookingKey
    struct BookingKeyEqual {
        bool operator()(const BookingKey& lhs, const BookingKey& rhs) const {
            return lhs.date_game == rhs.date_game && 
                   lhs.time_game == rhs.time_game &&
                   lhs.place_game == rhs.place_game &&
                   lhs.client_id == rhs.client_id;
        }
    };



    struct ButtonData {
        std::string time;
        int price;
        int availability_place;
    };

    // Структура для хранения данных клиента
    struct Client_data {
        std::string first_name;
        std::string last_name;
        std::string phone;
        std::string email;
        std::string current_phone;
    };

    struct Booking_data {
        std::string place_game;
        std::string type_game;
        std::string name_game;
        std::string date_game;
        std::string time_game;
        int players_count = 0;
        int price = 0;
        std::string comment_game;
        std::string who_reservation;
        std::string book_status;
        std::string current_date_game;
        std::string current_time_game;
    };

    struct AvailabilityData{
        std::string date;
        std::string namegame;
        std::string placegame;
        std::string typegame;
    };

    struct FindData{
        std::string place_game;
        std::string date;
    };

    struct info_by_email{
        std::string_view place_game;
        std::string_view type_game;
        std::string_view name_game;
        std::string_view date_game;
        std::string_view times_game_and_players_count;
        int price;
        std::string_view client_first_name;
        std::string_view client_last_name;
        std::string_view client_phone;
        std::string_view client_email;
        std::string_view comment_game;
        std::string_view status_book;

    };

    class ConnectionGuard {
    public:
        ConnectionGuard(std::unique_ptr<sql::Connection> conn, ConnectionPool& pool)
            : conn_(std::move(conn)), pool_(pool) {}

        ~ConnectionGuard() {
            if (conn_) {
                pool_.ReleaseConnection(std::move(conn_));
            }
        }

        sql::Connection* getConnection() const {
            return std::move(conn_.get());
        }

    private:
        std::unique_ptr<sql::Connection> conn_;
        ConnectionPool& pool_;
    };



    class ScopeGuard {
public:
    ScopeGuard(AvailabilityData& data)
        : data_(&data), bookings_(nullptr), clients_(nullptr), find_data_(nullptr) {}


    ScopeGuard(FindData& data)
        : find_data_(&data), bookings_(nullptr), clients_(nullptr), data_(nullptr) {}
    
    ScopeGuard(std::vector<Booking_data>& bookings)
        : data_(nullptr), bookings_(&bookings), clients_(nullptr), find_data_(nullptr) {}

    ScopeGuard(Client_data& clients)
        : data_(nullptr), bookings_(nullptr), clients_(&clients), find_data_(nullptr) {}

    ScopeGuard(std::vector<Booking_data>& bookings, Client_data& clients)
        : data_(nullptr), bookings_(&bookings), clients_(&clients), find_data_(nullptr) {}

    ~ScopeGuard() {
        if (bookings_) {
            for (auto& booking : *bookings_) {
                booking.comment_game.clear();
                booking.date_game.clear();
                booking.name_game.clear();
                booking.players_count = 0;
                booking.time_game.clear();
                booking.type_game.clear();
                booking.place_game.clear();
                booking.price = 0;
                booking.current_date_game.clear();
                booking.current_time_game.clear();
                booking.who_reservation.clear();
                booking.book_status.clear();
            }
            bookings_->clear(); 
        }

        if (clients_) {
            clients_->first_name.clear();
            clients_->last_name.clear();
            clients_->phone.clear();
            clients_->email.clear();
        }

        if (data_) {
            data_->date.clear();
            data_->namegame.clear();
            data_->placegame.clear();
            data_->typegame.clear();
        }

        if (find_data_) {
            find_data_->place_game.clear();
            find_data_->date.clear();
        }
    }

private:
    FindData* find_data_;
    AvailabilityData* data_;
    std::vector<Booking_data>* bookings_;
    Client_data* clients_;

};

    
    class Booking {
    public:
        Booking(ConnectionPool& pool) : pool_(pool) { gameTables = LoadGameTables("../games/games.conf");}
        virtual ~Booking() = default;

        void AddDataByCheckAvailability(const AvailabilityData& data);
        void AddDataByInsertAndUpdate(const Client_data& client, const std::vector<Booking_data>& bookings);
        void AddDataByDelete(const std::vector<Booking_data>& booking);
        std::string GetAdminBooking(std::string& date, std::string& place_game);
        //void Update();
        //void Delete();

    protected:
        void executeTransactionInitButtonArena(sql::Connection* conn, std::string& response);
        void executeTransactionInitButtonCubes(sql::Connection* conn, std::string& response);
        void executeTransactionInsert(sql::Connection* conn);
        void GetAdminBooking(sql::Connection* conn, std::string& date, std::string& place_game, std::string& response);
        //void executeTransactionDelete(std::shared_ptr<sql::Connection> conn);
        //void executeTransactionUpdate(std::shared_ptr<sql::Connection> conn);
        void PrintInsertBooking();
        void PrintDeleteBooking();
        void PrintUpdateBooking();
        std::string generate_email_body(const info_by_email& email_data);
        void sendToEmail();

        
    
        Client_data clients_;
        std::vector<Booking_data> bookings_;
        ConnectionPool& pool_;
        AvailabilityData availability_;
        FindData find_data_;
        std::map<std::string, ButtonData> button_data_;


        template <typename Func>
        void executeTransactionWithRetry(sql::Connection* conn, Func&& func) {
            const int max_retries = 5;
            const int base_retry_delay_ms = 500;
            for (int attempt = 0; attempt < max_retries; ++attempt) {
                try {
                    Transaction transaction(conn); 
                    func(conn); 
                    transaction.commit(); 
                    return;
                } catch (const sql::SQLException& e) {
                    handleSQLException(e, attempt, max_retries, base_retry_delay_ms, conn);
                } catch (const std::exception& e) {
                    handleStdException(e, conn);
                }
            }
        }


    private:
        bool IsHolidayOrWeekend(const std::string& date);
        std::unordered_map<std::string, std::string> gameTables;
        std::unordered_map<std::string, std::string> LoadGameTables(const std::string& filename);
        std::string urlEncode(const std::string &value);
        bool checkAvailableSlots(sql::Connection* conn, const Booking_data& booking);
        void insertClient(sql::Connection* conn);
        void handleSQLException(const sql::SQLException& e, int attempt, int max_retries, int base_retry_delay_ms, sql::Connection* conn);
        void handleStdException(const std::exception& e, sql::Connection* conn);
        void checkButtonDataArena(sql::Connection* conn);
        void checkButtonDataCubes(sql::Connection* conn);
        std::unordered_map<std::string, int> checkAvailabilityPlaceArena(sql::Connection* conn);
        std::unordered_map<std::string, int> checkAvailabilityPlaceCubes(sql::Connection* conn);

    };



    // Класс для управления ареной
    class Arena : public Booking {
    public:
        Arena(ConnectionPool& pool) : Booking(pool) {}
        std::string initBunnot();
        void Open_arena();
        void Close_arena();
    private:
        void ProcessArenaTransaction(const std::string& operationName);
        
    };

    // Класс для управления кубами
    class Cubes : public Booking {
    public:
        Cubes(ConnectionPool& pool) : Booking(pool) {}
        std::string initBunnot();
        void Open_cubes();
    };

}