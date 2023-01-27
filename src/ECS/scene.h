#pragma once

#include <entt/entt.hpp>
#include <SDL2/SDL.h>

#include "components.h"
#include "../AssetStore/AssetStore.h"


class Entity;

class Scene
{
private:
  entt::registry _registry;

  void print_system();
  void move_system(double deltaTime);
  void RenderSystem(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore);
  void AnimationSystem();
  void CollisionSystem();
  bool CheckAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH);

public:
  Scene();
  ~Scene();

  Entity create_entity(std::string tag);

  void update(double deltaTime);
  void render(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore);
  void SortZIndex();

  friend class Entity;
};