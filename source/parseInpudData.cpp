#include "parseInpudData.h"

#include <algorithm>
#include <sstream>
#include <iostream>

namespace vr {
    // Функция для разбиения строки на вектор строк по разделителю
    std::vector<std::string> split_string(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::istringstream stream(str);
        std::string item;

        while (std::getline(stream, item, delimiter)) {
            result.push_back(item);
        }

        return result;
    }

    // Функция для парсинга строки чисел в вектор целых чисел
    std::vector<int> parse_int_list(const std::string& str) {
        std::vector<int> result;
        std::vector<std::string> tokens = split_string(str, ',');

        for (const auto& token : tokens) {
            try {
                result.push_back(std::stoi(token));
            } catch (const std::exception&) {
                throw std::runtime_error("Invalid integer in list");
            }
        }

        return result;
    }

    // Функция для парсинга строки запроса в формате x-www-form-urlencoded
    std::unordered_map<std::string, std::string> parse_form_data(const std::string& target) {
        std::unordered_map<std::string, std::string> result;
        auto pos = target.find('?');
        if (pos != std::string::npos) {
            auto query = target.substr(pos + 1);  // Извлечение строки запроса после '?'
            std::istringstream stream(query);
            std::string key_value;

            while (std::getline(stream, key_value, '&')) {
                auto equal_pos = key_value.find('=');
                if (equal_pos != std::string::npos) {
                    std::string key = key_value.substr(0, equal_pos);
                    std::string value = key_value.substr(equal_pos + 1);

                    // Декодирование URL-элементов
                    std::replace(key.begin(), key.end(), '+', ' '); // Замена '+' на пробел
                    std::replace(value.begin(), value.end(), '+', ' '); // Замена '+' на пробел
                    
                    // Обработка кодировок
                    std::string::size_type pos = 0;
                    while ((pos = value.find('%', pos)) != std::string::npos) {
                        if (pos + 2 < value.size()) {
                            int code;
                            std::istringstream hex_stream(value.substr(pos + 1, 2));
                            hex_stream >> std::hex >> code;
                            value[pos] = static_cast<char>(code);
                            value.erase(pos + 1, 2);
                        }
                    }

                    // Добавление пары ключ-значение в unordered_map
                    result[key] = value;
                }
            }
        }
        return result;
    }
}