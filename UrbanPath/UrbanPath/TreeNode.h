#pragma once

#include "Station.h"

using namespace std;

class TreeNode
{
private:
    Station station;     // Station data stored in this node
    TreeNode* left;      // Pointer to left child
    TreeNode* right;     // Pointer to right child

public:
    // Constructors
    TreeNode();
    TreeNode(const Station& station);
    
    // Destructor
    ~TreeNode();
    
    // Getters
    Station getStation() const;
    TreeNode* getLeft() const;
    TreeNode* getRight() const;
    
    // Setters
    void setStation(const Station& station);
    void setLeft(TreeNode* left);
    void setRight(TreeNode* right);
    
    // Utility
    bool isLeaf() const;  // Check if node is a leaf (no children)
};

