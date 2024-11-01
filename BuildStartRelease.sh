#!/bin/bash

# Удалить папку build, если она существует
rm -rf build

# Создать папку build
mkdir build

# Перейти в папку build
cd build

# Настроить проект с помощью CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Построить проект
cmake --build . --config Release

# Запустить исполняемый файл
./Booking_vr_server
