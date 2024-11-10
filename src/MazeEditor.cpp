#include "MazeEditor.h"
#include "ResourceManager.h"
#include <unordered_set>
#include <queue>

MazeEditor::MazeEditor()
    : m_graphData{}, m_renderer{}
{}

void MazeEditor::render(Renderer::Primitive nodePrimitive)
{
    for (const auto& node : m_graphData.getNodes())
    {
        m_renderer.render(node, ResourceManager::getShader("quad"), nodePrimitive);
        //TODO: fix, render color can't be changed by IMGUI
        m_renderer.renderText(std::to_string(node->getInternalID()),
                              ResourceManager::getShader("text"),
                              node->getPosition(),
                              true,
                              0.5f,
                              glm::vec3{1.0f, 0.0f, 1.0f});
    }
}

GraphData& MazeEditor::getGraphData()
{
    return m_graphData;
}

void MazeEditor::loadFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cout << "Can't open file at: " << filePath << "\n";
        return;
    }

    m_graphData.clear();
    m_entrances.clear();
    m_exits.clear();

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    int rows = 0, cols = 0;
    unsigned int value;
    std::string line;

    while (std::getline(ss, line))
    {
        rows++;

        std::istringstream lineStream(line);
        int currentCols = 0;
        while (lineStream >> value)
        {
            currentCols++;
            unsigned int nodeInternalID = static_cast<unsigned int>(m_graphData.getNodes().size());

            if (value == MazeCell::Entrance) m_entrances.push_back(nodeInternalID);
            else if (value == MazeCell::Exit) m_exits.push_back(nodeInternalID);

            m_graphData.addNode(new GraphNode{
                {50.0f + currentCols * 2 * MazeEditor::kNodeSize, 50.0f + rows * 2 * MazeEditor::kNodeSize},
                std::to_string(value),
                nodeInternalID,
                glm::vec2{MazeEditor::kNodeSize, MazeEditor::kNodeSize},
                getCellColor(value)});
        }
        cols = std::max(cols, currentCols);
    }

    addEdgesBetweenCells(rows, cols);
}

void MazeEditor::addEdgesBetweenCells(int rows, int cols)
{
    for (int index = 0; index < rows * cols; ++index)
    {
        int nodeToTheLeft = index - 1;
        int nodeToTheRight = index + 1;
        int nodeBelow = index + cols;
        int nodeAbove = index - cols;

        const std::vector<GraphNode*>& nodes = m_graphData.getNodes();

        if (nodeToTheRight < rows * cols && (nodeToTheRight % cols != 0))
        {
            m_graphData.addEdge(nodes[index], nodes[nodeToTheRight]);
        }

        if (nodeBelow < rows * cols)
        {
            m_graphData.addEdge(nodes[index], nodes[nodeBelow]);
        }

        if (nodeToTheLeft >= 0 && (index % cols != 0))
        {
            m_graphData.addEdge(nodes[index], nodes[nodeToTheLeft]);
        }

        if (nodeAbove >= 0)
        {
            m_graphData.addEdge(nodes[index], nodes[nodeAbove]);
        }
    }
}

glm::vec4 MazeEditor::getCellColor(int value)
{
    glm::vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
    if (value == MazeCell::Wall)          color = { 0.0f, 0.0f, 0.0f, 1.0f };
    else if (value == MazeCell::Path)     color = { 1.0f, 1.0f, 1.0f, 1.0f };
    else if (value == MazeCell::Entrance) color = { 0.0f, 0.0f, 1.0f, 1.0f };
    else if (value == MazeCell::Exit)     color = { 1.0f, 0.0f, 0.0f, 1.0f };
    return color;
}

void MazeEditor::solveMaze()
{
    for (int entrance : m_entrances)
    {
        const std::vector<int>& parents = BFS(entrance);

        for (int exit : m_exits)
        {
            int currentNode = parents[exit];
            while (currentNode != entrance && currentNode != -1)
            {
                if (m_graphData.getNode(currentNode)->getLabel() == std::to_string(MazeCell::Path))
                {
                    m_graphData.getNode(currentNode)->setColor(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });
                }

                currentNode = parents[currentNode];
            }
        }
    }
}

std::vector<int> MazeEditor::BFS(unsigned int startNodeID)
{
    std::vector<std::unordered_set<int>> adjList = m_graphData.getAdjacencyList();

    int nodesSize = m_graphData.getSize();
    std::vector<int> parents(nodesSize, -1);
    std::queue<int> visited; visited.push(startNodeID);
    std::unordered_set<int> unvisited;
    for (int id = 0; id < nodesSize; ++id)
    {
        if (id == startNodeID) continue;

        unvisited.insert(id);
    }

    while (!visited.empty())
    {
        int nodeToVisit = visited.front();
        visited.pop();

        for (int adjNodeID : adjList[nodeToVisit])
        {
            if (m_graphData.getNode(adjNodeID)->getLabel() == std::to_string(MazeCell::Wall)) continue;

            if (unvisited.contains(adjNodeID))
            {
                visited.push(adjNodeID);
                unvisited.erase(adjNodeID);
                parents[adjNodeID] = nodeToVisit;
            }
        }
    }

    return parents;
}
