#include "Application.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Application::Application()
	: m_state{ State::GraphEditor }, m_graph{}
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init("#version 430");
}

Application::~Application()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Application::render()
{
	glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_graph.render();
	if (m_state == State::GraphEditor)
	{
		m_graph.handleInput();
	}
	else if (m_state == State::BFS)
	{
		m_graph.clear();

		//TODO: Implement BFS
	}

	renderUI();

	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}

static float nodeColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
static float edgeColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
static float textColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

void Application::renderUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	renderToolbar();

	if (m_state == State::GraphEditor)
	{
		renderGraphEditorUI();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::renderGraphEditorUI() //TODO: ADD VIEW ADJ MATRIX BUTTON
{
	ImGui::Begin("Graph");
	bool orientedCheckboxState = m_graph.isOriented();
	if (ImGui::Checkbox("Oriented", &orientedCheckboxState))
	{
		if (orientedCheckboxState)
		{
			m_graph.setOriented(true);
		}
		else
		{
			m_graph.setOriented(false);
		}
		m_graph.logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
	}
	if (ImGui::Button("clear"))
	{
		m_graph.clear();
	}
	if (ImGui::Button("undo"))
	{
		m_graph.undo();
	}
	if (ImGui::ColorEdit4("Node color", nodeColor))
	{
		ResourceManager::getShader("circle").bind();
		ResourceManager::getShader("circle").setVec4("color", glm::vec4{ nodeColor[0], nodeColor[1], nodeColor[2], 1.0f });
	}
	if (ImGui::ColorEdit4("Edge color", edgeColor))
	{
		ResourceManager::getShader("edge").bind();
		ResourceManager::getShader("edge").setVec4("color", glm::vec4{ edgeColor[0], edgeColor[1], edgeColor[2], 1.0f });
	}
	if (ImGui::ColorEdit4("Text color", textColor))
	{
		ResourceManager::getShader("text").bind();
		ResourceManager::getShader("text").setVec3("textColor", glm::vec4{ textColor[0], textColor[1], textColor[2], 1.0f });
	}

	ImGui::End();
}

void Application::renderToolbar()
{
	ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 30));

	if (ImGui::Button("Graph Editor"))
	{
		m_state = State::GraphEditor;
	}
	ImGui::SameLine();

	if (ImGui::Button("BFS"))
	{
		m_state = State::BFS;
	}

	ImGui::End();
}
