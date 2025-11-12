#include "StationBST.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

// Constructor
StationBST::StationBST() : root(nullptr)
{
}

// Destructor
StationBST::~StationBST()
{
    clear();
}

// Insert a station into the BST
void StationBST::insert(const Station& station)
{
    root = insertHelper(root, station);
}

// Private helper for insertion (recursive)
TreeNode* StationBST::insertHelper(TreeNode* node, const Station& station)
{
    if (node == nullptr)
    {
        return new TreeNode(station);
    }
    
    if (station.getId() < node->getStation().getId())
    {
        node->setLeft(insertHelper(node->getLeft(), station));
    }
    else if (station.getId() > node->getStation().getId())
    {
        node->setRight(insertHelper(node->getRight(), station));
    }
    // If IDs are equal, don't insert (no duplicates)
    
    return node;
}

// Search for a station by ID
Station* StationBST::search(int id)
{
    TreeNode* result = searchHelper(root, id);
    if (result != nullptr)
    {
        // Return pointer to the station in the node
        static Station temp;
        temp = result->getStation();
        return &temp;
    }
    return nullptr;
}

// Private helper for search (recursive)
TreeNode* StationBST::searchHelper(TreeNode* node, int id) const
{
    if (node == nullptr)
    {
        return nullptr;
    }
    
    int nodeId = node->getStation().getId();
    
    if (id == nodeId)
    {
        return node;
    }
    else if (id < nodeId)
    {
        return searchHelper(node->getLeft(), id);
    }
    else
    {
        return searchHelper(node->getRight(), id);
    }
}

// Remove a station by ID
bool StationBST::remove(int id)
{
    bool found = false;
    root = removeHelper(root, id, found);
    return found;
}

// Private helper for removal (recursive)
TreeNode* StationBST::removeHelper(TreeNode* node, int id, bool& found)
{
    if (node == nullptr)
    {
        found = false;
        return nullptr;
    }
    
    int nodeId = node->getStation().getId();
    
    if (id < nodeId)
    {
        node->setLeft(removeHelper(node->getLeft(), id, found));
    }
    else if (id > nodeId)
    {
        node->setRight(removeHelper(node->getRight(), id, found));
    }
    else
    {
        // Node found
        found = true;
        
        // Case 1: Leaf node
        if (node->isLeaf())
        {
            delete node;
            return nullptr;
        }
        // Case 2: One child
        else if (node->getLeft() == nullptr)
        {
            TreeNode* temp = node->getRight();
            delete node;
            return temp;
        }
        else if (node->getRight() == nullptr)
        {
            TreeNode* temp = node->getLeft();
            delete node;
            return temp;
        }
        // Case 3: Two children
        else
        {
            TreeNode* minNode = findMin(node->getRight());
            node->setStation(minNode->getStation());
            node->setRight(removeHelper(node->getRight(), minNode->getStation().getId(), found));
        }
    }
    
    return node;
}

// Find minimum node in subtree
TreeNode* StationBST::findMin(TreeNode* node) const
{
    while (node != nullptr && node->getLeft() != nullptr)
    {
        node = node->getLeft();
    }
    return node;
}

// Check if tree is empty
bool StationBST::isEmpty() const
{
    return root == nullptr;
}

// Count nodes in tree
int StationBST::count() const
{
    return countHelper(root);
}

// Private helper for counting nodes
int StationBST::countHelper(TreeNode* node) const
{
    if (node == nullptr)
    {
        return 0;
    }
    return 1 + countHelper(node->getLeft()) + countHelper(node->getRight());
}

// In-order traversal (Left, Root, Right)
QList<Station> StationBST::inOrder() const
{
    QList<Station> result;
    inOrderHelper(root, result);
    return result;
}

void StationBST::inOrderHelper(TreeNode* node, QList<Station>& result) const
{
    if (node != nullptr)
    {
        inOrderHelper(node->getLeft(), result);
        result.append(node->getStation());
        inOrderHelper(node->getRight(), result);
    }
}

// Pre-order traversal (Root, Left, Right)
QList<Station> StationBST::preOrder() const
{
    QList<Station> result;
    preOrderHelper(root, result);
    return result;
}

void StationBST::preOrderHelper(TreeNode* node, QList<Station>& result) const
{
    if (node != nullptr)
    {
        result.append(node->getStation());
        preOrderHelper(node->getLeft(), result);
        preOrderHelper(node->getRight(), result);
    }
}

// Post-order traversal (Left, Right, Root)
QList<Station> StationBST::postOrder() const
{
    QList<Station> result;
    postOrderHelper(root, result);
    return result;
}

void StationBST::postOrderHelper(TreeNode* node, QList<Station>& result) const
{
    if (node != nullptr)
    {
        postOrderHelper(node->getLeft(), result);
        postOrderHelper(node->getRight(), result);
        result.append(node->getStation());
    }
}

// Export traversals to file
bool StationBST::exportTraversals(const QString& filename) const
{
    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error: No se pudo abrir el archivo" << filename << "para escritura.";
        return false;
    }
    
    QTextStream out(&file);
    
    // Export In-Order traversal
    out << "=== RECORRIDO IN-ORDER (Izquierda, Raiz, Derecha) ===\n";
    QList<Station> inOrderList = inOrder();
    for (const Station& station : inOrderList)
    {
        out << station.toString() << "\n";
    }
    out << "\n";
    
    // Export Pre-Order traversal
    out << "=== RECORRIDO PRE-ORDER (Raiz, Izquierda, Derecha) ===\n";
    QList<Station> preOrderList = preOrder();
    for (const Station& station : preOrderList)
    {
        out << station.toString() << "\n";
    }
    out << "\n";
    
    // Export Post-Order traversal
    out << "=== RECORRIDO POST-ORDER (Izquierda, Derecha, Raiz) ===\n";
    QList<Station> postOrderList = postOrder();
    for (const Station& station : postOrderList)
    {
        out << station.toString() << "\n";
    }
    
    file.close();
    
    qDebug() << "Recorridos exportados exitosamente a:" << filename;
    return true;
}

// Clear all nodes
void StationBST::clear()
{
    clearHelper(root);
    root = nullptr;
}

// Private helper for clearing (recursive)
void StationBST::clearHelper(TreeNode* node)
{
    if (node != nullptr)
    {
        clearHelper(node->getLeft());
        clearHelper(node->getRight());
        delete node;
    }
}
