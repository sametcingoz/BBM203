#include "ClassificationYard.h"
#include <iostream>
#include <utility>

ClassificationYard::ClassificationYard() {}

ClassificationYard::~ClassificationYard() {
    clear();
}

WagonList& ClassificationYard::getBlockTrain(int destination, int cargoType) {
    return blockTrains[destination][cargoType];
}

WagonList* ClassificationYard::getBlocksFor(Destination dest) {
    return blockTrains[static_cast<int>(dest)];
}

void ClassificationYard::insertWagon(Wagon* w) {
    if (!w) return;
    int dest = static_cast<int>(w->getDestination());
    int cargo = static_cast<int>(w->getCargoType());
    blockTrains[dest][cargo].insertSorted(w);
}

Train* ClassificationYard::assembleTrain(Destination dest, const std::string& trainName) {
    WagonList* blocksToSort[NUM_CARGOTYPES_INT];
    int blockCount = 0;
    Wagon* hazardousWagon = nullptr;
    int destIndex = static_cast<int>(dest);

    for (int i = 0; i < NUM_CARGOTYPES_INT; ++i) {
        WagonList& currentBlock = blockTrains[destIndex][i];
        if (!currentBlock.isEmpty()) {
            if (static_cast<CargoType>(i) == CargoType::HAZARDOUS) {
                if (!currentBlock.isEmpty()) {
                    hazardousWagon = currentBlock.detachById(currentBlock.getFront()->getID());
                }
            } else {
                blocksToSort[blockCount++] = &currentBlock;
            }
        }
    }

    if (blockCount == 0 && hazardousWagon == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < blockCount - 1; ++i) {
        for (int j = 0; j < blockCount - i - 1; ++j) {
            if (blocksToSort[j]->getFront()->getWeight() < blocksToSort[j + 1]->getFront()->getWeight()) {
                WagonList* temp = blocksToSort[j];
                blocksToSort[j] = blocksToSort[j + 1];
                blocksToSort[j + 1] = temp;
            }
        }
    }

    Train* newTrain = new Train(trainName, dest);

    for (int i = 0; i < blockCount; ++i) {
        newTrain->appendWagonList(*blocksToSort[i]);
    }

    if (hazardousWagon != nullptr) {
        newTrain->addWagonToRear(hazardousWagon);
    }

    return newTrain;
}

bool ClassificationYard::isEmpty() const {
    for (int i = 0; i < NUM_DESTINATIONS_INT; ++i) {
        for (int j = 0; j < NUM_CARGOTYPES_INT; ++j) {
            if (!blockTrains[i][j].isEmpty()) {
                return false;
            }
        }
    }
    return true;
}

void ClassificationYard::clear() {
    for (int i = 0; i < NUM_DESTINATIONS_INT; ++i) {
        for (int j = 0; j < NUM_CARGOTYPES_INT; ++j) {
            blockTrains[i][j].clear();
        }
    }
}

void ClassificationYard::print() const {
    for (int i = 0; i < static_cast<int>(Destination::NUM_DESTINATIONS); ++i) {
        bool destHasWagons = false;
        for (int j = 0; j < static_cast<int>(CargoType::NUM_CARGOTYPES); ++j) {
            if (!blockTrains[i][j].isEmpty()) {
                destHasWagons = true;
                break;
            }
        }
        
        auto dest = destinationToString(static_cast<Destination>(i));
        std::cout << "Destination " << dest << ":\n";
        if (destHasWagons) {
            for (int j = 0; j < static_cast<int>(CargoType::NUM_CARGOTYPES); ++j) {
                if (!blockTrains[i][j].isEmpty()) {
                    auto type = cargoTypeToString(static_cast<CargoType>(j));
                    std::cout << "  CargoType " << type << ": ";
                    blockTrains[i][j].print();
                }
            }
        }
    }
}
