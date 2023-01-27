#ifndef ANTARES_H
#define ANTARES_H

#include <SDL2/SDL.h>
#include "../ECS/scene.h"
#include "../AssetStore/AssetStore.h"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Antares {
private:
	bool _isRunning;
	int _millisecsPreviousFrame = 0;
	SDL_Window* _window;
	SDL_Renderer* _renderer;
	Scene* _scene; //TODO: change to smart pointers
	std::unique_ptr<AssetStore> _assetStore;

public:
	Antares();
	~Antares();
	void Initialize();
	void Run();
	void Setup();
	void ProcessInput();
	void Update();
	void Render();
	void Destroy();
	void LoadScene(Scene* newScene);

	int _windowWidth;
	int _windowHeight;
};

#endif