#include "WagonList.h"
#include <iostream>

WagonList::~WagonList() {
    clear();
}

void WagonList::clear() {
    Wagon* current = front;
    while (current != nullptr) {
        Wagon* next = current->getNext();
        delete current;
        current = next;
    }
    front = nullptr;
    rear = nullptr;
    totalWeight = 0;
}

WagonList::WagonList(WagonList&& other) noexcept
    : front(other.front), rear(other.rear), totalWeight(other.totalWeight) {
    other.front = nullptr;
    other.rear = nullptr;
    other.totalWeight = 0;
}

WagonList& WagonList::operator=(WagonList&& other) noexcept {
    if (this != &other) {
        clear();
        front = other.front;
        rear = other.rear;
        totalWeight = other.totalWeight;
        other.front = nullptr;
        other.rear = nullptr;
        other.totalWeight = 0;
    }
    return *this;
}

Wagon* WagonList::findById(int id) {
    Wagon* current = front;
    while (current != nullptr) {
        if (current->getID() == id) {
            return current;
        }
        current = current->getNext();
    }
    return nullptr;
}

void WagonList::addWagonToRear(Wagon* w) {
    if (w == nullptr) {
        return;
    }
    w->setPrev(rear);
    w->setNext(nullptr);
    if (isEmpty()) {
        front = w;
    } else {
        rear->setNext(w);
    }
    rear = w;
    totalWeight += w->getWeight();
}

int WagonList::getTotalWeight() const {
    return totalWeight;
}

bool WagonList::isEmpty() const {
    return front == nullptr;
}

void WagonList::insertSorted(Wagon* wagon) {
    if (wagon == nullptr) {
        return;
    }

    if (isEmpty()) {
        front = wagon;
        rear = wagon;
        wagon->setNext(nullptr);
        wagon->setPrev(nullptr);
    } else if (wagon->getWeight() > front->getWeight()) {
        wagon->setNext(front);
        front->setPrev(wagon);
        front = wagon;
        wagon->setPrev(nullptr);
    } else {
        Wagon* current = front;
        while (current->getNext() != nullptr && current->getNext()->getWeight() >= wagon->getWeight()) {
            current = current->getNext();
        }
        wagon->setNext(current->getNext());
        if (current->getNext() != nullptr) {
            current->getNext()->setPrev(wagon);
        } else {
            rear = wagon;
        }
        current->setNext(wagon);
        wagon->setPrev(current);
    }
    totalWeight += wagon->getWeight();
}

void WagonList::appendList(WagonList&& other) {
    if (other.isEmpty()) {
        return;
    }
    if (this->isEmpty()) {
        front = other.front;
        rear = other.rear;
        totalWeight = other.totalWeight;
    } else {
        this->rear->setNext(other.front);
        other.front->setPrev(this->rear);
        this->rear = other.rear;
        this->totalWeight += other.totalWeight;
    }
    other.front = nullptr;
    other.rear = nullptr;
    other.totalWeight = 0;
}

Wagon* WagonList::detachById(int id) {
    Wagon* toRemove = findById(id);
    if (toRemove == nullptr) {
        return nullptr;
    }

    if (toRemove->getPrev() != nullptr) {
        toRemove->getPrev()->setNext(toRemove->getNext());
    } else {
        front = toRemove->getNext();
    }

    if (toRemove->getNext() != nullptr) {
        toRemove->getNext()->setPrev(toRemove->getPrev());
    } else {
        rear = toRemove->getPrev();
    }

    totalWeight -= toRemove->getWeight();
    toRemove->setPrev(nullptr);
    toRemove->setNext(nullptr);

    std::cout << "Wagon " << id << " detached from Wagon List. " << std::endl;
    return toRemove;
}

WagonList WagonList::splitAtById(int id) {
    WagonList newList;
    Wagon* splitNode = findById(id);

    if (splitNode == nullptr) {
        return newList;
    }

    newList.front = splitNode;
    newList.rear = this->rear;
    
    this->rear = splitNode->getPrev();

    if (this->rear != nullptr) {
        this->rear->setNext(nullptr);
    } else {
        this->front = nullptr;
    }
    splitNode->setPrev(nullptr);

    int newThisWeight = 0;
    Wagon* current = this->front;
    while(current != nullptr) {
        newThisWeight += current->getWeight();
        current = current->getNext();
    }
    this->totalWeight = newThisWeight;

    int newListWeight = 0;
    current = newList.front;
    while(current != nullptr) {
        newListWeight += current->getWeight();
        current = current->getNext();
    }
    newList.totalWeight = newListWeight;

    return newList;
}

void WagonList::print() const {
    std::cout << *this << std::endl;
}

std::ostream& operator<<(std::ostream& os, const WagonList& list) {
    if (list.isEmpty()) {
        return os;
    }
    Wagon* current = list.front;
    while (current) {
        os << "W" << current->getID() << "(" << current->getWeight() << "ton)";
        if (current->getNext()) {
            os << " - ";
        }
        current = current->getNext();
    }
    return os;
}