#pragma once
#include "GraphEditor.h"
#include "MazeEditor.h"
#include "MapEditor.h"

class Application
{
public:
    enum class State
    {
        GraphEditor,
        Maze,
        Map,
    };
        
    Application();
    ~Application();

    void render();

private:
    void renderUI();
    void renderGraphEditorUI();
    void renderMazeUI();
    void renderMapUI();
    void renderToolbar();
private:
    State m_state;
    GraphEditor m_graphEditor;
    MazeEditor  m_Maze;
    MapEditor   m_map;
};