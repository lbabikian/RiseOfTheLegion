#ifndef GAMEDECISIONTREE_H
#define GAMEDECISIONTREE_H

#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Node.h"
#include "Story.h"

// Helper function just for to trimming whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

template <typename T>
class GameDecisionTree {
private:
    Node<T>* root;

public:
    GameDecisionTree() : root(nullptr) {}

    // Function to load story data from a text file and build the binary tree
    void loadStoryFromFile(const std::string& filename, char delimiter) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file." << std::endl;
            return;
        }

        std::unordered_map<int, Node<T>*> eventMap; // Temp map to hold nodes by event #
        std::string line;

        // Reads each line from the file
        while (std::getline(file, line)) {
            // Trims whitespace from line & skip if empty or a comment
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue; // Skip blank lines & comments
            }

            std::stringstream ss(line);
            std::string eventNumStr, desc, leftNumStr, rightNumStr;

            // Split the line based on the delimiter
            if (!std::getline(ss, eventNumStr, delimiter) ||
                !std::getline(ss, desc, delimiter) ||
                !std::getline(ss, leftNumStr, delimiter) ||
                !std::getline(ss, rightNumStr, delimiter)) {
                std::cerr << "Error parsing line: " << line << std::endl;
                continue; // Skip this line and go to the next
            }

            try {
                // Trim whitespace from the parsed strings
                eventNumStr = trim(eventNumStr);
                desc = trim(desc);
                leftNumStr = trim(leftNumStr);
                rightNumStr = trim(rightNumStr);

                // Convert event number and choices to integers
                int eventNum = std::stoi(eventNumStr);
                int leftNum = std::stoi(leftNumStr);
                int rightNum = std::stoi(rightNumStr);

                // Create a Story object and Node for each line
                Story story(desc, eventNum, leftNum, rightNum);
                Node<T>* node = new Node<T>(story);

                // Store the node in the map
                eventMap[story.eventNumber] = node;

                // Set the root node if this is the first entry
                if (root == nullptr) {
                    root = node;
                }
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid number format in line: " << line << std::endl;
                continue; // Skip this line and go to the next
            }
        }

        // Link nodes based on left and right event numbers
        for (auto& pair : eventMap) {
            Node<T>* node = pair.second;

            int leftNum = node->data.leftEventNumber;
            int rightNum = node->data.rightEventNumber;

            // If valid event numbers exist, link to the corresponding nodes
            if (leftNum != -1 && eventMap.count(leftNum)) {
                node->left = eventMap[leftNum];
            }
            if (rightNum != -1 && eventMap.count(rightNum)) {
                node->right = eventMap[rightNum];
            }
        }

        file.close();
    }

    // Function to start the game and traverse the tree based on user input
    void playGame() {
        // Display intro sections without requiring an input
        Node<T>* current = root;
        while (current != nullptr && current->data.leftEventNumber == -1 && current->data.rightEventNumber != -1) {
            std::cout << current->data.description << "\n\n";
            current = current->right; // Move to the next intro node
        }

        // This should point to the first interactive node
        if (current != nullptr) {
            std::cout << current->data.description << "\n";
        }

        // Interactive part of the game
        while (current != nullptr && (current->left != nullptr || current->right != nullptr)) {
            std::cout << "Choose (L for left, R for right): ";
            char choice;
            std::cin >> choice;

            if (choice == 'L' || choice == 'l') {
                current = current->left;
            } else if (choice == 'R' || choice == 'r') {
                current = current->right;
            } else {
                std::cout << "Invalid choice. Try again.\n";
            }

            if (current != nullptr) {
                std::cout << current->data.description << "\n";
            }
        }

        // End of game message
        if (current != nullptr && current->left == nullptr && current->right == nullptr) {
            std::cout << "Game Over. Thank you for coming onto this journey." << std::endl;
        }
    }
};

#endif // GAMEDECISIONTREE_H
