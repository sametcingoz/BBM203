#include "TrainTrack.h"
#include <iostream>

bool TrainTrack::autoDispatch = false;

TrainTrack::TrainTrack()
    : firstLocomotive(nullptr),
      lastLocomotive(nullptr),
      destination(Destination::OTHERS),
      totalWeight(0), trainCounter(0)
{
}

TrainTrack::TrainTrack(Destination _dest)
    : firstLocomotive(nullptr),
      lastLocomotive(nullptr),
      destination(_dest),
      totalWeight(0), trainCounter(0)
{
}

TrainTrack::~TrainTrack() {
    Train* current = firstLocomotive;
    while (current != nullptr) {
        Train* next = current->getNext();
        delete current;
        current = next;
    }
    firstLocomotive = nullptr;
    lastLocomotive = nullptr;
}

std::string TrainTrack::generateTrainName() {
    ++trainCounter;
    return "Train_" + destinationToString(destination) + "_" + std::to_string(trainCounter);
}

void TrainTrack::addTrain(Train* train) {
    if (train == nullptr) {
        return;
    }

    if (isEmpty()) {
        firstLocomotive = train;
        lastLocomotive = train;
    } else {
        lastLocomotive->setNext(train);
        lastLocomotive = train;
    }
    totalWeight += train->getTotalWeight();

    while (autoDispatch && totalWeight > AUTO_DISPATCH_LIMIT) {
        Train* departed = departTrain();
        if (departed) {
            std::cout << "Auto-dispatch: departing " << departed->getName() << " to make room.\n";
            // FAZLADAN OLAN VE SİLİNEN SATIR BURADAYDI.
            delete departed;
        } else {
            break;
        }
    }
}

Train* TrainTrack::departTrain() {
    if (isEmpty()) {
        return nullptr;
    }

    Train* removed = firstLocomotive;
    firstLocomotive = firstLocomotive->getNext();

    if (firstLocomotive == nullptr) {
        lastLocomotive = nullptr;
    }

    totalWeight -= removed->getTotalWeight();
    removed->setNext(nullptr);

    return removed;
}

bool TrainTrack::isEmpty() const {
    return firstLocomotive == nullptr;
}

Train* TrainTrack::findTrain(const std::string& name) const {
    Train* current = firstLocomotive;
    while (current != nullptr) {
        if (current->getName() == name) {
            return current;
        }
        current = current->getNext();
    }
    return nullptr;
}

void TrainTrack::printTrack() const {
    if (isEmpty()) {
        return;
    }

    Train* current = firstLocomotive;

    std::cout << "[Track " << static_cast<int>(destination) << "] ";
    while (current) {
        std::cout << current->getName() << "(" << current->getTotalWeight() << "ton)-" << current->getWagons() << " -> ";
        current = current->getNext();
    }
    std::cout << std::endl;
}