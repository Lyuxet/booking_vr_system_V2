#include "Connect_to_DB.h"
#include "booking_vr.h"
#include "test_insert_data.h"
#include "test_delete_data.h"
#include "test_update_data.h"

#include <iostream>
#include <thread>
#include <vector>
#include <httplib.h>



int main(){


    try {
        // Путь к файлу конфигурации
        std::string configPath = "db_config.conf";

        // Инициализируем пул соединений
        ConnectionPool pool(5, configPath);
        pool.Init_pool();

        // Создаем и запускаем несколько потоков
        const int numThreads = 10;
        std::vector<std::thread> threads;
        
        TestIncludeOpenArena(pool, numThreads, threads);
        TestUpdateOpenArena(pool, numThreads, threads);
        TestDelete(pool, numThreads, threads);
    } 
    catch (const sql::SQLException& e) {
        std::cerr << "Main Ошибка: " << e.what() << std::endl;
    }
    

    /*
   httplib::Server server;

   server.Post("/data", [](const httplib::Request& req, httplib::Response& res){
      
   });

   server.Post("/register", [](const httplib::Request& req, httplib::Response& res) {
        
   });

   server.Options("/*", [](const httplib::Request&, httplib::Response& res) {
        
    });

    std::cout << "Starting server on port 8080..." << std::endl;
    server.listen("0.0.0.0", 8080);

    */

}