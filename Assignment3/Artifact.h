#ifndef ARTIFACT_H
#define ARTIFACT_H

#include <string>

struct Artifact
{
    int artifactID;             
    std::string name;            
    int rarityLevel;             
    int researchValue;           
    std::string assignedToName;  

    int assignmentCount = 0;     

    Artifact();
    Artifact(int id, const std::string &n, int rarity, int value);

    void updateValueBasedOnUsage();
};

#endif