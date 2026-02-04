#ifndef ARTIFACT_MANAGER_H
#define ARTIFACT_MANAGER_H

#include <string>
#include "AVLTree.h"
#include "RedBlackTree.h"

class ArtifactManager
{
public:
    ArtifactManager();
    ~ArtifactManager();
    ArtifactManager(const ArtifactManager &) = delete;
    ArtifactManager &operator=(const ArtifactManager &) = delete;

    void parseAndExecute(const std::string &line);

private:
    AVLTree artifactTree;
    RedBlackTree researcherTree;
    int tokenize(const std::string &line, std::string tokens[], int maxTokens) const;

    void handleAddArtifact(const std::string tokens[], int count);
    void handleRemoveArtifact(const std::string tokens[], int count);
    void handleHireResearcher(const std::string tokens[], int count);
    void handleFireResearcher(const std::string tokens[], int count);
    void handleRequest(const std::string tokens[], int count);
    void handleReturn(const std::string tokens[], int count);
    void handleReturnAll(const std::string tokens[], int count);
    void handleResearcherLoad(const std::string tokens[], int count);
    void handleMatchRarity(const std::string tokens[], int count);
    void handlePrintUnassigned(const std::string tokens[], int count);
    void handlePrintStats(const std::string tokens[], int count);
    void handleClear(const std::string tokens[], int count);
};

#endif // ARTIFACT_MANAGER_H