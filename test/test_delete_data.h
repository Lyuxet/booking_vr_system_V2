#include "Connect_to_DB.h"
#include "booking_vr.h"
#include <mutex>
#include <iomanip>  // Для std::put_time
#include <sstream>  // Для std::stringstream
#include <chrono>   // Для std::chrono
#include <thread>
#include <vector>

std::mutex mtxDelete; // Мьютекс для синхронизации доступа к консоли


void WorkerThreadDelete(ConnectionPool& pool, int thread_index) {
    try {
        Booking_data booking = {"OPEN", "ARENA QUEST", "2024-08-30", "13:00:00", 1, 1200, "Комментарий к игре"};
        
        // Изменяем время на основе индекса потока
        std::string new_time = add_hours_to_time(booking.time_game, thread_index);
        int new_players_count = booking.players_count + thread_index;
        
        // Обновляем время в структуре бронирования
        booking.time_game = new_time;
        
        // Создаем объект Arena и удаляем данные
        Arena arena(pool);  // Передаем пул соединений в объект Arena
        arena.AddDataByDelete(booking);
        arena.Delete();

    } 
    catch (const sql::SQLException& e) {
        std::lock_guard<std::mutex> lock(mtxDelete);
        std::cerr << "Поток " << thread_index << ": Ошибка в потоке: " << e.what() << std::endl;
    }
}

void TestDelete(ConnectionPool& pool, const int numThreads, std::vector<std::thread>& threads){
    // Создаем и запускаем несколько потоков
    
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(WorkerThreadDelete, std::ref(pool), i);
    }

    // Ожидаем завершения всех потоков
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}
