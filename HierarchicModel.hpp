#pragma once
#include <vector>
#include <iostream>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include "header.h"
// #include "Shader.hpp"
// #include "Mesh.hpp"
// #include "Includes/vml.hpp"
// #include "Includes/struct.hpp"
#include "IModel.hpp"


using namespace vml;

class Mesh;

struct VertexKey {
    int v, vt, vn;
    bool operator==(VertexKey const& o) const noexcept {
        return v == o.v && vt == o.vt && vn == o.vn;
    }
};
struct VertexKeyHash {
    size_t operator()(VertexKey const& k) const noexcept {
        return static_cast<size_t>(
            (k.v * 73856093) ^ (k.vt * 19349663) ^ (k.vn * 83492791)
        );
    }
};

struct ModelNode {
	std::string name;
	mat4 localTransform = vml::identity<float, 4>();
	mat4 globalTransform = vml::identity<float, 4>();
	Mesh* mesh;

	std::vector<std::string> children;
	std::string parent;
	
	vec3 pivotInit = {0,0,0};
	vec3 pivotWorld = {0,0,0};
	vec3 pivotLocal = {0,0,0};

	ModelNode() {};
	ModelNode(const std::string _name) : name(_name) {}
} typedef MNode;

struct ModelStruct {
	std::map<std::string, MNode *> nodes;
	std::vector<std::string> order;
	// might be moved to model destructor for safety
	~ModelStruct() {
		for(auto& [key, value]: nodes)
			delete value;
	}
} typedef MStruct;

class HierarchicModel : public IModel
{
	public:
		//constructors and destructors
		HierarchicModel();
		HierarchicModel(std::string path);
		HierarchicModel& operator=(const HierarchicModel& oth);
		~HierarchicModel();

		// call function to draw each meshes in model
		void Draw(Shader &shader);

		void printMeshMatNames();
		//getters
		size_t ms();
		vec3 min();
		vec3 max();
		MNode* getNode(const std::string& name) {
			if (model.nodes.count(name))
				return model.nodes[name];
			return nullptr;
		}
	private:
		// model data
		MStruct model;
		std::unordered_map<std::string, Material> materials;
		std::string directory;
		std::string _name;
		vec3 _min = { +MAXFLOAT, +MAXFLOAT, +MAXFLOAT };
		vec3 _max = { -MAXFLOAT, -MAXFLOAT, -MAXFLOAT };
		bool final = false;

		void	loadMtl(std::string path);
		
		//loader utils
		void	defineMinMax(float x, float y, float z);
		bool	validObjPath(std::string path);
		void	convertMtlPath(std::string& mtlpath);

		//in ModelLoadObj.cpp
		void	loadModel(std::string path);
		void	printGraph(MStruct& obj, MNode *node);
		void	checkLink(MStruct& final, MNode* node, std::deque<std::string> visited);
		void	loadSkeleton(const std::string& path);
		
		//loadObj sub functions
		int		faceLineParse(std::stringstream& ss, std::vector<vec3>& temp_v, std::vector<vec2>& temp_vt,
					std::vector<vec3>& temp_vn, Mesh& currentMesh, std::unordered_map<VertexKey, unsigned int, VertexKeyHash>& cache);
		void	usemtl(std::stringstream& ss, Mesh& currentMesh, std::string& prevMat, std::unordered_map<VertexKey, unsigned int, VertexKeyHash>& cache);
		void	finishAndResetMesh(Mesh& currentMesh, std::string prevMat, std::unordered_map<VertexKey, unsigned int, VertexKeyHash>& cache, bool reset);
};
