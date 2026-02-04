#include "QuakeAssistController.h"
#include <iostream>
#include <fstream>
#include <string>


int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);

    QuakeAssistController controller;
    
    std::string line;
    
    while (std::getline(std::cin, line)) {
        controller.parseAndExecute(line);
    }
    return 0;
}