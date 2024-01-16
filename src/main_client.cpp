#include <iostream>
#include <thread>

#include "client.hpp"
#include "common.hpp"

bool is_client_connection_close(const std::string& msg) {
    return msg.find(CLIENT_CLOSE_CONNECTION_SYMBOL) != std::string::npos;
}


int main() {
    Client client;
    client.create_client_socket();

    std::string server_address;
    std::cout << "Enter server address: ";
    std::cin >> server_address;

    client.connect_to_server(server_address);

    std::string client_nickname;
    std::cout << "Enter your nickname: ";
    std::cin >> client_nickname;

    std::thread(&Client::receive_messages, &client).detach();

    std::string message;
    while (true) {
        std::getline(std::cin, message);

        if (is_client_connection_close(message)) {
            break;
        }

        if (message.size() > 1 && message[0] == '@') {
            std::string recipient_nickname = message.substr(1, message.find(' ') - 1);
            std::string private_message = message.substr(message.find(' ') + 1);

            client.send_private_message(recipient_nickname, private_message);
        } else {
            client.send_message(message);
        }
    }

    return 0;
}
