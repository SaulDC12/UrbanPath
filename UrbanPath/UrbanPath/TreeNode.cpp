#include "TreeNode.h"

// Default constructor
TreeNode::TreeNode() : station(), left(nullptr), right(nullptr)
{
}

// Parameterized constructor
TreeNode::TreeNode(const Station& station) 
    : station(station), left(nullptr), right(nullptr)
{
}

// Destructor - does NOT delete children (BST handles this)
TreeNode::~TreeNode()
{
    // Children are managed by the BST class
    left = nullptr;
    right = nullptr;
}

// Getters
Station TreeNode::getStation() const
{
    return station;
}

TreeNode* TreeNode::getLeft() const
{
    return left;
}

TreeNode* TreeNode::getRight() const
{
    return right;
}

// Setters
void TreeNode::setStation(const Station& station)
{
    this->station = station;
}

void TreeNode::setLeft(TreeNode* left)
{
    this->left = left;
}

void TreeNode::setRight(TreeNode* right)
{
    this->right = right;
}

// Utility
bool TreeNode::isLeaf() const
{
    return (left == nullptr && right == nullptr);
}
