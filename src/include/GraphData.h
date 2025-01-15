#pragma once
#include "GraphNode.h"
#include "Edge.h"
#include "Action.h"
#include <stack>
#include <unordered_set>

class GraphData
{
public:
    struct PairHash
    {
        int operator()(const std::pair<int, int>& p) const
        {
            return p.first ^ p.second;
        }
    };
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
    Edge* getEdge(unsigned int startNodeID, unsigned int endNodeID);
    void inverseGraph();

    GraphNode* findRoot();
    bool isTree();
    bool isWeaklyConnected();
    bool checkCycles() const;
    void reconstructGraphFromComponents(const std::vector<std::vector<unsigned int>>& components); // for strongly connected only    
    std::vector<std::vector<unsigned int>> weaklyConnectedComponents(const GraphNode* const startNode);
    std::vector<std::vector<unsigned int>> stronglyConnectedComponents(const GraphNode* const startNode);

    std::vector<unsigned int> dijkstraMinimumCost(const GraphNode* const startNode, const GraphNode* const endNode);

    std::vector<std::pair<int, int>> primMST();
    std::vector<std::pair<int, int>> genericMST();
    std::vector<std::pair<int, int>> kruskalMST();
    std::pair<std::vector<std::pair<int, int>>, int> fordFulkersonMinCut(
        const GraphNode* const sourceNode,
        const GraphNode* const sinkNode
    );
    unsigned int bfsFindAugmentingPath(
        unsigned int sourceID,
        unsigned int sinkID,
        const std::unordered_map<std::pair<int, int>, int, PairHash>& residualCapacities,
        std::vector<int>& parent
    );
    void bfsReachableNodes(
        unsigned int sourceID,
        const std::unordered_map<std::pair<int, int>, int, GraphData::PairHash>& residualCapacities,
        std::unordered_set<unsigned int>& reachable
    );
    std::vector<int>          totalDFS(const GraphNode* const startNode) const;
    std::vector<unsigned int> BFS(const GraphNode* const startNode) const;
    std::vector<unsigned int> DFS(const GraphNode* const startNode) const;
    std::vector<unsigned int> genericPathTraversal(const GraphNode* const startNode) const;
    std::vector<unsigned int> totalGenericPathTraversal(const GraphNode* const startNode) const;
    std::vector<unsigned int> topologicalSort(const GraphNode* const startNode);




    void updateAdjacencyList();
    const std::vector<Edge>& getEdges() const;
    std::vector<Edge>& getEdgesRef();
    const std::vector<std::unordered_set<int>>& getAdjacencyList() const;
    std::unordered_map<std::pair<int, int>, int, PairHash>& getEdgeWeights();

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