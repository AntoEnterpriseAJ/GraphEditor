#pragma once
#include "GraphEditor.h"
#include "MazeEditor.h"

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
    GraphEditor m_graphEditor;
    MazeEditor m_Maze;
};