#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <mutex>

class Server {
public:
    Server();
    ~Server();

    void start_server();
    void handle_clients();

private:
    void bind_server_socket();
    void listen_for_clients();
    void accept_client();
    void receive_messages(int client_socket, const std::string& client_nickname);
    void broadcast_message(const std::string& sender_nickname, const std::string& message);
    void send_message(const std::string& recipent_nickname, const std::string& message);

private:
    int server_socket;
    std::vector<int> client_sockets;
    std::vector<std::string> client_nicknames;
    std::mutex client_mutex;
};

#endif //  SERVER_HPP
