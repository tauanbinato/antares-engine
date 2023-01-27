#include "Antares.h"
#include "../ECS/entity.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>

Antares::Antares() {
	_isRunning = false;
	_assetStore = std::make_unique<AssetStore>();
}

Antares::~Antares() {
	Logger::Log("Antares destructor called!");
}

void Antares::Initialize() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		Logger::Err("Error initializing SDL.");
		return;
	}
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	_windowWidth = displayMode.w;
	_windowHeight = displayMode.h;
	_window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_windowWidth,
		_windowHeight,
		SDL_WINDOW_BORDERLESS
	);
	if (!_window) {
		Logger::Err("Error creating SDL window.");
		return;
	}
	_renderer = SDL_CreateRenderer(_window, -1, 0);
	if (!_renderer) {
		Logger::Err("Error creating SDL renderer.");
		return;
	}
	SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
	_isRunning = true;

	// Alloc in heap a scene
	_scene = new Scene();
}

void Antares::ProcessInput() {
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			_isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				_isRunning = false;
			}
			break;
		}
	}
}


void Antares::LoadScene(Scene* newScene) {
	// Adding assets to the asset store
	_assetStore->AddTexture(_renderer, "tank-image", "./assets/images/tank-panther-right.png");
	_assetStore->AddTexture(_renderer, "truck-image", "./assets/images/truck-ford-right.png");
	_assetStore->AddTexture(_renderer, "tilemap-image", "./assets/tilemaps/jungle.png");
	_assetStore->AddTexture(_renderer, "chopper-image", "./assets/images/chopper.png");
	_assetStore->AddTexture(_renderer, "radar-image", "./assets/images/radar.png");

	// Load the tilemap
	int tileSize = 32;
	double tileScale = 4.0;
	int mapNumCols = 25;
	int mapNumRows = 20;

	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			std::string tileTag = "tile-x: " + std::to_string(srcRectX) + " -y: " + std::to_string(srcRectY);
			Entity tile = newScene->create_entity(tileTag);
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, srcRectX, srcRectY, 0);
		}
	}
	mapFile.close();

	// Create an entity tank
	Entity tank = newScene->create_entity("tank");
	tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(2.0, 2.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(5.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 0, 0, 1);

	Entity tank2 = newScene->create_entity("truck");
	tank2.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.5, 1.5), 0.0);
	tank2.AddComponent<RigidBodyComponent>(glm::vec2(2.0, 0.0));
	tank2.AddComponent<SpriteComponent>("truck-image", 32, 32, 0, 0, 1);

	Entity chopper = newScene->create_entity("chopper");
	chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(10.0, 10.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 0.0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 0, 0, 3);
	chopper.AddComponent<AnimationComponent>(2, 15, true);

	Entity radar = newScene->create_entity("ui-radar");
	radar.AddComponent<TransformComponent>(glm::vec2(_windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 0, 0, 5);
	radar.AddComponent<AnimationComponent>(8, 5, true);

	newScene->SortZIndex();
}

void Antares::Setup() {
	LoadScene(_scene);
}

void Antares::Update() {

	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - _millisecsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}

	// The difference in ticks since the last frame in seconds
	double deltaTime = (SDL_GetTicks() - _millisecsPreviousFrame) / 1000.0;

	// Store the "previous" frame time
	_millisecsPreviousFrame = SDL_GetTicks();

	_scene->update(deltaTime);
}

void Antares::Render() {
	SDL_SetRenderDrawColor(_renderer, 21, 21, 21, 255);
	SDL_RenderClear(_renderer);


	_scene->render(_renderer, _assetStore);
	SDL_RenderPresent(_renderer);
}

void Antares::Run() {
	Setup();

	while (_isRunning) {
		ProcessInput();
		Update();
		Render();
	}
}

void Antares::Destroy() {
	delete _scene;
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}