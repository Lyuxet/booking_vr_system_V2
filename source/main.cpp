#include "Connect_to_DB.h"
#include "booking_vr.h"
#include <iostream>



int main() {
    try {
        // Путь к файлу конфигурации
        std::string configPath = "db_config.conf";

        // Инициализируем пул соединений
        ConnectionPool pool(5, configPath);
        pool.Init_pool();

        // Создаем данные клиента и бронирования
        Client_data client = {"Иван", "Иванов", "+79161234560", "ivanov@example.ru"};
        Booking_data booking = {"","ARENA QUEST", "2024-08-30", "13:00:00", 3, "Комментарий к игре"};

        // Создаем объект Arena и добавляем данные
        Arena arena(pool);  // Передаем пул соединений в объект Arena
        arena.Add_date(client, booking);
        arena.Open_arena();

        std::cout << "Данные успешно добавлены в базу данных." << std::endl;
    } 
    catch (const sql::SQLException& e) {
        std::cerr << "Main Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
