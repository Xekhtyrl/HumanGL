#include "JSONParser.hpp"

std::string JSONToString(std::string path) {
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("JSONToString: File Could not be opened!");
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

std::string JSONTypeToString(JSONType type) {
	switch (type)
	{
		case JSONType::NONE:
			return "None";
		case JSONType::BOOL:
			return "Bool";
		case JSONType::STRING:
			return "String";
		case JSONType::NUMERIC:
			return "Numeric";
		case JSONType::ARRAY:
			return "Array";
		case JSONType::OBJECT:
			return "Object";
		default:
			return "Unknown";
	}
}

std::string TokenTypeToString(TokenType type) {
	switch (type)
	{
		case TokenType::LBRACKET:
			return "LBRACKET";
		case TokenType::RBRACKET:
			return "RBRACKET";
		case TokenType::LBRACE:
			return "LBRACE";
		case TokenType::RBRACE:
			return "RBRACE";
		case TokenType::COMMA:
			return "COMMA";
		case TokenType::COLON:
			return "COLON";
		case TokenType::STRING:
			return "STRING";
		case TokenType::NUMBER:
			return "NUMBER";
		case TokenType::TRUE:
			return "TRUE";
		case TokenType::FALSE:
			return "FALSE";
		case TokenType::NONE:
			return "NONE";
		case TokenType::EOI:
			return "EOI";
		default:
			return "Unknown";
	}
}