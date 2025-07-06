#include "Utils.h"
#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <ctime>
#include <chrono>
#include <cstdio>
#include <string>

int Utils::getRandomNumber(int min, int max) {
	static std::random_device rd;  // Seed
	static std::mt19937 gen(rd()); // Mersenne Twister engine, whatever tf that is.
	std::uniform_int_distribution<> distr(min, max);
	return distr(gen);
}
int Utils::Clamp(int value, int min, int max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}
void Utils::delay(unsigned int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
void Utils::clearScreen() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}
void Utils::printSeparator(char c, int length) {
	if (length <= 0)
		length = 30; // Default length if not specified

	if (c == '\0')
		c = '-'; // Default character if not specified

	for (int i = 0; i < length; i++) std::cout << c;
	std::cout << std::endl;
}


void Utils::SaveToFile(const std::string& filename, std::string Input) {
	std::ofstream file(filename, std::ofstream::trunc);
	if (file.is_open()) {

		file << Input;

		file.close();
	}
	else {
		Utils::Exit(1, "Error opening file.\nTry running as administrator.\n"); // Exit if the file cannot be opened CODE: 1
	}
}


bool Utils::IsSave(const std::string& filename) {
	std::ifstream file(filename);
	std::string text;
	file >> text;
	if (text.empty()) {
		file.close();
		return 0; // Return False if the file is empty
	}
	file.close();
	return 1; // Return True if the file is not empty
}


void Utils::Exit(int Code, const std::string& Message) {
	std::cerr << "[FATAL ERROR] " << Message << std::endl;
	std::cout << "Exiting with code: " << Code << std::endl;
	std::cout << "Press Enter to continue..." << std::endl;
	std::cin.ignore(); std::cin.get();
	std::exit(Code);
}

/*
ERROR CODES:
1 - File not found or cannot be opened

*/