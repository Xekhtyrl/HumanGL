#pragma once
#include <vector>
#include <iostream>
#include <algorithm>

#include "Includes/vml.hpp"
#include "Shader.hpp"
#include "Includes/struct.hpp"
#include "Includes/header.h"

class Shader;

class Mesh {
    public:
        Mesh();
		Mesh(const Mesh& oth);
        Mesh& operator=(const Mesh& oth);

		void Draw(Shader &shader, Material material, mat4 transfrom, vec3 pivot);
		void setupMesh(vec3 min, vec3 size);

		//getters
        std::vector<Vertex>& vertices();
        const std::vector<Vertex>& vertices() const;
        std::vector<unsigned int>& indices();
        const std::vector<unsigned int>& indices() const;
        std::string materialName() const;
        std::string name() const;
		GLuint& VAO();
		GLuint& VBO();
		GLuint& EBO();
		const GLuint& VAO() const;
		const GLuint& VBO() const;
		const GLuint& EBO() const;
		bool vnPresent() const;
		bool vtPresent() const;

		//setters
        void vertices(std::vector<Vertex>& vertices);
        void indices(std::vector<unsigned int>& idxs);
        void materialName(std::string matName);
        void name(std::string name);
		void vnPresent(bool present);
		void vtPresent(bool present);

    private:
		        // mesh data
        std::string                 _name;
        std::vector<Vertex>			_vertices;
        std::vector<unsigned int>	_indices;
        std::string                 _materialName;
		bool						_vnPresent = false;
		bool						_vtPresent = false;
		GLuint 						_VAO;
		GLuint 						_VBO;
		GLuint 						_EBO;

		vec2 generateCubicUV(const vec3& p, const vec3& n, 
                     const vec3& min, const vec3& size);
        void generateDefaultVT(vec3 min, vec3 max);
		void generateDefaultVN(vec3 min, vec3 size);
};
