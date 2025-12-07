#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector> 

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

    void createPlatform(float x, float y, float width, float height);

    sf::RenderWindow window;
    b2WorldId worldId;
    
    //BEN 10
    b2BodyId benBodyId;
    sf::Texture benTexture; // La imagen en memoria
    sf::Sprite benSprite;   // El dibujo en pantalla 

    //Animación
    float animationTimer; //Cronometro para cambiar el dibujo
    int currentFrame; //En que dibujo vamos del 1 al 6 
    
    //Suelo
    std::vector<sf::RectangleShape> platformShapes; //Para aplicar dibujos para el escenario
};
