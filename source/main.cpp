#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "test_insert_data.h"
#include <iostream>
#include <thread>
#include <vector>




int main() {
    try {
        // Путь к файлу конфигурации
        std::string configPath = "db_config.conf";

        // Инициализируем пул соединений
        ConnectionPool pool(20, configPath);
        pool.Init_pool();

        TestIncludeOpenArena(pool);
        TestDelete(pool);
        
    } 
    catch (const sql::SQLException& e) {
        std::cerr << "Main Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
