#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

class Game {
public:
//Constructor y Destructor
    Game();
    ~Game();

    void run();

private:
//Funciones internas
    void processEvents();
    void update();
    void render();

    sf::RenderWindow window;
    b2WorldId worldId;

    b2BodyId benBodyId;
    sf::Texture benTexture; // La imagen en memoria
    sf::Sprite benSprite;   // El dibujo en pantalla
    
    //Suelo
    b2BodyId groundId;
    sf::RectangleShape groundShape;
};
