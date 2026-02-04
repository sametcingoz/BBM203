#include "Researcher.h"

Researcher::Researcher() 
: fullName(""), capacity(0), assignedArtifacts(nullptr), numAssigned(0) {}

Researcher::Researcher(const std::string &name, int cap) 
: fullName(name), capacity(cap), assignedArtifacts(nullptr), numAssigned(0) {
    if (capacity > 0) assignedArtifacts = new int[capacity];
}

Researcher::~Researcher() {
    delete[] assignedArtifacts;
}

Researcher::Researcher(const Researcher &other) {
    fullName = other.fullName;
    capacity = other.capacity;
    numAssigned = other.numAssigned;
    if (capacity > 0) {
        assignedArtifacts = new int[capacity];
        for (int i = 0; i < numAssigned; i++) assignedArtifacts[i] = other.assignedArtifacts[i];
    } else assignedArtifacts = nullptr;
}

Researcher &Researcher::operator=(const Researcher &other) {
    if (this != &other) {
        delete[] assignedArtifacts;
        fullName = other.fullName;
        capacity = other.capacity;
        numAssigned = other.numAssigned;
        if (capacity > 0) {
            assignedArtifacts = new int[capacity];
            for (int i = 0; i < numAssigned; i++) assignedArtifacts[i] = other.assignedArtifacts[i];
        } else assignedArtifacts = nullptr;
    }
    return *this;
}

bool Researcher::addArtifact(int id) {
    if (numAssigned >= capacity) return false;
    assignedArtifacts[numAssigned++] = id;
    return true;
}

bool Researcher::removeArtifact(int id) {
    int idx = -1;
    for (int i = 0; i < numAssigned; i++) if (assignedArtifacts[i] == id) { idx = i; break; }
    if (idx == -1) return false;
    for (int i = idx; i < numAssigned - 1; i++) assignedArtifacts[i] = assignedArtifacts[i+1];
    numAssigned--;
    return true;
}

void Researcher::removeAllArtifacts() { numAssigned = 0; }

bool Researcher::hasArtifact(int id) const {
    for (int i = 0; i < numAssigned; i++) if (assignedArtifacts[i] == id) return true;
    return false;
}