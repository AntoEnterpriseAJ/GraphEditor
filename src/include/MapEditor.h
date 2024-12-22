#pragma once
#include "GraphNode.h"
#include "Edge.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "GraphData.h"

class MapEditor
{
public:
    MapEditor();

    void render();
    void handleInput();

    void loadFromFile(const std::string& filePath);
    void findMinDistance();

    GraphData& getGraphData();
    const GraphNode* const getSelectedNode() const;
    void setSelectedNode(GraphNode* node);

private:
    glm::vec2 convertToScreenCoordinates(double latitude, double longitude, int screenWidth, int screenHeight);
    void checkNodeSelect(glm::vec2 position);
private:
    static constexpr float kNodeRadius = 30.0f;
private:
    GraphNode* m_leftClickSelectedNode;
    GraphNode* m_rightClickSelectedNode;

    GraphData m_graphData;
    Renderer m_renderer;
};