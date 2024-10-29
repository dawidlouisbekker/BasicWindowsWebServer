#include <fstream>
#include <strstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "iphlpapi.lib")
//#pragma comment(lib, "winhttp.lib")

const int PORT = 8080;

std::string LoadHtmlFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return ""; // Return an empty string if file not found
    }

    std::strstream buffer;
    buffer << file.rdbuf(); // Read the file's content into a stringstream
    return buffer.str(); // Convert to string and return
}

int StartHttpServer(bool& isFinished) {
    WSADATA wsaData;
    SOCKET listeningSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 0;
    }

    // Create a socket
    listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 0;
    }

    // Set up the sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Bind the socket
    if (bind(listeningSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return 0;
    }

    // Listen for incoming connections
    listen(listeningSocket, SOMAXCONN);

    std::cout << "Server is listening on port 8080..." << std::endl;

    // Accept a client connection


        // Receive data in a loop
    while (!isFinished) {
        clientSocket = accept(listeningSocket, (sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed." << std::endl;
            continue; // Continue to accept new connections
        }

        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate the string
            std::cout << "Received:\n" << buffer << std::endl;

            // Load HTML content from the file
            std::string htmlContent = LoadHtmlFile("base.html");
            if (htmlContent.empty()) {
                // Handle file read error by sending a simple error response
                const std::string errorResponse =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "404 Not Found\n";
                send(clientSocket, errorResponse.c_str(), errorResponse.length(), 0);
            }
            else {
                // Construct a successful HTTP response with the HTML content
                std::string httpResponse =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + std::to_string(htmlContent.length()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" + htmlContent;

                // Send the HTTP response back to the client
                send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
            }
        }
        //break; // Exit the receiving loop after responding

        // Send the HTTP response back to the client







       closesocket(clientSocket);

    }

    //isFinished = true;
    closesocket(listeningSocket);
    WSACleanup();
}

int main() {
    bool isFinished = false;
    int i = StartHttpServer(isFinished);
    return 0;
}
