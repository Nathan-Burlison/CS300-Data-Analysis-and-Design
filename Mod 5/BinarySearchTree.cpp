//============================================================================
// Name        : BinarySearchTree.cpp
// Author      : Nathan Burlison
// Version     : 1.0
// Copyright   : Copyright 2023 SNHU COCE
// Description : Lab 5-2 Binary Search Tree
//============================================================================

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// Forward declarations
double strToDouble(string str, char ch);

// Define a structure to hold bid information.
struct Bid {
    string bidId; // Unique identifier
    string title;
    string fund;
    double amount;

    Bid() {
        amount = 0.0;
    }
};

// Internal structure for a tree node.
struct Node {
    Bid bid;
    Node* left;
    Node* right;

    // Default constructor creates a leaf node.
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // Initialize a node with a bid.
    Node(Bid aBid) : Node() {
        bid = aBid;
    }
};

//============================================================================
// Binary Search Tree class definition
//============================================================================

/**
 * Class containing the data members and methods needed to implement a
 * binary search tree of bids. Bid IDs are used as the tree keys.
 */
class BinarySearchTree {

private:
    Node* root;

    void addNode(Node* node, Bid bid);
    void inOrder(Node* node);
    void postOrder(Node* node);
    void preOrder(Node* node);
    Node* removeNode(Node* node, string bidId);
    void destroyTree(Node* node);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void InOrder();
    void PostOrder();
    void PreOrder();
    void Insert(Bid bid);
    void Remove(string bidId);
    Bid Search(string bidId);
};

/**
 * Default constructor.
 */
BinarySearchTree::BinarySearchTree() {
    // An empty tree begins with no root node.
    root = nullptr;
}

/**
 * Destructor.
 */
BinarySearchTree::~BinarySearchTree() {
    // Delete every node with a post-order cleanup so children are deleted
    // before their parent node.
    destroyTree(root);
    root = nullptr;
}

/**
 * Recursively release all nodes in the tree.
 *
 * @param node Current node being deleted
 */
void BinarySearchTree::destroyTree(Node* node) {
    if (node == nullptr) {
        return;
    }

    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

/**
 * Traverse the tree in order: left subtree, node, right subtree.
 */
void BinarySearchTree::InOrder() {
    inOrder(root);
}

/**
 * Traverse the tree in post-order: left subtree, right subtree, node.
 */
void BinarySearchTree::PostOrder() {
    postOrder(root);
}

/**
 * Traverse the tree in pre-order: node, left subtree, right subtree.
 */
void BinarySearchTree::PreOrder() {
    preOrder(root);
}

/**
 * Insert a bid into the binary search tree.
 *
 * @param bid Bid to insert
 */
void BinarySearchTree::Insert(Bid bid) {
    // The first bid becomes the root. Later bids are placed recursively.
    if (root == nullptr) {
        root = new Node(bid);
    } else {
        addNode(root, bid);
    }
}

/**
 * Remove a bid by its bid ID.
 *
 * @param bidId Key of the bid to remove
 */
void BinarySearchTree::Remove(string bidId) {
    // removeNode returns the updated subtree root. Assigning the result is
    // required because deleting the root can change the tree's root pointer.
    root = removeNode(root, bidId);
}

/**
 * Search for a bid by bid ID.
 *
 * @param bidId Key to find
 * @return Matching bid, or an empty Bid if the key is not found
 */
Bid BinarySearchTree::Search(string bidId) {
    Node* current = root;

    // Follow one branch at each comparison until a match is found or the
    // search reaches an empty child pointer.
    while (current != nullptr) {
        if (current->bid.bidId == bidId) {
            return current->bid;
        }

        if (bidId < current->bid.bidId) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    // The default Bid has an empty bidId, which main() uses as "not found."
    return Bid();
}

/**
 * Add a bid below an existing node.
 *
 * @param node Current node in the tree
 * @param bid Bid to add
 */
void BinarySearchTree::addNode(Node* node, Bid bid) {
    // Smaller bid IDs belong in the left subtree. Equal or larger IDs go to
    // the right subtree so insertion and searching use a consistent rule.
    if (bid.bidId < node->bid.bidId) {
        if (node->left == nullptr) {
            node->left = new Node(bid);
        } else {
            addNode(node->left, bid);
        }
    } else {
        if (node->right == nullptr) {
            node->right = new Node(bid);
        } else {
            addNode(node->right, bid);
        }
    }
}

/**
 * Display bids in ascending order by bid ID.
 *
 * @param node Current node in the traversal
 */
void BinarySearchTree::inOrder(Node* node) {
    if (node == nullptr) {
        return;
    }

    inOrder(node->left);
    cout << node->bid.bidId << ": " << node->bid.title << " | "
         << node->bid.amount << " | " << node->bid.fund << endl;
    inOrder(node->right);
}

/**
 * Display bids using post-order traversal.
 *
 * @param node Current node in the traversal
 */
void BinarySearchTree::postOrder(Node* node) {
    if (node == nullptr) {
        return;
    }

    postOrder(node->left);
    postOrder(node->right);
    cout << node->bid.bidId << ": " << node->bid.title << " | "
         << node->bid.amount << " | " << node->bid.fund << endl;
}

/**
 * Display bids using pre-order traversal.
 *
 * @param node Current node in the traversal
 */
void BinarySearchTree::preOrder(Node* node) {
    if (node == nullptr) {
        return;
    }

    cout << node->bid.bidId << ": " << node->bid.title << " | "
         << node->bid.amount << " | " << node->bid.fund << endl;
    preOrder(node->left);
    preOrder(node->right);
}

/**
 * Remove a bid from a subtree.
 *
 * @param node Root of the current subtree
 * @param bidId Key of the bid to remove
 * @return Updated root of the current subtree
 */
Node* BinarySearchTree::removeNode(Node* node, string bidId) {
    // Reaching nullptr means the bid does not exist in this subtree.
    if (node == nullptr) {
        return nullptr;
    }

    // Locate the node using the same ordering rule used by insertion.
    if (bidId < node->bid.bidId) {
        node->left = removeNode(node->left, bidId);
    } else if (bidId > node->bid.bidId) {
        node->right = removeNode(node->right, bidId);
    } else {
        // Case 1: The node is a leaf.
        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            return nullptr;
        }

        // Case 2: The node has only a right child.
        if (node->left == nullptr) {
            Node* child = node->right;
            delete node;
            return child;
        }

        // Case 3: The node has only a left child.
        if (node->right == nullptr) {
            Node* child = node->left;
            delete node;
            return child;
        }

        // Case 4: The node has two children. Replace its bid with the
        // smallest bid in the right subtree, then delete that successor.
        Node* successor = node->right;
        while (successor->left != nullptr) {
            successor = successor->left;
        }

        node->bid = successor->bid;
        node->right = removeNode(node->right, successor->bid.bidId);
    }

    return node;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display bid information to the console.
 *
 * @param bid Struct containing the bid information
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
         << bid.fund << endl;
}

/**
 * Load a CSV file containing bids into a binary search tree.
 *
 * @param csvPath Path to the CSV file
 * @param bst Tree that receives the bids
 */
void loadBids(string csvPath, BinarySearchTree* bst) {
    cout << "Loading CSV file " << csvPath << endl;

    try {
        // Initialize the CSV parser using the supplied path.
        csv::Parser file = csv::Parser(csvPath);

        // Read and display the header row.
        vector<string> header = file.getHeader();
        for (auto const& column : header) {
            cout << column << " | ";
        }
        cout << endl;

        // Convert each CSV row into a Bid and insert it into the tree.
        for (unsigned int i = 0; i < file.rowCount(); ++i) {
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');
            bst->Insert(bid);
        }

        cout << file.rowCount() << " bids read" << endl;
    } catch (csv::Error& error) {
        cerr << error.what() << endl;
    }
}

/**
 * Convert a currency string to a double after removing unwanted characters.
 *
 * @param str String to convert
 * @param ch Character to remove
 * @return Numeric value represented by the string
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    str.erase(remove(str.begin(), str.end(), ','), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method.
 */
int main(int argc, char* argv[]) {

    // Process command-line arguments.
    string csvPath;
    string bidKey;

    switch (argc) {
        case 2:
            csvPath = argv[1];
            bidKey = "98223";
            break;
        case 3:
            csvPath = argv[1];
            bidKey = argv[2];
            break;
        default:
            csvPath = "eBid_Monthly_Sales.csv";
            bidKey = "98223";
            break;
    }

    clock_t ticks;

    // Define a binary search tree to hold all bids.
    BinarySearchTree* bst = new BinarySearchTree();
    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                ticks = clock();
                loadBids(csvPath, bst);
                ticks = clock() - ticks;

                cout << "time: " << ticks << " clock ticks" << endl;
                cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC
                     << " seconds" << endl;
                break;

            case 2:
                bst->InOrder();
                break;

            case 3:
                ticks = clock();
                bid = bst->Search(bidKey);
                ticks = clock() - ticks;

                if (!bid.bidId.empty()) {
                    displayBid(bid);
                } else {
                    cout << "Bid Id " << bidKey << " not found." << endl;
                }

                cout << "time: " << ticks << " clock ticks" << endl;
                cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC
                     << " seconds" << endl;
                break;

            case 4:
                bst->Remove(bidKey);
                break;

            case 9:
                break;

            default:
                cout << "Please enter a valid menu choice." << endl;
                break;
        }
    }

    // Deleting the tree calls its destructor and frees every allocated node.
    delete bst;
    cout << "Good bye." << endl;

    return 0;
}
