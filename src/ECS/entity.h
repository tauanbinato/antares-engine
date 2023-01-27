#pragma once

#include <entt/entt.hpp>
#include "scene.h"


class Entity
{
private:
  entt::entity entity_handle = entt::null;
  Scene* scene = NULL;

public:
  Entity(entt::entity s_entity_handle, Scene* s_scene);

  inline entt::entity get_handle() { return entity_handle; }

  template<typename T, typename... Args>
  T& AddComponent(Args&&... args)
  {
    return scene->_registry.emplace<T>(entity_handle, std::forward<Args>(args)...);
  }

  template<typename T>
  T& get_component()
  {
    return scene->_registry.get<T>(entity_handle);
  }

  template<typename T>
  void remove_component()
  {
    return scene->_registry.remove<T>(entity_handle);
  }

  template<typename T>
  bool has_component()
  {
    return scene->_registry.all_of<T>(entity_handle);
  }
};