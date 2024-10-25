#include "booking_vr.h"
#include "logger.h"



namespace vr{


    void Booking::PrintInsertClient(){
    std::cout << "\n================ НОВЫЙ КЛИЕНТ ================" << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "Имя:               " << clients_.first_name << std::endl;
    std::cout << "Фамилия:           " << clients_.last_name << std::endl;
    std::cout << "Телефон:           " << clients_.phone << std::endl;
    std::cout << "Почта:             " << clients_.email << std::endl;
    std::string datetime = currentDateTime();
    std::cout << "Время регистрации: " << datetime << std::endl;      
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "================ Конец списка ================\n" << std::endl;
    }

    void Booking::PrintInsertBooking() {
        if (bookings_.empty()) {
            std::cout << "Нет бронирований для вставки." << std::endl;
            return;
        }

        std::cout << "=============== Забронированно ===============" << std::endl;
        std::cout << "--------------------------------------------------------------------------------------" << std::endl;
        std::cout << "Игра:             " << bookings_[0].name_game << std::endl;
        std::cout << "Тип:              " <<  bookings_[0].type_game << std::endl;

        for (const auto& booking : bookings_) {
            std::cout << "Дата и время:     " << booking.date_game << " " << booking.time_game << "         ";
            std::cout << "Количество игроков:    " <<  booking.players_count << std::endl;
        }
        std::cout << "Кем забронирован: " << bookings_[0].who_reservation << std::endl;
        std::string datetime = currentDateTime(); 
        std::cout << "Время брони:      " << datetime << std::endl;      
        std::cout << "--------------------------------------------------------------------------------------" << std::endl;
        std::cout << "================ Конец списка ================" << std::endl;
    }

    void Booking::PrintDeleteBooking() {
        if (bookings_.empty()) {
            std::cout << "Нет бронирований для удаления." << std::endl;
            return;
        }

        std::cout << "=== Список удаленных бронирований ===" << std::endl;
        std::cout << "Игра:            " <<  bookings_[0].name_game << std::endl;
        std::cout << "Тип:             " <<  bookings_[0].type_game << std::endl;
        for (const auto& booking : bookings_) {
            std::cout << "Дата и время:    " <<  bookings_[0].date_game << " " <<  bookings_[0].time_game << std::endl;
        }
        std::cout << "=== Конец списка ===" << std::endl;
        std::cout << "-------------------------------" << std::endl;

    }

    void Booking::PrintUpdateBooking() {
        if (bookings_.empty()) {
            std::cout << "Нет бронирований для обновления." << std::endl;
            return;
        }

        std::cout << "=== Список обновленных бронирований ===" << std::endl;
        for (const auto& booking : bookings_) {
            std::cout << "Обновлено бронирование:" << std::endl;
            std::cout << "Игра:            " << booking.name_game << std::endl;
            std::cout << "Тип:             " << booking.type_game << std::endl;
            std::cout << "С:               " << booking.current_date_game << " " << booking.current_time_game << std::endl;
            std::cout << "На:              " << booking.date_game << " " << booking.time_game << std::endl;
            std::cout << "-------------------------------" << std::endl;
        }
        std::cout << "=== Конец списка ===" << std::endl;
    }

}