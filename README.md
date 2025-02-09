# OpenGL Graph, Maze, and Map Editors

A C++ project that visualizes and manipulates various graphs, mazes, and map data using **OpenGL** for rendering. It includes:

- **Graph Editor**: Custom node/edge drawing, plus **comprehensive graph algorithms**.  
- **Maze Editor**: Load a maze from a text file and solve it (BFS).  
- **Map Editor**: Load a map (XML) and find shortest paths using Dijkstra.

## Features

### 1. Graph Editor
- **Custom OpenGL Rendering**  
  Draw nodes, edges, and highlights from scratch.
- **Graph Algorithms**  
  - **BFS** (Breadth-First Search)  
  - **DFS** (Depth-First Search), with a “total” DFS variant for unconnected components  
  - **Topological Sort**  
  - **Shortest Path** using **Dijkstra**  
  - **MST** (Minimum Spanning Tree) with **Prim’s**, **Kruskal’s**, and **Generic** MST  
  - **Connectivity**: **Weakly** and **Strongly** connected components, `isWeaklyConnected()`, `reconstructGraphFromComponents()`, etc.  
  - **Cycle Detection**: `checkCycles()`  
  - **Tree Check**: `isTree()`, `findRoot()`  
  - **Min-Cut** via **Ford-Fulkerson** (augmenting path search)  
  - **Undo / Inverse**: Maintains a stack of changes; flip edge directions.  
- **Orientation & Weighting**  
  Toggle directed/undirected and weighted/unweighted edges.  
- **Logging**  
  Option to log the adjacency matrix to a file.

### 2. Maze Editor
- **Load Maze from File**  
  Text file describing walls, paths, entrances, and exits.  
- **Render Maze**  
  Each cell is drawn as a node with color-coded cell types.  
- **Solve Maze**  
  Finds a path from entrance(s) to exit(s) using BFS.

### 3. Map Editor
- **Load Map from XML**  
  Parse lat/long data and convert it to screen coordinates.  
- **Render Map**  
  Each location is displayed as a node on the screen.  
- **Shortest Path**  
  Select two nodes and use Dijkstra’s algorithm to compute the path.

## Screenshots
![image](https://github.com/user-attachments/assets/880f8dec-14aa-4d47-a5ef-3dca3d45dded)
![image](https://github.com/user-attachments/assets/41ab6c53-38c0-4518-97bb-24cbbefc7730)
![image](https://github.com/user-attachments/assets/5f810a00-5204-4bcd-bf51-ef6df8ac12aa)

## Contributing
This project was created for study purposes. Contributions are welcome but not actively sought.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
