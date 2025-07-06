#pragma once

#include <string>
#include <winsock2.h>

class Network {
public:
	static std::string YourName;
	static std::string FriendName;
	static bool Connect(const std::string& ip);
	static std::string GetPublicIP();
	static bool Host();
	static std::string GetPrivateIP();
private:
	static void ChatLoop(SOCKET sock);
	static void ReceiveLoop(SOCKET sock);
	static void SendLoop(SOCKET sock);
};