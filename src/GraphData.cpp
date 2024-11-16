#include "GraphData.h"

#include <execution>
#include <queue>
#include <set>
#include <imgui/imgui_internal.h>

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

void GraphData::inverseGraph()
{
    std::vector<Edge> newEdges;
    newEdges.reserve(m_edges.size());

    for (const auto& edge : m_edges)
    {
        newEdges.emplace_back(edge.getEndNode(), edge.getStartNode());
    }

    m_edges = newEdges;
    logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
    updateAdjacencyList();
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

bool GraphData::checkForCyclesOriented() const
{
    std::vector<int> parents(m_nodes.size(), -1);
    std::unordered_set<unsigned int> visiting;   
    std::unordered_set<unsigned int> visited;    
    std::stack<unsigned int> visitedStack;       

    for (const auto node : m_nodes)
    {
        unsigned int startNodeID = node->getInternalID();

        if (visited.contains(startNodeID))
            continue;

        visitedStack.push(startNodeID);
        visiting.insert(startNodeID);

        while (!visitedStack.empty())
        {
            unsigned int nodeToVisit = visitedStack.top();
            bool foundAdjacentNode = false;

            for (unsigned int adjNode : m_adjacencyList[nodeToVisit])
            {
                if (visited.contains(adjNode))
                {
                    continue;
                }
                if (visiting.contains(adjNode))
                {
                    return true;
                }

                visitedStack.push(adjNode);
                visiting.insert(adjNode);
                parents[adjNode] = nodeToVisit;
                foundAdjacentNode = true;
                break;
            }

            if (!foundAdjacentNode)
            {
                visitedStack.pop();
                visiting.erase(nodeToVisit);
                visited.insert(nodeToVisit);
            }
        }
    }

    return false;
}

void GraphData::reconstructGraphFromComponents(const std::vector<std::vector<unsigned int>>& components)
{
    std::vector<GraphNode*> newGraph;

    for (const auto& component : components)
    {
        glm::vec2 position = m_nodes[component[0]]->getPosition();
        std::string label;

        for (unsigned int nodeID : component)
        {
            label += std::to_string(nodeID) + " ";
        }

        newGraph.push_back(new GraphNode{ position, label, static_cast<unsigned int>(newGraph.size()) });
    }

    std::unordered_map<unsigned int, unsigned int> component_map;
    for (unsigned int componentID = 0; componentID < components.size(); ++componentID)
    {
        for (unsigned int nodeID : components[componentID])
        {
            component_map[nodeID] = componentID;
        }
    }

    std::vector<Edge> newEdges;
    for (const auto& edge : m_edges)
    {
        unsigned int startNodeID = edge.getStartNode()->getInternalID();
        unsigned int endNodeID = edge.getEndNode()->getInternalID();

        unsigned int startComponent = component_map[startNodeID];
        unsigned int endComponent = component_map[endNodeID];

        if (startComponent != endComponent)
        {
            newEdges.emplace_back(newGraph[startComponent], newGraph[endComponent]);
        }
    }

    for (auto node : m_nodes)
    {
        delete node;
    }
    m_edges.clear();

    m_nodes = std::move(newGraph);
    m_edges = std::move(newEdges);

    m_actions = std::stack<Action>{};
    logAdjacencyMatrix("res/adjMatrix/adjMatrix.txt");
    updateAdjacencyList();
}


std::vector<std::vector<unsigned int>> GraphData::stronglyConnectedComponents(const GraphNode* const startNode)
{
    std::vector<int> t2 = totalDFS(startNode);

    std::priority_queue<std::pair<int, int>> pq;
    for (int i = 0; i < t2.size(); ++i)
    {
        pq.emplace(t2[i], i);
    }

    this->inverseGraph();

    std::unordered_set<unsigned int> unvisited;
    for (unsigned int i = 0; i < m_nodes.size(); ++i)
    {
        unvisited.insert(i);
    }

    std::vector<std::vector<unsigned int>> components;

    while (!pq.empty())
    {
        auto [value, startNodeID] = pq.top();
        pq.pop();

        if (!unvisited.contains(startNodeID))
        {
            continue;
        }

        std::vector<unsigned int> currentComponent;
        std::stack<unsigned int> stack;
        stack.push(startNodeID);

        unvisited.erase(startNodeID);

        while (!stack.empty())
        {
            unsigned int nodeToVisit = stack.top();
            stack.pop();
            currentComponent.push_back(nodeToVisit);

            for (unsigned int adjNode : m_adjacencyList[nodeToVisit])
            {
                if (unvisited.contains(adjNode))
                {
                    stack.push(adjNode);
                    unvisited.erase(adjNode);
                }
            }
        }

        components.push_back(currentComponent);
    }

    this->inverseGraph();

    for (const auto& component : components)
    {
        glm::vec4 color = glm::vec4{
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            1.0f
        };

        for (unsigned int node : component)
        {
            m_nodes[node]->setColor(color);
            std::cout << node << " ";
        }
        std::cout << "\n";
    }

    return components;
}

void GraphData::weaklyConnectedComponents(const GraphNode* const startNode)
{
    this->setOriented(false);

    unsigned int startNodeID = startNode->getInternalID();
    std::stack<unsigned int> visited; visited.push(startNodeID);
    std::stack<unsigned int> visitedAndAnalyzed;

    std::unordered_set<unsigned int> unvisited;
    for (const auto node : m_nodes)
    {
        unsigned int nodeID = node->getInternalID();
        if (nodeID != startNodeID)
        {
            unvisited.insert(nodeID);
        }
    }

    std::vector<std::vector<unsigned int>> components;
    std::vector<unsigned int> currentComponent;
    currentComponent.push_back(startNodeID);

    while (!unvisited.empty() || !visited.empty())
    {
        while (!visited.empty())
        {
            unsigned int nodeToVisit = visited.top();

            bool foundAdjNode = false;

            for (unsigned int adjNode : m_adjacencyList[nodeToVisit])
            {
                if (unvisited.contains(adjNode))
                {
                    foundAdjNode = true;
                    visited.push(adjNode);
                    currentComponent.push_back(adjNode);
                    unvisited.erase(adjNode);
                    break;
                }
            }

            if (!foundAdjNode)
            {
                visitedAndAnalyzed.push(nodeToVisit);
                visited.pop();
            }
        }

        components.push_back(currentComponent);

        if (!unvisited.empty())
        {
            unsigned int newStartNode = *unvisited.begin();
            unvisited.erase(newStartNode);

            currentComponent.clear();
            currentComponent.push_back(newStartNode);

            visited.push(newStartNode);
        }
    }

    for (const auto& component : components)
    {
        glm::vec4 color;
        color = glm::vec4{ static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, 1.0f };
        for (unsigned int node : component)
        {
            m_nodes[node]->setColor(color);
            std::cout << node << " ";
        }
        std::cout << "\n";
    }

    this->setOriented(true);
}

std::vector<unsigned int> GraphData::BFS(const GraphNode* const startNode) const
{
    std::vector<unsigned int> visitedAndAnalyzed;
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
            if (!unvisited.contains(adjacentNode))
            {
                continue;
            }

            visited.push(adjacentNode);
            unvisited.erase(adjacentNode);
        }

        visitedAndAnalyzed.push_back(nodeToVisit);
    }

    return visitedAndAnalyzed;
}

std::vector<unsigned int> GraphData::DFS(const GraphNode* const startNode) const
{
    unsigned int startNodeID = startNode->getInternalID();
    std::stack<unsigned int> toVisit; toVisit.push(startNodeID);
    std::vector<unsigned int> visitedAndAnalyzed;
    std::unordered_set<unsigned int> visited;

    while (!toVisit.empty())
    {
        unsigned int nodeToVist = toVisit.top();
        toVisit.pop();
        visited.insert(nodeToVist);

        for (unsigned int adjNodeID : m_adjacencyList[nodeToVist])
        {
            if (!visited.contains(adjNodeID))
            {
                toVisit.push(adjNodeID);
            }
        }

        visitedAndAnalyzed.push_back(nodeToVist);
    }

    return visitedAndAnalyzed;
}

std::vector<int> GraphData::totalDFS(const GraphNode* const startNode) const
{
    unsigned int startNodeID = startNode->getInternalID();

    std::vector<int> parents(m_nodes.size(), -1);
    std::vector<unsigned int> visitedAndAnalyzed;
    std::stack<unsigned int> visited; visited.push(startNodeID);
    std::unordered_set<unsigned int> unvisited;
    for (auto node : m_nodes)
    {
        unsigned int nodeID = node->getInternalID();
        if (nodeID != startNodeID)
        {
            unvisited.insert(nodeID);
        }
    }

    int inf = std::numeric_limits<int>::max();

    std::vector<int> t1(m_nodes.size(), inf);
    std::vector<int> t2(m_nodes.size(), inf);

    int t = 0;
    t1[startNodeID] = ++t;

    while (!unvisited.empty() || !visited.empty())
    {
        while (!visited.empty())
        {
            unsigned int nodeToVisit = visited.top();

            bool foundAdjNode = false;

            for (unsigned int adjNode : m_adjacencyList[nodeToVisit])
            {
                if (unvisited.contains(adjNode))
                {
                    foundAdjNode = true;

                    visited.push(adjNode);
                    unvisited.erase(adjNode);
                    t1[adjNode] = ++t;
                    parents[adjNode] = nodeToVisit;
                    break;
                }
            }

            if (!foundAdjNode)
            {
                visited.pop();
                visitedAndAnalyzed.push_back(nodeToVisit);
                t2[nodeToVisit] = ++t;
            }
        }

        if (!unvisited.empty())
        {
            unsigned int newStartNode = *unvisited.begin();
            visited.push(newStartNode);
            unvisited.erase(newStartNode);
            t1[newStartNode] = ++t;
        }
    }

    return t2;
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

std::stack<unsigned int> GraphData::topologicalSort() const
{
    if (this->checkForCyclesOriented())
    {
        std::cout << "Can't perform topological sort on a graph with cycles\n";
        return {};
    }

    unsigned int startNodeID = m_nodes[std::rand() % m_nodes.size()]->getInternalID();
    std::cout << "Start node: " << startNodeID << "\n";
    std::stack<unsigned int> visited; visited.push(startNodeID);
    std::stack<unsigned int> visitedAndAnalyzed;

    std::unordered_set<unsigned int> unvisited;
    for (const auto node : m_nodes)
    {
        unsigned int nodeID = node->getInternalID();
        if (nodeID != startNodeID)
        {
            unvisited.insert(nodeID);
        }
    }

    while (!unvisited.empty())
    {
        while (!visited.empty())
        {
            unsigned int nodeToVisit = visited.top();

            bool foundAdjNode = false;

            for (unsigned int adjNode : m_adjacencyList[nodeToVisit])
            {
                if (unvisited.contains(adjNode))
                {
                    foundAdjNode = true;
                    visited.push(adjNode);
                    unvisited.erase(adjNode);
                    break;
                }
            }

            if (!foundAdjNode)
            {
                visitedAndAnalyzed.push(nodeToVisit);
                unvisited.erase(nodeToVisit); //TODO: check PTDF
                visited.pop();
            }
        }

        if (!unvisited.empty())
        {
            unsigned int newStartNode = *unvisited.begin();
            visited.push(newStartNode);
        }
    }

    return visitedAndAnalyzed;
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
