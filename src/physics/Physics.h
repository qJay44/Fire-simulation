#include "objects/Flame.hpp"
#include "../utils/ThreadPool.h"
#include "myutils.hpp"

class Physics {
  std::vector<Cell> grid;
  std::vector<VerletObject> circles;
  std::vector<Flame> flames;

  ThreadPool* tp = nullptr;
  VerletObject* grabbedCircle = nullptr;

  void threadedSolveCollisions(int x0, int x1);
  void solveCollisions();
  void updatePosition(float dt);
  void updateFlames();

  public:
    Physics(ThreadPool* tp);
    ~Physics();

    void addCircle(sf::Vector2f posPrev, sf::Vector2f posCurr);
    void grabCircle(sf::Vector2f mouse);
    void releaseCircle();
    void update(float dt);

    int getCirclesSize() const;
    const VerletObject& getCircle(int index) const;
    int getGridSize() const;

    const Cell& setCellHighlight(int index, sf::Color color);
};

