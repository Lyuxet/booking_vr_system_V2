#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "test_insert_data.h"
#include "test_delete_data.h"
#include "test_update_data.h"
#include <iostream>
#include <thread>
#include <vector>




int main() {
    try {
        // Путь к файлу конфигурации
        std::string configPath = "db_config.conf";

        // Инициализируем пул соединений
        ConnectionPool pool(10, configPath);
        pool.Init_pool();

        const int numThreads = 10;
        std::vector<std::thread> threads;

        TestIncludeOpenArena(pool, numThreads, threads);
        TestUpdateOpenArena(pool, numThreads, threads);
        TestDelete(pool, numThreads, threads);
        
    } 
    catch (const sql::SQLException& e) {
        std::cerr << "Main Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
