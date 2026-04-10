#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
// #include "Includes/header.h"

void strTrim(std::string& str, std::string arr = " \t\r\n") {
	size_t start = str.find_first_not_of(arr);
	if (start > str.length()) {
		str = "";
		return;
	}
	size_t end = str.find_last_not_of(arr);
	str = str.substr(start, end - start + 1);	
}

static int checkContentOutofBracket(const std::string& content, int start, int size){
	std::string beforeFirstBraket = content.substr(start, size);

	std::string line;
	std::stringstream parentStream(beforeFirstBraket);
	while (getline(parentStream, line)){
		strTrim(line);
		if (line.size() > 0 && line.find("//") != 0)
			return -1;
	}
	return 0;
}

static int checkPartBeforeFirstBracket(const std::string& content) {
	size_t firstBracketPosition = content.find_first_of("{");
	if (firstBracketPosition == std::string::npos)
		return -1;
	if (checkContentOutofBracket(content, 0, firstBracketPosition) == -1)
		return -1;
	return firstBracketPosition;
}

static int checkPartAfterLastBracket(const std::string& content) {
	size_t lastBracketPosition = content.find_last_of("}");
	if (lastBracketPosition == std::string::npos)
		return -1;
	if (checkContentOutofBracket(content, lastBracketPosition + 1, content.size() - lastBracketPosition) == -1)
		return -1;
	return lastBracketPosition;
}

std::string JSONToString(std::string path, bool removeFirstBrackets = false) {
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("JSONToString: File Could not be opened!");
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	int startBracket = checkPartBeforeFirstBracket(content);
	if (startBracket == -1)
		throw std::runtime_error("JSONToString: Invalid JSON file");
	int endBracket = checkPartAfterLastBracket(content);
	if (endBracket == -1)
		throw std::runtime_error("JSONToString: Invalid JSON file after");
	return content;
}

int main(){
	try {
		std::string json = JSONToString("Ressources/Movement.json");
		// std:: cout << json << std::endl;
	}
	catch (std::exception& e){
		std::cerr << e.what() <<std::endl;
	}
	return 0;
}