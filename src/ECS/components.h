#pragma once

#include <string>
#include <glm/glm.hpp>

struct TagComponent
{
  std::string tag;
};

struct PositionComponent
{
  double x, y;
};

struct VelocityComponent
{
  double x, y;
};

struct AccelerationComponent
{
  double x, y;
};
