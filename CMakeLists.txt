cmake_minimum_required(VERSION 3.25)

project(MyChatApp)

set(CMAKE_CXX_STANDARD 17)

# Настройка путей к заголовочным файлам
include_directories(include)

add_subdirectory(third-party)
add_subdirectory(src)

