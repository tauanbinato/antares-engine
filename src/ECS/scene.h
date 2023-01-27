#pragma once

#include <entt/entt.hpp>
#include <SDL2/SDL.h>

#include "components.h"


class Entity;

class Scene
{
private:
  entt::registry registry;

  void print_system();
  void move_system(double deltaTime);
  void RenderSystem(SDL_Renderer* renderer);

public:
  Scene();
  ~Scene();

  Entity create_entity(std::string tag);

  void update(double deltaTime);
  void render(SDL_Renderer* renderer);

  friend class Entity;
};