#include "Edge.h"

Edge::Edge(unsigned int nodeStartID, unsigned int nodeEndID)
	: m_nodeStartID{ nodeStartID }, m_nodeEndID{ nodeEndID }
{}

unsigned int Edge::getStartNodeID() const
{
	return m_nodeStartID;
}

unsigned int Edge::getEndNodeID() const
{
	return m_nodeEndID;
}
