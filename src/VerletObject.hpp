#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "myutils.hpp"
#include "preferences.h"
#include "utils/config.h"
#include <cmath>

struct VerletObject : sf::CircleShape {
  bool grabbed = false;

  VerletObject(sf::Vector2f posPrev, sf::Vector2f posCurr) : positionPrevious(posPrev) {
    setRadius(RADIUS);
    setOriginToCenter();
    setPosition(posCurr);
  }

  void updatePosition(float dt) {
    if (grabbed) return;
    acceleration += config::gravity;

    sf::Vector2f currPos = getPosition();
    sf::Vector2f velocity = currPos - positionPrevious;
    sf::Vector2f nextPos;

    positionPrevious = currPos;
    nextPos = currPos + velocity;
    nextPos.y += acceleration * dt * dt;
    nextPos.y -= upwardForce();

    // Check horizontal bounds
    if      (nextPos.x - RADIUS < 0)     {nextPos.x = RADIUS; onLeftBoundHit();}
    else if (nextPos.x + RADIUS > WIDTH) {nextPos.x = WIDTH - RADIUS; onRightBoundHit();}

    // Check vertical bounds
    if      (nextPos.y - RADIUS < 0)      {nextPos.y = RADIUS; onTopBoundHit();}
    else if (nextPos.y + RADIUS > HEIGHT) {nextPos.y = HEIGHT - RADIUS; onBottomBoundHit(nextPos);}

    temperature *= config::cooling;
    acceleration = {};

    setPosition(nextPos);
    setFillColor(generateHeatColor());
  }

  void checkCollision(VerletObject* rhs) {
    if (grabbed) return;

    sf::Vector2f pos1 = getPosition();
    sf::Vector2f pos2 = rhs->getPosition();
    float minDist = RADIUS * 2;

    sf::Vector2f v = pos1 - pos2;
    float distSquared = v.x * v.x + v.y * v.y;
    float minDistSquared = minDist * minDist;

    if (distSquared < minDistSquared) {
      float dist = sqrt(distSquared);
      dist = std::clamp(dist, 0.1f, RADIUS * 2.f);
      float delta = minDist - dist;
      sf::Vector2f n = v / dist;
      sf::Vector2f move = 0.5f * delta * n;

      setPosition(pos1 + move);
      rhs->setPosition(pos2 - move);
      transferTemperature(temperature, rhs->temperature);

    } else if (distSquared == minDistSquared)
      transferTemperature(temperature, rhs->temperature);
  }

  void setGrabPosition(sf::Vector2f pos) {
    setPosition(pos);
    positionPrevious = pos;
  }

  const float getTemperature() const {
    return temperature;
  }

  private:
    sf::Vector2f positionPrevious;
    float acceleration = 0.f;
    float temperature = 10000.f;

    static void transferTemperature(float& t1, float& t2) {
      if (t1 > t2) {
        t2 += t1 * config::heatTransferFactor;
        t1 -= t1 * config::heatTransferFactor;
      } else {
        t1 += t2 * config::heatTransferFactor;
        t2 -= t2 * config::heatTransferFactor;
      }
    }

    inline float upwardForce()  {
      return pow(100, temperature / MAX_TEMPERATURE + 1.f) / HEIGHT * 0.003f;
    }

    void onTopBoundHit() {}
    void onRightBoundHit() {}
    void onLeftBoundHit() {}

    void onBottomBoundHit(sf::Vector2f& pos) {
      temperature = std::clamp(temperature * config::heating, 0.f, MAX_TEMPERATURE);
      temperature *= config::heating - temperature / MAX_TEMPERATURE;
    }

    // Use it after raduis set
    void setOriginToCenter() {
      sf::FloatRect floatRect = getLocalBounds();
      setOrigin(
        floatRect.left + floatRect.width / 2.f,
        floatRect.top + floatRect.height / 2.f
      );
    }

    sf::Color generateHeatColor() {
      sf::Color col;
      float t = temperature / MAX_TEMPERATURE * 3 * 256;

      col.r = std::clamp(t, 0.f, 255.f);
      col.g = std::clamp(t - col.r, 0.f, 255.f);
      col.b = std::clamp(t - col.r - col.g, 0.f, 255.f);

      return col;
    }
};

