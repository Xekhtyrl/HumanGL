#include <iostream>
#include <vector>
#include <cmath>
#include <boost/json/src.hpp>
#include <fstream>
#include <sstream>

namespace json = boost::json;

std::string fileToStr(const std::string& filePath) {
    std::ifstream file(filePath);
	if (!file.is_open())
		throw std::runtime_error("Could not open File: " + filePath);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

json::value parseJsonFile(const std::string& path) {
	std::string file = fileToStr(path);
	return json::parse(file);
}

void dump(json::value const& v, int indent = 0)
{
    auto pad = std::string(indent, ' ');

    if (v.is_object())
    {
        for (auto const& [k, val] : v.as_object())
        {
            std::cout << pad << k << ":\n";
            dump(val, indent + 2);
        }
    }
    else if (v.is_array())
    {
        for (auto const& e : v.as_array())
            dump(e, indent + 2);
    }
    else
    {
        std::cout << pad << v << "\n";
    }
}


int main() {
	try {

		json::value js = parseJsonFile("Ressources/HumanGL.json");
		json::object obj = js.as_object();
		
		dump(obj);
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}