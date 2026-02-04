#include "Train.h"
#include <iostream>
#include <utility> // std::move için eklendi

Train::Train() : name(""), destination(Destination::OTHERS), totalWeight(0), nextLocomotive(nullptr) {}
Train::Train(const std::string& _name, Destination _dest) : name(_name), destination(_dest), totalWeight(0), nextLocomotive(nullptr) {}

Train::~Train() {
    clear();
}

void Train::appendWagonList(WagonList& wl) {
    wagons.appendList(std::move(wl));
    totalWeight = wagons.getTotalWeight();
}

void Train::addWagonToRear(Wagon* w) {
    wagons.addWagonToRear(w);
    totalWeight = wagons.getTotalWeight();
}

void Train::clear() {
    wagons.clear();
    totalWeight = 0;
}

void Train::print() const {
    std::cout << "Train " << name << " (" << totalWeight << " tons): ";
    std::cout << wagons << std::endl;
}

Train* Train::verifyCouplersAndSplit(int splitCounter) {
    Wagon* current = wagons.getRear();
    if (current == nullptr) {
        return nullptr;
    }

    int trailingWeight = 0;
    while (current->getPrev() != nullptr) {
        trailingWeight += current->getWeight();
        Wagon* prevWagon = current->getPrev();

        if (trailingWeight > prevWagon->getMaxCouplerLoad()) {
            int splitId = current->getID();
            WagonList detachedList = wagons.splitAtById(splitId);

            std::string newTrainName = this->name + "_split_" + std::to_string(splitCounter);
            Train* newTrain = new Train(newTrainName, this->destination);
            newTrain->appendWagonList(detachedList);
            
            this->totalWeight = this->wagons.getTotalWeight();
            
            std::cout << "Train " << this->name << " split due to coupler overload before Wagon " << splitId << std::endl;
            std::cout << newTrain->getWagons() << std::endl;

            return newTrain;
        }
        current = prevWagon;
    }

    return nullptr;
}