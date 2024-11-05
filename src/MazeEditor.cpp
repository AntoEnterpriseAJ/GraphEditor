#include "MazeEditor.h"
#include "ResourceManager.h"

MazeEditor::MazeEditor()
    : m_graphData{}, m_renderer{}
{}

void MazeEditor::render(Renderer::Primitive nodePrimitive)
{
    for (const auto& node : m_graphData.getNodes())
    {
        m_renderer.render(node, ResourceManager::getShader("circle"), nodePrimitive);
    }
}

GraphData& MazeEditor::getGraphData()
{
    return m_graphData;
}

void MazeEditor::readMazeFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cout << "Can't open file at: " << filePath << "\n";
        return;
    }

    m_graphData.clear();

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

            glm::vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
            if (value == MazeCell::Wall)          color = { 0.0f, 0.0f, 0.0f, 1.0f };
            else if (value == MazeCell::Path)     color = { 1.0f, 1.0f, 1.0f, 1.0f };
            else if (value == MazeCell::Entrance) color = { 0.0f, 0.0f, 1.0f, 1.0f };
            else if (value == MazeCell::Exit)     color = { 1.0f, 0.0f, 0.0f, 1.0f };

            m_graphData.addNode(new GraphNode{
                {50.0f + currentCols * 2 * MazeEditor::kNodeSize, 50.0f + rows * 2 * MazeEditor::kNodeSize},
                std::to_string(value),
                static_cast<unsigned int>(m_graphData.getNodes().size() + 1),
                glm::vec2{MazeEditor::kNodeSize, MazeEditor::kNodeSize},
                color});
        }

        cols = std::max(cols, currentCols);
    }

    for (int index = 0; index < rows * cols; ++index)
    {
        int nodeToTheLeft = index - 1;
        int nodeToTheRight = index + 1;
        int nodeBelow = index + cols;
        int nodeAbove = index - cols;

        //std::cout << "currently at index " << index << ", right = " << nodeToTheRight << ", left = " << nodeToTheLeft << "\n";  

        auto m_nodes = m_graphData.getNodes(); // TODO: CHANGE THIS

        if (nodeToTheRight < rows * cols && (nodeToTheRight % cols != 0))
        {
            m_graphData.addEdge(m_nodes[index], m_nodes[nodeToTheRight]);
            //std::cout << "added the node to the right\n";
        }

        if (nodeBelow < rows * cols)
        {
            m_graphData.addEdge(m_nodes[index], m_nodes[nodeBelow]);
            //std::cout << "added the node below\n";
        }

        if (nodeToTheLeft >= 0 && (index % cols != 0))
        {
            m_graphData.addEdge(m_nodes[index], m_nodes[nodeToTheLeft]);
            //std::cout << "added the node to the left\n";  
        }

        if (nodeAbove >= 0)
        {
            m_graphData.addEdge(m_nodes[index], m_nodes[nodeAbove]);
            //std::cout << "added the node above\n";  
        }
    }
}
