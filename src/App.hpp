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
  sf::Shader circleShader;

  std::vector<VerletObject> circles;
  sf::RectangleShape dummyRect;
  float dt;

  sf::Texture backgroundTexture;
  sf::Sprite background;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Template text", sf::Style::Close);
    window.setFramerateLimit(75);

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");

    circleShader.loadFromFile("../../src/shaders/circle.frag", sf::Shader::Fragment);
    circleShader.setUniform("resolution", sf::Glsl::Vec2{WIDTH, HEIGHT});
    circleShader.setUniform("radius", RADIUS);

    backgroundTexture.create(WIDTH, HEIGHT);
    background.setTexture(backgroundTexture);

    dummyRect.setSize({WIDTH, HEIGHT});
    dummyRect.setFillColor(sf::Color::Transparent);
  }

  void setupProgram() {
    srand((unsigned)time(NULL));

    sf::Vector2f pos{WIDTH / 2.f, HEIGHT / 2.f};
    addCircle(VerletObject(pos, pos));
  }

  void addCircle(VerletObject vo) {
    circles.push_back(vo);
  }

  void updatePosition() {
    for (VerletObject& vo : circles)
      vo.updatePosition(dt);
  }

  void solveCollisions() {
    for (int i = 0; i < circles.size(); i++) {
      VerletObject& vo1 = circles[i];
      for (int j = i + 1; j < circles.size(); j++) {
        vo1.checkCollision(circles[j]);
      }
    }
  }

  void update() {
    solveCollisions();
    updatePosition();
  }

  void draw() {
    for (int i = 0; i < circles.size(); i++) {
      const VerletObject& circle = circles[i];
      circleShader.setUniform("circle", circle.getPosition());
      circleShader.setUniform("temperature", circle.getTemperature());
      window.draw(dummyRect, &circleShader);
    }
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

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
              addCircle(VerletObject(mousePrev, mouseCurr));

            mousePrev = mouseCurr;
          }

          if (event.type == sf::Event::MouseButtonReleased)
            if (event.mouseButton.button == sf::Mouse::Left)
              addCircle(VerletObject(mousePrev, mouseCurr));
        }

        dt = clock.restart().asSeconds();
        update();

        window.clear(sf::Color::Transparent);

        draw();

        window.display();
      }
    }
};

