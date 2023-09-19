#include "Cell.hpp"

class Flame {
  std::vector<const Cell*> area;
  sf::Clock lifeTime;

  public:
    Flame(const std::vector<Cell>& grid, int x0) {
      for (int x = x0; x < x0 + FLAME_WIDTH; x++)
       for (int y = 0; y < ROWS; y++)
          area.push_back(&grid[IX(x, y)]);
    }

    // Returns elapsed time in seconds
    float execute() {
      for (const Cell* cell : area)
        for (VerletObject* circle : cell->container)
          circle->toss();

      return lifeTime.getElapsedTime().asSeconds();
    }
};

