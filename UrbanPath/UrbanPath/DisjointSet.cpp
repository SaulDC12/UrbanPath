#include "DisjointSet.h"
#include <QMap>
#include <QSet>

// Constructor - Initialize n disjoint sets (1 to n)
DisjointSet::DisjointSet(int n) : size(n)
{
    // Resize vectors to hold n+1 elements (index 0 unused, 1 to n used)
    parent.resize(n + 1);
    rank.resize(n + 1);
    
    // Initialize each element as its own parent with rank 0
    for (int i = 1; i <= n; i++)
    {
        parent[i] = i;
        rank[i] = 0;
    }
}

// Find the representative of the set containing 'node' with path compression
int DisjointSet::find(int node)
{
    // Validate input
    if (node < 1 || node > size)
    {
        qDebug() << "Error: Nodo" << node << "fuera de rango [1," << size << "]";
        return -1;
    }
    
    // Path compression: make every node point directly to root
    if (parent[node] != node)
    {
        parent[node] = find(parent[node]);
    }
    
    return parent[node];
}

// Union two sets by rank
void DisjointSet::unionSets(int u, int v)
{
    // Find representatives of both sets
    int rootU = find(u);
    int rootV = find(v);
    
    // Check for invalid nodes
    if (rootU == -1 || rootV == -1)
    {
        return;
    }
    
    // If already in same set, do nothing
    if (rootU == rootV)
    {
        return;
    }
    
    // Union by rank: attach smaller tree under root of deeper tree
    if (rank[rootU] < rank[rootV])
    {
        parent[rootU] = rootV;
    }
    else if (rank[rootU] > rank[rootV])
    {
        parent[rootV] = rootU;
    }
    else
    {
        // If ranks are equal, make one root and increment its rank
        parent[rootV] = rootU;
        rank[rootU]++;
    }
}

// Check if two elements are in the same set
bool DisjointSet::connected(int u, int v)
{
    int rootU = find(u);
    int rootV = find(v);
    
    // Check for invalid nodes
    if (rootU == -1 || rootV == -1)
    {
        return false;
    }
    
    return rootU == rootV;
}

// Reset all sets to individual elements
void DisjointSet::reset()
{
    for (int i = 1; i <= size; i++)
    {
        parent[i] = i;
        rank[i] = 0;
    }
    qDebug() << "Disjoint Set reiniciado:" << size << "conjuntos independientes.";
}

// Print current sets grouped by root
void DisjointSet::printSets() const
{
    qDebug() << "\nConjuntos actuales del Disjoint Set:";
    
    // Group elements by their root
    QMap<int, QVector<int>> sets;
    
    for (int i = 1; i <= size; i++)
    {
        // Need to find root without modifying parent (const method)
        int root = i;
        while (parent[root] != root)
        {
            root = parent[root];
        }
        sets[root].append(i);
    }
    
    // Print each set
    QString output;
    for (auto it = sets.begin(); it != sets.end(); ++it)
    {
        output += "{";
        const QVector<int>& members = it.value();
        for (int i = 0; i < members.size(); i++)
        {
            output += QString::number(members[i]);
            if (i < members.size() - 1)
            {
                output += ", ";
            }
        }
        output += "} ";
    }
    
    qDebug() << output.trimmed();
}

// Get total number of elements
int DisjointSet::getSize() const
{
    return size;
}
