# BEN-10-JUEGO
hola esto es una prueba de conexion.
// ... (Debajo de las plataformas viejas)

    // REFERENCIAS VISUALES PARA EL CAMINO
    createPlatform(1200.0f, 400.0f, 200.0f, 20.0f); // Kilómetro 1
    createPlatform(2000.0f, 300.0f, 200.0f, 20.0f); // Kilómetro 2
    createPlatform(3000.0f, 450.0f, 200.0f, 20.0f); // Kilómetro 3
    createPlatform(4000.0f, 350.0f, 200.0f, 20.0f); // Kilómetro 4


// ------------------------------------------
    // --- ACTUALIZAR EJÉRCITO DE ENEMIGOS ---
    // ------------------------------------------
    for (size_t i = 0; i < enemies.size(); i++) {
        
        // 1. Sincronizar dibujo con física
        b2Vec2 ePos = b2Body_GetPosition(enemies[i].bodyId);
        enemies[i].shape.setPosition(ePos.x, ePos.y);

        // 2. IA DE PATRULLA INTELIGENTE
        // Calculamos sus límites basados en su punto de inicio (startX)
        float limitRight = enemies[i].startX + enemies[i].patrolRange;
        float limitLeft  = enemies[i].startX - enemies[i].patrolRange;

        // Si llegó al límite derecho
        if (ePos.x > limitRight && enemies[i].speed > 0) {
            enemies[i].speed = -100.0f; // Dar vuelta a la izq
        }
        // Si llegó al límite izquierdo
        else if (ePos.x < limitLeft && enemies[i].speed < 0) {
            enemies[i].speed = 100.0f;  // Dar vuelta a la der
        }

        // Aplicar velocidad
        b2Vec2 vel = b2Body_GetLinearVelocity(enemies[i].bodyId);
        vel.x = enemies[i].speed;
        b2Body_SetLinearVelocity(enemies[i].bodyId, vel);

        // 3. COLISIÓN CON BEN (DAÑO)
        if(benSprite.getGlobalBounds().intersects(enemies[i].shape.getGlobalBounds())){
             if(damageTimer <= 0.0f){
                currentHealth -= 25;
                std::cout << "¡GOLPE DE DRON! Vida: " << currentHealth << std::endl;
                damageTimer = 2.0f; // Invencibilidad
                
                // Empujón
                b2Vec2 knockback = {-500.0, -300.0};
                if (benSprite.getScale().x < 0) knockback.x = 500.0f;
                b2Body_SetLinearVelocity(benBodyId, knockback);

                // Actualizar barra de vida
                float percentage = (float)currentHealth / (float)maxHealth;
                if(percentage < 0) percentage = 0;
                healthBar.setSize(sf::Vector2f(200.0f * percentage, 20.0f));

                // Game Over
                if (currentHealth <= 0){
                    std::cout << "GAME OVER" << std::endl;
                    b2Body_SetTransform(benBodyId, {100.0f, 450.0f}, {1.0f, 0.0f}); // Reinicio al inicio
                    currentHealth = 100;
                    healthBar.setSize(sf::Vector2f(200.0f, 20.0f));
                }
             }
        }
    }

    // 4. COLISIÓN BALAS VS ENEMIGOS (Esto va dentro del bucle de balas)
    // (Necesitas actualizar tu bucle de proyectiles para que revise 'enemies' en vez de 'enemyShape')
