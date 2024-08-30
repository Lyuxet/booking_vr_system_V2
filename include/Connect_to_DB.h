#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <memory>
#include <string>
#include <stdexcept>

struct DBConfig {
    std::string host;
    std::string dbname;
    std::string user;
    std::string password;
};

class ConfigFileException : public std::runtime_error {
public:
    explicit ConfigFileException(const std::string& message)
        : std::runtime_error(message) {}
};

class Connection {
public:
    Connection();
    ~Connection();

    void Connect_database();
    void Disconnect_database();
    sql::Connection* Check_Connect() const noexcept;

private:
    std::unique_ptr<sql::Connection> conn_;
};

DBConfig ReadDBConfig(const std::string& file_name);
