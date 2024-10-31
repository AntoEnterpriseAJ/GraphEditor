#include "GraphData.h"

#ifdef _DEBUG
#include <iostream>
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x) do {} while (0)
#endif

GraphData::GraphData()
   : m_oriented{ true }, m_actions{}, m_nodes{}, m_edges{}
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
   m_actions.push(Action::newNode);
}

void GraphData::addEdge(GraphNode* edgeStart, GraphNode* edgeEnd)
{
   LOG("Edge added");
   m_edges.emplace_back(edgeStart, edgeEnd);
   logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
   m_actions.push(Action::newEdge);
}

void GraphData::setOriented(bool oriented)
{
   m_oriented = oriented;
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
}

void GraphData::undo()
{
   if (!m_actions.empty() && !m_edges.empty() && m_actions.top() == Action::newEdge)
   {
       m_edges.pop_back();
       m_actions.pop();
       logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
   }
   else if (!m_actions.empty() && !m_nodes.empty() && m_actions.top() == Action::newNode)
   {
       delete m_nodes.back();
       m_nodes.pop_back();
       m_actions.pop();
       logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
   }
   else
   {
       LOG("Nothing to undo");
   }
}

std::vector<GraphNode*>& GraphData::getNodes()
{
   return m_nodes;
}

std::vector<Edge>& GraphData::getEdges()
{
   return m_edges;
}

void GraphData::logAdjacencyMatrix(const std::string& fileName) const
{
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
           adjMatrix[startNodeID - 1][endNodeID - 1] = 1;
       }
       else
       {
           adjMatrix[startNodeID - 1][endNodeID - 1] = 1;
           adjMatrix[endNodeID - 1][startNodeID - 1] = 1;
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

bool GraphData::isOriented() const
{
   return m_oriented;
}
