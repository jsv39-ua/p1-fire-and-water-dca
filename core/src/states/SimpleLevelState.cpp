 #include <states/SimpleLevelState.hpp>
 #include <states/MainMenuState.hpp>
 #include <states/MainGameState.hpp>
 #include <systems/AudioManager.hpp>
 #include <systems/LocalizationManager.hpp>
 #include <states/StateMachine.hpp>
 #include <iostream>

extern "C" {
    #include <raylib.h>
}

SimpleLevelState::SimpleLevelState() {}

void SimpleLevelState::init() {
}

void SimpleLevelState::handleInput() {
    
    if (IsKeyPressed(KEY_SPACE)) {
        AudioManager::getInstance()->playButtonClick();
        this->state_machine->add_state(std::make_unique<MainGameState>(), true);
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
    }
}

void SimpleLevelState::update(float deltaTime) {
    mousePos = GetMousePosition();
}

void SimpleLevelState::render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    const char* message = _("¡Enhorabuena por completar la demo de Elements!");
    int textWidth = MeasureText(message, 30);
    int x = (GetScreenWidth() - textWidth) / 2;
    int y = (GetScreenHeight() / 2) - 50;
    DrawText(message, x, y, 30, DARKBLUE);

    const char* message2 = _("Pulsa ESPACIO para volver a jugar o ESC para menu");
    int textWidth2 = MeasureText(message2, 18);
    DrawText(message2, (GetScreenWidth() - textWidth2) / 2, y + 40, 18, DARKGRAY);

    renderGlobalUI();
    EndDrawing();
}
