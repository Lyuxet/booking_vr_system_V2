#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <map>  // Для хранения лог-файлов с их именами

class Logger {
public:
    // Получение единственного экземпляра логгера (Singleton)
    static Logger& getInstance();

    // Метод для записи сообщения в конкретный лог файл
    void log(const std::string& message, const std::string& logFileName);

    // Метод для закрытия всех лог файлов
    void closeAll();

private:
    std::map<std::string, std::ofstream> logFiles_;  // Хранит файлы логов по их именам
    std::mutex mutex_;

    // Приватный конструктор для Singleton
    Logger() = default;  // Убираем жестко заданный путь к файлу

    // Запрет копирования и присваивания
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Получение текущей даты и времени в формате YYYY-MM-DD HH:MM:SS
    std::string currentDateTime();
    // Деструктор для закрытия всех файлов
    ~Logger();
};