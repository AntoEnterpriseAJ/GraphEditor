#pragma once
#include "GraphNode.h"
#include "Edge.h"
#include "Action.h"
#include <stack>
#include <list>

class GraphData
{
public:
    GraphData();
    ~GraphData();

    void addNode(GraphNode* node);
    void addEdge(GraphNode* edgeStart, GraphNode* edgeEnd);
    void setOriented(bool oriented);
    void setLogAdjacency(bool log);
    void clear();
    void undo();

    std::vector<GraphNode*>& getNodes();
    std::vector<Edge>& getEdges();

    void logAdjacencyMatrix(const std::string& fileName) const;
    bool isOriented() const;
private:
    void updateAdjacencyList();
private:
    bool m_logAdjacencyMatrix;
    bool m_oriented;
    std::stack<Action> m_actions;
    std::vector<std::list<int>> m_adjacencyList;
    std::vector<GraphNode*> m_nodes;
    std::vector<Edge> m_edges;
};