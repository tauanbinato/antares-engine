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

void Scene::AnimationSystem() {
  auto view = _registry.view<SpriteComponent, AnimationComponent>();

  // use forward iterators and get only the components of interest
  for (auto entity : view) {
    auto& sprite = view.get<SpriteComponent>(entity);
    auto& animation = view.get<AnimationComponent>(entity);

    animation.currentFrame = ((SDL_GetTicks() - animation.startTime) * animation.frameSpeedRate / 1000) % animation.numFrames;
    sprite.srcRect.x = animation.currentFrame * sprite.width;
  }
}

void Scene::CollisionSystem() {
  auto view = _registry.view<BoxColliderComponent, TransformComponent>();

  // use forward iterators and get only the components of interest
  for (auto entityA : view) {
    auto& aCollider = view.get<BoxColliderComponent>(entityA);
    auto& aTransform = view.get<TransformComponent>(entityA);
    for (auto entityB : view) {
      auto& bCollider = view.get<BoxColliderComponent>(entityB);
      auto& bTransform = view.get<TransformComponent>(entityB);

      // Perform the AABB collision check between entities a and b
      bool collisionHappened = CheckAABBCollision(
        aTransform.position.x + aCollider.offset.x,
        aTransform.position.y + aCollider.offset.y,
        aCollider.width,
        aCollider.height,
        bTransform.position.x + bCollider.offset.x,
        bTransform.position.y + bCollider.offset.y,
        bCollider.width,
        bCollider.height
      );

      if (collisionHappened) {
        Logger::Log("Entity  is colliding with entity ");

        // TODO: emit an event...
      }
    }

  }
}

bool Scene::CheckAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH) {
  return (
    aX < bX + bW &&
    aX + aW > bX &&
    aY < bY + bH &&
    aY + aH > bY
    );
}


void Scene::update(double deltaTime) {
  move_system(deltaTime);
  AnimationSystem();
  CollisionSystem();
}

void Scene::render(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {
  RenderSystem(renderer, assetStore);
}