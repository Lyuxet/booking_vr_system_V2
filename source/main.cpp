#include "Connect_to_DB.h"
#include "booking_vr.h"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <iomanip>  // Для std::put_time
#include <sstream>  // Для std::stringstream
#include <chrono>   // Для std::chrono

std::mutex mtx; // Мьютекс для синхронизации доступа к консоли

std::string add_hours_to_time(const std::string& time_str, int hours_to_add) {
    std::istringstream iss(time_str);
    std::tm tm = {};
    iss >> std::get_time(&tm, "%H:%M:%S");

    // Добавляем часы
    tm.tm_hour += hours_to_add;

    // Нормализуем дату и время
    std::mktime(&tm);

    // Форматируем результат в строку
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");

    return oss.str();
}

void WorkerThread(ConnectionPool& pool, int thread_index) {
    try {
        // Создаем данные клиента и бронирования
        Client_data client = {"Иван", "Иванов", "+79161234560", "ivanov@example.ru"};

        // Бронирование с уникальным временем
        Booking_data booking = {"OPEN", "ARENA QUEST", "2024-08-30", "13:00:00", 3, "Комментарий к игре"};
        
        // Изменяем время на основе индекса потока
        std::string new_time = add_hours_to_time(booking.time_game, thread_index);

        // Обновляем время в структуре бронирования
        booking.time_game = new_time;

        // Создаем объект Arena и добавляем данные
        Arena arena(pool);  // Передаем пул соединений в объект Arena
        arena.Add_date(client, booking);
        arena.Open_arena();

        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Поток " << thread_index << ": Данные успешно добавлены в базу данных с временем " << booking.time_game << "." << std::endl;
    } 
    catch (const sql::SQLException& e) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cerr << "Поток " << thread_index << ": Ошибка в потоке: " << e.what() << std::endl;
    }
}

int main() {
    try {
        // Путь к файлу конфигурации
        std::string configPath = "db_config.conf";

        // Инициализируем пул соединений
        ConnectionPool pool(5, configPath);
        pool.Init_pool();

        // Создаем и запускаем несколько потоков
        const int numThreads = 10;
        std::vector<std::thread> threads;
        
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(WorkerThread, std::ref(pool), i);
        }

        // Ожидаем завершения всех потоков
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    } 
    catch (const sql::SQLException& e) {
        std::cerr << "Main Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
