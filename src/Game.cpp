#include "Game.hpp"
#include <cmath>
#include <iostream> 

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
    // 3.1. El suelo Principal
    createPlatform(2500.0f, 580.0f, 5000.0f, 40.0f);

    // 3.2. Plataformas flotantes
    createPlatform(600.0f, 450.0f, 200.0f, 20.0f); 
    createPlatform(200.0f, 350.0f, 200.0f, 20.0f); 
    createPlatform(500.0f, 200.0f, 150.0f, 20.0f); 
    
    // Plataformas extra del mapa largo
    createPlatform(2500.0f, 400.0f, 300.0f, 20.0f);
    createPlatform(1200.0f, 400.0f, 200.0f, 20.0f); 
    createPlatform(2000.0f, 300.0f, 200.0f, 20.0f); 
    createPlatform(3000.0f, 450.0f, 200.0f, 20.0f); 
    createPlatform(4000.0f, 350.0f, 200.0f, 20.0f); 

    // 4. Crear Cuerpo Físico de Ben
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {100.0f, 450.0f}; 
    benBodyId = b2CreateBody(worldId, &bodyDef);
    b2Body_SetFixedRotation(benBodyId, true);

    b2Polygon dynamicBox = b2MakeBox(30.0f, 60.0f); 
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    b2CreatePolygonShape(benBodyId, &shapeDef, &dynamicBox);

    // --- 5. CONFIGURAR GRÁFICOS DE BEN ---
    sf::Image benImage;
    if (!benImage.loadFromFile("assets/images/ben10.png")) {
        std::cerr << "ERROR cargando imagen ben10.png" << std::endl;
    }
    benImage.createMaskFromColor(sf::Color::Magenta);
    benTexture.loadFromImage(benImage);
    benSprite.setTexture(benTexture);
    benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84)); 
    benSprite.setOrigin(25.5f, 42.0f); 
    benSprite.setScale(1.5f, 1.5f);
    animationTimer = 0.0f;
    currentFrame = 0; 

    // 5.1. Cargar Graficos de FUEGO
    sf::Image heatblastImage;
    if(!heatblastImage.loadFromFile("assets/images/Fuego.png")){
        std::cerr << "ERROR cargando imagen Fuego.png" << std::endl;
    }
    heatblastImage.createMaskFromColor(sf::Color(255, 0, 255));
    heatblastTexture.loadFromImage(heatblastImage);
    isHeatblast = false;
    
    // ----------------------------------------------------
    // 6. CREAR EJÉRCITO DE DRONES (¡NUEVO!) 🤖🤖🤖
    // spawnEnemy(Posicion X, Posicion Y, Distancia de Patrulla)
    // ----------------------------------------------------
    spawnEnemy(600.0f, 400.0f, 100.0f); // El guardia original
    spawnEnemy(1200.0f, 350.0f, 100.0f); // Km 1
    spawnEnemy(2000.0f, 250.0f, 100.0f); // Km 2
    spawnEnemy(3000.0f, 400.0f, 150.0f); // Km 3
    
    // Guardianes de la Meta
    spawnEnemy(4700.0f, 530.0f, 150.0f);
    spawnEnemy(4900.0f, 530.0f, 150.0f);

    // 7. Crear la META
    goalShape.setRadius(20.0f); 
    goalShape.setFillColor(sf::Color::Yellow); 
    goalShape.setOutlineThickness(2.0f);
    goalShape.setOutlineColor(sf::Color::White);
    goalShape.setOrigin(20.0f, 20.0f);
    goalShape.setPosition(4800.0f, 500.0f); // Meta al final del mundo

    shootCooldown = 0.0f; 

    // CONFIGURAR VIDA
    maxHealth = 100;
    currentHealth = 100; 
    damageTimer = 0.0f; 
    
    // 1. Fondo de la barra
    healthBarBack.setSize(sf::Vector2f(200.0f, 20.0f)); 
    healthBarBack.setFillColor(sf::Color(50, 50, 50)); 
    healthBarBack.setPosition(20.0f, 20.0f); 
    
    // 2. Barra de Vida
    healthBar.setSize(sf::Vector2f(200.0f, 20.0f));
    healthBar.setFillColor(sf::Color(0, 255, 0)); 
    healthBar.setPosition(20.0f, 20.0f); 

    // CONFIGURAR ENERGÍA
    maxEnergy = 100.0f; 
    currentEnergy = 100.0f; 
    
    // 1. Fondo barra energía
    energyBarBack.setSize(sf::Vector2f(200.0f, 10.0f)); 
    energyBarBack.setFillColor(sf::Color(50, 50, 50));
    energyBarBack.setPosition(20.0f, 45.0f); 
    
    // 2. Barra Amarilla
    energyBar.setSize(sf::Vector2f(200.0f, 10.0f));
    energyBar.setFillColor(sf::Color::Yellow); 
    energyBar.setPosition(20.0f, 45.0f);
    
    //Cargar audio
    //1.Musica 
    if(bgMusic.openFromFile("assets/music/music.mp3")){
        bgMusic.setLoop(true); //Repetir siempre
        bgMusic.setVolume(30); //Volumen moderado
        bgMusic.play(); //Play a la musica 
    }else{
        std::cout << "No se encontro music.mp3" << std::endl;
    }
    //2.Efecto de salto
    if(jumpBuffer.loadFromFile("assets/music/jump.wav")){
        jumpSound.setBuffer(jumpBuffer);
        jumpSound.setVolume(50);
    }
    //3.Efecto de disparo
    if(shootBuffer.loadFromFile("assets/music/shoot.wav")){
        shootSound.setBuffer(shootBuffer);
        shootSound.setVolume(60);
    }
    //4.Transformación
    if(transformBuffer.loadFromFile("assets/music/transform.wav")){
        transformSound.setBuffer(transformBuffer);
        transformSound.setVolume(70);
    }
    //5.Efecto golpe
    if(hitBuffer.loadFromFile("assets/music/hit.wav")){
        hitSound.setBuffer(hitBuffer);
        hitSound.setVolume(50);
    }
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
            
            //Salto
            if(event.key.code == sf::Keyboard::Space){
                b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);
                if (std::abs(velocity.y)<0.5){
                    velocity.y = -900.0f; 
                    b2Body_SetLinearVelocity(benBodyId, velocity);
                    
                    // ¡SONIDO DE SALTO!
                    jumpSound.play();
                }
            }

            // --- TRANSFORMACIÓN ---
            if(event.key.code == sf::Keyboard::Z){
                if (!isHeatblast && currentEnergy < 10.0f) {
                    std::cout << "¡Sin energía!" << std::endl;
                }
                else {
                    isHeatblast = !isHeatblast;
                    
                    // ¡SONIDO DE TRANSFORMACIÓN!
                    transformSound.play();

                    if(isHeatblast){
                        // Fuego
                        std::cout << "¡FUEGO!" << std::endl;
                        benSprite.setTexture(heatblastTexture);
                        benSprite.setTextureRect(sf::IntRect(9, 11, 35, 53));
                        benSprite.setScale(2.2f, 2.2f);
                        benSprite.setOrigin(17.5f, 26.5f);
                    }else{
                        // Ben
                        std::cout << "Destransformado..." << std::endl;
                        benSprite.setTexture(benTexture);
                        benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
                        benSprite.setOrigin(25.5f, 42.0f);
                        benSprite.setScale(1.5f, 1.5f);
                    }
                }
            }
            
            // --- DISPARO ---
            if(event.key.code == sf::Keyboard::X && isHeatblast && shootCooldown <= 0.0f){
                
                // ¡SONIDO DE DISPARO!
                shootSound.play();
                
                Projectile newProj;
                newProj.shape.setRadius(10.0f);
                newProj.shape.setFillColor(sf::Color(255, 165, 0)); 
                newProj.shape.setOrigin(10.0f, 10.0f); 
                
                sf::Vector2f benPos = benSprite.getPosition();
                newProj.shape.setPosition(benPos.x, benPos.y - 15.0f);
                
                if(benSprite.getScale().x > 0) newProj.speed = 600.0f;
                else newProj.speed = -600.0f; 
                
                newProj.destroy = false;
                newProj.lifetime = 0.0f; 
                projectiles.push_back(newProj);
                shootCooldown = 0.4f; 
            }
            
            // Truco K
            if(event.key.code == sf::Keyboard::K){
                currentHealth -= 10;
                if(currentHealth < 0) currentHealth = 0;
                float percentage = (float)currentHealth / (float)maxHealth;
                healthBar.setSize(sf::Vector2f(200.0f * percentage, 20.0f));
            }
        }
    }
}

void Game::update() {
    float dt = dtClock.restart().asSeconds(); 

    // 1. Física Ben 
    b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);
    bool isMoving = false; 
    
    float currentScale = 1.5f; 
    if(isHeatblast) currentScale = 2.2f; 

    // 2. Controles
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        velocity.x = 200.0f; 
        benSprite.setScale(currentScale, currentScale);
        isMoving = true; 
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        velocity.x = -200.0f; 
        benSprite.setScale(-currentScale, currentScale);
        isMoving = true; 
    }
    else{
        velocity.x = 0.0f;
        isMoving = false;
    }

    // Sistema de Energía
    if (isHeatblast) {
        currentEnergy -= 15.0f * dt;
        if (currentEnergy <= 0.0f) {
            currentEnergy = 0.0f;
            isHeatblast = false; // Destransformar
            benSprite.setTexture(benTexture);
            benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
            benSprite.setOrigin(25.5f, 42.0f);
            benSprite.setScale(1.5f, 1.5f); 
        }
    } else {
        currentEnergy += 5.0f * dt;
        if (currentEnergy > maxEnergy) currentEnergy = maxEnergy;
    }
    // Actualizar barra amarilla
    float energyPct = currentEnergy / maxEnergy;
    energyBar.setSize(sf::Vector2f(200.0f * energyPct, 10.0f));

    // 3. Sistema de Animación
    if(isHeatblast){
        if(isMoving){
            animationTimer += dt; 
            if(animationTimer >= 0.1f){
                animationTimer = 0.0f;
                currentFrame++;
                if(currentFrame >= 6) currentFrame = 0;
            }
            int fuegoX[] = {16, 67, 106, 155, 210, 250};
            int fuegoW[] = {25, 20, 34, 32, 21, 36};
            int currentX = fuegoX[currentFrame];
            int currentW = fuegoW[currentFrame];
            
            benSprite.setTextureRect(sf::IntRect(currentX, 82, currentW, 55));
            benSprite.setOrigin(currentW / 2.0f, 26.5f);
        }
        else{
            benSprite.setTextureRect(sf::IntRect(9, 11, 35, 53));
            benSprite.setOrigin(17.5f, 26.5f);
        }
    }
    else{ 
        if(isMoving){
            animationTimer += dt; 
            if(animationTimer >= 0.1f){
                animationTimer = 0.0f; 
                currentFrame++; 
                if(currentFrame >= 6) currentFrame = 0;
            }
            int walkingFramesX[] = {17, 78, 137, 181, 238, 295};
            int frameX = walkingFramesX[currentFrame]; 
            benSprite.setTextureRect(sf::IntRect(frameX, 382, 47, 79));
            benSprite.setOrigin(23.5f, 39.5f);
        }
        else{
            currentFrame = 0; 
            benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
            benSprite.setOrigin(25.5f, 42.0f);
        }
    }

    // 4. Aplicar físicas
    b2Body_SetLinearVelocity(benBodyId, velocity);
    b2World_Step(worldId, 1.0f / 60.0f, 4);

    b2Vec2 pos = b2Body_GetPosition(benBodyId);
    benSprite.setPosition(pos.x,pos.y);

    // Zona de Muerte
    if (pos.y > 800.0f) {
        b2Body_SetTransform(benBodyId, {100.0f, 450.0f}, {1.0f, 0.0}); // Reinicio al principio
        b2Body_SetLinearVelocity(benBodyId, {0.0f, 0.0f});
    }
    //EJERCITO DE ENEMIGOS
    for (size_t i = 0; i < enemies.size(); i++) {
        
        // 1. Sincronizar dibujo
        b2Vec2 ePos = b2Body_GetPosition(enemies[i].bodyId);
        enemies[i].shape.setPosition(ePos.x, ePos.y);

        // 2. IA DE PATRULLA
        float limitRight = enemies[i].startX + enemies[i].patrolRange;
        float limitLeft  = enemies[i].startX - enemies[i].patrolRange;

        if (ePos.x > limitRight && enemies[i].speed > 0) {
            enemies[i].speed = -100.0f; 
        }
        else if (ePos.x < limitLeft && enemies[i].speed < 0) {
            enemies[i].speed = 100.0f;  
        }

        b2Vec2 vel = b2Body_GetLinearVelocity(enemies[i].bodyId);
        vel.x = enemies[i].speed;
        b2Body_SetLinearVelocity(enemies[i].bodyId, vel);

        // 3. COLISIÓN CON BEN (DAÑO)
        if(benSprite.getGlobalBounds().intersects(enemies[i].shape.getGlobalBounds())){
             if(damageTimer <= 0.0f){
                //Sonido de golpe
                hitSound.play();
                currentHealth -= 25;
                std::cout << "¡GOLPE DE DRON! Vida: " << currentHealth << std::endl;
                damageTimer = 2.0f; 
                
                b2Vec2 knockback = {-500.0, -300.0};
                if (benSprite.getScale().x < 0) knockback.x = 500.0f;
                b2Body_SetLinearVelocity(benBodyId, knockback);

                float percentage = (float)currentHealth / (float)maxHealth;
                if(percentage < 0) percentage = 0;
                healthBar.setSize(sf::Vector2f(200.0f * percentage, 20.0f));

                if (currentHealth <= 0){
                    std::cout << "GAME OVER" << std::endl;
                    b2Body_SetTransform(benBodyId, {100.0f, 450.0f}, {1.0f, 0.0f}); 
                    currentHealth = 100;
                    healthBar.setSize(sf::Vector2f(200.0f, 20.0f));
                }
             }
        }
    }
    
    // Temporizador de daño
    if (damageTimer > 0.0f) damageTimer -= dt;
    // Color barra
    if (damageTimer > 0.0f) healthBar.setFillColor(sf::Color::Red); 
    else healthBar.setFillColor(sf::Color::Green);

    // VICTORIA
    if(benSprite.getGlobalBounds().intersects(goalShape.getGlobalBounds())){
        std::cout << "¡NIVEL COMPLTADO!" << std::endl;
        b2Body_SetTransform(benBodyId, {100.0f, 450.0f}, {1.0, 0.0f});
        b2Body_SetLinearVelocity(benBodyId, {0.0f, 0.0f});
    }
    
    // Armas
    if (shootCooldown > 0.0f) shootCooldown -= dt; 
    
    // 2. Actualización de balas
    for(size_t i = 0; i < projectiles.size(); i++){
        projectiles[i].shape.move(projectiles[i].speed * dt, 0.0f); 
        projectiles[i].lifetime += dt;
        if (projectiles[i].lifetime > 2.0f) projectiles[i].destroy = true;
        
        // COLISIÓN BALA VS CUALQUIER ENEMIGO
        for (size_t k = 0; k < enemies.size(); k++) {
            // Solo chocar si el enemigo no está "muerto"
            if (!enemies[k].destroy && projectiles[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds())) {
                
                projectiles[i].destroy = true; // Adios bala
                
                // Adios enemigo (lo mandamos al inframundo)
                b2Body_SetTransform(enemies[k].bodyId, {-1000.0f, -1000.0f}, {1.0f, 0.0f});
                enemies[k].shape.setPosition(-1000.0f, -1000.0f);
                enemies[k].destroy = true; // Marcar como muerto
                
                std::cout << "¡DRON ELIMINADO!" << std::endl;
            }
        }
    }
    // Borrar balas viejas
    auto iterator = std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& p){ return p.destroy;});
    projectiles.erase(iterator, projectiles.end());
}

void Game::render(){
    window.clear(sf::Color::Black); 

    sf::Vector2f benPos = benSprite.getPosition();
    sf::View camera(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f, 600.0f));
    camera.setCenter(benPos);
    window.setView(camera);

    for (const auto& shape : platformShapes) {
        window.draw(shape);
    }
    window.draw(goalShape); 
    
    // DIBUJAR EJÉRCITO
    for (const auto& enemy : enemies) {
        // Solo dibujar si no está "muerto"
        if (!enemy.destroy) {
            window.draw(enemy.shape);
        }
    }
    
    for (const auto& proj : projectiles){
        window.draw(proj.shape);
    }
    window.draw(benSprite);
    
    // HUD
    sf::View gameView = window.getView();
    window.setView(window.getDefaultView());
    window.draw(healthBarBack); 
    window.draw(healthBar); 
    window.draw(energyBarBack);
    window.draw(energyBar);
    window.setView(gameView);

    window.display(); 
}

// Función para crear un enemigo nuevo en cualquier lugar (NUEVO)
void Game::spawnEnemy(float x, float y, float range) {
    Enemy newEnemy;

    // 1. Configurar Física
    b2BodyDef enemyDef = b2DefaultBodyDef();
    enemyDef.type = b2_kinematicBody; 
    enemyDef.position = {x, y};
    enemyDef.fixedRotation = true;
    newEnemy.bodyId = b2CreateBody(worldId, &enemyDef);

    b2Polygon enemyBox = b2MakeBox(15.0f, 15.0f);
    b2ShapeDef enemyShapeDef = b2DefaultShapeDef();
    enemyShapeDef.density = 1.0f;
    b2CreatePolygonShape(newEnemy.bodyId, &enemyShapeDef, &enemyBox);

    // 2. Configurar Gráficos
    newEnemy.shape.setSize(sf::Vector2f(30.0f, 30.0f));
    newEnemy.shape.setOrigin(15.0f, 15.0f);
    newEnemy.shape.setFillColor(sf::Color::Red);
    newEnemy.shape.setPosition(x, y);

    // 3. Configurar IA
    newEnemy.speed = 100.0f; 
    newEnemy.startX = x;     
    newEnemy.patrolRange = range; 
    newEnemy.destroy = false;

    // 4. ¡Alistar en el ejército!
    enemies.push_back(newEnemy);
}

// Aplicacion de las plataformas
void Game::createPlatform(float x, float y, float width, float height){
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = {x, y}; 
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Polygon box = b2MakeBox(width / 2.0f, height / 2.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(bodyId, &shapeDef, &box);
    sf::RectangleShape shape(sf::Vector2f(width, height));
    shape.setOrigin(width / 2.0f, height / 2.0); 
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color(0, 180, 0)); 
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::White);
    platformShapes.push_back(shape);
}