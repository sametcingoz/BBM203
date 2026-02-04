#include "RequestQueue.h"
#include <new> // for std::nothrow

RequestQueue::RequestQueue()
    : data(nullptr),
      capacity(0),
      front(0),
      rear(0),
      count(0) {
    // start with a small default capacity
    resize(4);
}

RequestQueue::RequestQueue(int initialCapacity)
    : data(nullptr),
      capacity(0),
      front(0),
      rear(0),
      count(0) {
    if (initialCapacity < 1) {
        initialCapacity = 4;
    }
    resize(initialCapacity);
}

RequestQueue::~RequestQueue() {
    delete[] data;
}

bool RequestQueue::isEmpty() const {
    return count == 0;
}

bool RequestQueue::isFull() const {
    return count == capacity;
}

int RequestQueue::size() const {
    return count;
}

int RequestQueue::nextIndex(int idx) const {
    if (capacity == 0) return 0;
    return (idx + 1) % capacity;
}

bool RequestQueue::enqueue(const Request& req) {
    // Implement enqueue function as explained in the PDF.
    
    // Check if the queue is full, if so, resize double the capacity.
    if (isFull()) {
        if (!resize(capacity * 2)) {
            return false; // Memory allocation failed
        }
    }

    // Insert the new request at the rear position.
    data[rear] = req;

    // Move rear pointer to the next position circularly.
    rear = nextIndex(rear);
    
    // Increment the element count.
    count++;
    
    return true;
}

bool RequestQueue::dequeue(Request& outReq) {
    // Implement dequeue function as explained in the PDF.
    
    // Check if queue is empty.
    if (isEmpty()) {
        return false;
    }

    // Retrieve the element from the front.
    outReq = data[front];

    // Move front pointer to the next position circularly.
    front = nextIndex(front);

    // Decrement the element count.
    count--;

    return true;
}

bool RequestQueue::peek(Request& outReq) const {
    // Implement peek function as explained in the PDF.
    
    if (isEmpty()) {
        return false;
    }
    // Just return the front element without removing it.
    outReq = data[front];
    return true;
}

void RequestQueue::clear() {
    front = 0;
    rear = 0;
    count = 0;
    // We do not deallocate memory here, just reset indices.
}

bool RequestQueue::removeById(const std::string& id) {
    // Implement removeById function as explained in the PDF.
    
    if (isEmpty()) {
        return false;
    }

    int foundLogicalIndex = -1;

    // 1. Search for the request with the given ID.
    // We iterate through logical indices (0 to count-1).
    for (int i = 0; i < count; ++i) {
        // Calculate the actual physical index in the circular array.
        int actualIdx = (front + i) % capacity;
        
        if (data[actualIdx].getId() == id) {
            foundLogicalIndex = i;
            break;
        }
    }

    // If not found, return false.
    if (foundLogicalIndex == -1) {
        return false;
    }

    // 2. Remove the element and shift remaining elements to fill the gap.
    // We shift elements starting from the found index to the end.
    for (int i = foundLogicalIndex; i < count - 1; ++i) {
        int currentPhysical = (front + i) % capacity;
        int nextPhysical = (front + i + 1) % capacity;
        
        // Shift the next element to the current position.
        data[currentPhysical] = data[nextPhysical];
    }

    // Decrement count.
    count--;

    // Update rear index. Rear should point to the next empty slot.
    // New rear is (front + count) % capacity.
    rear = (front + count) % capacity;

    return true;
}

bool RequestQueue::resize(int newCapacity) {
    // Implement resize function as explained in the PDF.
    
    // Allocate new array with the new capacity.
    Request* newData = new (std::nothrow) Request[newCapacity];
    if (!newData) {
        return false; // Allocation failed
    }

    // Copy elements from the old array to the new array linearly.
    // This "unwraps" the circular buffer into a straight array.
    for (int i = 0; i < count; ++i) {
        newData[i] = data[(front + i) % capacity];
    }

    // Delete the old array.
    delete[] data;

    // Point to the new array.
    data = newData;
    capacity = newCapacity;

    // Reset front and rear.
    front = 0;
    rear = count; // Rear points to the slot after the last element.

    return true;
}




