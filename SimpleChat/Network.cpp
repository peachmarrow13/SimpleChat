#include <iostream>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <thread>
#include <ws2tcpip.h>
#include "Utils.h"
#include "Network.h"
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#include <fstream>
#pragma comment(lib, "ws2_32.lib")

std::string Network::YourName = "";
std::string Network::FriendName = "";

bool Network::Connect(const std::string& ip) {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\nL79";
        return false;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failedL87\n";
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);

    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
        using namespace std;

        std::cerr << "Invalid IP address format.L100\n";
        closesocket(sock);
        WSACleanup();  
        return false;
    }

    std::cout << "Connecting to " << ip << "...\n";
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect.L108\n";
        closesocket(sock);
        WSACleanup();
        return false;
    }

    std::cout << "Connected successfully! Starting chat...\n";
    ChatLoop(sock);

    closesocket(sock);
    WSACleanup();
    return true;
}

bool Network::Host() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.L128\n";
        return false;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.L136\n";
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    serverAddr.sin_port = htons(5555);        // Choose a port (match client!)

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed.L147\n";
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
        std::cerr << "Failed to accept connection.L163\n";
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    std::cout << "Client connected!\n";

    std::cout << "Client connected! Starting chat...\n";
    ChatLoop(clientSocket);

    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return true;
}

void Network::ReceiveLoop(SOCKET sock) {
    char buffer[512];
    while (true) {
        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "\nConnection closed.\n";
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "\n" << buffer << "\n" << "\033[36m" << Network::YourName << "\033[0m" << " > ";
    }
}


void Network::SendLoop(SOCKET sock) {
    std::string msg;
    while (true) {
        std::cout << "\033[36m" << Network::YourName << "\033[0m" << " > ";
        std::getline(std::cin, msg);
        if (msg == "/quit") {
            shutdown(sock, SD_SEND);
            break;
        }
        
		if (msg.empty()) {
			std::cout << "Message cannot be empty.\n";
			continue;
		}

        std::string fullMsg = "\033[36m" + Network::YourName + "\033[0m" + ": " + msg;
        send(sock, fullMsg.c_str(), (int)fullMsg.length(), 0);
    }
}


void Network::ChatLoop(SOCKET sock) {
    // Send your username first
    send(sock, Network::YourName.c_str(), (int)Network::YourName.length(), 0);

    // Receive friend's username
    char nameBuffer[64];
    int bytesReceived = recv(sock, nameBuffer, sizeof(nameBuffer) - 1, 0);
    if (bytesReceived <= 0) {
        std::cout << "Failed to receive username.\n";
        return;
    }
    nameBuffer[bytesReceived] = '\0';
    Network::FriendName = nameBuffer;

    std::cout << "You are now chatting with: " << "\033[36m" << Network::FriendName << "\033[0m" << "\n";

    std::thread receiver(Network::ReceiveLoop, sock);
    SendLoop(sock);
    shutdown(sock, SD_SEND);
    receiver.join();
}


std::string Network::GetPublicIP() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[2048];
    std::string result;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return "WSAStartup failed";
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return "Socket creation failed";
    }

    struct hostent* host = gethostbyname("api.ipify.org");
    if (!host) {
        closesocket(sock);
        WSACleanup();
        return "Failed to resolve host";
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    memcpy(&server.sin_addr, host->h_addr, host->h_length);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        closesocket(sock);
        WSACleanup();
        return "Connection failed";
    }

    std::string request = "GET / HTTP/1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n";
    send(sock, request.c_str(), (int)request.length(), 0);

    int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::string response(buffer);

        // Extract IP from the response
        size_t ipStart = response.find("\r\n\r\n");
        if (ipStart != std::string::npos)
            result = response.substr(ipStart + 4);
        else
            result = "Failed to parse response";
    }
    else {
        result = "No response received";
    }

    closesocket(sock);
    WSACleanup();
    return result;
}

std::string Network::GetPrivateIP() {
    WSADATA wsaData;
    char hostname[256];
    std::string ip = "Unavailable";

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return "WSAStartup failed";

    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        WSACleanup();
        return "gethostname failed";
    }

    struct hostent* host = gethostbyname(hostname);
    if (host == nullptr) {
        WSACleanup();
        return "gethostbyname failed";
    }

    // Loop over all IPs (just in case multiple interfaces exist)
    for (int i = 0; host->h_addr_list[i] != nullptr; ++i) {
        struct in_addr addr;
        memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
        ip = inet_ntoa(addr); // convert to string
        break; // just return the first one
    }

    WSACleanup();
    return ip;
}