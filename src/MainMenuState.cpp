#include <MainMenuState.hpp>
#include <OptionsMenuState.hpp>

// Declaraciones externas para acceder a las variables globales de música
extern bool g_musicEnabled;



MainMenuState::MainMenuState(){};

// Para cuando necesitemos hacer cosas en el destructor
//MainMenuState::~MainMenuState(){};

void MainMenuState::init(){


    playButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 - 50, 200, 50 };
    optionsButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 25, 200, 50 };
    exitButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 100, 200, 50 };
};

void MainMenuState::handleInput(){



};

void MainMenuState::update(float deltaTime){

    mousePos = GetMousePosition();

    mouseOverPlayButton = CheckCollisionPointRec(mousePos, playButton);
    mouseOverOptionsButton = CheckCollisionPointRec(mousePos, optionsButton);
    mouseOverExitButton = CheckCollisionPointRec(mousePos, exitButton);

    if (mouseOverPlayButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        this->state_machine->add_state(std::make_unique<MainGameState>(), true);
    }
    
    if (mouseOverOptionsButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        this->state_machine->add_state(std::make_unique<OptionsMenuState>(&g_musicEnabled), false);
    }

    if (mouseOverExitButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        this->state_machine->remove_state(true);
    }
};

void MainMenuState::render(){

    mousePos = GetMousePosition();

    BeginDrawing();

    ClearBackground(SKYBLUE);
    DrawText("ELEMENTS", GetScreenWidth()/2 - 70, 30, 25, YELLOW);

    DrawRectangleRec(playButton, mouseOverPlayButton ? GRAY : DARKGRAY);
    DrawText("JUGAR", playButton.x + 70, playButton.y + 12, 20, BLACK);

    DrawRectangleRec(optionsButton, mouseOverOptionsButton ? GRAY : DARKGRAY);
    DrawText("OPCIONES", optionsButton.x + 50, optionsButton.y + 12, 20, BLACK);

    DrawRectangleRec(exitButton, mouseOverExitButton ? GRAY : DARKGRAY);
    DrawText("SALIR", exitButton.x + 70, exitButton.y + 12, 20, BLACK);

    EndDrawing();
};






