//============================================================================
// Name        : HashTable.cpp
// Author      : Your Name
// Version     : 1.0
// Copyright   : Copyright (c) 2023 SNHU COCE
// Description : Lab 4-2 Hash Table
//============================================================================

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// Forward declaration
double strToDouble(string str, char ch);

// Structure used to hold bid information.
struct Bid {
    string bidId;
    string title;
    string fund;
    double amount;

    Bid() {
        amount = 0.0;
    }
};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Hash table that stores bids. Each vector position is a bucket. When two
 * bid IDs produce the same bucket key, linked nodes are used for chaining.
 */
class HashTable {

private:
    // A node stores one bid and a pointer to the next collision, if one exists.
    struct Node {
        Bid bid;
        unsigned int key;
        Node* next;

        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        Node(Bid aBid) : Node() {
            bid = aBid;
        }

        Node(Bid aBid, unsigned int aKey) : Node(aBid) {
            key = aKey;
        }
    };

    unsigned int tableSize;
    vector<Node> nodes;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Bid bid);
    void PrintAll();
    void Remove(string bidId);
    Bid Search(string bidId);
    size_t Size();
};

/**
 * Default constructor. Creates the requested number of empty buckets.
 */
HashTable::HashTable() {
    tableSize = DEFAULT_SIZE;
    nodes.resize(tableSize);
}

/**
 * Constructor that allows a custom table size.
 * A zero size is replaced with the default to prevent division by zero.
 */
HashTable::HashTable(unsigned int size) {
    tableSize = (size == 0) ? DEFAULT_SIZE : size;
    nodes.resize(tableSize);
}

/**
 * Destructor. The vector automatically releases its bucket nodes, but nodes
 * created dynamically for collision chains must be deleted manually.
 */
HashTable::~HashTable() {
    for (Node& bucket : nodes) {
        Node* current = bucket.next;

        while (current != nullptr) {
            Node* nodeToDelete = current;
            current = current->next;
            delete nodeToDelete;
        }

        bucket.next = nullptr;
    }

    nodes.clear();
}

/**
 * Calculate a bucket index from a numeric bid ID.
 *
 * @param key Numeric form of the bid ID
 * @return Valid index in the nodes vector
 */
unsigned int HashTable::hash(int key) {
    return static_cast<unsigned int>(key) % tableSize;
}

/**
 * Insert a bid into the table. If its bucket is occupied, traverse the linked
 * list and append a new node. An existing bid ID is updated instead of stored
 * twice.
 *
 * @param bid The bid to insert
 */
void HashTable::Insert(Bid bid) {
    unsigned int key = hash(atoi(bid.bidId.c_str()));
    Node* bucket = &nodes.at(key);

    // An unused bucket becomes the first node for this key.
    if (bucket->key == UINT_MAX) {
        bucket->key = key;
        bucket->bid = bid;
        bucket->next = nullptr;
        return;
    }

    Node* current = bucket;

    while (current != nullptr) {
        // Prevent duplicate IDs if the same file is loaded more than once.
        if (current->bid.bidId == bid.bidId) {
            current->bid = bid;
            return;
        }

        if (current->next == nullptr) {
            break;
        }

        current = current->next;
    }

    // The bucket was occupied, so add the collision to the end of its chain.
    current->next = new Node(bid, key);
}

/**
 * Print every bid in every bucket, including chained collision nodes.
 */
void HashTable::PrintAll() {
    ios::fmtflags originalFlags = cout.flags();
    streamsize originalPrecision = cout.precision();

    for (const Node& bucket : nodes) {
        if (bucket.key == UINT_MAX) {
            continue;
        }

        const Node* current = &bucket;

        while (current != nullptr) {
            cout << "Key " << current->key << ": "
                 << current->bid.bidId << ": "
                 << current->bid.title << " | $"
                 << fixed << setprecision(2) << current->bid.amount << " | "
                 << current->bid.fund << endl;

            current = current->next;
        }
    }

    cout.flags(originalFlags);
    cout.precision(originalPrecision);
}

/**
 * Remove a bid by ID. Removing the first item in a bucket promotes the next
 * collision node into the bucket so the remaining chain stays reachable.
 *
 * @param bidId The bid ID to remove
 */
void HashTable::Remove(string bidId) {
    unsigned int key = hash(atoi(bidId.c_str()));
    Node* bucket = &nodes.at(key);

    if (bucket->key == UINT_MAX) {
        return;
    }

    // Handle removal from the vector-owned head node.
    if (bucket->bid.bidId == bidId) {
        if (bucket->next != nullptr) {
            Node* promotedNode = bucket->next;
            bucket->bid = promotedNode->bid;
            bucket->key = promotedNode->key;
            bucket->next = promotedNode->next;
            delete promotedNode;
        } else {
            bucket->bid = Bid();
            bucket->key = UINT_MAX;
            bucket->next = nullptr;
        }
        return;
    }

    // Search the rest of the collision chain.
    Node* previous = bucket;
    Node* current = bucket->next;

    while (current != nullptr) {
        if (current->bid.bidId == bidId) {
            previous->next = current->next;
            delete current;
            return;
        }

        previous = current;
        current = current->next;
    }
}

/**
 * Search the correct bucket and its collision chain for a bid ID.
 *
 * @param bidId The bid ID to locate
 * @return Matching bid or an empty Bid when not found
 */
Bid HashTable::Search(string bidId) {
    Bid emptyBid;
    unsigned int key = hash(atoi(bidId.c_str()));
    const Node* current = &nodes.at(key);

    if (current->key == UINT_MAX) {
        return emptyBid;
    }

    while (current != nullptr) {
        if (current->bid.bidId == bidId) {
            return current->bid;
        }

        current = current->next;
    }

    return emptyBid;
}

/**
 * Count all stored bids, including bids in collision chains.
 */
size_t HashTable::Size() {
    size_t bidCount = 0;

    for (const Node& bucket : nodes) {
        if (bucket.key == UINT_MAX) {
            continue;
        }

        const Node* current = &bucket;
        while (current != nullptr) {
            ++bidCount;
            current = current->next;
        }
    }

    return bidCount;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display bid information to the console.
 */
void displayBid(Bid bid) {
    ios::fmtflags originalFlags = cout.flags();
    streamsize originalPrecision = cout.precision();

    cout << bid.bidId << ": " << bid.title << " | $"
         << fixed << setprecision(2) << bid.amount << " | "
         << bid.fund << endl;

    cout.flags(originalFlags);
    cout.precision(originalPrecision);
}

/**
 * Load bids from a CSV file into the hash table.
 */
void loadBids(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    csv::Parser file = csv::Parser(csvPath);

    // Display the file header so the user can verify the loaded data format.
    vector<string> header = file.getHeader();
    for (const string& column : header) {
        cout << column << " | ";
    }
    cout << endl;

    try {
        for (unsigned int i = 0; i < file.rowCount(); ++i) {
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            hashTable->Insert(bid);
        }
    } catch (csv::Error& error) {
        cerr << error.what() << endl;
    }
}

/**
 * Convert a currency string to a double after removing characters that are
 * not part of the numeric value.
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    str.erase(remove(str.begin(), str.end(), ','), str.end());
    str.erase(remove(str.begin(), str.end(), '"'), str.end());
    return atof(str.c_str());
}

/**
 * Main menu used to load, display, search, and remove bids.
 */
int main(int argc, char* argv[]) {
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
    HashTable* bidTable = new HashTable();
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
            loadBids(csvPath, bidTable);
            ticks = clock() - ticks;

            cout << bidTable->Size() << " bids are stored in the hash table." << endl;
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bidTable->PrintAll();
            break;

        case 3:
            ticks = clock();
            bid = bidTable->Search(bidKey);
            ticks = clock() - ticks;

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
                cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 4:
            bidTable->Remove(bidKey);
            break;

        case 9:
            break;

        default:
            cout << "Please enter a valid menu option." << endl;
            break;
        }
    }

    // Deleting the table calls its destructor and releases collision nodes.
    delete bidTable;
    bidTable = nullptr;

    cout << "Good bye." << endl;
    return 0;
}
