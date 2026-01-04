#pragma once

#include <string>

// Riddle system initialization and validation
// Returns true if riddles are loaded successfully, false otherwise
bool initRiddles(std::string& errorMessage);
void resetRiddlesIndex();

// Get the number of loaded riddles
int getRiddleCount();
