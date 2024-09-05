#include "Connect_to_DB.h"
#include "booking_vr.h"
#include <mutex>
#include <iomanip>  // Для std::put_time
#include <sstream>  // Для std::stringstream
#include <chrono>   // Для std::chrono
#include <thread>
#include <vector>

std::mutex mtxUpdate; // Мьютекс для синхронизации доступа к консоли

void WorkerThreadUpdate(ConnectionPool& pool, int thread_index) {
    try {
        // Создаем данные клиента и бронирования
        Client_data client = {"Иван", "Иванов", "+79161234560", "ivanov@example.ru"};

        client.current_phone = client.phone;

        // Бронирование с уникальным временем
        Booking_data booking = {"OPEN", "ARENA QUEST", "2024-08-30", "13:00:00", 1, "Комментарий к игре"};

        booking.current_time_game = booking.time_game;
        booking.current_date_game = booking.date_game;
        // Изменяем время на основе индекса потока
        std::string new_time  = add_hours_to_time(booking.current_time_game, thread_index);
        int new_players_count = 2;
        
        // Обновляем время в структуре бронирования
        booking.current_time_game = new_time;
        booking.time_game = new_time;
        booking.players_count = new_players_count;

        // Создаем объект Arena и добавляем данные
        Arena arena(pool);  // Передаем пул соединений в объект Arena
        arena.AddDataByInsertAndUpdate(client, booking);
        arena.Update();

    } 
    catch (const sql::SQLException& e) {
        std::lock_guard<std::mutex> lock(mtxUpdate);
        std::cerr << "Поток " << thread_index << ": Ошибка в потоке: " << e.what() << std::endl;
    }
}

void TestUpdateOpenArena(ConnectionPool& pool, const int numThreads, std::vector<std::thread>& threads){
    // Создаем и запускаем несколько потоков
    
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(WorkerThreadUpdate, std::ref(pool), i);
    }

    // Ожидаем завершения всех потоков
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

