#include "scene.h"

#include "entity.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


Scene::Scene()
{}

Scene::~Scene()
{}

Entity Scene::create_entity(std::string tag)
{
  Entity entity(registry.create(), this);
  entity.add_component<TagComponent>(tag);
  return entity;
}

void Scene::print_system()
{
  // Print System
  for (
    auto [entity, pos, vel, acc] :
    registry.view<PositionComponent, VelocityComponent, AccelerationComponent>().each()
    )
  {
    printf("pos: %lf  %lf\nvel: %lf  %lf\nacc: % lf % lf\n--------------------\n", pos.x, pos.y, vel.x, vel.y, acc.x, acc.y);
  }
}

void Scene::move_system(double deltaTime)
{
  // Move System
  for (
    auto [entity, pos, vel, acc] :
    registry.view<PositionComponent, VelocityComponent, AccelerationComponent>().each()
    )
  {
    vel.x += acc.x;
    vel.y += acc.y;

    pos.x += vel.x * deltaTime;
    pos.y += vel.y * deltaTime;
  }
}

void Scene::RenderSystem(SDL_Renderer* renderer) {
  // Move System
  for (
    auto [entity, pos, vel] :
    registry.view<PositionComponent, VelocityComponent>().each()
    )
  {
    // Loads a PNG texture
    SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // What is the destination rectangle that we want to place our texture
    SDL_Rect dstRect = {
      static_cast<int>(pos.x),
      static_cast<int>(pos.y),
      32,
      32
    };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);
  }
}

void Scene::update(double deltaTime)
{
  //print_system();
  move_system(deltaTime);
}

void Scene::render(SDL_Renderer* renderer) {
  RenderSystem(renderer);
}