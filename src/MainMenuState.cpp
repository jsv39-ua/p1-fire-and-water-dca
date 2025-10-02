#include <MainMenuState.hpp>


MainMenuState::MainMenuState(){};

// Para cuando necesitemos hacer cosas en el destructor
//MainMenuState::~MainMenuState(){};

void MainMenuState::init(){};

void MainMenuState::handleInput(){

};

void MainMenuState::update(float deltaTime){};

void MainMenuState::render(){

    BeginDrawing();

    ClearBackground(SKYBLUE);
    DrawText("ELEMENTS", GetScreenWidth()/2, GetScreenHeight()/2, 25, YELLOW);

    EndDrawing();
};






