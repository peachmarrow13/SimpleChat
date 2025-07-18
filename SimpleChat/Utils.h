#pragma once
#include <string>

class Utils {
public:
	int getRandomNumber(int min, int max);
	static int Clamp(int value, int min, int max);
	static void delay(unsigned int milliseconds);
	static void clearScreen();
	static void printSeparator(char c = '-', int length = 30);
	static void SaveToFile(const std::string& filename, std::string pet);
	static bool IsSave(const std::string& filename);
	static void Exit(int Code, const std::string& Message);
};