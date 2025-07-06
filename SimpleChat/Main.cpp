#include "Utils.h"
#include <iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <thread>
#include "Network.h"
#include <string>
#pragma comment(lib, "ws2_32.lib")

std::string Start();

std::string PickUsername();

int main() {
    while (true) {
        std::cout << "Are you(1)hosting or (2)joining a chat?: ";
        int choice;
        std::cin >> choice;

        Network::YourName = PickUsername(); // Get username before hosting

        if (choice != 1 && choice != 2) {
            std::cout << "Invalid choice. Please restart the program and choose either 1 or 2.\n";
            return 1;
        }
        else if (choice == 1) {
			
            if (Network::Host())
                std::cout << "Client connected successfully.\n";
            else
                std::cout << "Failed to host.L30\n"; std::cout << "Press Enter to continue..." << std::endl; std::cin.ignore(); std::cin.get();
            
        }
        else if (choice == 2) {
            std::string ip = Start();
            if (Network::Connect(ip))
                std::cout << "Connection established successfully.\n";
            else
                std::cout << "Failed to establish connection.L38\n"; std::cout << "Press Enter to continue..." << std::endl; std::cin.ignore(); std::cin.get();
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
		if (ChatterIP.contains("Local"))  // Check if the input contains Local
			ChatterIP = Network::GetPrivateIP(); // Get private IP

            return ChatterIP;
    }
}

std::string PickUsername() {
    std::cout << "Please enter your username: \n";

    std::getline(std::cin >> std::ws, Network::YourName); // Read username with spaces
    if (Network::YourName.empty()) {
		std::cout << "Username cannot be empty. Please try again.\n";
		return PickUsername(); // If username is empty, prompt again
    }
	return Network::YourName;
}