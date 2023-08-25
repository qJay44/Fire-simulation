#include "SFML/Window.hpp"
#include "VerletObject.hpp"
#include "preferences.h"
#include <stdlib.h>
#include <vector>

class App {
  sf::RenderWindow window;
  sf::Font genericFont;
  sf::Clock clock;
  sf::Vector2f mouseCurr;
  sf::Vector2f mousePrev;

  sf::Vector2f gravity{0.f, 300.f};
  std::vector<VerletObject> verletObjects;
  float radius = 30.f;
  float dt;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Template text", sf::Style::Close);
    window.setFramerateLimit(75);

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");
  }

  void setupProgram() {
    srand((unsigned)time(NULL));

    VerletObject verletObject({WIDTH / 2.f, HEIGHT / 2.f});
    verletObject.setRadius(radius);
    verletObject.setOriginToCenter();
    verletObject.setPosition(WIDTH / 2.f, HEIGHT / 2.f);
    verletObject.setFillColor(sf::Color::White);

    verletObjects.push_back(verletObject);
  }

  void applyGravity() {
    for (VerletObject& vo : verletObjects)
      vo.accelerate(gravity);
  }

  void updatePosition() {
    for (VerletObject& vo : verletObjects)
      vo.updatePosition(dt);
  }

  void solveCollisions() {
    for (VerletObject& vo1 : verletObjects)
      for (VerletObject& vo2 : verletObjects)
        vo1.checkCollision(vo2);
    /* for (int i = 0; i < verletObjects.size(); i++) { */
    /*   VerletObject& vo1 = verletObjects[i]; */
    /*   for (VerletObject& vo2 : verletObjects) */
    /*     vo1.checkCollision(vo2); */
    /* } */
  }

  void update() {
    int subSteps = 2;
    float subDt = dt / static_cast<float>(subSteps);
    for (int i = 0; i < subSteps; i++) {
      applyGravity();
      solveCollisions();
      updatePosition();
    }
  }

  void draw() {
    for (const VerletObject& vo : verletObjects)
      window.draw(vo);
  }

  public:
    App() {}

    ~App() {}

    void setup() {
      setupSFML();
      setupProgram();
    }

    void run() {
      while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
          if (event.type == sf::Event::Closed)
            window.close();

          if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::Q)
            window.close();

          if (event.type == sf::Event::MouseMoved) {
            mouseCurr = {
              static_cast<float>(sf::Mouse::getPosition(window).x),
              static_cast<float>(sf::Mouse::getPosition(window).y)
            };

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
              VerletObject newCircle(mousePrev);
              newCircle.setRadius(radius);
              newCircle.setOriginToCenter();
              newCircle.setPosition(mouseCurr);
              newCircle.setFillColor(sf::Color::White);

              verletObjects.push_back(newCircle);
            }

            mousePrev = mouseCurr;
          }
          if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::F) {
              VerletObject newCircle(mousePrev);
              newCircle.setRadius(radius);
              newCircle.setOriginToCenter();
              newCircle.setPosition(mouseCurr);
              newCircle.setFillColor(sf::Color::White);

              verletObjects.push_back(newCircle);
          }
        }

        dt = clock.restart().asSeconds();
        update();

        window.clear();

        draw();

        window.display();
      }
    }
};

