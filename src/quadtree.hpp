#include "preferences.h"
#include "VerletObject.hpp"
#include <stdexcept>
#include <vector>

struct Rectangle {
  // NOTE: Coords must point to the center of a rectangle
  float x, y, w, h;

  float top    = y - h;
  float right  = x + w;
  float bottom = y + h;
  float left   = x - w;

  bool contains(const sf::Vector2f& p) const {
    return (
      p.x >= x - w &&
      p.x <= x + w &&
      p.y >= y - h &&
      p.y <= y + h
    );
  }

  bool intersects(const Rectangle& r) const {
    return (
      top    < r.bottom ||
      right  < r.left   ||
      left   > r.right  ||
      bottom > r.top
    );
  }
};

class QuadTree {
  Rectangle boundary;
  std::vector<VerletObject*> circles;
  bool divided = false;

  QuadTree* northWest = nullptr;
  QuadTree* northEast = nullptr;
  QuadTree* southWest = nullptr;
  QuadTree* southEast = nullptr;

  void subdivide() {
    if (divided) return;
    divided = true;

    float& x = boundary.x;
    float& y = boundary.y;
    float& w = boundary.w;
    float& h = boundary.h;

    Rectangle nwRect{x - w / 2, y - h / 2, w / 2, h / 2};
    Rectangle neRect{x + w / 2, y - h / 2, w / 2, h / 2};
    Rectangle swRect{x - w / 2, y + h / 2, w / 2, h / 2};
    Rectangle seRect{x + w / 2, y + h / 2, w / 2, h / 2};

    northWest = new QuadTree(nwRect);
    northEast = new QuadTree(neRect);
    southWest = new QuadTree(swRect);
    southEast = new QuadTree(seRect);
  }

  public:

    QuadTree(Rectangle boundary)
      : boundary(boundary) {
        circles.reserve(QUAD_TREE_POINTS_CAPACITY);
    }

    ~QuadTree() {
      delete northWest;
      delete northEast;
      delete southWest;
      delete southEast;
    }

    bool insert(VerletObject& circle) {
      if (!boundary.contains(circle.getPosition())) return false;

      if (circles.size() < QUAD_TREE_POINTS_CAPACITY) {
        circles.push_back(&circle);
        return true;
      } else {
        subdivide();
        if (
          northWest->insert(circle) ||
          northEast->insert(circle) ||
          southWest->insert(circle) ||
          southEast->insert(circle)
        ) return true;
        else
          throw std::runtime_error("The point somehow didn't inserted in any quad");
      }
    }

    void query(std::vector<VerletObject*>& found, const Rectangle& range) {
      if (boundary.intersects(range)) {
        for (VerletObject* c : circles)
          if (range.contains(c->getPosition()))
            found.push_back(c);
      }

      if (divided) {
        northWest->query(found, range);
        northEast->query(found, range);
        southWest->query(found, range);
        southEast->query(found, range);
      }
    }

    void show(sf::RenderWindow& win) {
      float& x = boundary.x;
      float& y = boundary.y;
      float& w = boundary.w;
      float& h = boundary.h;

      sf::Vertex lineVertical[] = {
        sf::Vertex({x, y - h}),
        sf::Vertex({x, y + h})
      };

      sf::Vertex lineHorizontal[] = {
        sf::Vertex({x - w, y}),
        sf::Vertex({x + w, y})
      };

      win.draw(lineVertical, 2, sf::Lines);
      win.draw(lineHorizontal, 2, sf::Lines);

      if (divided) {
        northWest->show(win);
        northEast->show(win);
        southWest->show(win);
        southEast->show(win);
      }
    }
};

