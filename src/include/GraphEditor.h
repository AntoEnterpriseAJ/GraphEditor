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

    void render();
    void handleInput();

    GraphData& getGraphData();
    const GraphNode* const getSelectedNode() const;
    void setSelectedNode(GraphNode* node);

private:
    void tryAddEdge(GraphNode* edgeStart, GraphNode* edgeEnd);
    void checkNodeSelect(glm::vec2 position);
    bool checkValidNodePosition(glm::vec2 position);
private:
    static constexpr float kNodeRadius = 30.0f; //TODO: make it adjustable
private:
    GraphNode* m_selectedNode;
    GraphData m_graphData;
    Renderer m_renderer;
};