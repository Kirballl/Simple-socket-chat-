#include "server.hpp"

#include <spdlog/spdlog.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <thread>

#include "common.hpp"

Server::Server() : server_socket(0) {
    spdlog::info("Server initialized");
}

Server::~Server() {}

void Server::start_server() {
    bind_server_socket();
    listen_for_clients();
    handle_clients();
}

void Server::bind_server_socket() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Failed to create server socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(SERVER_PORT);
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, reinterpret_cast<struct sockaddr*>(&server_sockaddr),
            sizeof(server_sockaddr)) < 0) {
        perror("Falied to bind server socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    spdlog::info("Server socket bound");
}

void Server::listen_for_clients() {
    listen(server_socket, MAX_CLIENTS);
    spdlog::info("Server is listening for clients ...");
}

void Server::handle_clients() {
    while (true) {
        accept_client();
    }
}

void Server::accept_client() {
    sockaddr_in client_sockaddr;
    socklen_t client_size = sizeof(client_sockaddr);

    int client_socket = accept(server_socket, reinterpret_cast<struct sockaddr*>(&client_sockaddr), &client_size);
    if (client_socket < 0) {
        perror("Failed to accept client");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    client_sockets.push_back(client_socket);
    std::string client_nickname = "User" + std::to_string(client_socket);
    client_nicknames.push_back(client_nickname);

    spdlog::info("Accepted client: {}", client_nickname);

    std::thread(&Server::receive_messages, this, client_socket, client_nickname).detach();
}

void Server::receive_messages(int client_socket, const std::string &client_nickname) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            spdlog::error("Client {} disconnected", client_nickname);
            break;
        }

        //buffer[bytes_received] = '\0';
        std::string message(buffer);

        if (buffer[0] == '@') {
            std::string recipient_nickname = message.substr(1, message.find(' ') - 1);
            std::string private_message = message.substr(message.find(' ') + 1);
            send_message(recipient_nickname, client_nickname + " (private): " + private_message);
        } else {
            broadcast_message(client_nickname, buffer);
        }
    }
}

void Server::broadcast_message(const std::string &sender_nickname, const std::string &message) {
    std::lock_guard<std::mutex> lock(client_mutex);

    for (const auto& nickname : client_nicknames) {
        if (nickname != sender_nickname) {
            send_message(nickname, sender_nickname + ": " + message);
        }
    }

    spdlog::info("Broadcasted message from {} to all clients: {}", sender_nickname, message);
}

void Server::send_message(const std::string &recipent_nickname, const std::string &message) {
    auto it = std::find(client_nicknames.begin(), client_nicknames.end(), recipent_nickname);
    if (it == client_nicknames.end()) {
        spdlog::error("Recipient {} not found", recipent_nickname);
        return;
    }
    int recipient_socket = client_sockets[std::distance(client_nicknames.begin(), it)];
    send(recipient_socket, message.c_str(), message.length(), 0);
}
