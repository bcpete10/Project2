#ifndef BINARYSEARCHTREE_H_
#define BINARYSEARCHTREE_H_

#include <string>
#include "defns.h"


class BinarySearchTree {
public:
    explicit BinarySearchTree(annual_storms **dataArrays,
            const std::string &fieldName);
    ~BinarySearchTree();

    void Insert(char *key, int eventId, int year, int eventIndex);
    void Print();

private:
    bst *root;
    bst *left;
    bst *right;

    bst *Insert(bst *parent, bst *newNode);
    void InOrder(bst *node, void (*func)(bst *));
};

#endif // BINARYSEARCHTREE_H_
