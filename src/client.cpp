#include "client.hpp"

#include <iostream>
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

#include "common.hpp"

Client::Client() : client_socket(0) {
    spdlog::info("Client initialized");
}

Client::~Client() {
    close(client_socket);
}

void Client::create_client_socket() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Failed to connect client socket");
        exit(EXIT_FAILURE);
    }

    spdlog::info("Client socket created");
}

void Client::connect_to_server(const std::string &server_address) {
    sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, server_address.c_str(), &server_sockaddr.sin_addr);

    if (connect(client_socket,  reinterpret_cast<struct sockaddr*>(&server_sockaddr), sizeof(server_sockaddr)) < 0) {
        perror("Failed to connect to server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    spdlog::info("Connected to server");
}

void Client::send_message(const std::string &message) {
    send(client_socket, message.c_str(), message.length(), 0);
    spdlog::info("Sent message to the server: {}", message);
}

void Client::send_private_message(const std::string &recipient_nickname, const std::string &message) {
    send(client_socket, ("@" + recipient_nickname + " " + message).c_str(),
         recipient_nickname.length() + message.length() + 2, 0);
    spdlog::info("Sent private message to {}: {}", recipient_nickname, message);
}

void Client::receive_messages() {
    char buffer[BUFFER_SIZE];
    while (true) {
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            spdlog::error("Server disconnected");
            break;
        }

        buffer[bytes_received] = '\0';
        spdlog::info("Server: {}", buffer);
    }
}