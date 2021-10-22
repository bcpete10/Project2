#include <cstring>
#include "BinarySearchTree.h"
#include "Utility.h"
#include "HashTable.h"

// Global pointer to an array for storms
static annual_storms **GlobalDataArrays = nullptr;

static const char *FieldName = nullptr;

//Function to print the nodes of a tree
void PrintBstNode(bst *node) {
    storm_event *events = nullptr;
    storm_event *event = nullptr;
        for (int i = 0; GlobalDataArrays[i] != nullptr; i++) {
        if (GlobalDataArrays[i]->year == node->year) {
            events = GlobalDataArrays[i]->events;
            break;
        }
    }

    if (events != nullptr) {
        event = &(events[node->event_index]);
        std::cout << "\n\t" << FieldName << ": " << node->s << "\n"
                << "\tevent_id: " << event->event_id << "\n"
                << "\tyear: " << event->year << "\n"
                << "\tevent_type: " << event->event_type << "\n"
                << "\tcz_type: " << event->cz_type << "\n"
                << "\tcz_name: " << event->cz_name << "\n";
    }
}

//Deletes a node in a tree
void DeleteBstNode(bst *node) {
    delete node;
}


//Constructor that requires the Constants to be set to build a BST
//REMOVE IF NOT NEEDED, UNSURE
BinarySearchTree::BinarySearchTree(annual_storms **dataArrays, const std::string &fieldName) :
        root(nullptr), left(nullptr), right(nullptr) {
    GlobalDataArrays = dataArrays;
    FieldName = fieldName.c_str();
}

//Function to free allocated memory.
BinarySearchTree::~BinarySearchTree() {
    if (root != nullptr) {
        InOrder(root, DeleteBstNode);
    }

    // Reset global variables.
    GlobalDataArrays = nullptr;
    FieldName = nullptr;
}

//Creates a node and inserts it into the tree
void BinarySearchTree::Insert(char *key, int eventId, int year,
        int eventIndex) {
    bst *newNode = new bst { key, eventId, year, eventIndex, nullptr, nullptr };
    if (root == nullptr) {
        root = newNode;
        return;
    }

    if (Utility::CompareString(root->s, key) > 0) {
        root->left = Insert(root->left, newNode);
    } else if (Utility::CompareString(root->s, key) < 0) {
        root->right = Insert(root->right, newNode);
    } else {
        if (root->event_id > eventId) {
            root->left = Insert(root->left, newNode);
        } else {
            root->right = Insert(root->right, newNode);
        }
    }
}

//Inserts a node under the parent node of the BST
bst* BinarySearchTree::Insert(bst *parent, bst *newNode) {
    if (parent == nullptr) {
        parent = newNode;
    } else {
        if (Utility::CompareString(parent->s, newNode->s) > 0) {
            parent->left = Insert(parent->left, newNode);
        } else if (Utility::CompareString(parent->s, newNode->s) < 0) {
            parent->right = Insert(parent->right, newNode);
        } else {
            if (parent->event_id > newNode->event_id) {
                parent->left = Insert(parent->left, newNode);
            } else {
                parent->right = Insert(parent->right, newNode);
            }
        }
    }

    return parent;
}

//Display the whole BST in order
void BinarySearchTree::Print() {
    if (root == nullptr) {
        return;
    }

    InOrder(root, PrintBstNode);
}

//Helper function, traverses in order so that it can be displayed.
void BinarySearchTree::InOrder(bst *node, void (*func)(bst *)) {
    bst *left = node->left;
    bst *rigt = node->right;

    if (left != nullptr) {
        InOrder(left, func);
    }

    func(node);

    if (rigt != nullptr) {
        InOrder(rigt, func);
    }
}
