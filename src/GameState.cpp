#include "GameState.h"
#include <stdlib.h>
#include <time.h> 

using namespace std;

// Counters
int vehiclesSpawned = 0;

void GameState::initGameState() {
	Entity* e;
}

void GameState::menuEventHandler() {
	// Only handle events when in menu
	if (inMenu) {
		menuOptionIndex = 0;
	}
}