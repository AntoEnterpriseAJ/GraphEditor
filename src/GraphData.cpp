#include "GraphData.h"
#include <queue>

#ifdef _DEBUG
#include <iostream>
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x) do {} while (0)
#endif

GraphData::GraphData()
    : m_oriented{ true }, m_logAdjacencyMatrix{ false }, m_actions {}, m_nodes{}, m_edges{}
{}

GraphData::~GraphData()
{
   for (const auto& node : m_nodes)
   {
       delete node;
   }
}

void GraphData::addNode(GraphNode* node)
{
   m_nodes.push_back(node);
   logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
   updateAdjacencyList();
   m_actions.push(Action::newNode);
}

void GraphData::addEdge(GraphNode* edgeStart, GraphNode* edgeEnd)
{
   LOG("Edge added");
   m_edges.emplace_back(edgeStart, edgeEnd);
   logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
   updateAdjacencyList();
   m_actions.push(Action::newEdge);
}

void GraphData::setOriented(bool oriented)
{
   m_oriented = oriented;
   logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
   updateAdjacencyList();
}

void GraphData::setLogAdjacency(bool log)
{
    m_logAdjacencyMatrix = log;
}

void GraphData::clear()
{
   for (const auto& node : m_nodes)
   {
       delete node;
   }
   m_nodes.clear();
   m_edges.clear();
   logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
   updateAdjacencyList();
}

void GraphData::undo()
{
   if (!m_actions.empty() && !m_edges.empty() && m_actions.top() == Action::newEdge)
   {
       m_edges.pop_back();
       m_actions.pop();
       logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
       updateAdjacencyList();
   }
   else if (!m_actions.empty() && !m_nodes.empty() && m_actions.top() == Action::newNode)
   {
       delete m_nodes.back();
       m_nodes.pop_back();
       m_actions.pop();
       logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
       updateAdjacencyList();
   }
   else
   {
       LOG("Nothing to undo");
   }
}

const std::vector<std::unordered_set<int>>& GraphData::getAdjacencyList() const
{
    return m_adjacencyList;
}

const std::vector<Edge>& GraphData::getEdges() const
{
   return m_edges;
}

void GraphData::logAdjacencyMatrix(const std::string& fileName) const
{
   if (!m_logAdjacencyMatrix) return;

   std::ofstream file(fileName);
   if (!file.is_open())
   {
       std::cout << "Can't open file at: " << fileName << "\n";
       return;
   }

   LOG("Logging adjacency matrix");

   std::vector<std::vector<int>> adjMatrix(m_nodes.size(), std::vector<int>(m_nodes.size(), 0));
   for (const auto& edge : m_edges)
   {
       int startNodeID = edge.getStartNode()->getInternalID();
       int endNodeID = edge.getEndNode()->getInternalID();

       if (m_oriented)
       {
           adjMatrix[startNodeID][endNodeID] = 1;
       }
       else
       {
           adjMatrix[startNodeID][endNodeID] = 1;
           adjMatrix[endNodeID][startNodeID] = 1;
       }
   }

   file << m_nodes.size() << "\n";
   for (const auto& row : adjMatrix)
   {
       for (const auto& elem : row)
       {
           file << elem << " ";
       }
       file << "\n";
   }
}

std::vector<GraphNode*>& GraphData::getNodes()
{
    return m_nodes;
}

GraphNode* GraphData::getNode(unsigned int nodeID)
{
    if (nodeID < m_nodes.size())
    {
        return m_nodes[nodeID];
    }

    throw std::invalid_argument("the node doesn't exist");
}

std::vector<unsigned int> GraphData::BFS(const GraphNode* const startNode) const
{
    std::vector<unsigned int> visistedAndAnalyzed;
    std::queue<unsigned int> visited; visited.push(startNode->getInternalID());
    std::unordered_set<unsigned int> unvisited;
    for (const auto node : m_nodes)
    {
        if (node->getInternalID() != startNode->getInternalID())
        {
            unvisited.insert(node->getInternalID());
        }
    }

    while (!visited.empty())
    {
        unsigned int nodeToVisit = visited.front();
        visited.pop();

        for (unsigned int adjacentNode : m_adjacencyList[nodeToVisit])
        {
            visited.push(adjacentNode);
            unvisited.erase(adjacentNode);
        }

        visistedAndAnalyzed.push_back(nodeToVisit);
    }

    return visistedAndAnalyzed;
}

std::vector<unsigned int> GraphData::genericPathTraversal(const GraphNode* const startNode) const
{
    std::unordered_set<unsigned int> visited;
    visited.insert(startNode->getInternalID());
    std::unordered_set<unsigned int> unvisited;
    std::vector<unsigned int> visitedAndAnalyzed;
    for (int nodeInternalID = 0; nodeInternalID < m_nodes.size(); ++nodeInternalID)
    {
        if (nodeInternalID != startNode->getInternalID())
            unvisited.insert(nodeInternalID);
    }

    while (!visited.empty())
    {
        unsigned int nodeID = *visited.begin();
        visited.erase(nodeID);

        for (unsigned int adjacentID : m_adjacencyList[nodeID])
        {
            if (unvisited.contains(adjacentID))
            {
                visited.insert(adjacentID);
                unvisited.erase(adjacentID);
            }
        }

        visitedAndAnalyzed.push_back(nodeID);
    }

    return visitedAndAnalyzed;
}

std::vector<unsigned int> GraphData::topologicalSort(const GraphNode* const startNode) const
{
    return {};
}

std::vector<unsigned int> GraphData::totalGenericPathTraversal(const GraphNode* const startNode) const 
//TODO: fix for one node
//TOOD: repeat genericPathTraversal for the total
{
    std::unordered_set<unsigned int> visited;
    visited.insert(startNode->getInternalID());
    std::vector<unsigned int> visitedAndAnalyzed;
    std::unordered_set<unsigned int> unvisited;
    for (int nodeInternalID = 0; nodeInternalID < m_nodes.size(); ++nodeInternalID)
    {
        if (nodeInternalID != startNode->getInternalID())
            unvisited.insert(nodeInternalID);
    }

    while (!unvisited.empty())
    {
        while (!visited.empty())
        {
            unsigned int nodeID = *visited.begin();
            unvisited.erase(nodeID);
            visited.erase(nodeID);

            for (unsigned int adjacentID : m_adjacencyList[nodeID])
            {
                if (unvisited.contains(adjacentID))
                {
                    visited.insert(adjacentID);
                    unvisited.erase(adjacentID);
                }
            }

            visitedAndAnalyzed.push_back(nodeID);
        }

        if (!unvisited.empty())
        {
            unsigned int newStartNode = *unvisited.begin();
            visited.insert(newStartNode);
        }
    }

    return visitedAndAnalyzed;
}

int GraphData::getSize() const
{
    return m_nodes.size();
}

bool GraphData::isOriented() const
{
   return m_oriented;
}

void GraphData::updateAdjacencyList()
{
    m_adjacencyList.clear();
    m_adjacencyList.resize(m_nodes.size());

    for (const auto& edge : m_edges)
    {
        int startNodeID = edge.getStartNode()->getInternalID();
        int endNodeID = edge.getEndNode()->getInternalID();

        m_adjacencyList[startNodeID].insert(endNodeID);
        if (!m_oriented)
        {
            m_adjacencyList[endNodeID].insert(startNodeID);
        }
    }
}
