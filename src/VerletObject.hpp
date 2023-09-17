#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "myutils.hpp"
#include "preferences.h"
#include "utils/config.h"
#include <cmath>

struct VerletObject {
  sf::Vector2f positionPrevious;
  sf::Vector2f position;
  sf::Color color;
  float temperature = 2000.f;
  bool grabbed = false;

  VerletObject(sf::Vector2f prev, sf::Vector2f curr) : positionPrevious(prev), position(curr) {}

  void updatePosition(float dt) {
    if (grabbed) return;
    acceleration += config::gravity;

    sf::Vector2f velocity = position - positionPrevious;
    sf::Vector2f nextPos;

    positionPrevious = position;
    nextPos = position + velocity;
    nextPos.y += acceleration * dt * dt;

    // Check horizontal bounds
    if      (nextPos.x - RADIUS < 0)     {nextPos.x = RADIUS; onLeftBoundHit();}
    else if (nextPos.x + RADIUS > WIDTH) {nextPos.x = WIDTH - RADIUS; onRightBoundHit();}

    // Check vertical bounds
    if      (nextPos.y - RADIUS < 0)      {nextPos.y = RADIUS; onTopBoundHit();}
    else if (nextPos.y + RADIUS > HEIGHT) {nextPos.y = HEIGHT - RADIUS; onBottomBoundHit(nextPos);}

    acceleration = 0.f;
    config::temperature::cool(temperature);

    position = nextPos;
    generateHeatColor();
  }

  void checkCollision(VerletObject* rhs) {
    if (grabbed) return;

    sf::Vector2f v = position - rhs->position;
    float distSquared = v.x * v.x + v.y * v.y;

    if (distSquared < minDistSquared) {
      float dist = sqrt(distSquared);
      dist = std::clamp(dist, RADIUS * 1.f, RADIUS * 2.f);
      float delta = minDist - dist;
      sf::Vector2f n = v / dist;
      sf::Vector2f move = 0.5f * delta * n;

      position += move;
      rhs->position -= move;
      config::temperature::transfer(temperature, rhs->temperature);
    } else if (distSquared == minDistSquared)
      config::temperature::transfer(temperature, rhs->temperature);
  }

  void toss() {
    position.y -= config::upwardForce::calculate(temperature);
  }

  void setGrabPosition(sf::Vector2f pos) {
    position = pos;
    positionPrevious = pos;
  }

  private:
    const float minDist = RADIUS * 2;
    const float minDistSquared = minDist * minDist;
    float acceleration = 0.f;

    void onTopBoundHit() {}
    void onRightBoundHit() {}
    void onLeftBoundHit() {}

    void onBottomBoundHit(sf::Vector2f& pos) {
      config::temperature::heat(temperature);
    }

    void generateHeatColor() {
      float t = temperature / config::temperature::max * 3 * 256;

      color.r = std::clamp(t, 0.f, 255.f);
      color.g = std::clamp(t - color.r, 0.f, 255.f);
      color.b = std::clamp(t - color.r - color.g, 0.f, 255.f);
    }
};

