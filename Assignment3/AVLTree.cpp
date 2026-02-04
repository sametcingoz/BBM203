#include "AVLTree.h"
#include <algorithm>

AVLTree::AVLTree() : root(nullptr) {}
AVLTree::~AVLTree() { clear(); }

void AVLTree::clear() { clear(root); root = nullptr; }
void AVLTree::clear(ArtifactNode *n) { if (n) { clear(n->left); clear(n->right); delete n; } }

int AVLTree::height(ArtifactNode *n) const { return n ? n->height : -1; }
int AVLTree::getBalance(ArtifactNode *n) const { return n ? height(n->left) - height(n->right) : 0; }

ArtifactNode *AVLTree::rotateLeft(ArtifactNode *x) {
    ArtifactNode *y = x->right; x->right = y->left; y->left = x;
    x->height = 1 + std::max(height(x->left), height(x->right));
    y->height = 1 + std::max(height(y->left), height(y->right));
    return y;
}

ArtifactNode *AVLTree::rotateRight(ArtifactNode *y) {
    ArtifactNode *x = y->left; y->left = x->right; x->right = y;
    y->height = 1 + std::max(height(y->left), height(y->right));
    x->height = 1 + std::max(height(x->left), height(x->right));
    return x;
}

ArtifactNode *AVLTree::insert(ArtifactNode *n, const Artifact &a, bool &ins) {
    if (!n) { ins = true; return new ArtifactNode(a); }
    if (a.artifactID < n->data.artifactID) n->left = insert(n->left, a, ins);
    else if (a.artifactID > n->data.artifactID) n->right = insert(n->right, a, ins);
    else { ins = false; return n; }
    n->height = 1 + std::max(height(n->left), height(n->right));
    int b = getBalance(n);
    if (b > 1 && a.artifactID < n->left->data.artifactID) return rotateRight(n);
    if (b < -1 && a.artifactID > n->right->data.artifactID) return rotateLeft(n);
    if (b > 1 && a.artifactID > n->left->data.artifactID) { n->left = rotateLeft(n->left); return rotateRight(n); }
    if (b < -1 && a.artifactID < n->right->data.artifactID) { n->right = rotateRight(n->right); return rotateLeft(n); }
    return n;
}

ArtifactNode *AVLTree::remove(ArtifactNode *n, int id, bool &rem) {
    if (!n) return nullptr;
    if (id < n->data.artifactID) n->left = remove(n->left, id, rem);
    else if (id > n->data.artifactID) n->right = remove(n->right, id, rem);
    else {
        rem = true;
        if (!n->left || !n->right) { ArtifactNode *t = n->left ? n->left : n->right; delete n; return t; }
        ArtifactNode *t = n->right; while (t->left) t = t->left;
        n->data = t->data; n->right = remove(n->right, t->data.artifactID, rem);
    }
    n->height = 1 + std::max(height(n->left), height(n->right));
    int b = getBalance(n);
    if (b > 1 && getBalance(n->left) >= 0) return rotateRight(n);
    if (b > 1 && getBalance(n->left) < 0) { n->left = rotateLeft(n->left); return rotateRight(n); }
    if (b < -1 && getBalance(n->right) <= 0) return rotateLeft(n);
    if (b < -1 && getBalance(n->right) > 0) { n->right = rotateRight(n->right); return rotateLeft(n); }
    return n;
}

bool AVLTree::insertArtifact(const Artifact &a) { bool i = false; root = insert(root, a, i); return i; }
bool AVLTree::removeArtifact(int id) { bool r = false; root = remove(root, id, r); return r; }
ArtifactNode *AVLTree::findArtifact(int id) const {
    ArtifactNode *c = root;
    while(c) { if(id == c->data.artifactID) return c; c = (id < c->data.artifactID) ? c->left : c->right; }
    return nullptr;
}

void AVLTree::printUnassigned(ArtifactNode *n) const {
    if (!n) return; printUnassigned(n->left);
    if (n->data.assignedToName == "") std::cout << n->data.artifactID << " " << n->data.name << " " << n->data.rarityLevel << " " << n->data.researchValue << std::endl;
    printUnassigned(n->right);
}

void AVLTree::traversePostOrderForStats(ArtifactNode *n) const {
    if (!n) return; traversePostOrderForStats(n->left); traversePostOrderForStats(n->right);
    std::cout << n->data.artifactID << " " << n->data.name << " " << n->data.rarityLevel << " " << n->data.researchValue << " " << (n->data.assignedToName == "" ? "UNASSIGNED" : n->data.assignedToName) << std::endl;
}

void AVLTree::printUnassigned() const { printUnassigned(root); }
int AVLTree::getArtifactCount(ArtifactNode *n) const { return n ? 1 + getArtifactCount(n->left) + getArtifactCount(n->right) : 0; }
int AVLTree::getArtifactCount() const { return getArtifactCount(root); }
int AVLTree::getTotalRarity(ArtifactNode *n) const { return n ? n->data.rarityLevel + getTotalRarity(n->left) + getTotalRarity(n->right) : 0; }
int AVLTree::getTotalRarity() const { return getTotalRarity(root); }
void AVLTree::traversePostOrderForStats() const { traversePostOrderForStats(root); }
void AVLTree::setAssignedTo(int id, const std::string &name) { ArtifactNode *n = findArtifact(id); if (n) n->data.assignedToName = name; }
void AVLTree::clearAssignedTo(int id) { ArtifactNode *n = findArtifact(id); if (n) n->data.assignedToName = ""; }