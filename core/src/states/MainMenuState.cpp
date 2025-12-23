#include <states/MainMenuState.hpp>
#include <systems/AudioManager.hpp>
#include <systems/ResourceManager.hpp>
#include "states/OptionsState.hpp"
#include "systems/LocalizationManager.hpp"


MainMenuState::MainMenuState(){};

MainMenuState::~MainMenuState(){
    //no descargo la textura porque se queda en cache
};

void MainMenuState::init(){

    // Cargar el logo desde el ResourceManager 
    logo = ResourceManager::getInstance()->loadTexture("assets/images/logo.png");

    // Botones horizontales en la parte inferior
    float buttonWidth = 150;
    float buttonHeight = 50;
    float spacing = 20;
    float totalWidth = (buttonWidth * 3) + (spacing * 2);
    float startX = GetScreenWidth()/2 - totalWidth/2;
    float buttonY = GetScreenHeight() - buttonHeight - 30;

    playButton = { startX, buttonY, buttonWidth, buttonHeight };
    optionsButton = { startX + buttonWidth + spacing, buttonY, buttonWidth, buttonHeight };
    exitButton = { startX + (buttonWidth + spacing) * 2, buttonY, buttonWidth, buttonHeight };
};

void MainMenuState::handleInput(){



};

void MainMenuState::update(float deltaTime){

    mousePos = GetMousePosition();

    mouseOverPlayButton = CheckCollisionPointRec(mousePos, playButton);
    mouseOverOptionsButton = CheckCollisionPointRec(mousePos, optionsButton);
    mouseOverExitButton = CheckCollisionPointRec(mousePos, exitButton);

    // Lógica de hover para botón PLAY
    if (mouseOverPlayButton && mouseExitedPlayButton) {
        // Acabamos de entrar en el botón, reproducir sonido UNA vez
        AudioManager::getInstance()->playButtonHover();
        mouseExitedPlayButton = false;
    } else if (!mouseOverPlayButton) {
        // Salimos del botón, resetear flag
        mouseExitedPlayButton = true;
    }

    // Lógica de hover para botón OPTIONS
    if (mouseOverOptionsButton && mouseExitedOptionsButton) {
        // Acabamos de entrar en el botón, reproducir sonido UNA vez
        AudioManager::getInstance()->playButtonHover();
        mouseExitedOptionsButton = false;
    } else if (!mouseOverOptionsButton) {
        // Salimos del botón, resetear flag
        mouseExitedOptionsButton = true;
    }

    // Lógica de hover para botón EXIT
    if (mouseOverExitButton && mouseExitedExitButton) {
        // Acabamos de entrar en el botón, reproducir sonido UNA vez
        AudioManager::getInstance()->playButtonHover();
        mouseExitedExitButton = false;
    } else if (!mouseOverExitButton) {
        // Salimos del botón, resetear flag
        mouseExitedExitButton = true;
    }

    if (mouseOverPlayButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::getInstance()->playButtonClick();
        this->state_machine->add_state(std::make_unique<MainGameState>(), true);
    }
    
    if (mouseOverOptionsButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::getInstance()->playButtonClick();
        this->state_machine->add_state(std::make_unique<OptionsState>(), false);
    }

    if (mouseOverExitButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::getInstance()->playButtonClick();
        this->state_machine->remove_state(true);
    }
};

void MainMenuState::render(){

    mousePos = GetMousePosition();

    BeginDrawing();

    ClearBackground(SKYBLUE);
    
    // Dibujar el logo más grande y centrado en la parte superior
    if (logo.id != 0) {
        float scale = 0.5f; // Escalar al 50% del tamaño original (512x512)
        float logoWidth = logo.width * scale;
        float logoHeight = logo.height * scale;
        float logoX = GetScreenWidth()/2 - logoWidth/2;
        float logoY = -30;
        
        Rectangle source = { 0, 0, (float)logo.width, (float)logo.height };
        Rectangle dest = { logoX, logoY, logoWidth, logoHeight };
        Vector2 origin = { 0, 0 };
        
        DrawTexturePro(logo, source, dest, origin, 0.0f, WHITE);
    }

    DrawRectangleRec(playButton, mouseOverPlayButton ? GRAY : DARKGRAY);
    DrawText(_("JUGAR"), playButton.x + 45, playButton.y + 15, 20, BLACK);

    DrawRectangleRec(optionsButton, mouseOverOptionsButton ? GRAY : DARKGRAY);
    DrawText(_("OPCIONES"), optionsButton.x + 25, optionsButton.y + 15, 20, BLACK);

    DrawRectangleRec(exitButton, mouseOverExitButton ? GRAY : DARKGRAY);
    DrawText(_("SALIR"), exitButton.x + 50, exitButton.y + 15, 20, BLACK);

    renderGlobalUI();
    
    EndDrawing();
};






