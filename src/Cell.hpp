#include "VerletObject.hpp"
#include <map>
#include <vector>

struct Cell {
  std::map<int, Cell*> neighbours;
  std::vector<int> container;

  void addNeighbour(int place, Cell* neighbour) {
    neighbours.insert(std::pair(place, neighbour));
  }

  void checkCollisions(std::map<int, VerletObject>& circles) {
    // Check neighbours (around center)
    for (auto& [_, cell] : neighbours)
      for (int id1 : cell->container)
        for (int id2 : cell->container)
          if (id1 != id2)
            circles.at(id1).checkCollision(circles.at(id2));

    // Check yourself (in center)
    for (int id1 : container)
      for (int id2 : container)
        if (id1 != id2)
          circles.at(id1).checkCollision(circles.at(id2));
  }
};

