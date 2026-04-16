#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "JSONLexer.hpp"

enum class JSONType {
	NONE, BOOL, STRING, NUMERIC, ARRAY, OBJECT
};

std::string JSONToString(std::string path);
std::string JSONTypeToString(JSONType type);
std::string TokenTypeToString(TokenType type);


/// @brief Structure representing a JSON value, can be of type bool, string, numeric, array or object
struct JSONValue {
	JSONType type;
	bool	boolVal = false;
	double	numVal = 0.0;
	std::string strVal = "";
	std::vector<JSONValue> arrayValue = {};
	std::unordered_map<std::string, JSONValue> objectVal = {};

	/// @brief Check if a key exists in the object
	/// @param key The key to check
	/// @return true if key exists, false otherwise
	bool findKey(const std::string& key) {
		if (type != JSONType::OBJECT)
			throw std::runtime_error("JSONValue findKey: Not an object");
		return objectVal.find(key) != objectVal.end();
	}

	/// @brief Access operator for object type, will throw if not an object or if key does not exist.
	/// Use findKey before to check if key exists and avoid exception
	/// @param key The key to access
	/// @return The JSONValue associated with the key
	JSONValue operator[](const std::string& key) {
		if (type != JSONType::OBJECT)
			throw std::runtime_error("JSONValue operator[]: Not an object");
		if (objectVal.find(key) == objectVal.end())
			throw std::runtime_error("JSONValue operator[]: Key not found: " + key);
		return objectVal[key];
	}

	/// @brief Template function to get the value of the JSONValue in the desired type, will throw if type does not match or if type is invalid for getValue
	template<typename T>
	T getValue() {
		switch (type)
		{
			case JSONType::BOOL:
				if constexpr (std::is_same_v<T, bool>)
					return boolVal;
				break;
			case JSONType::NUMERIC:
				if constexpr (std::is_same_v<T, double>)
					return numVal;
				break;
			case JSONType::STRING:
				if constexpr (std::is_same_v<T, std::string>)
					return strVal;
				break;
			case JSONType::ARRAY:
				if constexpr (std::is_same_v<T, std::vector<JSONValue>>)
					return arrayValue;
				break;
			case JSONType::OBJECT:
				if constexpr (std::is_same_v<T, std::unordered_map<std::string, JSONValue>>)
					return objectVal;
				break;
			default:
				throw std::runtime_error("JSONValue getValue: Invalid type for getValue");
		}
		return T();
	}

	/// @brief Print the JSONValue in a human readable format, useful for debugging
	/// @param level The indentation level for pretty printing, default is 0 (no need to set it to another value, it will be handled by the function itself for nested objects/arrays)
	void print(int level = 0) {
		std::string indent(level * 2, ' ');
		std::string nextIndent((level + 1) * 2, ' ');
		
		switch (type)
		{
			case JSONType::BOOL:
				std::cout << (boolVal ? "true" : "false");
				break;
			case JSONType::NUMERIC:
				std::cout << numVal;
				break;
			case JSONType::STRING:
				std::cout << "\"" << strVal << "\"";
				break;
			case JSONType::ARRAY:
				if (arrayValue.empty()) {
					std::cout << "[]";
				} else {
					std::cout << "[";
					for (size_t i = 0; i < arrayValue.size(); i++){
						arrayValue[i].print(0);
						if (i < arrayValue.size() - 1)
							std::cout << ", ";
					}
					std::cout << "]";
				}
				break;
			case JSONType::OBJECT:
			{
				if (objectVal.empty()) {
					std::cout << "{}";
				} else {
					std::cout << "{" << std::endl;
					size_t count = 0;
					for (auto& pair : objectVal){
						std::cout << nextIndent << "\"" << pair.first << "\": ";
						(pair.second).print(level + 1);
						if (count < objectVal.size() - 1)
							std::cout << ",";
						std::cout << std::endl;
						count++;
					}
					std::cout << indent << "}";
				}
				break;
			}
			default:
				std::cout << "null";
		}
	}

	using Iterator = std::unordered_map<std::string, JSONValue>::iterator;
	/// @brief Begin iterator for object type, will throw if not an object
	/// @return Iterator to the beginning of the object
	Iterator begin() {
		if (type != JSONType::OBJECT)
			throw std::runtime_error("JSONValue begin: Not an object");
		return objectVal.begin();
	}

	/// @brief End iterator for object type, will throw if not an object
	/// @return Iterator to the end of the object
	Iterator end() {
		if (type != JSONType::OBJECT)
			throw std::runtime_error("JSONValue end: Not an object");
		return objectVal.end();
	}
};

/// @brief Class responsible for parsing a JSON string and creating a JSONValue representing the JSON structure
class JSONParser {
	private:
		JSONLexer lexer;
		Token token;

		JSONValue parseValue() {
			switch (token.type)
			{
				case TokenType::LBRACE:
					return parseObject();
				case TokenType::LBRACKET:
					return parseArray();
				case TokenType::NUMBER:
				{
					JSONValue val = {.type = JSONType::NUMERIC, .numVal=std::stod(token.raw)};
					advance();
					return val;
				}
				case TokenType::STRING:
				{
					JSONValue val = {.type = JSONType::STRING, .strVal=token.raw};
					advance();
					return val;
				}
				case TokenType::TRUE:
					advance();
					return {.type = JSONType::BOOL, .boolVal=true};
				case TokenType::FALSE:
					advance();
					return {.type = JSONType::BOOL, .boolVal=false};
				case TokenType::NONE:
					advance();
					return {JSONType::NONE};
				case TokenType::EOI:
					return {JSONType::NONE};
				default:
					throw std::runtime_error("Unexpected token found after parsing JSON(ParseValue): " + token.raw);
			}
		}

		JSONValue parseObject() {
			JSONValue res = {.type=JSONType::OBJECT};
			expect(TokenType::LBRACE);

			if (token.type == TokenType::RBRACE)
				return advance(), res;

			while (true){
				if (token.type != TokenType::STRING)
					throw std::runtime_error("Unexpected token found after parsing JSON(ParseObject): " + token.raw + " of type: " + TokenTypeToString(token.type));
				std::string key = token.raw;
				advance();
				expect(TokenType::COLON);
				res.objectVal[key] = parseValue();
				if (token.type == TokenType::COMMA){
					advance();
					continue;
				}
				break;
			}
			expect(TokenType::RBRACE);
			return res;			
		}
		JSONValue parseArray() {
			JSONValue res = {JSONType::ARRAY};
			expect(TokenType::LBRACKET);

			if (token.type == TokenType::RBRACKET)
				return res;

			TokenType arrayType = token.type;
			while (true) {
				if (arrayType != token.type)
					throw std::runtime_error("Unexpected token found after parsing JSON(ParseArray): " + token.raw);
				res.arrayValue.push_back(parseValue());
				if (token.type == TokenType::COMMA)
				{
					advance();
					continue;
				}
				break;
			}
			expect(TokenType::RBRACKET);
			return res;
		}

		void advance() {token = lexer.nextToken();}
		void expect(TokenType type) {
			if (token.type != type)
				throw std::runtime_error("Unexpected token found after parsing JSON(expect): " + token.raw + ", expected: " + TokenTypeToString(type) + ", got: " + TokenTypeToString(token.type));
			advance();
		}
	public:
		/// @brief Constructor for JSONParser, takes a JSON string as input and initializes the lexer with it, then advances to the first token
		/// @param input corresponds to the JSON string to parse, can be read from a file using JSONToString function
		JSONParser(std::string input){
			lexer = JSONLexer(input);
			advance();
		}

		/// @brief Main function of the JSONParser, will parse the JSON string and return a JSONValue representing the JSON structure, will throw if there are unexpected tokens after parsing the value
		/// @return JSONValue representing the JSON structure
		JSONValue parseJSON() {
			JSONValue value = parseValue();
			if (token.type != TokenType::EOI)
				throw std::runtime_error("Unexpected token found after parsing JSON(parseJSON): " + token.raw);
			return value;
		}
};