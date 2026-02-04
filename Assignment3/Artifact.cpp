#include "Artifact.h"

Artifact::Artifact()
: artifactID(-1), name(""), rarityLevel(1), researchValue(0), assignedToName(""), assignmentCount(0) {}

Artifact::Artifact(int id, const std::string &n, int rarity, int value)
: artifactID(id), name(n), rarityLevel(rarity), researchValue(value), assignedToName(""), assignmentCount(0) {}

void Artifact::updateValueBasedOnUsage() {
    assignmentCount++;
    researchValue += (rarityLevel * assignmentCount);
}