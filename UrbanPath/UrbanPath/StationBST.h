#pragma once

#include "TreeNode.h"
#include "Station.h"
#include <QString>
#include <QList>

using namespace std;

class StationBST
{
private:
    TreeNode* root;
    
    // Private helper methods for recursive operations
    TreeNode* insertHelper(TreeNode* node, const Station& station);
    TreeNode* searchHelper(TreeNode* node, int id) const;
    TreeNode* removeHelper(TreeNode* node, int id, bool& found);
    TreeNode* findMin(TreeNode* node) const;
    
    // Recursive traversal helpers
    void inOrderHelper(TreeNode* node, QList<Station>& result) const;
    void preOrderHelper(TreeNode* node, QList<Station>& result) const;
    void postOrderHelper(TreeNode* node, QList<Station>& result) const;
    
    // Memory management
    void clearHelper(TreeNode* node);
    
public:
    // Constructor and Destructor
    StationBST();
    ~StationBST();
    
    // Core BST operations
    void insert(const Station& station);
    Station* search(int id);
    bool remove(int id);
    bool isEmpty() const;
    int count() const;
    
    // Traversal methods
    QList<Station> inOrder() const;
    QList<Station> preOrder() const;
    QList<Station> postOrder() const;
    
    // File export
    bool exportTraversals(const QString& filename) const;
    
    // Utility
    void clear();
    
private:
    // Count helper
    int countHelper(TreeNode* node) const;
};

