#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H

#include "Researcher.h"
#include <iostream>

enum Color { RED, BLACK };

struct ResearcherNode
{
    Researcher data;
    Color color;
    ResearcherNode *left, *right, *parent;

    ResearcherNode(const Researcher &r)
        : data(r), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RedBlackTree
{
public:
    RedBlackTree();
    ~RedBlackTree();

    RedBlackTree(const RedBlackTree &) = delete;
    RedBlackTree &operator=(const RedBlackTree &) = delete;

    bool insertResearcher(const Researcher &researcher);
    bool removeResearcher(const std::string &fullName);
    ResearcherNode *findResearcher(const std::string &fullName) const;
    void traversePreOrderForStats() const;
    int getResearcherCount() const;
    int getTotalLoad() const;
    void clear();

private:
    ResearcherNode *root;
    void clear(ResearcherNode *node);
    ResearcherNode *find(ResearcherNode *node, const std::string &fullName) const;
    ResearcherNode *bstInsert(ResearcherNode *current, ResearcherNode *node, bool &inserted);
    void insertFixup(ResearcherNode *node);
    void deleteFixup(ResearcherNode *node, ResearcherNode *parent);
    void rotateLeft(ResearcherNode *x);
    void rotateRight(ResearcherNode *y);
    ResearcherNode *minimum(ResearcherNode *node) const;
    int getResearcherCount(ResearcherNode *node) const;
    int getTotalLoad(ResearcherNode *node) const;
    void traversePreOrderForStats(ResearcherNode *node) const;
};

#endif // REDBLACKTREE_H