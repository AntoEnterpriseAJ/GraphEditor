#pragma once
#include "GraphData.h"
#include "Renderer.h"

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
    void readMazeFromFile(const std::string& filePath);

private:
    static constexpr float kNodeSize = 30.0f; //TODO: make it adjustable
private:
    GraphData m_graphData;
    Renderer  m_renderer;
};