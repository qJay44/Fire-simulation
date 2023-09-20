#include "Render.h"

Render::Render(ThreadPool* tp, Physics* phys) : tp(tp), physics(phys) {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Fire simulation", sf::Style::Close);
    window.setFramerateLimit(90);

    if (!ImGui::SFML::Init(window))
      throw std::runtime_error("ImGui initialize fail");

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");

    // Main canvas setup
    backgroundTexture.create(WIDTH, HEIGHT);
    backgroundSprite.setTexture(backgroundTexture.getTexture());
    backgroundTexture.display();

    // Load circle texture
    circleTexture.loadFromFile("../../src/res/circle.png");
    circleTexture.generateMipmap();
    circleTexture.setSmooth(true);

    // Shader setup
    shader.loadFromFile("../../src/shaders/circle.frag", sf::Shader::Fragment);
    shader.setUniform("texture", backgroundTexture.getTexture());

    // FPS text setup
    fpsText.setString("75");
    fpsText.setFont(genericFont);
    fpsText.setCharacterSize(20);
    fpsText.setOutlineColor(sf::Color(31, 31, 31));
    fpsText.setOutlineThickness(3.f);
    fpsText.setPosition({ WIDTH - fpsText.getLocalBounds().width, 0 });
}

Render::~Render() {
  tp = nullptr;
  physics = nullptr;
}

void Render::run() {
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);

      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::KeyReleased) {
        switch (event.key.code) {
          case sf::Keyboard::Key::Q:
            window.close();
            break;
          case sf::Keyboard::Key::S:
            useShader = !useShader;
            break;
          case sf::Keyboard::Key::H:
            highlightCircleCell = !highlightCircleCell;
            break;
          case sf::Keyboard::Key::F:
            showFps = !showFps;
            break;
          case sf::Keyboard::Key::T:
            highlightThreadsAreaBoundaries = !highlightThreadsAreaBoundaries;
            break;
          default:
            break;
        }
      }

      // Update mouse coords and spawn circle if LMB pressed (while moving it)
      if (event.type == sf::Event::MouseMoved) {
        mouseCurr = {
          static_cast<float>(sf::Mouse::getPosition(window).x),
          static_cast<float>(sf::Mouse::getPosition(window).y)
        };
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !hoveringImGui())
          physics->addCircle(mousePrev, mouseCurr);
      }

      if (event.type == sf::Event::MouseButtonReleased) {
        // Add circle (while not moving)
        if (event.mouseButton.button == sf::Mouse::Left && !hoveringImGui())
          physics->addCircle(mousePrev, mouseCurr);

        // Leave grabbed circle
        if (event.mouseButton.button == sf::Mouse::Right)
          physics->releaseCircle(sf::Vector2f{mousePrev}, mouseCurr);
      }
    }

    // Grab a circle and drag it
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
      physics->grabCircle(sf::Mouse::getPosition(window));

    sf::Time deltaTime = clock.restart();
    physics->update(deltaTime.asSeconds());

    backgroundTexture.clear();
    window.clear(sf::Color::Black);

    draw(deltaTime.asSeconds());
    drawGUI(deltaTime);

    ImGui::SFML::Render(window);
    window.display();

    mousePrev = mouseCurr;
  }
}

bool Render::hoveringImGui() {
  return ImGui::IsAnyItemHovered() ||
         ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) ||
         ImGui::IsAnyItemActive();
}

void Render::prepareVertices() {
  int size = physics->getCirclesSize();
  va.resize(size * 4);
  float texWidght = circleTexture.getSize().x;
  float texHeight = circleTexture.getSize().y;

  for (int i = 0; i < size; i++) {
    const VerletObject& circle = physics->getCircle(i);
    const sf::Vector2f& pos = circle.getPosition();
    const sf::Color& color = circle.getColor();
    const int ii = i << 2;

    sf::Vertex& topLeft     = va[ii + 0];
    sf::Vertex& topRight    = va[ii + 1];
    sf::Vertex& bottomRight = va[ii + 2];
    sf::Vertex& bottomLeft  = va[ii + 3];

    topLeft.position     = pos + sf::Vector2f{-RADIUS, -RADIUS};
    topRight.position    = pos + sf::Vector2f{ RADIUS, -RADIUS};
    bottomRight.position = pos + sf::Vector2f{ RADIUS,  RADIUS};
    bottomLeft.position  = pos + sf::Vector2f{-RADIUS,  RADIUS};

    topLeft.texCoords     = {0.f      , 0.f      };
    topRight.texCoords    = {texWidght, 0.f      };
    bottomRight.texCoords = {texWidght, texHeight};
    bottomLeft.texCoords  = {0.f      , texHeight};

    topLeft.color     = color;
    topRight.color    = color;
    bottomRight.color = color;
    bottomLeft.color  = color;
  }
}

void Render::draw(float dt) {
  prepareVertices();
  backgroundTexture.draw(va, &circleTexture);
  window.draw(backgroundSprite);

  // Apply bloom shader
  if (useShader) {
    bool isHorizontal = true;
    for (int i = 0; i < config::shader::bloomIntensity; i++) {
      shader.setUniform("isHorizontal", isHorizontal);
      backgroundTexture.draw(backgroundSprite, &shader);
      isHorizontal = !isHorizontal;
    }
    window.draw(backgroundSprite, sf::BlendAdd);
  }

  // Highlight cell edges if there at least one circle
  if (highlightCircleCell) {
    for (int i = 0; i < physics->getGridSize(); i++) {
      window.draw(physics->setCellHighlight(i, sf::Color::Magenta));
    }
  }

  // Draw lines that show areas each thread is using
  if (highlightThreadsAreaBoundaries) {
    for (int i = 0; i < tp->size(); i++) {
      static const int slice = COLUMNS / tp->size();
      sf::RectangleShape rect({slice * CELL_SIZE * 1.f, HEIGHT});
      rect.setPosition(i * slice * CELL_SIZE, 0);
      rect.setFillColor(sf::Color::Transparent);
      rect.setOutlineThickness(1.f);
      rect.setOutlineColor(sf::Color::Magenta);
      window.draw(rect);
    }
  }

  // Show FPS
  if (showFps) {
    int fps = static_cast<int>((1.f / dt));
    fpsText.setString(std::to_string(fps));
    window.draw(fpsText);
  }
}

void Render::drawGUI(sf::Time deltaTime) {
  static bool doOnce = true;
  ImGui::SFML::Update(window, deltaTime);

  if (doOnce) {
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowCollapsed(true);
    doOnce = false;
  }

  ImGui::Begin("Settings");
  ImGui::SliderFloat("Gravity", &config::gravity, 20.f, 1000.f);

  ImGui::Text("Temperature");
  ImGui::SliderFloat("Heat transfer", &config::temperature::heatTransferFactor, 0.001f, 0.1f);
  ImGui::SliderFloat("Heating factor", &config::temperature::heatingFactor, 0.1f, 500.f);
  ImGui::SliderFloat("Cooling factor", &config::temperature::coolingFactor, 0.01f, 1.f);

  ImGui::Text("Upward force");
  ImGui::SliderFloat("Scale", &config::upwardForce::scale, 0.f, 100.f);

  ImGui::Text("Shader");
  ImGui::SliderInt("Bloom intensity", &config::shader::bloomIntensity, 0, 100);

  if (ImGui::Button("Reset"))
    config::reset();

  ImGui::End();
}

