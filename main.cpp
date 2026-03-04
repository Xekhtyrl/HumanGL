#include "Includes/header.h"
#include "Includes/vml.hpp"
#include "Camera.hpp"
#include "HierarchicModel.hpp"

#include "Includes/imgui/imgui.h"
#include "Includes/imgui/imgui_impl_glfw.h"
#include "Includes/imgui/imgui_impl_opengl3.h"


using namespace vml;

unsigned int SCR_WIDTH = 1600;
unsigned int SCR_HEIGHT = 1600;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;
Camera camera(vec3({0.,0.,3.}));
mat4 model;
Setup setup = Setup();
vec3 center;

/**
 * @brief resize window frame function
 * 
 * @param window glfw window pointer
 * @param width new width of window
 * @param height new height of window
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	(void)window;
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}

/**
 * @brief rendering loop function that will, in order: call functions to process input, redefine based on input the model matrix, draw each meshes in the model and redraw the UI imgui window.
 * 
 * @param window glfw window pointer.
 * @param shader shader class needed beforehand to draw the meshes with and send update to the program on the model.
 */
void renderLoop(GLFWwindow *window, Shader& shader, IModel* object, AnimManager& animManager) {

	while(!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window, object, animManager);
		// Set the clear color (RGBA)
		glClearColor(0.75, 0.75f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		defineMatrices(shader);
		
		object->Draw(shader);
		
		createUIImgui(object);
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}
}

/**
 * @brief setup the window and legend details as well as callbacks functions for mouse and keyboard
 * 
 * @param window pointer to the glfw window
 */
void setupOpenGL(GLFWwindow *window) {
	initImgui(window);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, setup_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	// Make window visible before setting cursor
	glfwShowWindow(window);

	// Center cursor after window is visible
	glfwSetCursorPos(window, SCR_WIDTH / 2.0, SCR_HEIGHT / 2.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

/**
 * @brief clean function called at the end of program(with or without errors) that close and free the imGui context window and the GLFW window as well as terminate it.
 * 
 *	@param window the GLFW window pointer
 */
void cleanProgram(GLFWwindow *window) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

/** @brief simple function to get the .obj name from path and put it in the Setup Struct
 * 
 * @param arg path name to extract name from
*/
void setObjName(std::string arg) {
	setup.modelName = arg.substr(arg.find_last_of('/') + 1);
}

int main(int argc, char **argv)
{
	std::string obj;
	if (argc == 2)
		obj = argv[1];
	else
		obj = "Ressources/HumanGL.obj";
	setObjName(obj);

	std::ofstream log;
	std::streambuf* coutbuf = std::cout.rdbuf();
	log.open("err.log");
	std::cout.rdbuf(log.rdbuf());

	GLFWwindow* window = initWindow(setup.modelName);
	if (window == NULL)
	{
		std::cout << "Program ended prematurely. Failed to create GLFW window." << std::endl;
		glfwTerminate();
		std::cout.rdbuf(coutbuf);
		log.close();
		return -1;
	}
	std::cout << "Window opened successfully!" << std::endl;
	
	glfwMakeContextCurrent(window);
	std::cout << "Window context created successfully!" << std::endl;
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Program ended prematurely. Failed to initialize GLAD." << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		std::cout.rdbuf(coutbuf);
		log.close();
		return -1;
	}
	
	try {
		std::cout << "Glad loaded successfully" << std::endl;
		setupOpenGL(window);
		std::cout << "OpenGL setuped" << std::endl;

		Shader shad("ShadersFiles/FinalVertexTexShad.glsl", "ShadersFiles/FinalFragTexShad.glsl");
		std::cout << "Shader created Successfully" << std::endl;
		// Create Model here
		IModel *object = new HierarchicModel(obj.c_str());
	
		std::cout << "Model created Successfully" << std::endl;
		AnimManager animManager;
		animManager.anims = loadAnimations("Ressources/Movement.json");
		setBaseModelMatrix(window, object, animManager);
		renderLoop(window, shad, object, animManager);
	}
	catch(std::exception& e){
		std::cout << "Exception catched: " << e.what() << std::endl;
		cleanProgram(window);
		std::cout.rdbuf(coutbuf);
		log.close();
		return -1;
	}
	cleanProgram(window);
	std::cout << "program closed without error or exception" << std::endl;
	std::cout.rdbuf(coutbuf);
	log.close();
	return 0;
}
