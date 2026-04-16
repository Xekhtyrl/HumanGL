#pragma once
#include <iostream>

enum class TokenType {
	LBRACKET, RBRACKET, LBRACE, RBRACE, COMMA, COLON, STRING, NUMBER, TRUE, FALSE, NONE, EOI
};

struct Token {
	TokenType type = TokenType::NONE;
	std::string raw = "";
};

class JSONLexer {
	std::string src;
	size_t pos;

	void skikWhiteSpaces() {
		while (isspace(src[pos]) && pos < src.size())
			pos++;
	}

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
		pos++;
		return res;
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
		return res;
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
			skikWhiteSpaces();
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
				case '-':
				case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
					return parseNum();
				case 't': return checkBool();
				case 'f': return checkBool();
				default:
					throw std::runtime_error("Unexpected token found in Lexer: " + src[pos]);
			}
		}

};
