#include "Game.hpp"
#include <cmath>
#include <iostream> // Para imprimir errores si la imagen no carga

Game::Game() {
    // 1. Iniciar Ventana
    window.create(sf::VideoMode(800, 600), "Ben 10: Savage Pursuit - Sprite Version");
    window.setFramerateLimit(60);

    // 2. Iniciar Física (Box2D v3)
    b2WorldDef worldDef = b2DefaultWorldDef();
    // Gravedad
    worldDef.gravity = {0.0f, 500.0f}; 
    worldId = b2CreateWorld(&worldDef);

    // 3. Diseñamos el nivel
    //3.1. El suelo Principal (Que sea Grande y este por debajo)
    createPlatform(400.0f, 580.0f, 800.0f, 40.0f);

    //3.2.Plataformas flotantes(Para hacer PARKOUUUR :D)
    createPlatform(600.0f, 450.0f, 200.0f, 20.0f); //Derecha baja
    createPlatform(200.0f, 350.0f, 200.0f, 20.0f); //Izquierda al medio
    createPlatform(500.0f, 200.0f, 150.0f, 20.0f); //Arriba centro

    //3.3.Muro alto a la izquierda para que no se salga el ben
    //createPlatform(-10.0f, 300.0f, 20.0f, 600.0f);
    //3.4.Un muro a la derecha
    //createPlatform(810.0f, 300.0f, 20.0f, 600.0f);
    

    // 4. Crear Cuerpo Físico de Ben (La caja invisible que choca)
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {400.0f, -100.0f}; //Aseguramos que inicie en el aire y que caiga a cierta altura
    benBodyId = b2CreateBody(worldId, &bodyDef);
    b2Body_SetFixedRotation(benBodyId, true);

    b2Polygon dynamicBox = b2MakeBox(30.0f, 60.0f); // Tamaño ajustado al cuerpo de Ben
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    b2CreatePolygonShape(benBodyId, &shapeDef, &dynamicBox);

    // --- 5. CONFIGURAR GRÁFICOS DE BEN 10 (EL SPRITE) ---
    
    // A. Cargar la imagen
    sf::Image benImage;
    if (!benImage.loadFromFile("assets/images/ben10.png")) {
        std::cerr << "ERROR cargarndo imagen ben10.png" << std::endl;
    }
    //Especificar que lo que sea magenta lo ponga transparente
    benImage.createMaskFromColor(sf::Color::Magenta);
    //Cargamos la textura de la imagen ya actualizada papa
    benTexture.loadFromImage(benImage);

    benSprite.setTexture(benTexture);

    // (X,Y,Ancho,Alto) de la imagen
    benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84)); 

    // C. Centrar (El Pivote)
    // 51 entre 2 = 25.5
    // 84 entre 2 = 42
    benSprite.setOrigin(25.5f, 42.0f); 

    // D. Escalar (Zoom)
    // Lo hacemos 3 veces más grande para verlo bien.
    benSprite.setScale(1.5f, 1.5f);
    animationTimer = 0.0f;
    currentFrame = 0; 
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
        
        if(event.type == sf::Event::KeyPressed){
            if(event.key.code == sf::Keyboard::Space){
                //Obtenemos aqui la velocidad solo para revisar si esta ben en el suelo
                b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);

                //Si la velocidad vertical es baja (esta en el suelo o casi)
                if (std::abs(velocity.y)<0.5){
                    velocity.y = -900.0f; //Impulso del salto
                    b2Body_SetLinearVelocity(benBodyId, velocity);
                }
            }
        }
    }
}

void Game::update() {
  //1.Obtener la velocidad física de ben 
  b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);
  bool isMoving = false; //Aplicamos bandera para saber si corremos

  //2.Controles
  //Vamo a la derecha
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
    velocity.x = 200.0f; //Asi aplicamos que se mueva la derecha en el eje x
    benSprite.setScale(1.5f, 1.5f);
    isMoving = true; //Aqui aplica que ben se esta moviendo
  }
  //Vamo a la Izquierda
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
    velocity.x = -200.0f;//Aplicamos que se mueva a la izquierda en el eje x
    benSprite.setScale(-1.5f, 1.5f);
    isMoving = true; // Se esta moviendo a la izquierda :D
  }
  //QUIETOO
  else{
    velocity.x = 0.0f;
    isMoving = false;
  }
  //3.Sistema de Animación
  if(isMoving){
    //3.1.Avanzar el cronómetro(Se suma el tiempo que paso, aproximado de 1/60seg)
    animationTimer += 1.0f / 60.0f;

    //3.2.Aplicar el cambio de dibujo
    if(animationTimer >= 0.1f){
        animationTimer = 0.0f; //Reiniciamos el cronometro
        currentFrame++; //Siguiente dibujo
    
        //Si llegamos al dibujo 6, regresar al primer dibujo(Loop)
        if(currentFrame >= 6){
            currentFrame = 0;
        }
    }
    //3.3.Calcular el recorte de correr obtenidos en la imagen
    // Lista exacta de tus coordenadas X:
        int walkingFramesX[] = {17, 78, 137, 181, 238, 295};

        // Usamos el 'currentFrame' para elegir el número de la lista
        int frameX = walkingFramesX[currentFrame]; 

        // Actualizamos el recorte usando la X exacta de la lista
        // (Y=382, Ancho=47, Alto=79 según tus medidas anteriores)
        benSprite.setTextureRect(sf::IntRect(frameX, 382, 46.5, 79));

        // Ajustamos el origen para el ancho de 47
        benSprite.setOrigin(23.5f, 39.5f);

  }
  else{
    //3.4.Si esta quieto volver al dibujo original 
    //Datos del dibujo original: X=8, Y=23, Alto= 51, Largo=84 aprox
    currentFrame = 0; //Resetear para que al correr empiece en el dibujo 1
    benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
    benSprite.setOrigin(25.5f, 42.0f);
  }
  //4.Aplicar las fisicas y sincronizamos
  b2Body_SetLinearVelocity(benBodyId, velocity);
  b2World_Step(worldId, 1.0f / 60.0f, 4);

  b2Vec2 pos = b2Body_GetPosition(benBodyId);
  benSprite.setPosition(pos.x,pos.y);

  //Hacemos la zona de Muerte GAME OVER
  //Si ben cae muy abajo (Y mayor a 800 pixeles)
 if (pos.y > 800.0f) {
        // 1. Lo regresamos al inicio (400, -100)
        // El ángulo lo dejamos en 0 (b2Rot_Identity si usas v3, o simplemente 0.0f en la función SetTransform)
        b2Body_SetTransform(benBodyId, {400.0f, -100.0f}, {1.0f, 0.0});//1.0 es Seno y 0.0 es Coseno asi se movera Ben
        
        // 2. Le quitamos la velocidad para que no siga cayendo a toda pastilla
        b2Body_SetLinearVelocity(benBodyId, {0.0f, 0.0f});
    }
}

void Game::render(){
    window.clear(sf::Color::Black); // Limpiar pantalla

    // Obtenemos la posición de Ben
    sf::Vector2f benPos = benSprite.getPosition();

    // Creamos una "Cámara" del tamaño de nuestra ventana (800x600)
    sf::View camera(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f, 600.0f));

    // Le decimos a la cámara: "Mira a Ben"
    // Por ahora, que lo siga en todo:
    camera.setCenter(benPos);
    // Aplicamos la cámara a la ventana
    window.setView(camera);

    // Dibujar plataformas
    for (const auto& shape : platformShapes) {
        window.draw(shape);
    }

    // Dibujar a Ben
    window.draw(benSprite);
    window.display();
}

//Aplicacion de las plataformas
void Game::createPlatform(float x, float y, float width, float height){
//1.Fisicas (Box2D)
b2BodyDef bodyDef = b2DefaultBodyDef();
bodyDef.position = {x, y}; //Box2D aplicamos para usar el centro
b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

//Box2D pide la mitad del ancho y alto
b2Polygon box = b2MakeBox(width / 2.0f, height / 2.0f);
b2ShapeDef shapeDef = b2DefaultShapeDef();
b2CreatePolygonShape(bodyId, &shapeDef, &box);

//2.SFML (Para aplicar los graficos)
sf::RectangleShape shape(sf::Vector2f(width, height));
shape.setOrigin(width / 2.0f, height / 2.0); //Origen al centro igual que Box2D
shape.setPosition(x, y);

//Aplicamos Color 
shape.setFillColor(sf::Color(0, 180, 0)); //Verde Oscuro
shape.setOutlineThickness(2.0f);
shape.setOutlineColor(sf::Color::White);

//3.Guardamos en la lista para despues dibujarlo
platformShapes.push_back(shape);
}