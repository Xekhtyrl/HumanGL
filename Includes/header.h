#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glext.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "vml.hpp"

// namespace json = boost::json;

//utils.cpp
void strTrim(std::string& str, std::string arr = " \t\r\n");
std::string fileToStr(const std::string& filePath);
// json::value parseJsonFile(const std::string& path);

#include "struct.hpp"
#include "../Shader.hpp"

class Shader;

extern Setup setup;
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern float lastX;
extern float lastY;
extern float deltaTime;

#include "../Camera.hpp"
extern Camera camera;
#include "../Mesh.hpp"

class Mesh;

#include "../IModel.hpp"

class IModel;

struct ModelNode;
typedef ModelNode MNode;

#include "../HierarchicModel.hpp"
#include "../Animation.hpp"

extern vml::mat4 model;
extern vml::vec3 center;

#include "../Camera.hpp"
//modelMatrices.cpp
void setBaseModelMatrix(GLFWwindow *window, IModel* object);
void defineMatrices(Shader& shad);

//controls.cpp
void scaleAndResetKey(GLFWwindow *window, IModel* object);
void rotationKey(GLFWwindow *window);
void animate(GLFWwindow *window, IModel *object, Animation *anim);
void rotateNode(IModel* object, MNode* node, vec3 angles);
void translationKey(GLFWwindow *window);
void changeSetup(GLFWwindow *window, int key, int action);
void changeLightSettings(GLFWwindow *window);
void processInput(GLFWwindow *window, IModel* object, Animation *anim);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setup_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

//window.cpp
GLFWwindow* initWindow(std::string name);
void initImgui(GLFWwindow* window);
void createUIImgui();

//animation.cpp
Animation loadAnimation(const std::string& filepath);