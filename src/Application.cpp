#include "Application.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Application::Application()
    : m_state{ State::GraphEditor }, m_graphEditor{}, m_Maze{}
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    ResourceManager::loadShader("res/shaders/circle.vert", "res/shaders/circle.frag", "circle");
    ResourceManager::loadShader("res/shaders/edge.vert", "res/shaders/edge.frag", "edge");
    ResourceManager::loadShader("res/shaders/text.vert", "res/shaders/text.frag", "text");
    ResourceManager::loadShader("res/shaders/quad.vert", "res/shaders/quad.frag", "quad");

    m_Maze.loadFromFile("res/maze/maze.txt");
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

    if (m_state == State::GraphEditor)
    {
        m_graphEditor.render();
        m_graphEditor.handleInput();
    }
    else if (m_state == State::Maze)
    {
        m_Maze.render(Renderer::Primitive::quad);
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
    GLFWwindow* window = glfwGetCurrentContext();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    renderToolbar();

    if (m_state == State::GraphEditor)
    {
        renderGraphEditorUI();
    }
    else if (m_state == State::Maze)
    {
        renderMazeUI();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::renderGraphEditorUI() //TODO: ADD VIEW ADJ MATRIX BUTTON
{
    ImGui::Begin("Graph");
    bool orientedCheckboxState = m_graphEditor.getGraphData().isOriented();
    bool weightedCheckboxState = m_graphEditor.getGraphData().isWeighted();

    if (ImGui::Checkbox("Oriented", &orientedCheckboxState))
    {
        if (orientedCheckboxState)
        {
            m_graphEditor.getGraphData().setOriented(true);
        }
        else
        {
            m_graphEditor.getGraphData().setOriented(false);
        }
    }
    if (ImGui::Checkbox("weighted", &weightedCheckboxState))
    {
        if (weightedCheckboxState)
        {
            m_graphEditor.getGraphData().setWeighted(true);
        }
        else
        {
            m_graphEditor.getGraphData().setWeighted(false);
        }
    }
    if (ImGui::Button("clear"))
    {
        m_graphEditor.getGraphData().clear();
    }
    if (ImGui::Button("undo")) 
    {
        m_graphEditor.getGraphData().undo();
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
    if (ImGui::Button("Generic path traversal"))
    {
        const GraphNode* startNode = m_graphEditor.getSelectedNode();
        if (startNode)
        {
            const auto& traversal = m_graphEditor.getGraphData().genericPathTraversal(startNode);
            for (unsigned int node : traversal)
            {
                std::cout << node << " ";
            }
            std::cout << "\n";
        }
        else
        {
            std::cout << "select the node first\n";
        }
    }
    if (ImGui::Button("Total generic path traversal"))
    {
        const GraphNode* startNode = m_graphEditor.getSelectedNode();
        if (startNode)
        {
            const auto& traversal = m_graphEditor.getGraphData().totalGenericPathTraversal(startNode);
            for (unsigned int node : traversal)
            {
                std::cout << node << " ";
            }
            std::cout << "\n";
        }
        else
        {
            std::cout << "select the node first\n";
        }
    }
    if (ImGui::Button("BFS"))
    {
        const GraphNode* startNode = m_graphEditor.getSelectedNode();
        if (startNode)
        {
            const auto& traversal = m_graphEditor.getGraphData().BFS(startNode);
            for (unsigned int node : traversal)
            {
                std::cout << node << " ";
            }
            std::cout << "\n";
        }
        else
        {
            std::cout << "select the node first\n";
        }
    }
    if (ImGui::Button("DFS"))
    {
        const GraphNode* startNode = m_graphEditor.getSelectedNode();
        if (startNode)
        {
            const auto& traversal = m_graphEditor.getGraphData().DFS(startNode);
            for (unsigned int node : traversal)
            {
                std::cout << node << " ";
            }
            std::cout << "\n";
        }
        else
        {
            std::cout << "select the node first\n";
        }
    }
    if (ImGui::Button("Topological sort"))
    {
        const GraphNode* const selectedNode = m_graphEditor.getSelectedNode();

        if (selectedNode)
        {
            auto sortedGraph = m_graphEditor.getGraphData().topologicalSort(selectedNode);

            std::ofstream fout("res/topologicalSort/topoSort.txt");

            if (!fout.is_open())
            {
                std::cout << "ERROR::FILE: could not open file\n";
                return;
            }

            for (const auto& component : sortedGraph)
            {
                for (unsigned int node : component)
                {
                    fout << node << " ";
                    std::cout << node << " ";
                }
            }
            std::cout << "\n";
            fout << "\n";
        }
    }
    if (ImGui::Button("Check cycles oriented"))
    {
        std::cout << (m_graphEditor.getGraphData().checkCycles() ? "Cycles found\n" : "No cycles found\n");
    }
    if (ImGui::Button("Weakly connected components"))
    {
        const GraphNode* startNode = m_graphEditor.getSelectedNode();
        if (startNode)
        {
            auto components = m_graphEditor.getGraphData().weaklyConnectedComponents(startNode);

            for (const auto& component : components)
            {
                 glm::vec4 color = glm::vec4{ static_cast<float>(rand()) / RAND_MAX,
                                   static_cast<float>(rand()) / RAND_MAX,
                                   static_cast<float>(rand()) / RAND_MAX,
                                   1.0f };

                for (unsigned int node : component)
                {
                    m_graphEditor.getGraphData().getNodes()[node]->setColor(color);
                    std::cout << node << " ";
                }
                std::cout << "\n";
            }
        }
        else
        {
            std::cout << "select the node first\n";
        }
    }
    if (ImGui::Button("Inverse graph"))
    {
        m_graphEditor.getGraphData().inverseGraph();
    }
    if (ImGui::Button("Strongly connected components"))
    {
        const GraphNode* startNode = m_graphEditor.getSelectedNode();
        if (startNode)
        {
            const auto& components = m_graphEditor.getGraphData().stronglyConnectedComponents(startNode);

            for (const auto& component : components)
            {
                glm::vec4 color = glm::vec4{
                    static_cast<float>(rand()) / RAND_MAX,
                    static_cast<float>(rand()) / RAND_MAX,
                    static_cast<float>(rand()) / RAND_MAX,
                    1.0f
                };

                for (unsigned int node : component)
                {
                    m_graphEditor.getGraphData().getNodes()[node]->setColor(color);
                    std::cout << node << " ";
                }
                std::cout << "\n";
            }
        }
        else
        {
            std::cout << "select the node first\n";
        }
    }
    if (ImGui::Button("Reconstruct graph from components"))
    {
        const GraphNode* startNode = m_graphEditor.getSelectedNode();

        if (startNode)
        {
            m_graphEditor.getGraphData().reconstructGraphFromComponents(m_graphEditor.getGraphData().stronglyConnectedComponents(startNode));
            m_graphEditor.setSelectedNode(nullptr);
        }
        else
        {
            std::cout << "select the node first\n";
        }
    }
    if (ImGui::Button("Find root"))
    {
        GraphNode* root = m_graphEditor.getGraphData().findRoot();

        if (root)
        {
            std::cout << "The root is: " << root->getInternalID() << "\n";
        }
    }

    ImGui::End();
}

static std::string mazeFilePath{"res/maze/maze.txt"};

void Application::renderMazeUI()
{
    ImGui::Begin("Maze");

    ImGui::InputText("Maze file path", mazeFilePath.data(), ImGuiInputTextFlags_EnterReturnsTrue);
    if (ImGui::Button("load maze"))
    {
        m_Maze.loadFromFile(mazeFilePath);
    }
    if (ImGui::Button("solve"))
    {
        m_Maze.solveMaze();
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

    if (ImGui::Button("Maze"))
    {
        m_state = State::Maze;
    }

    ImGui::End();
}
