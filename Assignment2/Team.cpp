#include "Team.h"

Team::Team()
    : teamID(-1),
      maxLoadCapacity(0),
      currentWorkload(0),
      missionStack(4) {
}

Team::Team(int id, int maxLoad)
    : teamID(id),
      maxLoadCapacity(maxLoad),
      currentWorkload(0),
      missionStack(4) {
}

int Team::getId() const {
    return teamID;
}

int Team::getMaxLoadCapacity() const {
    return maxLoadCapacity;
}

int Team::getCurrentWorkload() const {
    return currentWorkload;
}

void Team::setId(int id) {
    teamID = id;
}

void Team::setMaxLoadCapacity(int maxLoad) {
    maxLoadCapacity = maxLoad;
}

bool Team::hasActiveMission() const {
    return !missionStack.isEmpty();
}

bool Team::tryAssignRequest(const Request& req) {
    // Implement tryAssignRequest function as explained in the PDF.
    
    // 1. Calculate the workload contribution of the new request.
    int contribution = req.computeWorkloadContribution();

    // 2. Check if adding this request exceeds the team's capacity.
    if (currentWorkload + contribution > maxLoadCapacity) {
        return false; // Overload detected
    }

    // 3. Try pushing the request to the mission stack.
    if (missionStack.push(req)) {
        // Update current workload only if push is successful.
        currentWorkload += contribution;
        return true;
    }

    return false; // Push failed (e.g., memory allocation error)
}

void Team::rollbackMission(RequestQueue& supplyQueue, RequestQueue& rescueQueue) {
    // Implement rollbackMission function as explained in the PDF.
    
    // We need to move requests from the stack back to their original queues.
    // Crucially, we must preserve the relative order.
    // Since MissionStack is LIFO, popping elements one by one would reverse the order.
    // Instead, we access the underlying array directly and iterate from bottom (oldest) to top (newest).
    
    Request* stackData = missionStack.getData();
    int topIndex = missionStack.getTopIndex();

    for (int i = 0; i <= topIndex; ++i) {
        Request& req = stackData[i];
        
        // Return to the appropriate queue based on type.
        if (req.getType() == "SUPPLY") {
            supplyQueue.enqueue(req);
        } else {
            // Assume "RESCUE"
            rescueQueue.enqueue(req);
        }
    }

    // After returning all requests, clear the stack and reset workload.
    missionStack.clear();
    currentWorkload = 0;
}

void Team::clearMission() {
    missionStack.clear();
    currentWorkload = 0;
}

const MissionStack& Team::getMissionStack() const {
    return missionStack;
}