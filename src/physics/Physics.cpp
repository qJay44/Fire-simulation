#include "Physics.h"

Physics::Physics(ThreadPool* tp) : tp(tp) {
  grid.resize(COLUMNS * ROWS);
  grid.reserve(COLUMNS * ROWS);
  flames.reserve(FLAME_COUNT);

  // Setup all cells with its neighbours
  for (int x = 0; x < COLUMNS; x++) {
    for (int y = 0; y < ROWS; y++) {
      Cell& cell = grid[IX(x, y)];
      cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
      cell.setSize({CELL_SIZE, CELL_SIZE});
      cell.setFillColor(sf::Color::Transparent);
      cell.setOutlineThickness(1.f);

      std::vector<Cell*>& cellNeighbours = cell.neighbours;
      cellNeighbours.reserve(9);

      // Center
      cellNeighbours.push_back(&cell);

      // Left cell
      if (x > 0) {
        cellNeighbours.push_back(&grid[IX(x - 1, y)]);

        // Up-left cell
        if (y > 0)
          cellNeighbours.push_back(&grid[IX(x - 1, y - 1)]);

        // Down-left cell
        if (y < ROWS - 1)
          cellNeighbours.push_back(&grid[IX(x - 1, y + 1)]);
      }

      // Right cell
      if (x < COLUMNS - 1) {
        cellNeighbours.push_back(&grid[IX(x + 1, y)]);

        // Up-right cell
        if (y > 0)
          cellNeighbours.push_back(&grid[IX(x + 1, y - 1)]);

        // Down-right cell
        if (y < ROWS - 1)
          cellNeighbours.push_back(&grid[IX(x + 1, y + 1)]);
      }

      // Up cell
      if (y > 0)
        cellNeighbours.push_back(&grid[IX(x, y - 1)]);

      // Down cell
      if (y < ROWS - 1)
        cellNeighbours.push_back(&grid[IX(x, y + 1)]);
    }
  }

  // Spawn initial circles
  for (int i = 0; i < INITIAL_CIRCLES; i++) {
    sf::Vector2f pos{
      static_cast<float>(random(WIDTH)),
      static_cast<float>(random(HEIGHT))
    };

    addCircle(pos, pos);
  }
}

Physics::~Physics() {
  tp = nullptr;
  grabbedCircle = nullptr;
}

void Physics::addCircle(sf::Vector2f posPrev, sf::Vector2f posCurr) {
  circles.push_back(VerletObject(posPrev, posCurr));
}

void Physics::grabCircle(sf::Vector2f mouse) {
  if (!grabbedCircle) {
    int x = mouse.x;
    int y = mouse.y;
    std::vector<VerletObject*>& cont = grid[IX(x, y)].container;

    if (cont.size() > 0) {
      grabbedCircle = cont[0];
      grabbedCircle->setGrabStatus(true);
    }
  } else
    grabbedCircle->setGrabPosition(mouse);
}

void Physics::releaseCircle() {
  if (grabbedCircle) {
    grabbedCircle->setGrabStatus(false);
    grabbedCircle = nullptr;
  }
}

void Physics::threadedSolveCollisions(int x0, int x1) {
  // Check collisions for all circles in cells
  for (int x = x0; x < x1; x++)
    for (int y = 0; y < ROWS; y++)
      grid[IX(x, y)].checkCollisions();
}

void Physics::solveCollisions() {
  // Clear all cells containers;
  for (Cell& cell : grid)
    cell.container.clear();

  // Fill all cells with new circles
  for (VerletObject& circle : circles) {
    sf::Vector2f pos = circle.getPosition();
    int x = pos.x / CELL_SIZE;
    int y = pos.y / CELL_SIZE;

    grid[IX(x, y)].container.push_back(&circle);
  }

  // Queue collision detection function in the thread loop
  for (int i = 0; i < tp->size(); i++) {
    static const int slice = COLUMNS / tp->size();
    int x0 = i * slice;
    int x1 = x0 + slice;

    tp->queueJob([this, x0, x1] { threadedSolveCollisions(x0, x1); });
  }
  tp->waitForCompletion();

}

void Physics::updatePosition(float dt) {
  for (VerletObject& circle : circles)
    circle.updatePosition(dt);
}

void Physics::updateFlames() {
  // Create new flames if possible
  for (int i = flames.size(); i < FLAME_COUNT; i++) {
    int flameStart = random(COLUMNS - FLAME_WIDTH);
    flames.push_back(Flame(grid, flameStart));
  }

  // Execute flame job
  for (int i = 0; i < flames.size(); i++)
    if (flames[i].execute() > FLAME_MAX_LIFETIME)
      flames.erase(flames.begin() + i);
}

void Physics::update(float dt) {
  float subDt = dt / static_cast<float>(SUB_STEPS);
  for (int i = SUB_STEPS; i--;) {
    solveCollisions();
    updatePosition(subDt);
    updateFlames();
  }
}

int Physics::getCirclesSize() const {
  return circles.size();
}

const VerletObject& Physics::getCircle(int index) const {
  return circles[index];
}

int Physics::getGridSize() const {
  return grid.size();
}

const Cell& Physics::setCellHighlight(int index, sf::Color color) {
  Cell& cell = grid[index];
  cell.highlight(color);

  return cell;
}

