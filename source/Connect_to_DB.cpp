#include "Connect_to_DB.h"
#include <filesystem>
#include <fstream>
#include <iostream>

static DBConfig ReadDBConfig(const std::string& file_name) {
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

    if (config.host.empty() || config.dbname.empty() || config.user.empty() || config.password.empty()) {
        throw std::invalid_argument("Некоторые параметры конфигурации отсутствуют или пусты");
    }

    return config;
}

Connection::Connection() : conn_(nullptr) {}

void Connection::Connect_database() {
    try {
        std::string configPath = std::filesystem::current_path().string() + "/db_config.conf";
        DBConfig config = ReadDBConfig(configPath);

        std::string tcp_ip = "tcp://" + config.host;

        sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
        conn_ = std::unique_ptr<sql::Connection>(driver->connect(tcp_ip, config.user, config.password));
        conn_->setSchema(config.dbname);

        std::cout << "Connected to database: " << config.dbname << std::endl;
    }
    catch (const sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what() << std::endl;
        throw;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        throw;
    }
}

void Connection::Disconnect_database() {
    if (conn_) {
        conn_->close();
        conn_.reset();
        std::cout << "Disconnected from database." << std::endl;
    }
}

sql::Connection* Connection::Check_Connect() const noexcept {
    return conn_.get();
}

Connection::~Connection() {
    Disconnect_database();
}
