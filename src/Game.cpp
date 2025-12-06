#include "Game.hpp"
#include <cmath>

Game::Game() {
    // 1. Iniciar Ventana
    window.create(sf::VideoMode(800, 600), "Ben 10: Savage Pursuit - OOP Version");
    window.setFramerateLimit(60);

    // 2. Iniciar Física
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 9.8f};
    worldId = b2CreateWorld(&worldDef);

    // 3. Crear Suelo
    b2BodyDef groundDef = b2DefaultBodyDef();
    groundDef.position = {400.0f, 580.0f};
    groundId = b2CreateBody(worldId, &groundDef);

    b2Polygon groundBox = b2MakeBox(400.0f, 10.0f);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    // Gráfico del suelo
    groundShape.setSize(sf::Vector2f(800.0f, 20.0f));
    groundShape.setOrigin(400.0f, 10.0f);
    groundShape.setFillColor(sf::Color::White);

    // 4. Crear Ben (Caja Verde)
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {400.0f, 0.0f};
    benBodyId = b2CreateBody(worldId, &bodyDef);

    b2Polygon dynamicBox = b2MakeBox(20.0f, 20.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    // shapeDef.friction = 0.3f; // Quitamos esto por ahora

    b2CreatePolygonShape(benBodyId, &shapeDef, &dynamicBox);

    // Gráfico de Ben
    benShape.setSize(sf::Vector2f(40.0f, 40.0f));
    benShape.setOrigin(20.0f, 20.0f);
    benShape.setFillColor(sf::Color::Green);
}

Game::~Game() {
    b2DestroyWorld(worldId);
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void Game::update() {
    // Avanzar la física
    b2World_Step(worldId, 1.0f / 60.0f, 4);

    // Sincronizar Ben (Física -> Gráficos)
    b2Vec2 pos = b2Body_GetPosition(benBodyId);
    b2Rot rot = b2Body_GetRotation(benBodyId);
    float angle = std::atan2(rot.s, rot.c) * 180.0f / 3.14159f;

    benShape.setPosition(pos.x, pos.y);
    benShape.setRotation(angle);

    // Sincronizar Suelo
    b2Vec2 groundPos = b2Body_GetPosition(groundId);
    groundShape.setPosition(groundPos.x, groundPos.y);
}

void Game::render() {
    window.clear(sf::Color::Black);
    window.draw(groundShape);
    window.draw(benShape);
    window.display();
}