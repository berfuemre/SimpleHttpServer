#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Function to send a basic HTTP response
void send_response(int client_socket, const std::string& status, const std::string& content_type, const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "\r\n";
    response << body;

    send(client_socket, response.str().c_str(), response.str().size(), 0);
}

// Function to handle client connections and serve files
void handle_client(int client_socket) {
    const int buffer_size = 1024;
    char buffer[buffer_size];
    read(client_socket, buffer, buffer_size);

    // Parse HTTP request (only handling GET requests here)
    std::istringstream request(buffer);
    std::string method, path, version;
    request >> method >> path >> version;

    if (method == "GET") {
        if (path == "/") {
            path = "/index.html";  // Default to index.html for the root path
        }

        // Remove the leading '/' from path to work with file system
        std::ifstream file("." + path);
        if (file) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            send_response(client_socket, "200 OK", "text/html", content);
        } else {
            std::string not_found = "<html><body><h1>404 Not Found</h1></body></html>";
            send_response(client_socket, "404 Not Found", "text/html", not_found);
        }
    } else {
        std::string method_not_allowed = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
        send_response(client_socket, "405 Method Not Allowed", "text/html", method_not_allowed);
    }

    close(client_socket);
}

// Function to start the server and listen for connections
void start_server(int port) {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << port << std::endl;

    // Main loop to accept clients
    while (true) {
        client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Spawn a new thread to handle each client connection
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();  // Allow thread to run independently
    }
}

int main() {
    int port = 8080;
    start_server(port);
    return 0;
}
