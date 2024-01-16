# Добавляет исполняемый файл с именем "server"/"client"
add_executable(server main_server.cpp server.cpp)
add_executable(client main_client.cpp client.cpp)

# Добавляет директорию "include" в список директорий для поиска заголовочных файлов при сборке
target_include_directories(server PUBLIC include)
target_include_directories(client PUBLIC include)

# При линковке исполняемого файла должна использоваться библиотека spdlog
target_link_libraries(server spdlog)
target_link_libraries(client spdlog)
