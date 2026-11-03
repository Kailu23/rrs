#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in server_address = {};
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

  if (connect(client_fd, (sockaddr*)&server_address, sizeof(server_address)) == 0) {
    std::cout << "Connected to the server at 127.0.0.1:" << PORT << "\n";
  } else {
    std::cerr << "Failed to connect to the server.\n";
    return 1;
  }

  char buffer[BUFFER_SIZE];
  while (true) {
    std::cout << "Enter a message to send to the server (type 'exit' to quit): ";
    std::string message;
    std::getline(std::cin, message);

    if (message == "exit") {
      std::cout << "Closing connection...\n";
      break;
    }

    // Send the message to the server
    send(client_fd, message.c_str(), message.size(), 0);
    std::cout << "Sent to server: " << message << "\n";

    // Receive the echo response from the server
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = read(client_fd, buffer, BUFFER_SIZE - 1);

    if (bytes_received > 0) {
      std::cout << "Received from server: " << buffer << "\n";
    } else {
      std::cout << "Server disconnected.\n";
      break;
    }
  }

  close(client_fd);
  std::cout << "Client shut down.\n";

  return 0;
}
