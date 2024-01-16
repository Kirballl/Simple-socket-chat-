#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
    Client();
    ~Client();

    void create_client_socket();
    void connect_to_server(const std::string& server_address);
    void send_message(const std::string& message);
    void send_private_message(const std::string& recipient_nickname, const std::string& message);
    void receive_messages();

private:
    int client_socket;
};

#endif // CLIENT_HPP
