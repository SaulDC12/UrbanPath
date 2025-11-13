#include "Graph.h"
#include <algorithm>
#include <limits>

const double INF = std::numeric_limits<double>::infinity();

// Constructor
Graph::Graph(bool isDirected) : directed(isDirected)
{
}

// Add a station to the graph
void Graph::addStation(const Station& station)
{
    int id = station.getId();
    
    if (stations.contains(id))
    {
        qDebug() << "Advertencia: La estacion con ID" << id << "ya existe. Se actualizara.";
    }
    
    stations[id] = station;
    
    // Initialize adjacency list if not exists
    if (!adjList.contains(id))
    {
        adjList[id] = QList<QPair<int, double>>();
    }
}

// Remove a station from the graph
void Graph::removeStation(int id)
{
    if (!stations.contains(id))
    {
        qDebug() << "Error: La estacion con ID" << id << "no existe.";
        return;
    }
    
    // Remove station
    stations.remove(id);
    
    // Remove all edges connected to this station
    adjList.remove(id);
    
    // Remove edges pointing to this station
    for (auto& neighbors : adjList)
    {
        for (int i = 0; i < neighbors.size(); i++)
        {
            if (neighbors[i].first == id)
            {
                neighbors.removeAt(i);
                i--;
            }
        }
    }
}

// Check if station exists
bool Graph::containsStation(int id) const
{
    return stations.contains(id);
}

// Get station by ID
Station* Graph::getStation(int id)
{
    if (stations.contains(id))
    {
        return &stations[id];
    }
    return nullptr;
}

// Get all stations
QList<Station> Graph::getAllStations() const
{
    return stations.values();
}

// Get station count
int Graph::getStationCount() const
{
    return stations.size();
}

// Add an edge between two stations
void Graph::addEdge(int origin, int destination, double weight)
{
    if (!stations.contains(origin))
    {
        qDebug() << "Error: Estacion origen" << origin << "no existe.";
        return;
    }
    
    if (!stations.contains(destination))
    {
        qDebug() << "Error: Estacion destino" << destination << "no existe.";
        return;
    }
    
    if (weight < 0)
    {
        qDebug() << "Advertencia: Peso negativo detectado. Se usara valor absoluto.";
        weight = qAbs(weight);
    }
    
    // Add edge from origin to destination
    adjList[origin].append(QPair<int, double>(destination, weight));
    
    // If undirected, add reverse edge
    if (!directed)
    {
        adjList[destination].append(QPair<int, double>(origin, weight));
    }
}

// Remove an edge
void Graph::removeEdge(int origin, int destination)
{
    if (!adjList.contains(origin))
    {
        return;
    }
    
    // Remove edge from origin to destination
    QList<QPair<int, double>>& neighbors = adjList[origin];
    for (int i = 0; i < neighbors.size(); i++)
    {
        if (neighbors[i].first == destination)
        {
            neighbors.removeAt(i);
            break;
        }
    }
    
    // If undirected, remove reverse edge
    if (!directed && adjList.contains(destination))
    {
        QList<QPair<int, double>>& reverseNeighbors = adjList[destination];
        for (int i = 0; i < reverseNeighbors.size(); i++)
        {
            if (reverseNeighbors[i].first == origin)
            {
                reverseNeighbors.removeAt(i);
                break;
            }
        }
    }
}

// Check if edge exists
bool Graph::hasEdge(int origin, int destination) const
{
    if (!adjList.contains(origin))
    {
        return false;
    }
    
    const QList<QPair<int, double>>& neighbors = adjList[origin];
    for (const auto& neighbor : neighbors)
    {
        if (neighbor.first == destination)
        {
            return true;
        }
    }
    
    return false;
}

// Get edge weight
double Graph::getEdgeWeight(int origin, int destination) const
{
    if (!adjList.contains(origin))
    {
        return INF;
    }
    
    const QList<QPair<int, double>>& neighbors = adjList[origin];
    for (const auto& neighbor : neighbors)
    {
        if (neighbor.first == destination)
        {
            return neighbor.second;
        }
    }
    
    return INF;
}

// Clear all data
void Graph::clear()
{
    stations.clear();
    adjList.clear();
}

// Check if graph is empty
bool Graph::isEmpty() const
{
    return stations.isEmpty();
}

// Get neighbors of a station
QList<QPair<int, double>> Graph::getNeighbors(int stationId) const
{
    if (adjList.contains(stationId))
    {
        return adjList[stationId];
    }
    return QList<QPair<int, double>>();
}

// BFS traversal
QList<int> Graph::bfs(int startId)
{
    QList<int> result;
    
    if (!stations.contains(startId))
    {
        qDebug() << "Error: Estacion inicial" << startId << "no existe.";
        return result;
    }
    
    QSet<int> visited;
    QQueue<int> queue;
    
    queue.enqueue(startId);
    visited.insert(startId);
    
    while (!queue.isEmpty())
    {
        int current = queue.dequeue();
        
        // Skip closed stations
        if (isStationClosed(current))
        {
            continue;
        }
        
        result.append(current);
        
        // Visit all neighbors
        if (adjList.contains(current))
        {
            const QList<QPair<int, double>>& neighbors = adjList[current];
            for (const auto& neighbor : neighbors)
            {
                int neighborId = neighbor.first;
                
                // Skip closed routes and stations
                if (isRouteClosed(current, neighborId) || isStationClosed(neighborId))
                {
                    continue;
                }
                
                if (!visited.contains(neighborId))
                {
                    visited.insert(neighborId);
                    queue.enqueue(neighborId);
                }
            }
        }
    }
    
    return result;
}

// DFS traversal
QList<int> Graph::dfs(int startId)
{
    QList<int> result;
    
    if (!stations.contains(startId))
    {
        qDebug() << "Error: Estacion inicial" << startId << "no existe.";
        return result;
    }
    
    QSet<int> visited;
    dfsHelper(startId, visited, result);
    
    return result;
}

// DFS helper (recursive)
void Graph::dfsHelper(int nodeId, QSet<int>& visited, QList<int>& result)
{
    // Skip closed stations
    if (isStationClosed(nodeId))
    {
        return;
    }
    
    visited.insert(nodeId);
    result.append(nodeId);
    
    if (adjList.contains(nodeId))
    {
        const QList<QPair<int, double>>& neighbors = adjList[nodeId];
        for (const auto& neighbor : neighbors)
        {
            int neighborId = neighbor.first;
            
            // Skip closed routes and stations
            if (isRouteClosed(nodeId, neighborId) || isStationClosed(neighborId))
            {
                continue;
            }
            
            if (!visited.contains(neighborId))
            {
                dfsHelper(neighborId, visited, result);
            }
        }
    }
}

// Dijkstra's shortest path algorithm
QHash<int, double> Graph::dijkstra(int startId)
{
    QHash<int, double> distances;
    
    if (!stations.contains(startId))
    {
        qDebug() << "Error: Estacion inicial" << startId << "no existe.";
        return distances;
    }
    
    // Initialize distances
    for (int id : stations.keys())
    {
        distances[id] = INF;
    }
    distances[startId] = 0.0;
    
    QSet<int> visited;
    
    while (visited.size() < stations.size())
    {
        // Find unvisited node with minimum distance
        int minNode = -1;
        double minDist = INF;
        
        for (int id : stations.keys())
        {
            if (!visited.contains(id) && distances[id] < minDist)
            {
                minDist = distances[id];
                minNode = id;
            }
        }
        
        if (minNode == -1)
        {
            break;  // No more reachable nodes
        }
        
        visited.insert(minNode);
        
        // Skip closed stations
        if (isStationClosed(minNode))
        {
            continue;
        }
        
        // Update distances to neighbors
        if (adjList.contains(minNode))
        {
            const QList<QPair<int, double>>& neighbors = adjList[minNode];
            for (const auto& neighbor : neighbors)
            {
                int neighborId = neighbor.first;
                double edgeWeight = neighbor.second;
                
                // Skip closed routes and stations
                if (isRouteClosed(minNode, neighborId) || isStationClosed(neighborId))
                {
                    continue;
                }
                
                double newDist = distances[minNode] + edgeWeight;
                
                if (newDist < distances[neighborId])
                {
                    distances[neighborId] = newDist;
                }
            }
        }
    }
    
    return distances;
}

// Dijkstra with path reconstruction
QPair<QHash<int, double>, QHash<int, int>> Graph::dijkstraWithPath(int startId)
{
    QHash<int, double> distances;
    QHash<int, int> predecessors;  // To reconstruct path
    
    if (!stations.contains(startId))
    {
        qDebug() << "Error: Estacion inicial" << startId << "no existe.";
        return QPair<QHash<int, double>, QHash<int, int>>(distances, predecessors);
    }
    
    // Initialize distances and predecessors
    for (int id : stations.keys())
    {
        distances[id] = INF;
        predecessors[id] = -1;  // No predecessor
    }
    distances[startId] = 0.0;
    
    QSet<int> visited;
    
    while (visited.size() < stations.size())
    {
        // Find unvisited node with minimum distance
        int minNode = -1;
        double minDist = INF;
        
        for (int id : stations.keys())
        {
            if (!visited.contains(id) && distances[id] < minDist)
            {
                minDist = distances[id];
                minNode = id;
            }
        }
        
        if (minNode == -1)
        {
            break;  // No more reachable nodes
        }
        
        visited.insert(minNode);
        
        // Skip closed stations
        if (isStationClosed(minNode))
        {
            continue;
        }
        
        // Update distances to neighbors
        if (adjList.contains(minNode))
        {
            const QList<QPair<int, double>>& neighbors = adjList[minNode];
            for (const auto& neighbor : neighbors)
            {
                int neighborId = neighbor.first;
                double edgeWeight = neighbor.second;
                
                // Skip closed routes and stations
                if (isRouteClosed(minNode, neighborId) || isStationClosed(neighborId))
                {
                    continue;
                }
                
                double newDist = distances[minNode] + edgeWeight;
                
                if (newDist < distances[neighborId])
                {
                    distances[neighborId] = newDist;
                    predecessors[neighborId] = minNode;  // Store predecessor
                }
            }
        }
    }
    
    return QPair<QHash<int, double>, QHash<int, int>>(distances, predecessors);
}

// Floyd-Warshall all-pairs shortest path
QHash<QPair<int, int>, double> Graph::floydWarshall()
{
    QHash<QPair<int, int>, double> dist;
    QList<int> nodeIds = stations.keys();
    
    // Initialize distances
    for (int i : nodeIds)
    {
        for (int j : nodeIds)
        {
            if (i == j)
            {
                dist[QPair<int, int>(i, j)] = 0.0;
            }
            else if (hasEdge(i, j))
            {
                dist[QPair<int, int>(i, j)] = getEdgeWeight(i, j);
            }
            else
            {
                dist[QPair<int, int>(i, j)] = INF;
            }
        }
    }
    
    // Floyd-Warshall algorithm
    for (int k : nodeIds)
    {
        for (int i : nodeIds)
        {
            for (int j : nodeIds)
            {
                QPair<int, int> ij(i, j);
                QPair<int, int> ik(i, k);
                QPair<int, int> kj(k, j);
                
                double throughK = dist[ik] + dist[kj];
                if (throughK < dist[ij])
                {
                    dist[ij] = throughK;
                }
            }
        }
    }
    
    return dist;
}

// Get all edges in the graph
QList<Edge> Graph::getAllEdges() const
{
    QList<Edge> edges;
    QSet<QPair<int, int>> addedEdges;
    
    for (auto it = adjList.begin(); it != adjList.end(); ++it)
    {
        int from = it.key();
        const QList<QPair<int, double>>& neighbors = it.value();
        
        for (const auto& neighbor : neighbors)
        {
            int to = neighbor.first;
            double weight = neighbor.second;
            
            // For undirected graphs, avoid duplicate edges
            if (!directed)
            {
                QPair<int, int> edgePair = (from < to) ? QPair<int, int>(from, to) : QPair<int, int>(to, from);
                if (addedEdges.contains(edgePair))
                {
                    continue;
                }
                addedEdges.insert(edgePair);
            }
            
            edges.append(Edge(from, to, weight));
        }
    }
    
    return edges;
}

// Prim's MST algorithm
QList<QPair<int, int>> Graph::primMST()
{
    QList<QPair<int, int>> mstEdges;
    
    if (stations.isEmpty())
    {
        qDebug() << "Error: El grafo esta vacio.";
        return mstEdges;
    }
    
    QSet<int> inMST;
    QList<int> nodeIds = stations.keys();
    
    // Start with first node
    int startNode = nodeIds.first();
    inMST.insert(startNode);
    
    while (inMST.size() < stations.size())
    {
        double minWeight = INF;
        int minFrom = -1;
        int minTo = -1;
        
        // Find minimum edge connecting MST to non-MST node
        for (int node : inMST)
        {
            // Skip closed stations
            if (isStationClosed(node))
            {
                continue;
            }
            
            if (adjList.contains(node))
            {
                const QList<QPair<int, double>>& neighbors = adjList[node];
                for (const auto& neighbor : neighbors)
                {
                    int neighborId = neighbor.first;
                    double weight = neighbor.second;
                    
                    // Skip closed routes and stations
                    if (isRouteClosed(node, neighborId) || isStationClosed(neighborId))
                    {
                        continue;
                    }
                    
                    if (!inMST.contains(neighborId) && weight < minWeight)
                    {
                        minWeight = weight;
                        minFrom = node;
                        minTo = neighborId;
                    }
                }
            }
        }
        
        if (minTo == -1)
        {
            break;  // No more reachable nodes (disconnected graph)
        }
        
        mstEdges.append(QPair<int, int>(minFrom, minTo));
        inMST.insert(minTo);
    }
    
    return mstEdges;
}

// Kruskal's MST algorithm using DisjointSet
QList<QPair<int, int>> Graph::kruskalMST()
{
    QList<QPair<int, int>> mstEdges;
    
    if (stations.isEmpty())
    {
        qDebug() << "Error: El grafo esta vacio.";
        return mstEdges;
    }
    
    // Get all edges and sort by weight
    QList<Edge> edges = getAllEdges();
    std::sort(edges.begin(), edges.end());
    
    // Create disjoint set
    int maxId = 0;
    for (int id : stations.keys())
    {
        if (id > maxId)
        {
            maxId = id;
        }
    }
    
    DisjointSet ds(maxId);
    
    // Process edges in order of increasing weight
    for (const Edge& edge : edges)
    {
        int u = edge.from;
        int v = edge.to;
        
        // Skip closed routes and stations
        if (isRouteClosed(u, v) || isStationClosed(u) || isStationClosed(v))
        {
            continue;
        }
        
        // If adding this edge doesn't create a cycle
        if (!ds.connected(u, v))
        {
            mstEdges.append(QPair<int, int>(u, v));
            ds.unionSets(u, v);
            
            // MST complete when we have n-1 edges
            if (mstEdges.size() == stations.size() - 1)
            {
                break;
            }
        }
    }
    
    return mstEdges;
}

// Print graph structure
void Graph::printGraph() const
{
    qDebug() << "\n=== ESTRUCTURA DEL GRAFO ===";
    qDebug() << "Tipo:" << (directed ? "Dirigido" : "No dirigido");
    qDebug() << "Estaciones:" << stations.size();
    qDebug() << "Rutas:" << getAllEdges().size();
    
    qDebug() << "\nEstaciones registradas:";
    for (const Station& station : stations.values())
    {
        qDebug() << "  " << station.toString();
    }
}

// Print adjacency list
void Graph::printAdjacencyList() const
{
    qDebug() << "\n=== LISTA DE ADYACENCIA ===";
    
    for (auto it = adjList.begin(); it != adjList.end(); ++it)
    {
        int stationId = it.key();
        const QList<QPair<int, double>>& neighbors = it.value();
        
        QString line = QString("Estacion %1: ").arg(stationId);
        
        if (neighbors.isEmpty())
        {
            line += "(sin conexiones)";
        }
        else
        {
            for (int i = 0; i < neighbors.size(); i++)
            {
                line += QString("-> %1 (peso: %2)")
                    .arg(neighbors[i].first)
                    .arg(neighbors[i].second, 0, 'f', 1);
                
                if (i < neighbors.size() - 1)
                {
                    line += " ";
                }
            }
        }
        
        qDebug() << line;
    }
}

// ========== CLOSURE MANAGEMENT ==========

// Check if a station is closed
bool Graph::isStationClosed(int id) const
{
    return closedStations.contains(id);
}

// Check if a route is closed (checks both directions for undirected graphs)
bool Graph::isRouteClosed(int a, int b) const
{
    QPair<int, int> route1(a, b);
    QPair<int, int> route2(b, a);
    
    for (const auto& route : closedRoutes)
    {
        if (route == route1 || route == route2)
        {
            return true;
        }
    }
    
    return false;
}

// Close a station (block it)
void Graph::closeStation(int id)
{
    if (!stations.contains(id))
    {
        qDebug() << "Advertencia: No se puede cerrar estacion" << id << "(no existe).";
        return;
    }
    
    if (!closedStations.contains(id))
    {
        closedStations.insert(id);
        qDebug() << "Estacion" << id << "cerrada (bloqueada).";
    }
}

// Close a route (block it)
void Graph::closeRoute(int a, int b)
{
    if (!stations.contains(a) || !stations.contains(b))
    {
        qDebug() << "Advertencia: No se puede cerrar ruta" << a << "->" << b << "(estaciones no existen).";
        return;
    }
    
    QPair<int, int> route(a, b);
    
    if (!isRouteClosed(a, b))
    {
        closedRoutes.append(route);
        qDebug() << "Ruta" << a << "<->" << b << "cerrada (bloqueada).";
    }
}

// Open a station (unblock it)
void Graph::openStation(int id)
{
    if (closedStations.contains(id))
    {
        closedStations.remove(id);
        qDebug() << "Estacion" << id << "abierta (desbloqueada).";
    }
}

// Open a route (unblock it)
void Graph::openRoute(int a, int b)
{
    QPair<int, int> route1(a, b);
    QPair<int, int> route2(b, a);
    
    for (int i = 0; i < closedRoutes.size(); i++)
    {
        if (closedRoutes[i] == route1 || closedRoutes[i] == route2)
        {
            closedRoutes.removeAt(i);
            qDebug() << "Ruta" << a << "<->" << b << "abierta (desbloqueada).";
            return;
        }
    }
}

// Clear all closures
void Graph::clearClosures()
{
    closedStations.clear();
    closedRoutes.clear();
    qDebug() << "Todos los cierres han sido eliminados.";
}

// Get list of closed stations
QSet<int> Graph::getClosedStations() const
{
    return closedStations;
}

// Get list of closed routes
QList<QPair<int, int>> Graph::getClosedRoutes() const
{
    return closedRoutes;
}
