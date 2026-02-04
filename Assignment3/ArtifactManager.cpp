#include "ArtifactManager.h"
#include <iostream>
#include <sstream>
#include <cmath>

ArtifactManager::ArtifactManager() {}
ArtifactManager::~ArtifactManager() {}

int ArtifactManager::tokenize(const std::string &line, std::string tokens[], int maxTokens) const {
    std::istringstream iss(line); std::string t; int c = 0;
    while (iss >> t && c < maxTokens) tokens[c++] = t; return c;
}

void ArtifactManager::parseAndExecute(const std::string &line) {
    std::string t[10]; int c = tokenize(line, t, 10);
    if (c == 0) return; std::string cmd = t[0];
    if (cmd == "ADD_ARTIFACT") handleAddArtifact(t, c);
    else if (cmd == "REMOVE_ARTIFACT") handleRemoveArtifact(t, c);
    else if (cmd == "HIRE_RESEARCHER") handleHireResearcher(t, c);
    else if (cmd == "FIRE_RESEARCHER") handleFireResearcher(t, c);
    else if (cmd == "REQUEST") handleRequest(t, c);
    else if (cmd == "RETURN") handleReturn(t, c);
    else if (cmd == "RETURN_ALL") handleReturnAll(t, c);
    else if (cmd == "RESEARCHER_LOAD") handleResearcherLoad(t, c);
    else if (cmd == "MATCH_RARITY") handleMatchRarity(t, c);
    else if (cmd == "PRINT_UNASSIGNED") handlePrintUnassigned(t, c);
    else if (cmd == "PRINT_STATS") handlePrintStats(t, c);
    else if (cmd == "CLEAR") handleClear(t, c);
    else std::cout << "Error: Unknown command '" << cmd << "'." << std::endl;
}

void ArtifactManager::handleAddArtifact(const std::string t[], int c) {
    if (artifactTree.insertArtifact(Artifact(std::stoi(t[1]), t[2], std::stoi(t[3]), std::stoi(t[4]))))
        std::cout << "Artifact " << t[1] << " added." << std::endl;
}

void ArtifactManager::handleRemoveArtifact(const std::string t[], int c) {
    int id = std::stoi(t[1]); ArtifactNode* n = artifactTree.findArtifact(id);
    if (!n) { std::cout << "Error: Artifact " << id << " not found." << std::endl; return; }
    if (n->data.assignedToName != "") {
        ResearcherNode* r = researcherTree.findResearcher(n->data.assignedToName);
        if (r) r->data.removeArtifact(id);
    }
    artifactTree.removeArtifact(id); std::cout << "Artifact " << id << " removed." << std::endl;
}

void ArtifactManager::handleHireResearcher(const std::string t[], int c) {
    if (researcherTree.insertResearcher(Researcher(t[1], std::stoi(t[2]))))
        std::cout << "Researcher " << t[1] << " hired." << std::endl;
}

void ArtifactManager::handleFireResearcher(const std::string t[], int c) {
    ResearcherNode* r = researcherTree.findResearcher(t[1]);
    if (!r) { std::cout << "Error: Researcher " << t[1] << " not found." << std::endl; return; }
    for (int i = 0; i < r->data.numAssigned; i++) artifactTree.clearAssignedTo(r->data.assignedArtifacts[i]);
    researcherTree.removeResearcher(t[1]); std::cout << "Researcher " << t[1] << " fired." << std::endl;
}

void ArtifactManager::handleRequest(const std::string t[], int c) {
    ResearcherNode* r = researcherTree.findResearcher(t[1]);
    ArtifactNode* a = artifactTree.findArtifact(std::stoi(t[2]));
    if (!r) { std::cout << "Error: Researcher " << t[1] << " not found." << std::endl; return; }
    if (!a) { std::cout << "Error: Artifact " << t[2] << " not found." << std::endl; return; }
    if (a->data.assignedToName != "") { std::cout << "Error: Artifact " << t[2] << " is already assigned." << std::endl; return; }
    if (r->data.numAssigned >= r->data.capacity) { std::cout << "Error: Researcher " << t[1] << " is at full capacity." << std::endl; return; }
    r->data.addArtifact(a->data.artifactID); a->data.assignedToName = t[1]; a->data.updateValueBasedOnUsage();
    std::cout << "Artifact " << t[2] << " assigned to " << t[1] << "." << std::endl;
}

void ArtifactManager::handleReturn(const std::string t[], int c) {
    ResearcherNode* r = researcherTree.findResearcher(t[1]);
    ArtifactNode* a = artifactTree.findArtifact(std::stoi(t[2]));
    if (!r) { std::cout << "Error: Researcher " << t[1] << " not found." << std::endl; return; }
    if (!a) { std::cout << "Error: Artifact " << t[2] << " not found." << std::endl; return; }
    if (a->data.assignedToName != t[1]) { std::cout << "Error: Artifact " << t[2] << " is not assigned to " << t[1] << "." << std::endl; return; }
    r->data.removeArtifact(a->data.artifactID); a->data.assignedToName = "";
    std::cout << "Artifact " << t[2] << " returned by " << t[1] << "." << std::endl;
}

void ArtifactManager::handleReturnAll(const std::string t[], int c) {
    ResearcherNode* r = researcherTree.findResearcher(t[1]);
    if (!r) { std::cout << "Error: Researcher " << t[1] << " not found." << std::endl; return; }
    for (int i = 0; i < r->data.numAssigned; i++) artifactTree.clearAssignedTo(r->data.assignedArtifacts[i]);
    r->data.removeAllArtifacts(); std::cout << "All artifacts returned by " << t[1] << "." << std::endl;
}

void ArtifactManager::handleResearcherLoad(const std::string t[], int c) {
    ResearcherNode* r = researcherTree.findResearcher(t[1]);
    if (!r) { std::cout << "Error: Researcher " << t[1] << " not found." << std::endl; return; }
    std::cout << t[1] << " load: " << r->data.numAssigned << std::endl;
}

// Inorder traversal for MATCH_RARITY
void matchR(ArtifactNode* n, int m) {
    if (!n) return; matchR(n->left, m);
    if (n->data.rarityLevel >= m) {
        std::cout << n->data.artifactID << " " << n->data.name << " " << n->data.rarityLevel << " " << n->data.researchValue;
        if (n->data.assignedToName != "") std::cout << " ASSIGNED:" << n->data.assignedToName;
        std::cout << std::endl;
    }
    matchR(n->right, m);
}

// Global/Helper access because root is private. Alternatively use a wrapper in AVLTree.
void ArtifactManager::handleMatchRarity(const std::string t[], int c) {
    int m = std::stoi(t[1]); std::cout << "=== MATCH_RARITY " << m << " ===" << std::endl;
 
    struct H { static void run(ArtifactNode* n, int m) {
        if(!n) return; run(n->left, m);
        if(n->data.rarityLevel >= m) {
            std::cout << n->data.artifactID << " " << n->data.name << " " << n->data.rarityLevel << " " << n->data.researchValue;
            if(n->data.assignedToName != "") std::cout << " ASSIGNED:" << n->data.assignedToName;
            std::cout << std::endl;
        }
        run(n->right, m);
    }};

}

void ArtifactManager::handlePrintUnassigned(const std::string t[], int c) {
    std::cout << "Unassigned artifacts:" << std::endl; artifactTree.printUnassigned();
}

void ArtifactManager::handlePrintStats(const std::string t[], int c) {
    int ac = artifactTree.getArtifactCount(), rc = researcherTree.getResearcherCount();
    std::cout << "=== SYSTEM STATISTICS ===" << std::endl;
    std::cout << "Artifacts: " << ac << "\nResearchers: " << rc << std::endl;
    if (ac > 0) std::cout << "Average rarity: " << (int)floor((double)artifactTree.getTotalRarity()/ac) << std::endl;
    if (rc > 0) std::cout << "Average load: " << (int)floor((double)researcherTree.getTotalLoad()/rc) << std::endl;
    std::cout << "Researchers:" << std::endl; researcherTree.traversePreOrderForStats();
    std::cout << "Artifacts:" << std::endl; artifactTree.traversePostOrderForStats();
}

void ArtifactManager::handleClear(const std::string t[], int c) { artifactTree.clear(); researcherTree.clear(); }