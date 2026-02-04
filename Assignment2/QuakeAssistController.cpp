#include "QuakeAssistController.h"
#include <iostream>
#include <sstream>

QuakeAssistController::QuakeAssistController()
    : teams(nullptr),
      teamCount(0),
      supplyQueue(4),
      rescueQueue(4) {
}

QuakeAssistController::~QuakeAssistController() {
    delete[] teams;
}

bool QuakeAssistController::parseAndExecute(const std::string& line) {
    if (line.empty()) return true;

    std::stringstream ss(line);
    std::string command;
    ss >> command;

    if (command == "INIT_TEAMS") {
        int num;
        ss >> num;
        handleInitTeams(num);
    } 
    else if (command == "SET_TEAM_CAPACITY") {
        int id, cap;
        ss >> id >> cap;
        handleSetTeamCapacity(id, cap);
    } 
    else if (command == "ADD_SUPPLY") {
        std::string id, city, type;
        int amount, level;
        ss >> id >> city >> type >> amount >> level;
        handleAddSupply(id, city, type, amount, level);
    } 
    else if (command == "ADD_RESCUE") {
        std::string id, city, risk;
        int numPeople, level;
        ss >> id >> city >> numPeople >> risk >> level;
        handleAddRescue(id, city, numPeople, risk, level);
    } 
    else if (command == "REMOVE_REQUEST") {
        std::string id;
        ss >> id;
        handleRemoveRequest(id);
    } 
    else if (command == "HANDLE_EMERGENCY") {
        int id, k;
        ss >> id >> k;
        handleHandleEmergency(id, k);
    } 
    else if (command == "DISPATCH_TEAM") {
        int id;
        ss >> id;
        handleDispatchTeam(id);
    } 
    else if (command == "PRINT_QUEUES") {
        printQueues();
    } 
    else if (command == "PRINT_TEAM") {
        int id;
        ss >> id;
        printTeam(id);
    } 
    else if (command == "CLEAR") {
        clear();
    } 
    else {
        std::cout << "Error: Unknown command '" << command << "'." << std::endl;
    }

    return true;
}

bool QuakeAssistController::initializeTeams(int numTeams) {
    if (teams != nullptr) {
        delete[] teams;
    }
    teamCount = numTeams;
    teams = new Team[teamCount];
    
    // Set IDs
    for (int i = 0; i < teamCount; ++i) {
        teams[i].setId(i);
    }
    return true;
}

int QuakeAssistController::findTeamIndexById(int teamId) const {
    for (int i = 0; i < teamCount; ++i) {
        if (teams[i].getId() == teamId) {
            return i;
        }
    }
    return -1;
}

bool QuakeAssistController::handleInitTeams(int numTeams) {
    initializeTeams(numTeams);
    std::cout << "Initialized " << numTeams << " teams." << std::endl;
    return true;
}

bool QuakeAssistController::handleSetTeamCapacity(int teamId, int capacity) {
    int idx = findTeamIndexById(teamId);
    if (idx != -1) {
        teams[idx].setMaxLoadCapacity(capacity);
        std::cout << "Team " << teamId << " capacity set to " << capacity << "." << std::endl;
        return true;
    }
    return false;
}

bool QuakeAssistController::handleAddSupply(const std::string& id,
                                            const std::string& cityStr,
                                            const std::string& supplyTypeStr,
                                            int amount,
                                            int emergencyLevel) {
    Request req(id, cityStr, supplyTypeStr, amount, emergencyLevel);
    supplyQueue.enqueue(req);
    std::cout << "Request " << id << " added to SUPPLY queue." << std::endl;
    return true;
}

bool QuakeAssistController::handleAddRescue(const std::string& id,
                                            const std::string& cityStr,
                                            int numPeople,
                                            const std::string& riskStr,
                                            int emergencyLevel) {
    Request req(id, cityStr, numPeople, riskStr, emergencyLevel);
    rescueQueue.enqueue(req);
    std::cout << "Request " << id << " added to RESCUE queue." << std::endl;
    return true;
}

bool QuakeAssistController::handleRemoveRequest(const std::string& id) {
    // Try supply queue first
    if (supplyQueue.removeById(id)) {
        std::cout << "Request " << id << " removed from queues." << std::endl;
        return true;
    }
    // Try rescue queue
    if (rescueQueue.removeById(id)) {
        std::cout << "Request " << id << " removed from queues." << std::endl;
        return true;
    }
    
    std::cout << "Error: Request " << id << " not found." << std::endl;
    return true;
}

bool QuakeAssistController::handleHandleEmergency(int teamId, int k) {
    int idx = findTeamIndexById(teamId);
    if (idx == -1) {
        // This case shouldn't normally happen with valid input but good to handle safely
        return false;
    }

    Team& currentTeam = teams[idx];
    int assignedCount = 0;
    int supplyAssigned = 0;
    int rescueAssigned = 0;

    for (int i = 0; i < k; ++i) {
        // If both queues are empty, nothing more to do
        if (supplyQueue.isEmpty() && rescueQueue.isEmpty()) {
            break;
        }

        // Determine which queue to pick from
        bool pickSupply = false;

        if (rescueQueue.isEmpty()) {
            pickSupply = true;
        } else if (supplyQueue.isEmpty()) {
            pickSupply = false;
        } else {
            // Both non-empty, compare scores
            Request frontSupply, frontRescue;
            supplyQueue.peek(frontSupply);
            rescueQueue.peek(frontRescue);

            int sScore = frontSupply.computeEmergencyScore();
            int rScore = frontRescue.computeEmergencyScore();

            if (sScore > rScore) {
                pickSupply = true;
            } else if (rScore > sScore) {
                pickSupply = false;
            } else {
                // Equal score: Rescue takes priority
                pickSupply = false;
            }
        }

        Request candidate;
        if (pickSupply) {
            supplyQueue.peek(candidate);
        } else {
            rescueQueue.peek(candidate);
        }

        // Try to assign to team
        if (currentTeam.tryAssignRequest(candidate)) {
            // Success: remove from queue and update counters
            if (pickSupply) {
                supplyQueue.dequeue(candidate); // remove for real
                supplyAssigned++;
            } else {
                rescueQueue.dequeue(candidate); // remove for real
                rescueAssigned++;
            }
            assignedCount++;
        } else {
            // Overload!
            std::cout << "Overload on Team " << teamId << ": rolling back mission." << std::endl;
            currentTeam.rollbackMission(supplyQueue, rescueQueue);
            return true; // Stop processing this command
        }
    }

    // Only print summary if we actually assigned something and didn't rollback (which returns early above)
    if (assignedCount > 0) {
        std::cout << "Team " << teamId << " assigned " << assignedCount 
                  << " requests (" << supplyAssigned << " SUPPLY, " 
                  << rescueAssigned << " RESCUE), total workload " 
                  << currentTeam.getCurrentWorkload() << "." << std::endl;
    }
    
    return true;
}

bool QuakeAssistController::handleDispatchTeam(int teamId) {
    int idx = findTeamIndexById(teamId);
    if (idx == -1) {
        std::cout << "Error: Team " << teamId << " not found." << std::endl;
        return true;
    }
    Team& t = teams[idx];
    if (!t.hasActiveMission()) {
        std::cout << "Error: Team " << teamId << " has no active mission." << std::endl;
        return true;
    }
    int workload = t.getCurrentWorkload();
    std::cout << "Team " << teamId << " dispatched with workload " << workload << "." << std::endl;
    t.clearMission();
    return true;
}

void QuakeAssistController::printQueues() const {
    // 1. Print Supply Queue
    std::cout << "SUPPLY QUEUE:" << std::endl;
    if (!supplyQueue.isEmpty()) {
        Request* data = supplyQueue.getData();
        int f = supplyQueue.getFrontIndex();
        int count = supplyQueue.getCount();
        int cap = supplyQueue.getCapacity();

        for (int i = 0; i < count; ++i) {
            int actualIdx = (f + i) % cap;
            // Format: ID CITY TYPE AMOUNT LEVEL
            std::cout << data[actualIdx].getId() << " "
                      << data[actualIdx].getCity() << " "
                      << data[actualIdx].getSupplyType() << " "
                      << data[actualIdx].getAmount() << " "
                      << data[actualIdx].getEmergencyLevel() << std::endl;
        }
    }

    // 2. Print Rescue Queue
    std::cout << "RESCUE QUEUE:" << std::endl;
    if (!rescueQueue.isEmpty()) {
        Request* data = rescueQueue.getData();
        int f = rescueQueue.getFrontIndex();
        int count = rescueQueue.getCount();
        int cap = rescueQueue.getCapacity();

        for (int i = 0; i < count; ++i) {
            int actualIdx = (f + i) % cap;
            // Format: ID CITY NUMPEOPLE RISK LEVEL
            std::cout << data[actualIdx].getId() << " "
                      << data[actualIdx].getCity() << " "
                      << data[actualIdx].getNumPeople() << " "
                      << data[actualIdx].getRisk() << " "
                      << data[actualIdx].getEmergencyLevel() << std::endl;
        }
    }
}

void QuakeAssistController::printTeam(int teamId) const {
    int idx = findTeamIndexById(teamId);
    if (idx == -1) {
        std::cout << "Error: Team " << teamId << " not found." << std::endl;
        return;
    }

    const Team& t = teams[idx];
    std::cout << "TEAM " << teamId << " STACK:" << std::endl;

    const MissionStack& stack = t.getMissionStack();
    if (!stack.isEmpty()) {
        Request* data = stack.getData();
        int top = stack.getTopIndex();

        for (int i = top; i >= 0; --i) {
            if (data[i].getType() == "SUPPLY") {
                 std::cout << data[i].getId() << " "
                           << data[i].getCity() << " "
                           << data[i].getSupplyType() << " "
                           << data[i].getAmount() << " "
                           << data[i].getEmergencyLevel() << std::endl;
            } else {
                 std::cout << data[i].getId() << " "
                           << data[i].getCity() << " "
                           << data[i].getNumPeople() << " "
                           << data[i].getRisk() << " "
                           << data[i].getEmergencyLevel() << std::endl;
            }
        }
    }
}

void QuakeAssistController::clear() {
    supplyQueue.clear();
    rescueQueue.clear();
    for (int i = 0; i < teamCount; ++i) {
        teams[i].clearMission();
    }
    std::cout << "System cleared." << std::endl;
}