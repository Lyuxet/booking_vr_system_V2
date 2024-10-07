#include "Connect_to_DB.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <fstream>
#include <iostream>

// Конструктор
ConnectionPool::ConnectionPool(size_t poolSize, const std::string& configFilePath)
    : poolSize_(poolSize), configFilePath_(configFilePath) {
    Init_pool();
}

// Инициализация пула соединений
void ConnectionPool::Init_pool() {
    try
    {
        DBConfig config = ReadDBConfig(configFilePath_);
        for (size_t i = 0; i < poolSize_; ++i) {
            pool_.push(CreateConnection(config));
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
}

// Получение соединения из пула
std::shared_ptr<sql::Connection> ConnectionPool::GetConnection() {
    std::unique_lock<std::mutex> lock(mutex_);
    condVar_.wait(lock, [this] { return !pool_.empty(); });

    auto conn = pool_.front();
    pool_.pop();

    if (!isConnectionActive(conn)) {
        conn.reset(); 
        conn = CreateConnection(ReadDBConfig(configFilePath_));  
    }

    return conn;
}

bool ConnectionPool::isConnectionActive(std::shared_ptr<sql::Connection> conn){
    try {
        std::shared_ptr<sql::Statement> stmt(conn->createStatement());
        stmt->executeQuery("SELECT 1");  
        return true;  
    } catch (const sql::SQLException&) {
        return false;  
    }
}

// Возвращение соединения в пул
void ConnectionPool::ReleaseConnection(std::shared_ptr<sql::Connection> conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    pool_.push(std::move(conn));
    condVar_.notify_one(); 
}

// Чтение конфигурации из файла
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
            }
            else if (key == "dbname") {
                config.dbname = value;
            }
            else if (key == "user") {
                config.user = value;
            }
            else if (key == "password") {
                config.password = value;
            }
        }
    }
    return config;
}

// Создание нового соединения
std::shared_ptr<sql::Connection> ConnectionPool::CreateConnection(const DBConfig& config) {
    std::string tcp_ip = "tcp://" + config.host;
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    auto conn = std::shared_ptr<sql::Connection>(driver->connect(tcp_ip, config.user, config.password));
    conn->setSchema(config.dbname);
    return conn;
}


// Конструктор
Transaction::Transaction(std::shared_ptr<sql::Connection> conn)
    : conn_(conn), committed_(false) {
    conn_->setAutoCommit(false); // Отключаем автоматический коммит
}

// Деструктор
Transaction::~Transaction() {
    if (!committed_) {
        try {
            conn_->rollback(); // Откатываем изменения, если не закоммичено
            std::cerr << "Transaction rolled back due to failure." << std::endl;
        } catch (const sql::SQLException& e) {
            std::cerr << "Error during rollback: " << e.what() << std::endl;
        }
    }
}

// Коммит транзакции
void Transaction::commit() {
    try {
        conn_->commit(); // Фиксируем изменения
        committed_ = true;
    } catch (const sql::SQLException& e) {
        std::cerr << "Error during commit: " << e.what() << std::endl;
        throw; // Перебрасываем исключение дальше
    }
}