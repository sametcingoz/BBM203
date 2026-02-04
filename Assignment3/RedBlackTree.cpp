#include "RedBlackTree.h"

RedBlackTree::RedBlackTree() : root(nullptr) {}
RedBlackTree::~RedBlackTree() { clear(); }
void RedBlackTree::clear() { clear(root); root = nullptr; }
void RedBlackTree::clear(ResearcherNode *n) { if (n) { clear(n->left); clear(n->right); delete n; } }

void RedBlackTree::rotateLeft(ResearcherNode *x) {
    ResearcherNode *y = x->right; x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent) root = y; else if (x == x->parent->left) x->parent->left = y; else x->parent->right = y;
    y->left = x; x->parent = y;
}

void RedBlackTree::rotateRight(ResearcherNode *y) {
    ResearcherNode *x = y->left; y->left = x->right;
    if (x->right) x->right->parent = y;
    x->parent = y->parent;
    if (!y->parent) root = x; else if (y == y->parent->left) y->parent->left = x; else y->parent->right = x;
    x->right = y; y->parent = x;
}

void RedBlackTree::insertFixup(ResearcherNode *k) {
    while (k != root && k->parent->color == RED) {
        if (k->parent == k->parent->parent->left) {
            ResearcherNode *u = k->parent->parent->right;
            if (u && u->color == RED) { u->color = BLACK; k->parent->color = BLACK; k->parent->parent->color = RED; k = k->parent->parent; }
            else {
                if (k == k->parent->right) { k = k->parent; rotateLeft(k); }
                k->parent->color = BLACK; k->parent->parent->color = RED; rotateRight(k->parent->parent);
            }
        } else {
            ResearcherNode *u = k->parent->parent->left;
            if (u && u->color == RED) { u->color = BLACK; k->parent->color = BLACK; k->parent->parent->color = RED; k = k->parent->parent; }
            else {
                if (k == k->parent->left) { k = k->parent; rotateRight(k); }
                k->parent->color = BLACK; k->parent->parent->color = RED; rotateLeft(k->parent->parent);
            }
        }
    }
    root->color = BLACK;
}

ResearcherNode* RedBlackTree::bstInsert(ResearcherNode* cur, ResearcherNode* n, bool& ins) {
    if (!cur) { ins = true; return n; }
    if (n->data.fullName < cur->data.fullName) { cur->left = bstInsert(cur->left, n, ins); cur->left->parent = cur; }
    else if (n->data.fullName > cur->data.fullName) { cur->right = bstInsert(cur->right, n, ins); cur->right->parent = cur; }
    else ins = false;
    return cur;
}

bool RedBlackTree::insertResearcher(const Researcher &r) {
    bool i = false; ResearcherNode *n = new ResearcherNode(r);
    root = bstInsert(root, n, i);
    if (i) insertFixup(n); else delete n;
    return i;
}

void RedBlackTree::deleteFixup(ResearcherNode *x, ResearcherNode *p) {
    while (x != root && (!x || x->color == BLACK)) {
        if (x == p->left) {
            ResearcherNode *s = p->right;
            if (s && s->color == RED) { s->color = BLACK; p->color = RED; rotateLeft(p); s = p->right; }
            if ((!s || !s->left || s->left->color == BLACK) && (!s || !s->right || s->right->color == BLACK)) { if(s) s->color = RED; x = p; p = x->parent; }
            else {
                if (!s || !s->right || s->right->color == BLACK) { if(s && s->left) s->left->color = BLACK; if(s) s->color = RED; if(s) rotateRight(s); s = p->right; }
                if(s) { s->color = p->color; if(s->right) s->right->color = BLACK; }
                p->color = BLACK; rotateLeft(p); x = root;
            }
        } else {
            ResearcherNode *s = p->left;
            if (s && s->color == RED) { s->color = BLACK; p->color = RED; rotateRight(p); s = p->left; }
            if ((!s || !s->right || s->right->color == BLACK) && (!s || !s->left || s->left->color == BLACK)) { if(s) s->color = RED; x = p; p = x->parent; }
            else {
                if (!s || !s->left || s->left->color == BLACK) { if(s && s->right) s->right->color = BLACK; if(s) s->color = RED; if(s) rotateLeft(s); s = p->left; }
                if(s) { s->color = p->color; if(s->left) s->left->color = BLACK; }
                p->color = BLACK; rotateRight(p); x = root;
            }
        }
    }
    if (x) x->color = BLACK;
}

bool RedBlackTree::removeResearcher(const std::string &name) {
    ResearcherNode *z = findResearcher(name); if (!z) return false;
    ResearcherNode *x, *y = z, *p; Color y_c = y->color;
    if (!z->left) { x = z->right; p = z->parent; if (!z->parent) root = x; else if (z == z->parent->left) z->parent->left = x; else z->parent->right = x; if (x) x->parent = z->parent; }
    else if (!z->right) { x = z->left; p = z->parent; if (!z->parent) root = x; else if (z == z->parent->left) z->parent->left = x; else z->parent->right = x; if (x) x->parent = z->parent; }
    else { y = z->right; while (y->left) y = y->left; y_c = y->color; x = y->right; p = y->parent; 
        if (y->parent == z) { if(x) x->parent = y; p = y; } else { if(x) x->parent = y->parent; y->parent->left = x; y->right = z->right; y->right->parent = y; }
        if (!z->parent) root = y; else if (z == z->parent->left) z->parent->left = y; else z->parent->right = y;
        y->parent = z->parent; y->left = z->left; y->left->parent = y; y->color = z->color;
    }
    if (y_c == BLACK) deleteFixup(x, p); delete z; return true;
}

ResearcherNode* RedBlackTree::findResearcher(const std::string &n) const {
    ResearcherNode *c = root; while(c) { if(n == c->data.fullName) return c; c = (n < c->data.fullName) ? c->left : c->right; }
    return nullptr;
}

void RedBlackTree::traversePreOrderForStats(ResearcherNode *n) const {
    if (!n) return; std::cout << n->data.fullName << " " << n->data.capacity << " " << n->data.numAssigned << std::endl;
    traversePreOrderForStats(n->left); traversePreOrderForStats(n->right);
}

int RedBlackTree::getResearcherCount(ResearcherNode *n) const { return n ? 1 + getResearcherCount(n->left) + getResearcherCount(n->right) : 0; }
int RedBlackTree::getResearcherCount() const { return getResearcherCount(root); }
int RedBlackTree::getTotalLoad(ResearcherNode *n) const { return n ? n->data.numAssigned + getTotalLoad(n->left) + getTotalLoad(n->right) : 0; }
int RedBlackTree::getTotalLoad() const { return getTotalLoad(root); }
void RedBlackTree::traversePreOrderForStats() const { traversePreOrderForStats(root); }