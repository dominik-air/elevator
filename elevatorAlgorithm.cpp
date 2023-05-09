#include <Arduino.h>
#include <LinkedList.h>

const int INT_MAX = 999;

int ascendingOrder(int &a, int &b) {
    return a - b;
}

int descendingOrder(int &a, int &b) {
    return b - a;
}

void fillMissingIntegers(LinkedList<int> &list) {
    for (int i = 0; i < list.size() - 1;) {
        int current = list.get(i);
        int next = list.get(i + 1);
        int difference = next - current;

        if (difference != 1 && difference != -1) {
            list.add(i + 1, current + (difference > 1 ? 1 : -1));
        } else {
            i++;
        }
    }
}

void elevatorAlgorithm(int currentFloor, const LinkedList<int> &floorsToVisit, LinkedList<int> &elevatorPath) {
    
    LinkedList<int> above;
    LinkedList<int> below;

    for (uint8_t i = 0; i < floorsToVisit.size(); ++i) {
        int floor = floorsToVisit.get(i);
        if (floor > currentFloor) {
            above.add(floor);
        }
        else {
            below.add(floor);
        }
    }

    above.sort(ascendingOrder);
    below.sort(descendingOrder);

    elevatorPath.clear();
    elevatorPath.add(currentFloor);
    if (above.size() < below.size()) {
        for (uint8_t i = 0; i < above.size(); ++i) {
            elevatorPath.add(above.get(i));
        }
        for (uint8_t i = 0; i < below.size(); ++i) {
            elevatorPath.add(below.get(i));
        }
    }
    else {
        for (uint8_t i = 0; i < below.size(); ++i) {
            elevatorPath.add(below.get(i));
        }
        for (uint8_t i = 0; i < above.size(); ++i) {
            elevatorPath.add(above.get(i));
        }
    }
    fillMissingIntegers(elevatorPath);
}