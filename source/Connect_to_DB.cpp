#include "Connect_to_DB.h"
#include "logger.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <fstream>
#include <iostream>
#include <malloc.h>


ConnectionPool::ConnectionPool(size_t poolSize, const std::string& configFilePath)
    : poolSize_(poolSize), configFilePath_(configFilePath) {
    Init_pool();
}


void ConnectionPool::Init_pool() {
    try {
        DBConfig config = ReadDBConfig(configFilePath_);
        for (size_t i = 0; i < poolSize_; ++i) {
            pool_.push(std::move(CreateConnection(config)));
        }
    } catch (const std::exception& e) {
        Logger::getInstance().log("Ошибка инициализации пула соединений: " + std::string(e.what()) +
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../logs/error_connect.log");
    }
}

std::unique_ptr<sql::Connection> ConnectionPool::GetConnection() {
    std::unique_lock<std::mutex> lock(mutex_);
    condVar_.wait(lock, [this] { return !pool_.empty(); });

    auto conn = std::move(pool_.front());
    pool_.pop();
    if (!isConnectionActive(conn.get())) {
        conn.get()->close();
        conn.reset();
        malloc_trim(0);
        conn = CreateConnection(ReadDBConfig(configFilePath_));
    }
    return std::move(conn);
}


bool ConnectionPool::isConnectionActive(sql::Connection* conn) {
    try {
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 1"));
        return res->next();  
    } catch (const sql::SQLException&) {
        return false;
    }
}



void ConnectionPool::ReleaseConnection(std::unique_ptr<sql::Connection> conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    pool_.push(std::move(conn));
    condVar_.notify_one();
}


DBConfig ConnectionPool::ReadDBConfig(const std::string& file_name) {
    std::ifstream in_file(file_name);
    if (!in_file.is_open()) {
        throw ConfigFileException("Файл конфигурации не найден");
    }

    DBConfig config;
    std::string line;
    while (std::getline(in_file, line)) {
        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);
            if (key == "host") {
                config.host = value;
            } else if (key == "dbname") {
                config.dbname = value;
            } else if (key == "user") {
                config.user = value;
            } else if (key == "password") {
                config.password = value;
            }
        }
    }
    in_file.close();
    return config;
}

// Создание нового соединения
std::unique_ptr<sql::Connection> ConnectionPool::CreateConnection(const DBConfig& config) {
    std::string tcp_ip = "tcp://" + config.host;
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    std::unique_ptr<sql::Connection> conn(driver->connect(tcp_ip, config.user, config.password));
    conn->setSchema(config.dbname);
    malloc_trim(0);
    return std::move(conn);
}

// Закрытие всех соединений перед завершением работы программы
void ConnectionPool::CloseAllConnections() {
    while (!pool_.empty()) {
        auto conn = std::move(pool_.front());
        pool_.pop();
        if (conn) {
            conn.get()->close();
            conn.reset();
        }
    }

    malloc_trim(0);
}



// Конструктор Transaction
Transaction::Transaction(sql::Connection* conn)
    : conn_(conn), committed_(false) {
    conn_->setAutoCommit(false); // Отключаем автоматический коммит
}

// Деструктор Transaction
Transaction::~Transaction() {
    if (!committed_) {
        try {
            conn_->rollback(); // Откатываем изменения, если не закоммичено
            Logger::getInstance().log(
                "Ошибка отката транзакции в файле " + std::string(__FILE__) +
                " на строке " + std::to_string(__LINE__),
                "../logs/error_transaction.log"
            );
        } catch (const sql::SQLException& e) {
            Logger::getInstance().log("Ошибка отката изменений: " + std::string(e.what()) +
                " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
                "../logs/error_transaction.log");
        }
    }
}

// Коммит транзакции
void Transaction::commit() {
    try {
        conn_->commit(); // Фиксируем изменения
        committed_ = true;
    } catch (const sql::SQLException& e) {
        Logger::getInstance().log("Ошибка коммита изменений: " + std::string(e.what()) +
            " в файле " + __FILE__ + " строке " + std::to_string(__LINE__),
            "../logs/error_transaction.log");
        throw; // Перебрасываем исключение дальше
    }
}
