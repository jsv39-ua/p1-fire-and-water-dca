#include <MainMenuState.hpp>



MainMenuState::MainMenuState(){};

// Para cuando necesitemos hacer cosas en el destructor
//MainMenuState::~MainMenuState(){};

void MainMenuState::init(){


    playButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 - 50, 200, 50 };
    exitButton = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 20, 200, 50 };
};

void MainMenuState::handleInput(){



};

void MainMenuState::update(float deltaTime){

    mousePos = GetMousePosition();

    mouseOverPlayButton = CheckCollisionPointRec(mousePos, playButton);
    mouseOverExitButton = CheckCollisionPointRec(mousePos, exitButton);

    if (mouseOverPlayButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        this->state_machine->add_state(std::make_unique<MainGameState>(), true);

    }

    if (mouseOverExitButton && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        
        WindowShouldClose();
    }
};

void MainMenuState::render(){

    mousePos = GetMousePosition();

    BeginDrawing();

    ClearBackground(SKYBLUE);
    DrawText("ELEMENTS", GetScreenWidth()/2, 30, 25, YELLOW);

    DrawRectangleRec(playButton, mouseOverPlayButton ? GREEN : DARKGREEN);
    DrawText("JUGAR", playButton.x + 70, playButton.y + 10, 20, BLACK);
    DrawRectangleRec(exitButton, mouseOverExitButton ? RED : MAROON);
    DrawText("SALIR", exitButton.x + 70, exitButton.y + 10, 20, BLACK);

    EndDrawing();
};






