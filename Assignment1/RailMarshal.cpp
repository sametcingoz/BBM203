#include "RailMarshal.h"
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

RailMarshal::RailMarshal() {
    for (int i = 0; i < NUM_DESTINATIONS_INT; ++i) {
        departureYard[i] = TrainTrack(static_cast<Destination>(i));
    }
}

RailMarshal::~RailMarshal() {
    classificationYard.clear();
    for (int i = 0; i < NUM_DESTINATIONS_INT; ++i) {
        Train* train;
        while ((train = departureYard[i].departTrain()) != nullptr) {
            delete train;
        }
    }
}

ClassificationYard& RailMarshal::getClassificationYard() {
    return classificationYard;
}

TrainTrack& RailMarshal::getDepartureYard(Destination dest) {
    int idx = static_cast<int>(dest);
    return departureYard[idx];
}

void RailMarshal::processCommand(const std::string& line) {
    std::stringstream ss(line);
    std::string cmd;
    ss >> cmd;

    if (cmd == "ADD_WAGON") {
        int id, weight, maxLoad;
        std::string cargoStr, destStr;
        if (ss >> id >> cargoStr >> destStr >> weight >> maxLoad) {
            Wagon* w = new Wagon(id, parseCargo(cargoStr), parseDestination(destStr), weight, maxLoad);
            classificationYard.insertWagon(w);
            std::cout << "Wagon " << *w << " added to yard." << std::endl;
        } else {
            std::cout << "Error: Invalid ADD_WAGON parameters.\n";
        }
    } else if (cmd == "REMOVE_WAGON") {
        int id;
        if (ss >> id) {
            bool found = false;
            for (int i = 0; i < NUM_DESTINATIONS_INT && !found; ++i) {
                for (int j = 0; j < NUM_CARGOTYPES_INT && !found; ++j) {
                    Wagon* detached = classificationYard.getBlockTrain(i, j).detachById(id);
                    if (detached) {
                        delete detached;
                        std::cout << "Wagon " << id << " removed." << std::endl;
                        found = true;
                    }
                }
            }
            if (!found) {
                std::cout << "Error: Wagon " << id << " not found." << std::endl;
            }
        } else {
            std::cout << "Error: Invalid REMOVE_WAGON parameters.\n";
        }
    } else if (cmd == "ASSEMBLE_TRAIN") {
        std::string destStr;
        if (ss >> destStr) {
            Destination dest = parseDestination(destStr);
            TrainTrack& track = getDepartureYard(dest);
            std::string trainName = track.generateTrainName();
            Train* train = classificationYard.assembleTrain(dest, trainName);

            if (train == nullptr) {
                std::cout << "No wagons to assemble for " << destStr << std::endl;
            } else {
                std::cout << "Train " << train->getName() << " assembled with " << train->getWagons() << " wagons." << std::endl;
                
                Train* frontTrain = train;
                int splitCounter = 1;
                while (true) {
                    Train* newSplitTrain = frontTrain->verifyCouplersAndSplit(splitCounter);
                    if (newSplitTrain) {
                         std::cout << "Train " << newSplitTrain->getName() << " assembled after split with " << newSplitTrain->getWagons() << " wagons." << std::endl;
                        track.addTrain(newSplitTrain);
                        splitCounter++;
                    } else {
                        break;
                    }
                }
                track.addTrain(frontTrain);
            }
        } else {
            std::cout << "Error: Invalid ASSEMBLE_TRAIN parameters.\n";
        }
    } else if (cmd == "DISPATCH_TRAIN") {
        std::string destStr;
        if (ss >> destStr) {
            dispatchFromTrack(parseDestination(destStr));
        } else {
            std::cout << "Error: Invalid DISPATCH parameters.\n";
        }
    } else if (cmd == "PRINT_YARD") {
        std::cout << "--- classification Yard ---\n";
        classificationYard.print();
    } else if (cmd == "PRINT_TRACK") {
        std::string destStr;
        if (ss >> destStr) {
            getDepartureYard(parseDestination(destStr)).printTrack();
        } else {
            std::cout << "Error: Invalid PRINT_TRACK parameters.\n";
        }
    } else if (cmd == "AUTO_DISPATCH") {
        std::string mode;
        if (ss >> mode) {
            if (mode == "ON") {
                TrainTrack::autoDispatch = true;
                std::cout << "Auto dispatch enabled" << std::endl;
            } else if (mode == "OFF") {
                TrainTrack::autoDispatch = false;
                std::cout << "Auto dispatch disabled" << std::endl;
            } else {
                std::cout << "Error: Invalid AUTO_DISPATCH parameters.\n";
            }
        } else {
             std::cout << "Error: Invalid AUTO_DISPATCH parameters.\n";
        }
    } else if (cmd == "CLEAR") {
        classificationYard.clear();
        for (int i = 0; i < NUM_DESTINATIONS_INT; ++i) {
            Train* train;
            while ((train = departureYard[i].departTrain()) != nullptr) {
                 // EKLENEN SATIR BURASI:
                 std::cout << "Train " << train->getName() << " departed from Track " << destinationToString(train->getDestination()) << "." << std::endl;
                delete train;
            }
        }
        std::cout << "System cleared." << std::endl;
    } else {
        if (!cmd.empty())
            std::cout << "Error: Unknown command '" << cmd << "'" << std::endl;
    }
}

void RailMarshal::dispatchFromTrack(Destination dest) {
    std::string destStr = destinationToString(dest);
    TrainTrack& track = getDepartureYard(dest);
    Train* trainToDispatch = track.departTrain();

    if (trainToDispatch == nullptr) {
        std::cout << "Error: No trains to dispatch from track " << destStr << ".\n";
    } else {
        std::cout << "Dispatching " << trainToDispatch->getName() << " (" << trainToDispatch->getTotalWeight() << " tons)." << std::endl;
        std::cout << "Train " << trainToDispatch->getName() << " departed from Track " << destStr << "." << std::endl;
        delete trainToDispatch;
    }
}

void RailMarshal::printDepartureYard() const {
    for (int i = 0; i < NUM_DESTINATIONS_INT; ++i) {
        departureYard[i].printTrack();
    }
}

void RailMarshal::printStatus() const {
    std::cout << "--- classification Yard ---\n";
    classificationYard.print();
    std::cout << "--- Departure Yard ---\n";
    printDepartureYard();
}