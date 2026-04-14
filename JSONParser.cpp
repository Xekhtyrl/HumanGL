#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
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

enum class JSONType {
	NONE, BOOL, STRING, NUMERIC, ARRAY, OBJECT
};

enum class TokenType {
	LBRACKET, RBRACKET, LBRACE, RBRACE, COMMA, COLON, STRING, NUMBER, TRUE, FALSE, NONE, EOI
};

struct Token {
	TokenType type;
	std::string raw;
};

struct JSONValue {
	JSONType type;
	bool	boolVal;
	double	numVal;
	std::string strVal;
	std::vector<JSONValue> arrayValue;
	std::map<std::string, JSONValue> objectVal;
};

class JSONLexer {
	std::string src;
	int pos;

	Token parseStr() {
		Token res = {TokenType::STRING};
		while (src[pos] != '"'){
			if (src[pos] != '\\'){
				res.raw.append(1, src[pos]);
				pos++;
			}
			else if (pos < src.size() - 1 && src[pos + 1] == '"'){
				res.raw.append("\"");
				pos += 2;
			}
		}
	}
	Token parseNum() {
		Token res = {TokenType::NUMBER};
		bool dotPresent = false;
		if (src[pos] == '-'){
			pos++;
			res.raw.append("-");
		}
		while (isdigit(src[pos]) || src[pos] == '.'){
			if (src[pos] == '.'){
				if (dotPresent)
					throw std::runtime_error("JSON Lexer Error: invalid number");
				else
					dotPresent = true;
			}
			res.raw.append(1, src[pos]);
			pos++;
		}
	}

	Token checkBool() {
		Token res;
		if (src[pos] == 't'){
			if (src.substr(pos, 4) != "true")
				throw std::invalid_argument("JSON Lexer Error: Invalid Argument");
			pos += 4;
			res = {TokenType::TRUE};
		}
		else if (src[pos] == 'f'){
			if (src.substr(pos, 5) != "false")
				throw std::invalid_argument("JSON Lexer Error: Invalid Argument");
			pos += 5;
			res = {TokenType::FALSE};
		}
		return res;
	}

	public:
		JSONLexer() {pos = 0;};
		JSONLexer(std::string input) {src = input; pos = 0;};
		JSONLexer& operator=(const JSONLexer& oth) {
			if (this != &oth){
				src = oth.src;
				pos = oth.pos;
			}
			return *this;
		}
		Token nextToken(){
			if (pos >= src.size())
				return {TokenType::EOI};
			char c = src[pos];
			switch (c){
				case '{': pos++; return {TokenType::LBRACE};
				case '}': pos++; return {TokenType::RBRACE};
				case '[': pos++; return {TokenType::LBRACKET};
				case ']': pos++; return {TokenType::RBRACKET};
				case ',': pos++; return {TokenType::COMMA};
				case ':': pos++; return {TokenType::COLON};
				case '"': pos++; return parseStr();
				case '-' || isdigit(c): return parseNum();
				case 't': return checkBool();
				case 'f': return checkBool();
				default:
					throw std::runtime_error("Unexpected token found in Lexer: " + src[pos]);
			}
		}

};

class JSONParser {
	private:
		JSONLexer lexer;
		Token token;
	public:
		JSONParser(std::string input){
			lexer = JSONLexer(input);
		}
		JSONValue parseJSON();
		void advance() {token = lexer.nextToken();}
};