#include "../../pch.h"
#include "../../utils/config.h"
#include <cmath>

class VerletObject {
  sf::Vector2f positionPrevious;
  sf::Vector2f position;
  sf::Color color;

  float acceleration = 0.f;
  float temperature = 2000.f;
  bool grabbed = false;

  void generateHeatColor() {
    float t = temperature / config::temperature::max * 3 * 256;

    color.r = std::clamp(t, 0.f, 255.f);
    color.g = std::clamp(t - color.r, 0.f, 255.f);
    color.b = std::clamp(t - color.r - color.g, 0.f, 255.f);
  }

  public:
    VerletObject(sf::Vector2f prev, sf::Vector2f curr)
      : positionPrevious(prev), position(curr) {}

    void updatePosition(float dt) {
      if (grabbed) return;
      acceleration += config::gravity;

      sf::Vector2f velocity = position - positionPrevious;
      sf::Vector2f nextPos;

      positionPrevious = position;
      nextPos = position + velocity;
      nextPos.y += acceleration * dt * dt;

      // Check horizontal bounds
      if      (nextPos.x - RADIUS < 0)     {nextPos.x = RADIUS;}
      else if (nextPos.x + RADIUS > WIDTH) {nextPos.x = WIDTH - RADIUS;}

      // Check vertical bounds
      if      (nextPos.y - RADIUS < 0)      {nextPos.y = RADIUS;}
      else if (nextPos.y + RADIUS > HEIGHT) {nextPos.y = HEIGHT - RADIUS; config::temperature::heat(temperature); }

      acceleration = 0.f;
      config::temperature::cool(temperature);

      position = nextPos;
      generateHeatColor();
    }

    void checkContact(VerletObject* rhs) {
      if (grabbed) return;

      constexpr float minDist = RADIUS * 2;
      constexpr float minDistSquared = minDist * minDist;
      constexpr float threshold = 0.0001f;

      sf::Vector2f v = position - rhs->position;
      float distSquared = v.x * v.x + v.y * v.y;

      if (distSquared < minDistSquared && distSquared > threshold) {
        float dist = sqrt(distSquared);
        float delta = 0.5f * (minDist - dist);
        sf::Vector2f move = v / dist * delta;

        position += move;
        rhs->position -= move;

        config::temperature::transfer(temperature, rhs->temperature);
      } else if (distSquared == minDistSquared)
        config::temperature::transfer(temperature, rhs->temperature);
    }

    void toss() {
      position.y -= config::upwardForce::calculate(temperature);
    }

    void setGrabStatus(bool grabbed) {
      grabbed = grabbed;
    }

    void setGrabPosition(sf::Vector2f pos) {
      position = pos;
      positionPrevious = pos;
    }

    const sf::Vector2f& getPosition() const {
      return position;
    }

    const sf::Color& getColor() const {
      return color;
    }
};

