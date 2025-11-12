#pragma once

#include <QHash>
#include <QVector>
#include <QDebug>

using namespace std;

class DisjointSet
{
private:
    QVector<int> parent;  // Parent of each element
    QVector<int> rank;    // Rank (depth) of each tree for union by rank
    int size;             // Total number of elements

public:
    // Constructor
    DisjointSet(int n);
    
    // Core operations
    int find(int node);                    // Find representative with path compression
    void unionSets(int u, int v);          // Union two sets by rank
    bool connected(int u, int v);          // Check if two elements are in same set
    
    // Utility methods
    void reset();                          // Reset all sets to individual elements
    void printSets() const;                // Print current sets grouped by root
    int getSize() const;                   // Get total number of elements
};

