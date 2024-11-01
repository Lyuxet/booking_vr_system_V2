cmake_minimum_required(VERSION 3.10)
project(Booking_vr_server)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к заголовочным файлам и библиотекам MySQL Connector, Boost, nlohmann/json, httplib и Mailio
set(C_Connector_include_dir "/usr/include/mysql-cppconn")
set(Header_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(Header_test_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/test")

# Пути к Boost
find_package(Boost 1.71.0 REQUIRED COMPONENTS system thread json container)


# Добавляем пути к директориям с заголовочными файлами
include_directories(${C_Connector_include_dir})
include_directories(${Header_include_dir})
include_directories(${Header_test_include_dir})
include_directories(${Boost_INCLUDE_DIRS})

# Путь к исходникам
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

# Создаем исполняемый файл
add_executable(Booking_vr_server ${SOURCES})

# Ищем библиотеки MySQL Connector
find_library(C_Connector_LIB NAMES mysqlcppconn PATHS /usr/lib/x86_64-linux-gnu)

# Проверяем, что библиотека найдена
if (NOT C_Connector_LIB)
    message(FATAL_ERROR "MySQL Connector C++ library not found!")
endif()

# Линкуем MySQL Connector, Boost и Mailio с нашим проектом
target_link_libraries(Booking_vr_server ${C_Connector_LIB} Boost::system Boost::thread Boost::json Boost::container)

# Копируем файл конфигурации в папку с исполняемым файлом после сборки
add_custom_command(TARGET Booking_vr_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/db_config.conf"  # Путь к файлу конфигурации
    "$<TARGET_FILE_DIR:Booking_vr_server>/db_config.conf"  # Куда копируем
)
