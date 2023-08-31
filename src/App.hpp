#include "SFML/Window.hpp"
#include "preferences.h"
#include "quadtree.hpp"
#include <stdlib.h>
#include <vector>

class App {
  sf::RenderWindow window;
  sf::Font genericFont;
  sf::Clock clock;
  sf::Vector2f mouseCurr;
  sf::Vector2f mousePrev;
  sf::Shader shader;
  sf::Text fpsText;

  Rectangle* boundary;
  QuadTree* qt;

  std::vector<VerletObject> circles;
  float dt;

  sf::RenderTexture backgroundTexture;
  sf::Sprite background;

  bool showQT = false;
  bool useShader = true;
  bool showCollisionRectangle = false;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Template text", sf::Style::Close);
    window.setFramerateLimit(75);

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");

    // Main canvas setup
    backgroundTexture.create(WIDTH, HEIGHT);
    background.setTexture(backgroundTexture.getTexture());
    backgroundTexture.display();

    // Shader setup
    shader.loadFromFile("../../src/shaders/circle.frag", sf::Shader::Fragment);
    shader.setUniform("texture", backgroundTexture.getTexture());
    shader.setUniform("blurRadius", BLUR_RADIUS);
    shader.setUniform("blurWeight", BLUR_WEIGHT);
    shader.setUniform("blurDecreaseFactor", BLUR_WEIGHT_DECREASE_FACTOR);

    // FPS text setup
    fpsText.setString("75");
    fpsText.setFont(genericFont);
    fpsText.setCharacterSize(20);
    fpsText.setOutlineColor(sf::Color(31, 31, 31));
    fpsText.setOutlineThickness(3.f);
    fpsText.setPosition({ WIDTH - fpsText.getLocalBounds().width, 0 });
  }

  void setupProgram() {
    srand((unsigned)time(NULL));

    boundary = new Rectangle{WIDTH / 2.f, HEIGHT / 2.f, WIDTH / 2.f, HEIGHT / 2.f};
    qt = new QuadTree(*boundary);

    for (int i = 0; i < 500; i++) {
      sf::Vector2f pos{
        static_cast<float>(random(WIDTH)),
        static_cast<float>(random(HEIGHT))
      };
      addCircle(VerletObject(pos, pos));
    }
  }

  void addCircle(VerletObject vo) {
    circles.push_back(vo);
  }

  void updatePosition() {
    for (VerletObject& vo : circles)
      vo.updatePosition(dt);
  }

  void solveCollisions() {
    delete qt;
    qt = new QuadTree(*boundary);

    // Generate new quad tree
    for (VerletObject& vo : circles)
      qt->insert(vo);

    // Query quad tree for each circle then check collisions
    for (VerletObject& vo1: circles) {
      std::vector<VerletObject*> nearCircles;
      sf::Vector2f pos = vo1.getPosition();

      qt->query(nearCircles, Rectangle{pos.x, pos.y, RADIUS * 2, RADIUS * 2});

      for (VerletObject* vo2 : nearCircles)
        vo1.checkCollision(*vo2);
    }
  }

  void update() {
    updatePosition();
    solveCollisions();
  }

  void draw() {
    // Draw all circles on texture
    for (int i = 0; i < circles.size(); i++)
      backgroundTexture.draw(circles[i]);

    // Draw texture on screen
    if (useShader)
      window.draw(background, &shader);
    else
      window.draw(background);

    // Show quad tree grid
    if (showQT)
      qt->show(window);

    if (showCollisionRectangle) {
      float size = RADIUS * 2 + 20;
      std::vector<VerletObject*> nearCircles;
      qt->query(nearCircles, Rectangle{mouseCurr.x, mouseCurr.y, size, size});

      sf::RectangleShape rect({size * 2, size * 2});
      rect.setPosition(mouseCurr.x - size, mouseCurr.y - size);
      rect.setFillColor(sf::Color::Transparent);
      rect.setOutlineColor(sf::Color::Magenta);
      rect.setOutlineThickness(1.f);

      for (VerletObject* circle : nearCircles) {
        sf::Color origColor = circle->getFillColor();
        circle->setFillColor(sf::Color::Magenta);
        window.draw(*circle);
        circle->setFillColor(origColor);
      }

      window.draw(rect);
    }

    // Show FPS
    int fps = static_cast<int>((1.f / dt));
    fpsText.setString(std::to_string(fps));
    window.draw(fpsText);
  }

  public:
    App() {}

    ~App() {
      delete boundary;
      delete qt;
    }

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

          if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code) {
              case sf::Keyboard::Key::Q:
                window.close();
                break;
              case sf::Keyboard::Key::T:
                showQT = !showQT;
                break;
              case sf::Keyboard::Key::S:
                useShader = !useShader;
                break;
              case sf::Keyboard::Key::C:
                showCollisionRectangle = !showCollisionRectangle;
                break;
              default:
                break;
            }
          }

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

        backgroundTexture.clear();
        window.clear(sf::Color::Transparent);

        draw();

        window.display();
      }
    }
};

