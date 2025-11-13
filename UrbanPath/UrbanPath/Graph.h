#pragma once

#include "Station.h"
#include "DisjointSet.h"
#include <QList>
#include <QPair>
#include <QHash>
#include <QSet>
#include <QQueue>
#include <QDebug>

using namespace std;

// Edge structure for MST algorithms
struct Edge
{
    int from;
    int to;
    double weight;
    
    Edge(int f, int t, double w) : from(f), to(t), weight(w) {}
    
    // For sorting edges by weight
    bool operator<(const Edge& other) const
    {
        return weight < other.weight;
    }
};

class Graph
{
private:
    QHash<int, QList<QPair<int, double>>> adjList;  // Adjacency list (destination, weight)
    QHash<int, Station> stations;                    // Registered stations
    bool directed;                                   // Directed or undirected graph
    
    // Closures (blocked stations and routes)
    QSet<int> closedStations;                        // Blocked stations
    QList<QPair<int, int>> closedRoutes;            // Blocked routes
    
    // Accidents (increased weights on routes)
    QSet<QPair<int, int>> affectedRoutes;            // Routes with accidents applied
    QHash<QPair<int, int>, double> originalWeights;  // Original weights before accidents
    
    // Helper methods for DFS
    void dfsHelper(int nodeId, QSet<int>& visited, QList<int>& result);
    
    // Helper to get all edges
    QList<Edge> getAllEdges() const;

public:
    // Constructor
    Graph(bool isDirected = false);
    
    // Station management
    void addStation(const Station& station);
    void removeStation(int id);
    bool containsStation(int id) const;
    Station* getStation(int id);
    QList<Station> getAllStations() const;
    int getStationCount() const;
    
    // Edge management
    void addEdge(int origin, int destination, double weight);
    void removeEdge(int origin, int destination);
    bool hasEdge(int origin, int destination) const;
    double getEdgeWeight(int origin, int destination) const;
    
    // Graph operations
    void clear();
    bool isEmpty() const;
    
    // Traversal algorithms
    QList<int> bfs(int startId);
    QList<int> dfs(int startId);
    
    // Shortest path algorithms
    QHash<int, double> dijkstra(int startId);
    QPair<QHash<int, double>, QHash<int, int>> dijkstraWithPath(int startId);
    QHash<QPair<int, int>, double> floydWarshall();
    
    // Minimum spanning tree algorithms
    QList<QPair<int, int>> primMST();
    QList<QPair<int, int>> kruskalMST();
    
    // Utility methods
    void printGraph() const;
    void printAdjacencyList() const;
    
    // Get neighbors of a station
    QList<QPair<int, double>> getNeighbors(int stationId) const;
    
    // Closure management (blocking stations and routes)
    bool isStationClosed(int id) const;
    bool isRouteClosed(int a, int b) const;
    void closeStation(int id);
    void closeRoute(int a, int b);
    void openStation(int id);
    void openRoute(int a, int b);
    void clearClosures();
    QSet<int> getClosedStations() const;
    QList<QPair<int, int>> getClosedRoutes() const;
    
    // Accident management (increase route weights)
    bool applyAccident(int originId, int destId, double increment);
    bool loadAccidents(const QString& filename);
    void clearAccidents();
    bool restoreOriginalWeights();
    QSet<QPair<int, int>> getAffectedRoutes() const;
};

