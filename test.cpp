#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <fstream>
#include <sstream>
#include <map>
#include "Includes/vml.hpp"

using namespace vml;

class Mesh
{
private:
	/* data */
public:
	Mesh(/* args */);
	~Mesh();
};

Mesh::Mesh(/* args */)
{
}

Mesh::~Mesh()
{
}

struct ModelNode {
	std::string name;
	mat4 localTranform = vml::identity<float, 4>();
	Mesh* mesh;
	std::vector<std::string> children;
	vec3 pivot = {0,0,0};

	ModelNode() {};
	ModelNode(const std::string _name) : name(_name) {}
} typedef MNode;

struct ModelStruct {
	std::map<std::string, MNode *> nodes;
	std::vector<std::string> order;
	~ModelStruct() {
		for(auto& [key, value]: nodes)
			delete value;
	}
} typedef MStruct;

void strTrim(std::string& str, std::string arr = " \t\r\n") {
	size_t start = str.find_first_not_of(arr);
	if (start > str.length()) {
		str = "";
		return;
	}
	size_t end = str.find_last_not_of(arr);
	str = str.substr(start, end - start + 1);	
}

void printGraph(MStruct& obj, MNode *node) {
	std::cout << node->name << std::endl;
	std::cout << "\tpivot: " << node->pivot << std::endl;;
	if (node->children.size()) {
		std::cout << "\tchildren: ";
		for (auto& x: node->children)
			std::cout << x << " ";
		std::cout << std::endl;
		for (auto& x: node->children)
			printGraph(obj, obj.nodes[x]);
	}
}

void checkLink(MStruct& final, MNode* node, std::deque<std::string> visited) {
	if (std::find(visited.begin(), visited.end(), node->name) != visited.end())
		throw std::runtime_error("Error: skeleton file >> loop in model");
	visited.emplace_front(node->name);
	if (node->children.size()){
		for (auto& x: node->children)
			if (std::find(final.order.begin(), final.order.end(), x) == final.order.end())
				throw std::runtime_error("Error: skeleton file >> child nonexistent");
		for (auto& x: node->children){
			checkLink(final, final.nodes[x], visited);
		}
	}
}

void loadSkeleton(const std::string& path) {
	MStruct final;
	// std::map<std::string, MNode*> nodes;
	MNode *obj;
	std::string line;
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("Error: Could not open " + path);	

	while (getline(file, line)) {
		std::stringstream ss(line);
		std::string type;

		ss >> type;

		if (type.find('{') != std::string::npos || type.find('}') != std::string::npos && type.size() < 3)
			continue;
		// std::cout << type << std::endl;
		if (type.find("pivot") != std::string::npos){
			    // Skip until '['
			ss.ignore(std::numeric_limits<std::streamsize>::max(), '[');

			// Read numbers
			ss >> obj->pivot[0];
			ss.ignore(1); // skip ','
			ss >> obj->pivot[1];
			ss.ignore(1); // skip ','
			ss >> obj->pivot[2];

			// std::cout << obj->pivot[0] << " "
			// 		<< obj->pivot[1] << " "
			// 		<< obj->pivot[2] << "\n";
		}
		else if (type.find("children") != std::string::npos) {
			std::string val;
			std::string prev = type;
			std::vector<std::string> arr;

			ss.ignore(std::numeric_limits<std::streamsize>::max(), '[');
			while (ss >> val) {
				// prev = val;
				
				strTrim(val, ":,[]\"");
				ss.ignore(1);
				arr.push_back(val);
			}
			obj->children = arr;
		}
		else {
			strTrim(type, ":\"");
			obj = new MNode(type);
			final.order.push_back(type);
			final.nodes[type] = obj;
		}
	}
	std::cout << "Nodes not linked" <<std::endl;
	std::cout << "Nodes linked" <<std::endl;

	checkLink(final, final.nodes[final.order[0]], std::deque<std::string>());
	printGraph(final, final.nodes[final.order[0]]);

}


int main() {
	try {
		loadSkeleton("Ressources/HumanGL.json");
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}