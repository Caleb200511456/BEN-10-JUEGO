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
    // 3.1. El suelo Principal (Que sea Grande y este por debajo)
    createPlatform(2500.0f, 580.0f, 5000.0f, 40.0f);

    // 3.2. Plataformas flotantes (Para hacer PARKOUUUR :D)
    createPlatform(600.0f, 450.0f, 200.0f, 20.0f); // Derecha baja
    createPlatform(200.0f, 350.0f, 200.0f, 20.0f); // Izquierda al medio
    createPlatform(500.0f, 200.0f, 150.0f, 20.0f); // Arriba centro
    //Otra plataforma
    createPlatform(2500.0f, 400.0f, 300.0f, 20.0f);
    createPlatform(1200.0f, 400.0f, 200.0f, 20.0f); // Kilómetro 1
    createPlatform(2000.0f, 300.0f, 200.0f, 20.0f); // Kilómetro 2
    createPlatform(3000.0f, 450.0f, 200.0f, 20.0f); // Kilómetro 3
    createPlatform(4000.0f, 350.0f, 200.0f, 20.0f); // Kilómetro 4

    // 4. Crear Cuerpo Físico de Ben (La caja invisible que choca)
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {100.0f, 450.0f}; // Aseguramos que inicie en el aire y que caiga a cierta altura
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
        std::cerr << "ERROR cargando imagen ben10.png" << std::endl;
    }
    // Especificar que lo que sea magenta lo ponga transparente
    benImage.createMaskFromColor(sf::Color::Magenta);
    // Cargamos la textura de la imagen ya actualizada papa
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

    // 5.1. Cargar Graficos de FUEGO
    // 5.1.1. Cargar la imagen de fuego 
    sf::Image heatblastImage;
    if(!heatblastImage.loadFromFile("assets/images/Fuego.png")){
        std::cerr << "ERROR cargando imagen Fuego.png" << std::endl;
    }
    // 5.1.2. Quitar el fondo Magenta 
    heatblastImage.createMaskFromColor(sf::Color(255, 0, 255));
    // 5.1.3. Pasar la imagen limpia a la textura
    heatblastTexture.loadFromImage(heatblastImage);
    // 5.1.4. Inicializar estado (Empezar sin transformacion)
    isHeatblast = false;
    
    // 6. Crear el enemigo (Dron)
    b2BodyDef enemyDef = b2DefaultBodyDef();
    enemyDef.type = b2_kinematicBody; // Hacer que se mueva pero nada lo empuja
    enemyDef.position = {600.0f, 400.0f};
    // Bloqueamos rotación para que no ruede
    enemyDef.fixedRotation = true;
    enemyBodyId = b2CreateBody(worldId, &enemyDef);

    // Cramos forma fisica de la caja
    b2Polygon enemyBox = b2MakeBox(15.0f, 15.0f); 
    b2ShapeDef enemyShapeDef = b2DefaultShapeDef();
    enemyShapeDef.density = 1.0f;
    b2CreatePolygonShape(enemyBodyId, &enemyShapeDef, &enemyBox);

    // Forma Gráfica (Cubito rojo)
    enemyShape.setSize(sf::Vector2f(30.0f, 30.0f));
    enemyShape.setOrigin(15.0f, 15.0f);
    enemyShape.setFillColor(sf::Color::Red); 

    // Velocidad Inicial
    enemySpeed = 100.0f; // Se mueva a la derecha

    // 7. Crear la META (TARJETA SUMO)
    goalShape.setRadius(20.0f); // Tamaño de la esfera
    goalShape.setFillColor(sf::Color::Yellow); // Amarillo para la tarjeta de Sumo
    goalShape.setOutlineThickness(2.0f);
    goalShape.setOutlineColor(sf::Color::White);

    // Centrar el punto de origen
    goalShape.setOrigin(20.0f, 20.0f);

    // Posicion: En la plataforma floatante del centro-arriba
    goalShape.setPosition(4800.0f,500.0f);

    shootCooldown = 0.0f; // Listo para disparar

    // CONFIGURAR VIDA
    maxHealth = 100;
    currentHealth = 100; // Empezamos con toda la vida
    damageTimer = 0.0f; // Empezamos con vulnerabilidad
    
    // 1. Fondo de la barra (Gris oscuro)
    healthBarBack.setSize(sf::Vector2f(200.0f, 20.0f)); // 200 de largo
    healthBarBack.setFillColor(sf::Color(50, 50, 50)); // Gris
    healthBarBack.setPosition(20.0f, 20.0f); // Esquina arriba izquierda
    
    // 2. Barra de Vida (Verde Omnitrix)
    healthBar.setSize(sf::Vector2f(200.0f, 20.0f));
    healthBar.setFillColor(sf::Color(0, 255, 0)); // Verde
    healthBar.setPosition(20.0f, 20.0f); // En el mismo lugar

    // ------------------------------------
    // --- CONFIGURAR ENERGÍA (NUEVO) ---
    // ------------------------------------
    maxEnergy = 100.0f; 
    currentEnergy = 100.0f; // Bateria llena al inicio
    
    // 1. Fondo de la barra de energía
    energyBarBack.setSize(sf::Vector2f(200.0f, 10.0f)); // Un poco mas delgada
    energyBarBack.setFillColor(sf::Color(50, 50, 50));
    energyBarBack.setPosition(20.0f, 45.0f); // Debajo de la vida
    
    // 2. Barra Amarilla
    energyBar.setSize(sf::Vector2f(200.0f, 10.0f));
    energyBar.setFillColor(sf::Color::Yellow); // BARRA AMARILLA
    energyBar.setPosition(20.0f, 45.0f);
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
                // Obtenemos aqui la velocidad solo para revisar si esta ben en el suelo
                b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);

                // Si la velocidad vertical es baja (esta en el suelo o casi)
                if (std::abs(velocity.y)<0.5){
                    velocity.y = -900.0f; // Impulso del salto
                    b2Body_SetLinearVelocity(benBodyId, velocity);
                }
            }

            // Transformacion (Con bloqueo de energía)
            if(event.key.code == sf::Keyboard::Z){
                
                // NUEVO: Si quiero transformarme PERO no tengo pila
                if (!isHeatblast && currentEnergy < 10.0f) {
                    std::cout << "¡Sin energía! Espera a que recargue." << std::endl;
                }
                else {
                    // Cambiamos el estado (Si era falso, ahora verdadero y viceversa)
                    isHeatblast = !isHeatblast;
                    
                    if(isHeatblast){
                        std::cout << "¡FUEGO!" << std::endl;
                        // 1. Cambiar la imagen a la de fuego
                        benSprite.setTexture(heatblastTexture);
                        // 2. Usar coordenadas de la imagen de fuego (X=9, Y=11, Ancho=35, Alto=53)
                        benSprite.setTextureRect(sf::IntRect(9, 11, 35, 53));
                        // 3. Ajustar el centro (Mitad de 35 y 53)
                        benSprite.setScale(2.2f, 2.2f);
                        benSprite.setOrigin(17.5f, 26.5f);
                    }else{
                        // Volver a Ben
                        std::cout << "Destransformado..." << std::endl;
                        // 1. Volver a la imagen de Ben
                        benSprite.setTexture(benTexture);
                        // 2. Coordenadas originales de Ben quieto
                        benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
                        // 3. Centro original de Ben
                        benSprite.setOrigin(25.5f, 42.0f);
                        // Regresar la escala de Ben a la normal
                        benSprite.setScale(1.5f, 1.5f);
                    }
                }
            }
            
            // DISPARO
            if(event.key.code == sf::Keyboard::X && isHeatblast && shootCooldown <= 0.0f){
                Projectile newProj;
                
                // 1. Apariencia
                newProj.shape.setRadius(10.0f);
                newProj.shape.setFillColor(sf::Color(255, 165, 0)); // Naranja de fuego
                newProj.shape.setOrigin(10.0f, 10.0f); // Ajuste de centro
                
                // 2. Posicion de salida (Desde el centro de fuego)
                sf::Vector2f benPos = benSprite.getPosition();
                // Ajustamos un poco la altura para que salga de las manos/pecho
                newProj.shape.setPosition(benPos.x, benPos.y - 15.0f);
                
                // 3. Direccion
                if(benSprite.getScale().x > 0){
                    newProj.speed = 600.0f; // Disparo a la derecha
                }else{
                    newProj.speed = -600.0f; // Disparo a la izquiersa
                }
                newProj.destroy = false;
                newProj.lifetime = 0.0f; 
                
                // 4. Agregar al cargador
                projectiles.push_back(newProj);
                // Recalentar el arma
                shootCooldown = 0.4f; // Esperar 0.4 seg para el siguiente tiro
                std::cout << "¡Fiuuu! (Disparo)" << std::endl;
            }
            
            // Truco de prueba: daño infligido
            if(event.key.code == sf::Keyboard::K){
                currentHealth -= 10;
                if(currentHealth < 0) currentHealth = 0; // Corregí < 10 a < 0
                // Actualizar tamaño de barra
                // (vida actual / vida maxima) * Ancho original(100)
                float percentage = (float)currentHealth / (float)maxHealth;
                healthBar.setSize(sf::Vector2f(200.0f * percentage, 20.0f));
                std::cout << "Vida:" << currentHealth << std::endl;
            }
        }
    }
}

void Game::update() {
    // Esto mide el tiempo real entre "fotos". Arregla la velocidad loca.
    float dt = dtClock.restart().asSeconds(); 

    // 1. Obtener la velocidad física de ben 
    b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);
    bool isMoving = false; // Aplicamos bandera para saber si corremos
    
    // Definimos que tamaño debemos tener
    float currentScale = 1.5f; // Tamaño normal de ben
    if(isHeatblast){
        currentScale = 2.2f; // Tamaño para fuego grande
    }

    // 2. Controles
    // Vamo a la derecha
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        velocity.x = 200.0f; // Asi aplicamos que se mueva la derecha en el eje x
        benSprite.setScale(currentScale, currentScale);
        isMoving = true; // Aqui aplica que ben se esta moviendo
    }
    // Vamo a la Izquierda
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        velocity.x = -200.0f; // Aplicamos que se mueva a la izquierda en el eje x
        benSprite.setScale(-currentScale, currentScale);
        isMoving = true; // Se esta moviendo a la izquierda :D
    }
    // QUIETOO
    else{
        velocity.x = 0.0f;
        isMoving = false;
    }

    // ------------------------------------
    // --- SISTEMA DE ENERGÍA OMNITRIX (NUEVO) ---
    // ------------------------------------
    
    // CASO A: SI ERES FUEGO (Gastar Energía)
    if (isHeatblast) {
        // Restamos energía (15 unidades por segundo = dura unos 6-7 segundos)
        currentEnergy -= 15.0f * dt;

        // ¿SE ACABÓ LA PILA?
        if (currentEnergy <= 0.0f) {
            currentEnergy = 0.0f;
            
            // ¡DESTRANSFORMACIÓN FORZADA! ⚡
            isHeatblast = false;
            std::cout << "¡BATERÍA AGOTADA! volviendo a ser Ben..." << std::endl;

            // Restablecer físicas y gráficos de Ben
            benSprite.setTexture(benTexture);
            benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
            benSprite.setOrigin(25.5f, 42.0f);
            benSprite.setScale(1.5f, 1.5f); // Volver a tamaño normal
        }
    } 
    // CASO B: SI ERES BEN (Recargar Energía)
    else {
        // Recuperamos energía lento (5 unidades por segundo)
        currentEnergy += 5.0f * dt;
        
        // No pasar del máximo
        if (currentEnergy > maxEnergy) {
            currentEnergy = maxEnergy;
        }
    }

    // ACTUALIZAR EL TAMAÑO VISUAL DE LA BARRA DE ENERGÍA
    float energyPct = currentEnergy / maxEnergy;
    energyBar.setSize(sf::Vector2f(200.0f * energyPct, 10.0f));

    // 3. Sistema de Animación
    if(isHeatblast){
        if(isMoving){
            // A. Cronometro
            animationTimer += dt; 
            if(animationTimer >= 0.1f){
                animationTimer = 0.0f;
                currentFrame++;
                if(currentFrame >= 6) currentFrame = 0;
            }
            // B. Listas Maestras
            int fuegoX[] = {16, 67, 106, 155, 210, 250};
            int fuegoW[] = {25, 20, 34, 32, 21, 36};
            
            // C. Obtener cuadro actual 
            int currentX = fuegoX[currentFrame];
            int currentW = fuegoW[currentFrame];
            
            // D. Aplicar recorte (Con Y=82)
            benSprite.setTextureRect(sf::IntRect(currentX, 82, currentW, 55));
            
            // E. Centrado Dinamico
            benSprite.setOrigin(currentW / 2.0f, 26.5f);
        }
        else{
            // QUIETO
            benSprite.setTextureRect(sf::IntRect(9, 11, 35, 53));
            benSprite.setOrigin(17.5f, 26.5f);
        }
    }
    else{ 
        // Si somos BEN
        if(isMoving){
            // 3.1. Avanzar el cronómetro
            animationTimer += dt; 
            // 3.2. Aplicar el cambio de dibujo
            if(animationTimer >= 0.1f){
                animationTimer = 0.0f; 
                currentFrame++; 
                if(currentFrame >= 6) currentFrame = 0;
            }
            // 3.3. Calcular el recorte
            int walkingFramesX[] = {17, 78, 137, 181, 238, 295};
            int frameX = walkingFramesX[currentFrame]; 
            benSprite.setTextureRect(sf::IntRect(frameX, 382, 47, 79));
            benSprite.setOrigin(23.5f, 39.5f);
        }
        else{
            // 3.4. Si esta quieto
            currentFrame = 0; 
            benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
            benSprite.setOrigin(25.5f, 42.0f);
        }
    }

    // 4. Aplicar las fisicas y sincronizamos
    b2Body_SetLinearVelocity(benBodyId, velocity);
    // Fisica se queda fija en 1/60 para que Box2D no se rompa
    b2World_Step(worldId, 1.0f / 60.0f, 4);

    b2Vec2 pos = b2Body_GetPosition(benBodyId);
    benSprite.setPosition(pos.x,pos.y);

    // Hacemos la zona de Muerte GAME OVER
    // Si ben cae muy abajo (Y mayor a 800 pixeles)
    if (pos.y > 800.0f) {
        b2Body_SetTransform(benBodyId, {400.0f, -100.0f}, {1.0f, 0.0});// 1.0 es Seno y 0.0 es Coseno
        b2Body_SetLinearVelocity(benBodyId, {0.0f, 0.0f});
    }

    // IA DEL ENEMIGO 
    b2Vec2 enemyPos = b2Body_GetPosition(enemyBodyId);
    enemyShape.setPosition(enemyPos.x, enemyPos.y);

    if (enemyPos.x > 680.0f && enemySpeed > 0) {
        enemySpeed = -100.0f; // ¡Cambio a Izquierda!
    }
    else if (enemyPos.x < 520.0f && enemySpeed < 0) {
        enemySpeed = 100.0f;  // ¡Cambio a Derecha!
    }

    b2Vec2 enemyVel = b2Body_GetLinearVelocity(enemyBodyId);
    enemyVel.x = enemySpeed; 
    b2Body_SetLinearVelocity(enemyBodyId, enemyVel);
    
    // ACTUALIZACION TEMPORIZADOR DE DAÑO
    if (damageTimer > 0.0f){
        damageTimer -= dt; // Restamos tiempo
    }
    // COLISION DE BEN VS DRON
    if(benSprite.getGlobalBounds().intersects(enemyShape.getGlobalBounds())){
        // Solo recibimos daño si el timer ya se acabó
        if(damageTimer <= 0.0f){
            // 1. Bajar vida
            currentHealth -= 25; // Perdio vida Ben
            std::cout << "¡GOLPE! Vida restante" << currentHealth << std::endl;
            // 2. Activar invencibilidad (2seg)
            damageTimer = 2.0f;
            // 3. Empujamos a ben hacia atrás y arriba para alejarlo del peligro
            b2Vec2 knockback = {-500.0, -300.0}; // Izquierda y arriba
            if (benSprite.getScale().x < 0) knockback.x = 500.0f; 
            b2Body_SetLinearVelocity(benBodyId, knockback);
            // 4. Actualizar la barra visualmente
            float percentage = (float)currentHealth / (float)maxHealth;
            if(percentage < 0) percentage = 0;
            healthBar.setSize(sf::Vector2f(200.0f * percentage, 20.0f));
            // 5. MURIO?
            if (currentHealth <= 0){
                std::cout << "¿BEN HA CAIDO! GAME OVER" << std::endl;
                // AHORA REINICIAR EL NIVEL
                b2Body_SetTransform(benBodyId, {400.0f, -100.0f}, {1.0f, 0.0f});
                b2Body_SetLinearVelocity(benBodyId, {0.0f, 0.0f});
                // Restaurar vida completa
                currentHealth = 100;
                healthBar.setSize(sf::Vector2f(200.0f, 20.0f));
                damageTimer = 0.0f;
            }
        }
    }

    // VICTORIA: CUANDO BEN TOCA LA META
    if(benSprite.getGlobalBounds().intersects(goalShape.getGlobalBounds())){
        std::cout << "¡NIVEL COMPLTADO! ERES UN HEROE." << std::endl;
        b2Body_SetTransform(benBodyId, {400.0f, -100.0f}, {1.0, 0.0f});
        b2Body_SetLinearVelocity(benBodyId, {0.0f, 0.0f});
    }
    
    if (shootCooldown > 0.0f){
        shootCooldown -= dt; // Restamos el tiempo que pasó
    }
    
    // 2. Actualización de balas
    for(size_t i = 0; i < projectiles.size(); i++){
        // 1. Mover USANDO dt (Tiempo real)
        projectiles[i].shape.move(projectiles[i].speed * dt, 0.0f); 
        // Envejecer la bala
        projectiles[i].lifetime += dt;
        // ¿Murio bala vieja?
        if (projectiles[i].lifetime > 2.0f){
            projectiles[i].destroy = true;
        }
        // Colision de bala vs dron
        if(projectiles[i].shape.getGlobalBounds().intersects(enemyShape.getGlobalBounds())){
            projectiles[i].destroy = true; // la bala explota
            // Matar al enemigo 
            b2Body_SetTransform(enemyBodyId, {-1000.0f, -1000.0f}, {1.0f, 0.0f});
            enemySpeed = 0; // Para que ya no se mueva
            std::cout << "¡ENEMIGO ELIMINADO!" << std::endl;
        }
    }
    // BORRAR BALAS VIEJAS
    auto iterator = std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& p){ return p.destroy;});
    projectiles.erase(iterator, projectiles.end());

    // CAMBIO DE COLOR DE BARRA
    if (damageTimer > 0.0f){
        healthBar.setFillColor(sf::Color::Red); // En peligro/ invencible
    } else{
        healthBar.setFillColor(sf::Color::Green); // Sano/ Listo
    }
}

void Game::render(){
    window.clear(sf::Color::Black); // 1. Limpiar pantalla

    // --- CÁMARA ---
    sf::Vector2f benPos = benSprite.getPosition();
    sf::View camera(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f, 600.0f));
    camera.setCenter(benPos);
    window.setView(camera);

    // --- DIBUJAR TODO ---

    // 1. Plataformas
    for (const auto& shape : platformShapes) {
        window.draw(shape);
    }
    
    // 2. Objetos
    window.draw(goalShape); 
    window.draw(enemyShape);
    
    // 3. Balas (¡AQUÍ ES SU LUGAR!)
    for (const auto& proj : projectiles){
        window.draw(proj.shape);
    }

    // 4. Ben (Lo dibujamos al final para que salga encima de todo)
    window.draw(benSprite);
    
    // Dibujar Interfaz (HUD)
    // Guardar la vista del juego (Camara de Ben)
    sf::View gameView = window.getView();
    // 2. Cambiar a la vista por defecto (pegada a la pantalla 0,0)
    window.setView(window.getDefaultView());
    
    // 3. Dibujar la Interfaz
    window.draw(healthBarBack); // Fondo Gris
    window.draw(healthBar); // Barra verde
    
    // DIBUJAR ENERGÍA (NUEVO)
    window.draw(energyBarBack);
    window.draw(energyBar);
    
    // 4. Restaurar la vista del juego
    window.setView(gameView);

    // --- MOSTRAR PANTALLA (SOLO UNA VEZ AL FINAL) ---
    window.display(); 
}

// Aplicacion de las plataformas
void Game::createPlatform(float x, float y, float width, float height){
    // 1. Fisicas (Box2D)
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = {x, y}; 
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2Polygon box = b2MakeBox(width / 2.0f, height / 2.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(bodyId, &shapeDef, &box);

    // 2. SFML
    sf::RectangleShape shape(sf::Vector2f(width, height));
    shape.setOrigin(width / 2.0f, height / 2.0); 
    shape.setPosition(x, y);

    shape.setFillColor(sf::Color(0, 180, 0)); 
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::White);

    // 3. Guardamos
    platformShapes.push_back(shape);
}