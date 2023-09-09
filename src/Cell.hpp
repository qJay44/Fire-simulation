#include "VerletObject.hpp"
#include <map>
#include <vector>

struct Cell : sf::RectangleShape {
  std::vector<Cell*> neighbours; // Also has itself
  std::vector<VerletObject*> container;

  void checkCollisions() const {
    for (const Cell* cell : neighbours)
      for (VerletObject* circle1 : container)
        for (VerletObject* circle2 : cell->container)
          if (circle1 != circle2)
            circle1->checkCollision(circle2);
  }

  void highlight(sf::Color col) {
    if (container.size() > 0)
      setOutlineColor(col);
    else
      setOutlineColor(sf::Color(20, 20, 20));
  }
};

