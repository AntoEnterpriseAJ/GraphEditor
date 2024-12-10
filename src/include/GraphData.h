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
    void setWeighted(bool weighted);
    void setLogAdjacency(bool log);
    void clear();
    void undo();
    
    int getSize() const;
    bool isWeighted() const;
    bool isOriented() const;
    void logAdjacencyMatrix(const std::string& fileName) const;
    std::vector<GraphNode*>& getNodes();
    GraphNode* getNode(unsigned int nodeID);
    Edge* getEdgeUnoriented(unsigned int startNodeID, unsigned int endNodeID);
    void inverseGraph();

    GraphNode* findRoot();
    bool isTree();
    bool isWeaklyConnected();
    bool checkCycles() const;
    void disconnectComponents(const std::vector<std::vector<unsigned int>>& components);
    void reconstructGraphFromComponents(const std::vector<std::vector<unsigned int>>& components); // for strongly connected only    
    std::vector<std::vector<unsigned int>> weaklyConnectedComponents(const GraphNode* const startNode);
    std::vector<std::vector<unsigned int>> stronglyConnectedComponents(const GraphNode* const startNode);
    std::vector<std::vector<unsigned int>> topologicalSort(const GraphNode* const startNode);

    std::vector<std::pair<int, int>> primMST();
    std::vector<std::pair<int, int>> genericMST();
    std::vector<std::pair<int, int>> kruskalMST();
    std::vector<int>          totalDFS(const GraphNode* const startNode) const;
    std::vector<unsigned int> BFS(const GraphNode* const startNode) const;
    std::vector<unsigned int> DFS(const GraphNode* const startNode) const;
    std::vector<unsigned int> genericPathTraversal(const GraphNode* const startNode) const;
    std::vector<unsigned int> totalGenericPathTraversal(const GraphNode* const startNode) const;

    struct PairHash
    {
        int operator()(const std::pair<int, int>& p) const
        {
            return p.first ^ p.second;
        }
    };

    const std::vector<Edge>& getEdges() const;
    std::vector<Edge>& getEdgesRef();
    const std::vector<std::unordered_set<int>>& getAdjacencyList() const;
    std::unordered_map<std::pair<int, int>, int, PairHash>& getEdgeWeights();

private:
    void updateAdjacencyList();
private:
    bool m_logAdjacencyMatrix;
    bool m_oriented;
    bool m_weighted;
    std::stack<Action>                   m_actions;
    std::vector<std::unordered_set<int>> m_adjacencyList;
    std::vector<GraphNode*>              m_nodes;
    std::vector<Edge>                    m_edges;
    std::unordered_map<std::pair<int, int>, int, PairHash> m_edgeWeights;
};