#pragma once
#include "GraphNode.h"
#include "Edge.h"
#include "Action.h"
#include <stack>
#include <unordered_set>

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
    
    int getSize() const;
    bool isOriented() const;
    void logAdjacencyMatrix(const std::string& fileName) const;
    std::vector<GraphNode*>& getNodes();
    GraphNode* getNode(unsigned int nodeID);

    bool checkForCyclesOriented() const;
    void weaklyConnectedComponents(const GraphNode* const startNode);
    std::stack<unsigned int>  topologicalSort() const;
    std::vector<unsigned int> BFS(const GraphNode* const startNode) const;
    std::vector<unsigned int> DFS(const GraphNode* const startNode) const;
    std::vector<unsigned int> genericPathTraversal(const GraphNode* const startNode) const;
    std::vector<unsigned int> totalGenericPathTraversal(const GraphNode* const startNode) const;

    const std::vector<Edge>& getEdges() const;
    const std::vector<std::unordered_set<int>>& getAdjacencyList() const;

private:
    void updateAdjacencyList();
private:
    bool m_logAdjacencyMatrix;
    bool m_oriented;
    std::stack<Action>                   m_actions;
    std::vector<std::unordered_set<int>> m_adjacencyList;
    std::vector<GraphNode*>              m_nodes;
    std::vector<Edge>                    m_edges;
};