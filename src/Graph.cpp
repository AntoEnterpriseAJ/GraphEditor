#include "Graph.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include <thread>
#include <chrono>
#include <fstream>

#ifdef _DEBUG
    #include <iostream>
    #define LOG(x) std::cout << x << std::endl
#else
    #define LOG(x) do {} while (0)
#endif

Graph::Graph()
	: m_nodes{}, m_renderer{}, m_oriented{ true }
{
	ResourceManager::loadShader("res/shaders/circle.vert", "res/shaders/circle.frag", "circle");
	ResourceManager::loadShader("res/shaders/text.vert", "res/shaders/text.frag", "text");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init("#version 430");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

Graph::~Graph()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Graph::render()
{
	glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	this->handleInput(); // TODO: move this

	for (const auto& edge : m_edges)
	{
		m_renderer.render(edge, m_nodes, ResourceManager::getShader("circle"), m_oriented);
	}

	for (const auto& node : m_nodes)
	{
		m_renderer.render(node, ResourceManager::getShader("circle"), ResourceManager::getShader("text"));
	}

    renderUI();

	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}

void Graph::addNode(const GraphNode& node)
{
	m_nodes.push_back(node);
}

void Graph::logAdjacencyMatrix(const std::string& fileName)
{
	std::ofstream file(fileName);
	if (!file.is_open())
	{
		std::cout << "Can't open file at: " << fileName << "\n";
		return;
	}

    LOG("Logging adjacency matrix\n");

	std::vector<std::vector<int>> adjMatrix(m_nodes.size(), std::vector<int>(m_nodes.size(), 0));
	for (const auto& edge : m_edges)
	{
		int startNodeID = m_nodes[edge.getStartNodeID() - 1].getID();
		int endNodeID = m_nodes[edge.getEndNodeID() - 1].getID();

		if (m_oriented)
		{
			adjMatrix[startNodeID - 1][endNodeID - 1] = 1;
		}
		else
		{
			adjMatrix[startNodeID - 1][endNodeID - 1] = 1;
			adjMatrix[endNodeID - 1][startNodeID - 1] = 1;
		}
	}

	file << m_nodes.size() << "\n";
	for (const auto& row : adjMatrix)
	{
		for (const auto& elem : row)
		{
			file << elem << " ";
		}
		file << "\n";
	}
}

void Graph::clear()
{
    m_nodes.clear();
    m_edges.clear();
    logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
}

static bool nodeSelected = false;
static bool pressed = false;
static bool longClick = false;
static float pressStartTime = 0.0f;
static float holdThreshold = 0.35f;
static GraphNode* nodeToDrag = nullptr;

void Graph::handleInput()
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse)
    {
        return;
    }

	GLFWwindow* window = glfwGetCurrentContext();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (!pressed)
		{
			pressed = true;
			pressStartTime = glfwGetTime();
		}

		float pressDuration = glfwGetTime() - pressStartTime;
		if (!longClick && pressDuration >= holdThreshold)
		{
			longClick = true;
			LOG("long click\n");
		}
	}

	if (longClick)
	{
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		if (nodeToDrag == nullptr)
		{
			for (auto& node : m_nodes)
			{
				if (glm::distance(node.getPosition(), glm::vec2{ xPos, yPos }) <= node.getSize().x)
				{
					nodeToDrag = &node;
					break;
				}
			}
		}

		if (nodeToDrag)
		{
			LOG("node with id: " << nodeToDrag->getID() << " selected\n");
			nodeToDrag->setPosition(glm::vec2{ xPos, yPos });
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && pressed)
	{
		if (longClick && nodeToDrag)
		{
			for (const auto& node : m_nodes)
			{
				if (&node == nodeToDrag)
				{
					continue;
				}

				if (glm::distance(node.getPosition(), nodeToDrag->getPosition()) < node.getSize().x * 2.0f)
				{
					glm::vec2 offsetDir = glm::normalize(nodeToDrag->getPosition() - node.getPosition());
					float offset = (node.getSize().x + nodeToDrag->getSize().x) - glm::distance(node.getPosition(), nodeToDrag->getPosition());

					nodeToDrag->setPosition(nodeToDrag->getPosition() + offsetDir * offset);
				}
			}
		}

		if (!longClick)
		{
			LOG("short click\n");

			double xPos, yPos;
			glfwGetCursorPos(window, &xPos, &yPos);

			if (checkValidNodePosition(glm::vec2{ xPos, yPos }))
			{
				this->addNode(GraphNode{ glm::vec2{xPos, yPos}, static_cast<unsigned int>(m_nodes.size() + 1) });
                logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
				nodeSelected = false;
			}
			else
			{
				checkNodeSelect(glm::vec2{ xPos, yPos });
			}
		}

		nodeToDrag = nullptr;
		pressed = false;
		longClick = false;
	}
}

static GraphNode* edgeStart;

void Graph::checkNodeSelect(glm::vec2 position)
{
	for (auto& node : m_nodes)
	{
		if (glm::distance(node.getPosition(), position) >= node.getSize().x)
		{
			continue;
		}

		if (nodeSelected)
		{
			tryAddEdge(*edgeStart, node);
			nodeSelected = false;
			edgeStart = nullptr;
			return;
		}

		LOG("node selected\n");
		nodeSelected = true;
		edgeStart = &node;
		return;
	}
	nodeSelected = false;
}

void Graph::tryAddEdge(GraphNode& edgeStart, GraphNode& edgeEnd)
{
	if (edgeStart.getPosition() == edgeEnd.getPosition())
	{
		LOG("Cannot add edge to the same node\n");
		return;
	}

    for (const auto& edge : m_edges)
    {
        const auto& existingEdgeStart = m_nodes[edge.getStartNodeID() - 1];
        const auto& existingEdgeEnd = m_nodes[edge.getEndNodeID() - 1];

        bool isSameDirection = (existingEdgeStart.getPosition() == edgeStart.getPosition()
                                && existingEdgeEnd.getPosition() == edgeEnd.getPosition());
        bool isOppositeDirection = (existingEdgeStart.getPosition() == edgeEnd.getPosition()
                                    && existingEdgeEnd.getPosition() == edgeStart.getPosition());

        if ((m_oriented && isSameDirection) || (!m_oriented && (isSameDirection || isOppositeDirection)))
        {
            LOG("Edge already exists\n");
            return;
        }
    }

	LOG("Edge added\n");
    m_edges.push_back(Edge{ edgeStart.getID(), edgeEnd.getID() });
    logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
	nodeSelected = false;

	for (const auto& edge : m_edges)
	{
		LOG("Current edges: \n"
		    << "Start: " << m_nodes[edge.getStartNodeID() - 1].getPosition().x
			<< " " << m_nodes[edge.getStartNodeID() - 1].getPosition().y << "\n"
		    << "End: " << m_nodes[edge.getEndNodeID() - 1].getPosition().x
			<< " " << m_nodes[edge.getEndNodeID() - 1].getPosition().y << "\n");
	}
}

bool Graph::checkValidNodePosition(glm::vec2 position) const
{
	for (const auto& node : m_nodes)
	{
		if (glm::distance(node.getPosition(), position) < node.getSize().x * 2.0f)
		{
			return false;
		}
	}
	return true;
}

static float my_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

void Graph::renderUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::Begin("Graph");
    if (ImGui::Checkbox("Oriented", &m_oriented))
    {
        logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
    }
    if (ImGui::Button("clear"))
    {
        clear();
    }

	if (ImGui::ColorEdit4("Node color", my_color))
	{
		ResourceManager::getShader("circle").bind();
		ResourceManager::getShader("circle").setVec4("color", glm::vec4{my_color[0], my_color[1], my_color[2], 1.0f});
	}

    ImGui::End();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
