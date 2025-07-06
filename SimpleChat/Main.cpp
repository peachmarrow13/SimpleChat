#include "Utils.h"
#include <iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>



#pragma comment(lib, "ws2_32.lib")

std::string Start();
bool Connect(const std::string& ip);
bool Host();

int main() {
    while (true) {
        std::cout << "Are you (1)hosting or (2)joining a chat?: ";
        int choice;
        std::cin >> choice;

        if (choice != 1 && choice != 2) {
            std::cout << "Invalid choice. Please restart the program and choose either 1 or 2.\n";
            return 1;
        }
        else if (choice == 1) {
            if (Host())
                std::cout << "Client connected successfully.\n";
            else
                std::cout << "Failed to host.\n";
        }
        else if (choice == 2) {
            std::string ip = Start();
            if (Connect(ip))
                std::cout << "Connection established successfully.\n";
            else
                std::cout << "Failed to establish connection.\n";
        }

        break;  // remove this if you want to loop back to main menu
    }

    return 0;
}


std::string Start() {
	std::string ChatterIP;

	Utils::clearScreen();
	Utils::printSeparator();
	std::cout << "Welcome to SimpleChat!\n";
	std::cout << "This is a simple chat application.\n";
	std::cout << "You can chat with a real person.\n";
	Utils::printSeparator();

    while (true) {



        std::cout << "Who would you like to chat with?\nType their IP to connect\n";

        std::cin >> ChatterIP;

        if (ChatterIP.empty() or !ChatterIP.contains("192.168")) {
            std::cout << "\nInvalid IP address. Please enter a valid IP address.\n\n";
			continue; // Prompt again for a valid IP address
        }
        else
            return ChatterIP;
    }
}

bool Connect(const std::string& ip) {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);

    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Invalid IP address format.\n";
        closesocket(sock);
        WSACleanup();
        return false;
    }

    std::cout << "Connecting to " << ip << "...\n";
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect.\n";
        closesocket(sock);
        WSACleanup();
        return false;
    }

    std::cout << "Connected successfully!\n";

    // TODO: Send/receive chat messages here

    closesocket(sock);
    WSACleanup();
    return true;
}

bool Host() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return false;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.\n";
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    serverAddr.sin_port = htons(5555);        // Choose a port (match client!)

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed.\n";
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    if (listen(listenSocket, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed.\n";
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    std::cout << "Waiting for someone to connect...\n";
    SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to accept connection.\n";
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    std::cout << "Client connected!\n";

    // Optional: receive a test message
    char buffer[512];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "Client says: " << buffer << "\n";
    }

    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return true;
}
