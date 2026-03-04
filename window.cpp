#include "Includes/header.h"

#include "Includes/imgui/imgui.h"
#include "Includes/imgui/imgui_impl_glfw.h"
#include "Includes/imgui/imgui_impl_opengl3.h"


/**
 * @brief set of functions needed for initializing the GLFW window with a set name and size
 * 
 * @param name window name that will appears on top of it
 * 
 * @return GLFWwindow pointer of the created window
 */
GLFWwindow* initWindow(std::string name) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	return glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, name.size() > 0 ? name.c_str() : "HumanGL", NULL, NULL);
}

/**
 * @brief set of function to initialize the use of imgui in the current GLFW window
 * 
 * @param window pointer to the GLFW window
 */
void initImgui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();
	ImGui::StyleColorsDark();

	// Initialize GLFW + OpenGL3 backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void legendUI() {
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	
	ImGui::SliderFloat("Scale", &setup.scaleFactor, 0.1f, 10.0f);
	ImGui::Checkbox("Show Faces (F)", &setup.showFaces);
	ImGui::Checkbox("Show Lines (L)", &setup.showLines);
	ImGui::Checkbox("Show Points (P)", &setup.showPoints);

	ImGui::Text("\nLegend:\n\n");
	ImGui::Text("Light Settings:\n");
	ImGui::TextColored({0,0.8,0.8,1}, "light position (1, 2, 3):\n\t %f , %f, %f", setup.lightPos[0], setup.lightPos[1], setup.lightPos[2]);
	ImGui::TextColored({0,0.8,0.8,1}, "light color (4, 5, 6):\n\t %f , %f, %f", setup.lightColor[0], setup.lightColor[1], setup.lightColor[2]);
	ImGui::TextColored({0,0.8,0.8,1}, "view position (7, 8, 9):\n\t %f , %f, %f", setup.viewPos[0], setup.viewPos[1], setup.viewPos[2]);
	ImGui::Text("Reset light settings (0)\n");

	ImGui::Text("\nModel Controls:\n");
	ImGui::Text("\tRotation X: left Arrow, right Arrow\n");
	ImGui::Text("\tRotation Y: up Arrow, down Arrow\n");
	ImGui::Text("\tRotation Z (CTRL +): left Arrow, right Arrow\n");
	ImGui::Text("\tTranslation Y: NUM2, NUM8\n");
	ImGui::Text("\tTranslation X: NUM4, NUM6\n");
	ImGui::Text("\tTranslation Z: NUM1, NUM9\n");

	ImGui::Text("\nAnimation : ");
	ImGui::Text("\tWalk: H, run : J, jump : K, danse : O");
	ImGui::Text("\tOctopus: B, dabe : N, hélico : U, kaos : I");
	ImGui::Text("\nCamera Controls:\n");
	ImGui::Text("\tMouse, A, W, S, D\n");
	
	ImGui::TextColored({0.8,0.8,0,1} ,"Reset Position & Camera (R)\n");
	ImGui::TextColored({0.7,0.1,0,1} ,"Quit (Esc)\n");

	ImGui::End();
}

void applyNewColorToSelectedParts(HierarchicModel* model, float color[3]) {
	for (const auto& partName : model->getModel().order) {
		MNode* node = model->getNode(partName);
		if (node && node->selected && node->mesh) {
			node->mesh->material().diffuse = {color[0], color[1], color[2]};
			node->mesh->material().ambient = {color[0], color[1], color[2]};
		}
	}
}

void applyNewScaleToSelectedParts(HierarchicModel* model, float scaleFactor) {
	for (const auto& partName : model->getModel().order) {
		MNode* node = model->getNode(partName);
		if (node && node->selected) {
			node->scale *= scaleFactor;
			node->updateLocalMatrix();

			updateNodeWorldMatrixModel(model, model->getNode("torso"));
		}
	}
}

void bodyPartUI(IModel* object) {
	HierarchicModel* hierModel = dynamic_cast<HierarchicModel*>(object);
	if (!hierModel) return;
	ImGui::Begin("Body Part Selection", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	std::vector<std::string> bodyParts = hierModel->getModel().order;
	ImGui::Text("Select a body part to modify its details:");
	if (ImGui::CollapsingHeader("Body Parts"))
	{
		for (const auto& part : bodyParts) {
			ImGui::Selectable(part.c_str(), &hierModel->getNode(part)->selected);
		}
	}
	ImGui::Text("Change selected parts color (R, G, B):");
	static float color[3] = {0.0f, 1.0f, 1.0f};
	ImGui::ColorEdit3("Part Color", color);
	applyNewColorToSelectedParts(hierModel, color);
	static double scaleFactor = 1.0f;
	ImGui::Text("Scale selected parts:");

	if (ImGui::Button("-"))
	{
		scaleFactor = 9.0f / 10.0f;
		applyNewScaleToSelectedParts(hierModel, scaleFactor);
	}

	ImGui::SameLine();

	if (ImGui::Button("+"))
	{
		scaleFactor = 10.0f / 9.0f;
		applyNewScaleToSelectedParts(hierModel, scaleFactor);
	}
	ImGui::End();
}

/**
 * @brief create and draw Imgui frame on the window and fill it with the details of the program
 * 
 * Give details on the view mode activated, the light parameter and the legend on the controls
 */
void createUIImgui(IModel* object) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	// ImGui::SetNextWindowSize(ImVec2(SCR_WIDTH / 5, 0), ImGuiCond_Always);
	// ImGui::GetStyle().FontScaleMain = 0.8f;
	legendUI();
	bodyPartUI(object);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}