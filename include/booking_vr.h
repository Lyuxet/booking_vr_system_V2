#pragma once
#include "Connect_to_DB.h"
#include <memory>
#include <string>
#include <nlohmann/json.hpp>


namespace vr{

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
        std::string place_game;
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

    struct AvailabilityData{
        std::string date;
        std::string namegame;
        std::string placegame;
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
    // Конструктор для сброса данных через ссылки
    ScopeGuard(AvailabilityData& data)
        : data_(&data), bookings_(nullptr), clients_(nullptr) {}
    
    ScopeGuard(std::vector<Booking_data>& bookings)
        : data_(nullptr), bookings_(&bookings), clients_(nullptr) {}

    ScopeGuard(Client_data& clients)
        : data_(nullptr), bookings_(nullptr), clients_(&clients) {}

    ScopeGuard(std::vector<Booking_data>& bookings, Client_data& clients)
        : data_(nullptr), bookings_(&bookings), clients_(&clients) {}

    ~ScopeGuard() {
        if (bookings_) {
            // Очистка каждого элемента вектора
            for (auto& booking : *bookings_) {
                booking.comment_game.clear();
                booking.date_game.clear();
                booking.name_game.clear();
                booking.players_count = 0;
                booking.time_game.clear();
                booking.type_game.clear();
                booking.place_game.clear();
                booking.price = 0;
            }
            bookings_->clear(); // Также очищаем сам вектор
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
        }
    }

private:
    AvailabilityData* data_;
    std::vector<Booking_data>* bookings_;
    Client_data* clients_;
};

    // Базовый класс для управления бронированиями
    class Booking {
    public:
        Booking(ConnectionPool& pool) : pool_(pool) {}
        virtual ~Booking() = default;

        void AddDataByCheckAvailability(const AvailabilityData& data);
        void AddDataByInsertAndUpdate(const Client_data& client, const std::vector<Booking_data>& bookings);
        void AddDataByDelete(const std::vector<Booking_data>& booking);
        //void Update();
        //void Delete();

    protected:
        void executeTransactionCheckAvailabilityArena(std::shared_ptr<sql::Connection> conn, std::string& response);
        void executeTransactionCheckAvailabilityCubes(std::shared_ptr<sql::Connection> conn, std::string& response);
        void executeTransactionInsert(std::shared_ptr<sql::Connection> conn);
        //void executeTransactionDelete(std::shared_ptr<sql::Connection> conn);
        //void executeTransactionUpdate(std::shared_ptr<sql::Connection> conn);
        void PrintInsertBooking();
        void PrintDeleteBooking();
        void PrintUpdateBooking();
    
        Client_data clients_;
        std::vector<Booking_data> bookings_;
        ConnectionPool& pool_;
        AvailabilityData availability_;

        // Добавление метода для выполнения транзакции с повтором
        template <typename Func>
        void executeTransactionWithRetry(std::shared_ptr<sql::Connection> conn, Func&& func) {
            const int max_retries = 5;
            const int base_retry_delay_ms = 500;

            for (int attempt = 0; attempt < max_retries; ++attempt) {
                try {
                    Transaction transaction(conn); // Создаем транзакцию

                    func(conn); // Выполняем нужную логику транзакции

                    transaction.commit(); // Коммит транзакции, если все прошло успешно
                    return;
                } 
                catch (const sql::SQLException& e) {
                    handleSQLException(e, attempt, max_retries, base_retry_delay_ms, conn);
                } 
                catch (const std::exception& e) {
                    handleStdException(e, conn);
                }
            }
        }

    private:
        std::string urlEncode(const std::string &value);
        bool checkAvailableSlots(std::shared_ptr<sql::Connection> conn, const Booking_data& booking);
        void insertClient(std::shared_ptr<sql::Connection> conn);
        void handleSQLException(const sql::SQLException& e, int attempt, int max_retries, int base_retry_delay_ms, std::shared_ptr<sql::Connection> conn);
        void handleStdException(const std::exception& e, std::shared_ptr<sql::Connection> conn);
    };



    // Класс для управления ареной
    class Arena : public Booking {
    public:
        Arena(ConnectionPool& pool) : Booking(pool) {}
        std::string CheckAvailabilityPlace();
        void Open_arena();
        void Close_arena();
    private:
        void ProcessArenaTransaction(const std::string& operationName);
        
    };

    // Класс для управления кубами
    class Cubes : public Booking {
    public:
        Cubes(ConnectionPool& pool) : Booking(pool) {}
        std::string CheckAvailabilityPlace();
        void Open_cubes();
    };

}