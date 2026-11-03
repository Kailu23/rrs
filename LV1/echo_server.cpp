#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_fd) {
  char buffer[BUFFER_SIZE];
  while (true) {
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = read(client_fd, buffer, BUFFER_SIZE - 1);

    if (bytes_received <= 0) {
      std::cout << "Client disconnected.\n";
      break;
    }

    std::cout << "Received from client: " << buffer << " --> ";
    send(client_fd, buffer, bytes_received, 0);
    std::cout << "Echoed back to client: " << buffer << "\n";
  }

  close(client_fd);
}

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address = {AF_INET, htons(PORT), INADDR_ANY};
  int option_value = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int));
  bind(server_fd, (sockaddr*)&address, sizeof(address));
  listen(server_fd, 3);

  std::cout << "Server is running on port " << PORT << " and waiting for connections...\n";

  std::vector<std::thread> client_threads;

  while (true) {
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd >= 0) {
      std::cout << "Client connected.\n";
      // Start a new thread to handle the client
      client_threads.emplace_back(handle_client, client_fd);
    }
  }

  // Wait for all client threads to finish (never reached in this infinite loop)
  for (auto& t : client_threads) {
    t.join();
  }

  close(server_fd);
  return 0;
}
