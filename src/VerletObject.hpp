#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "myutils.hpp"
#include "preferences.h"
#include <algorithm>
#include <cmath>

struct VerletObject : sf::CircleShape {

  VerletObject(sf::Vector2f posPrev, sf::Vector2f posCurr) : positionPrevious(posPrev) {
    setRadius(RADIUS);
    setOriginToCenter();
    setPosition(posCurr);
    setFillColor(sf::Color::White);
  }

  void updatePosition(float dt) {
    acceleration += GRAVITY;

    sf::Vector2f currPos = getPosition();
    sf::Vector2f velocity = currPos - positionPrevious;
    sf::Vector2f nextPos;

    positionPrevious = currPos;
    nextPos = currPos + velocity;
    nextPos.y += acceleration * dt * dt;

    // Check horizontal bounds
    if      (nextPos.x - RADIUS <= 0)     nextPos.x = RADIUS;
    else if (nextPos.x + RADIUS >= WIDTH) nextPos.x = WIDTH - RADIUS;

    // Check vertical bounds
    if      (nextPos.y - RADIUS <= 0)      nextPos.y = RADIUS;
    else if (nextPos.y + RADIUS >= HEIGHT) nextPos.y = HEIGHT - RADIUS;

    setPosition(nextPos);
    acceleration = {};
  }

  void checkCollision(VerletObject& rhs) {
    if (this == &rhs) return;

    sf::Vector2f pos1 = getPosition();
    sf::Vector2f pos2 = rhs.getPosition();
    float minDist = RADIUS * 2;

    sf::Vector2f collisionAxis = {pos1.x - pos2.x, pos1.y - pos2.y};
    float dist = sqrt(collisionAxis.x * collisionAxis.x + collisionAxis.y * collisionAxis.y);

    if (dist < minDist) {
      sf::Vector2f n = collisionAxis / dist;
      float delta = minDist - dist;

      setPosition(pos1 + (0.5f * delta * n));
      rhs.setPosition(pos2 - (0.5f * delta * n));
    }
  }

  private:
    sf::Vector2f positionPrevious;
    float acceleration = 0.f;

    // Use it after raduis set
    void setOriginToCenter() {
      sf::FloatRect floatRect = getLocalBounds();
      setOrigin(
        floatRect.left + floatRect.width / 2.f,
        floatRect.top + floatRect.height / 2.f
      );
    }
};

