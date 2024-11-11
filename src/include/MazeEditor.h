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

    void loadFromFile(const std::string& filePath);

    void solveMaze();
    std::vector<int> BFS(unsigned int entrance);

private:
    void addEdgesBetweenCells(int rows, int cols);
    glm::vec4 getCellColor(int value);
private:
    static constexpr float kNodeSize = 30.0f;
private:
    std::unordered_set<unsigned int> m_entrances;
    std::unordered_set<unsigned int> m_exits;

    GraphData m_graphData;
    Renderer  m_renderer;
};