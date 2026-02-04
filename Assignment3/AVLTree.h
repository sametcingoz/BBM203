#ifndef AVLTREE_H
#define AVLTREE_H

#include "Artifact.h"
#include <iostream>

struct ArtifactNode
{
    Artifact data;
    ArtifactNode *left;
    ArtifactNode *right;
    int height;

    ArtifactNode(const Artifact &a)
        : data(a), left(nullptr), right(nullptr), height(0)
    {
    }
};

class AVLTree
{
public:
    AVLTree();
    ~AVLTree();

    AVLTree(const AVLTree &) = delete;
    AVLTree &operator=(const AVLTree &) = delete;

    bool insertArtifact(const Artifact &artifact);
    bool removeArtifact(int artifactID);
    ArtifactNode *findArtifact(int artifactID) const;
    void printUnassigned() const;
    void setAssignedTo(int artifactID, const std::string &researcherName);
    void clearAssignedTo(int artifactID);
    int getArtifactCount() const;
    int getTotalRarity() const;
    void traversePostOrderForStats() const;
    void clear();

private:
    ArtifactNode *root;
    void clear(ArtifactNode *node);
    ArtifactNode *insert(ArtifactNode *node, const Artifact &artifact, bool &inserted);
    ArtifactNode *remove(ArtifactNode *node, int artifactID, bool &removed);
    ArtifactNode *find(ArtifactNode *node, int artifactID) const;
    int height(ArtifactNode *node) const;
    int getBalance(ArtifactNode *node) const;
    ArtifactNode *rotateLeft(ArtifactNode *x);
    ArtifactNode *rotateRight(ArtifactNode *y);
    ArtifactNode *findMinNode(ArtifactNode *node) const;
    void printUnassigned(ArtifactNode *node) const;
    int getArtifactCount(ArtifactNode *node) const;
    int getTotalRarity(ArtifactNode *node) const;
    void traversePostOrderForStats(ArtifactNode *node) const;
};

#endif // AVLTREE_H