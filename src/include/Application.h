#pragma once
#include "Graph.h"

class Application
{
public:
	enum class State
	{
		GraphEditor,
		BFS,
	};

	Application();
	~Application();

	void render();

private:
	void renderUI();
private:
	State m_state;
	Graph m_graph;
};