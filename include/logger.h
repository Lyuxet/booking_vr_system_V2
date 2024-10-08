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
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // Метод для записи сообщения в конкретный лог файл
    void log(const std::string& message, const std::string& logFileName) {
        std::lock_guard<std::mutex> lock(mutex_);
        // Если файл не открыт, открыть его
        if (logFiles_.find(logFileName) == logFiles_.end()) {
            std::ofstream logFile(logFileName, std::ios::app);
            if (!logFile) {
                throw std::runtime_error("Не удалось открыть файл логов: " + logFileName);
            }
            logFiles_[logFileName] = std::move(logFile);
        }

        // Запись сообщения в соответствующий файл
        logFiles_[logFileName] << currentDateTime() << " " << message << std::endl;

        if (!logFiles_[logFileName]) {
            throw std::runtime_error("Ошибка записи в файл логов: " + logFileName);
        }
    }

    // Метод для закрытия всех лог файлов
    void closeAll() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& pair : logFiles_) {
            if (pair.second.is_open()) {
                pair.second.close();
            }
        }
    }

private:
    std::map<std::string, std::ofstream> logFiles_;  // Хранит файлы логов по их именам
    std::mutex mutex_;

    // Приватный конструктор для Singleton
    Logger() = default;  // Убираем жестко заданный путь к файлу

    // Запрет копирования и присваивания
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Получение текущей даты и времени в формате YYYY-MM-DD HH:MM:SS
    std::string currentDateTime() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm buf;
#ifdef _WIN32
        localtime_s(&buf, &in_time_t);
#else
        localtime_r(&in_time_t, &buf);
#endif
        std::ostringstream ss;
        ss << std::put_time(&buf, "%Y-%m-%d %X");
        return ss.str();
    }

    // Деструктор для закрытия всех файлов
    ~Logger() {
        closeAll(); // Закрываем все файлы логов
    }
};