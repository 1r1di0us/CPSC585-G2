#pragma once
#include <vector>
#include "Entity.h"

class GameState {
public:
	// Public Functions
	GameState() {}
	void menuEventHandler();

	// Entity Tracking
	std::vector<Entity>* entityList;
	Entity* winner;

	// Flags
	bool inMenu = true;
	bool loading = false;
	bool quit = false;
	bool gameEnded = false;
	int menuOptionIndex = 0;
	int nbMenuOptions = 2; // Currently options are play and quit

	bool inGameMenu = false;
	int ingameOptionIndex = 0;
	int nbIngameOptions = 2; // Options will be main menu and quit

	// Game Parameters
	int numPlayers = 1;
	int numVehicles = 4;

	//// Audio 
	//AudioManager* audio_ptr = nullptr;
	//glm::vec3 listener_position;

private:
	void initGameState();
};