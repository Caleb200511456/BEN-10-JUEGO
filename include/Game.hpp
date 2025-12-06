#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

class Game {
public:
    //Constructor y Destructor
    Game();
    ~Game();

    //Función principal para arrancar el juego
    void run();

private:
//Funciones internas
    void processEvents();
    void update();
    void render();

//variables del Motor
sf::RenderWindow window;
b2WorldId worldId;

//Variables del Juego por ahora solo BEN
b2BodyId benBodyId;
sf::RectangleShape benShape;

//Suelo
b2BodyId groundId;
sf::RectangleShape groundShape;
};
