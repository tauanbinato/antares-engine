#include "scene.h"

#include "entity.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "./components.h"
#include "../AssetStore/AssetStore.h"
#include "../Logger/Logger.h"

Scene::Scene() {}
Scene::~Scene() {}

Entity Scene::create_entity(std::string tag) {
  Entity entity(_registry.create(), this);
  entity.AddComponent<TagComponent>(tag);
  return entity;
}

void Scene::print_system()
{
  // Print System
  for (auto [entity, transform] : _registry.view<TransformComponent>().each()) {
    printf("pos: %lf  %lf \n--------------------\n", transform.position.x, transform.position.y);
  }
}

void Scene::move_system(double deltaTime) {

  auto view = _registry.view<TransformComponent, RigidBodyComponent>();

  // use forward iterators and get only the components of interest
  for (auto entity : view) {
    auto& transform = view.get<TransformComponent>(entity);
    auto& rigidbody = view.get<RigidBodyComponent>(entity);
    transform.position.x += rigidbody.velocity.x * deltaTime;
    transform.position.y += rigidbody.velocity.y * deltaTime;
  }
}

void Scene::SortZIndex() {
  // Sorting by Z-Index
  _registry.sort<SpriteComponent>([](const auto& lhs, const auto& rhs) { return  lhs.zIndex < rhs.zIndex; });
}

void Scene::RenderSystem(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {

  // use a range-for
  for (auto [entity, sprite, transform] : _registry.view<SpriteComponent, TransformComponent>().each()) {

    // Set the source rectangle of our original sprite texture
    SDL_Rect srcRect = sprite.srcRect;
    //Logger::Log(std::to_string(sprite.zIndex));

    // Set the destination rectangle with the x,y position to be rendered
    SDL_Rect dstRect = {
        static_cast<int>(transform.position.x),
        static_cast<int>(transform.position.y),
        static_cast<int>(sprite.width * transform.scale.x),
        static_cast<int>(sprite.height * transform.scale.y)
    };

    // Render the texture on the destination renderer window
    SDL_RenderCopyEx(
      renderer,
      assetStore->GetTexture(sprite.assetId),
      &srcRect,
      &dstRect,
      transform.rotation,
      NULL,
      SDL_FLIP_NONE
    );
  }
}

void Scene::update(double deltaTime) {
  move_system(deltaTime);
  //print_system();
}

void Scene::render(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {
  RenderSystem(renderer, assetStore);
}