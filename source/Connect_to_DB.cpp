#include "Connect_to_DB.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <fstream>
#include <iostream>
ConnectionPool::ConnectionPool(size_t poolSize, const std::string& configFilePath)
    : poolSize_(poolSize), configFilePath_(configFilePath) {}

void ConnectionPool::Init_pool() {
    DBConfig config = ReadDBConfig(configFilePath_);
    for (size_t i = 0; i < poolSize_; ++i) {
        pool_.push(CreateConnection(config));
    }
}

std::unique_ptr<sql::Connection> ConnectionPool::GetConnection() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pool_.empty()) {
        throw std::runtime_error("No available connections in the pool.");
    }
    std::unique_ptr<sql::Connection> conn = std::move(pool_.front());
    pool_.pop();
    return conn;
}

void ConnectionPool::ReleaseConnection(std::unique_ptr<sql::Connection> conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    pool_.push(std::move(conn));
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
    in_file.close();
    return config;
}

std::unique_ptr<sql::Connection> ConnectionPool::CreateConnection(const DBConfig& config) {
    std::string tcp_ip = "tcp://" + config.host;
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    std::unique_ptr<sql::Connection> conn(driver->connect(tcp_ip, config.user, config.password));
    conn->setSchema(config.dbname);
    return conn;
}

Transaction::Transaction(std::shared_ptr<sql::Connection> conn)
    : conn_(conn), committed_(false) {
    conn_->setAutoCommit(false);
}

Transaction::~Transaction() {
    if (!committed_) {
        conn_->rollback();
        std::cerr << "Transaction rolled back due to failure." << std::endl;
    }
}

void Transaction::commit() {
    conn_->commit();
    committed_ = true;
}