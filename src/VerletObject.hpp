#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "myutils.hpp"
#include "preferences.h"
#include <algorithm>
#include <cmath>

struct VerletObject : sf::CircleShape {

  VerletObject(sf::Vector2f posPrev, sf::Vector2f posCurr) : positionPrevious(posPrev) {
    setRadius(RADIUS);
    setOriginToCenter();
    setPosition(posCurr);
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
    if      (nextPos.x - RADIUS <= 0)     {nextPos.x = RADIUS; onLeftBoundHit();}
    else if (nextPos.x + RADIUS >= WIDTH) {nextPos.x = WIDTH - RADIUS; onRightBoundHit();}

    // Check vertical bounds
    if      (nextPos.y - RADIUS <= 0)      {nextPos.y = RADIUS; onTopBoundHit();}
    else if (nextPos.y + RADIUS >= HEIGHT) {nextPos.y = HEIGHT - RADIUS; onBottomBoundHit(nextPos);}

    setPosition(nextPos);
    acceleration = {};
    temperature -= COOL_VALUE;
  }

  void checkCollision(VerletObject& rhs) {
    if (this == &rhs) return;

    sf::Vector2f pos1 = getPosition();
    sf::Vector2f pos2 = rhs.getPosition();
    float minDist = RADIUS * 2;

    sf::Vector2f collisionAxis = {pos1.x - pos2.x, pos1.y - pos2.y};
    float dist = sqrt(collisionAxis.x * collisionAxis.x + collisionAxis.y * collisionAxis.y);

    if (dist <= minDist) {
      transferTemperature(temperature, rhs.temperature);

      sf::Vector2f n = collisionAxis / dist;
      float delta = minDist - dist;

      setPosition(pos1 + (0.5f * delta * n));
      rhs.setPosition(pos2 - (0.5f * delta * n));
    }
  }

  const float getTemperature() const {
    return temperature;
  }

  private:
    sf::Vector2f positionPrevious;
    sf::Color col;
    float acceleration = 0.f;
    float temperature = 0.f;

    static void transferTemperature(float& t1, float& t2) {
      if (t1 > t2) {
        t1 -= EXCHANGE_VALUE;
        t2 += EXCHANGE_VALUE;
      } else {
        t1 += EXCHANGE_VALUE;
        t2 -= EXCHANGE_VALUE;
      }
    }

    void onTopBoundHit() {}
    void onRightBoundHit() {}
    void onLeftBoundHit() {}

    void onBottomBoundHit(sf::Vector2f& pos) {
      temperature = std::clamp(temperature + 20.f, 0.f, MAX_TEMPERATURE);

      if (random(0.f, 1.f) > 0.99f) {
        pos.y -= (8.f * temperature / MAX_TEMPERATURE);
      }
    }

    // Use it after raduis set
    void setOriginToCenter() {
      sf::FloatRect floatRect = getLocalBounds();
      setOrigin(
        floatRect.left + floatRect.width / 2.f,
        floatRect.top + floatRect.height / 2.f
      );
    }
};

