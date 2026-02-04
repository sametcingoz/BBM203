#ifndef RESEARCHER_H
#define RESEARCHER_H

#include <string>

struct Researcher
{
    std::string fullName; 
    int capacity;          
    int *assignedArtifacts;
    int numAssigned;        

    Researcher();
    Researcher(const std::string &name, int cap);
    ~Researcher();
    
    Researcher(const Researcher &other);
    Researcher &operator=(const Researcher &other);

    bool addArtifact(int artifactID);
    bool removeArtifact(int artifactID);
    void removeAllArtifacts();
    bool hasArtifact(int artifactID) const;
};

#endif // RESEARCHER_H