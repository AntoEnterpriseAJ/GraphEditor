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
	void renderGraphEditorUI();
	void renderToolbar();
private:
	State m_state;
	Graph m_graphEditor;
	Graph m_graphBFS;
};