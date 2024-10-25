#include "logger.h"


Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string& message, const std::string& logFileName) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFiles_.find(logFileName) == logFiles_.end()) {
        std::ofstream logFile(logFileName, std::ios::app);
        if (!logFile) {
            throw std::runtime_error("Не удалось открыть файл логов: " + logFileName);
        }
        logFiles_[logFileName] = std::move(logFile);
    }

    logFiles_[logFileName] << currentDateTime() << " " << message << std::endl;

    if (!logFiles_[logFileName]) {
        throw std::runtime_error("Ошибка записи в файл логов: " + logFileName);
    }
}

void Logger::closeAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& pair : logFiles_) {
        if (pair.second.is_open()) {
            pair.second.close();
        }
    }
}


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
Logger::~Logger() {
        closeAll(); // Закрываем все файлы логов
}
