#pragma once
#include "GraphNode.h"
#include "Edge.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "GraphData.h"

class GraphEditor
{
public:
    GraphEditor();

    void render(Renderer::Primitive nodePrimitive = Renderer::Primitive::circle);
    void handleInput();

    GraphData& getGraphData();
    void readMazeFromFile(const std::string& filePath);

private:
    void tryAddEdge(GraphNode* edgeStart, GraphNode* edgeEnd);
    void checkNodeSelect(glm::vec2 position);
    bool checkValidNodePosition(glm::vec2 position);
private:
    static constexpr float kNodeRadius = 30.0f; //TODO: make it adjustable
private:
    GraphData m_graphData;
    Renderer m_renderer;
};