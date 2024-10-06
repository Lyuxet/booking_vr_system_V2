#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <memory>
#include <string>
#include <stdexcept>
#include <queue>
#include <mutex>
#include <condition_variable>

// Структура для хранения конфигурации базы данных
struct DBConfig {
    std::string host;
    std::string dbname;
    std::string user;
    std::string password;
};

// Исключение для обработки ошибок конфигурации
class ConfigFileException : public std::runtime_error {
public:
    explicit ConfigFileException(const std::string& message)
        : std::runtime_error(message) {}
};

// Класс для управления пулом соединений с базой данных
class ConnectionPool {
public:
    ConnectionPool(size_t poolSize, const std::string& configFilePath);

    void Init_pool();
    std::shared_ptr<sql::Connection> GetConnection();
    void ReleaseConnection(std::shared_ptr<sql::Connection> conn);

private:
    std::string configFilePath_;
    std::queue<std::shared_ptr<sql::Connection>> pool_;
    std::mutex mutex_;
    std::condition_variable condVar_;
    size_t poolSize_;
    bool isConnectionActive(std::shared_ptr<sql::Connection> conn);

    DBConfig ReadDBConfig(const std::string& file_name);
    std::shared_ptr<sql::Connection> CreateConnection(const DBConfig& config);
};

// Класс для управления транзакциями
class Transaction {
public:
    explicit Transaction(std::shared_ptr<sql::Connection> conn);
    ~Transaction();

    void commit();

private:
    std::shared_ptr<sql::Connection> conn_;
    bool committed_;
};
