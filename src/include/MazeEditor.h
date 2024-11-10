#pragma once
#include "GraphData.h"
#include "Renderer.h"
#include <vector>

class MazeEditor
{
public:
    enum MazeCell {
        Wall     = 0,     
        Path     = 1,   
        Exit     = 2,  
        Entrance = 3
    };
public:

    MazeEditor();

    void render(Renderer::Primitive nodePrimitive = Renderer::Primitive::circle);

    GraphData& getGraphData(); // maybe unused TODO: check it out
    void loadFromFile(const std::string& filePath);

    void solveMaze();
    std::vector<int> BFS(unsigned int startNodeID);

private:
    void addEdgesBetweenCells(int rows, int cols);
    glm::vec4 getCellColor(int value);
    static constexpr float kNodeSize = 30.0f; //TODO: make it adjustable
private:
    std::vector<int> m_entrances;
    std::vector<int> m_exits;

    GraphData m_graphData;
    Renderer  m_renderer;
};