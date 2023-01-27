#include "Game.h"
#include "../ECS/entity.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>

Game::Game() {
	_isRunning = false;
	Logger::Log("Game constructor called!");
}

Game::~Game() {
	Logger::Log("Game destructor called!");
}

void Game::Initialize() {
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

void Game::ProcessInput() {
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

glm::vec2 tankPosition;
glm::vec2 tankVelocity;

void Game::Setup() {
	tankPosition = glm::vec2(10.0, 20.0);
	tankVelocity = glm::vec2(5.0, 0.0);

	Entity tank = _scene->create_entity("tank");
	Entity tank2 = _scene->create_entity("tank2");
	tank.add_component<PositionComponent>(tankPosition.x, tankPosition.y);
	tank.add_component<VelocityComponent>(tankVelocity.x, tankVelocity.y);
	tank.add_component<AccelerationComponent>(0.0, 0.0);

	tank2.add_component<PositionComponent>(50.0, 200.0);
	tank2.add_component<VelocityComponent>(0.0, 0.0);
	tank2.add_component<AccelerationComponent>(5.0, 0.0);

	// PositionComponent& pos = tank.get_component<PositionComponent>();
}

void Game::Update() {

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

void Game::Render() {
	SDL_SetRenderDrawColor(_renderer, 21, 21, 21, 255);
	SDL_RenderClear(_renderer);


	_scene->render(_renderer);
	SDL_RenderPresent(_renderer);
}

void Game::Run() {
	Setup();

	while (_isRunning) {
		ProcessInput();
		Update();
		Render();
	}
}

void Game::Destroy() {
	delete _scene;
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}