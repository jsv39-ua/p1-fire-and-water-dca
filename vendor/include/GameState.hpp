#pragma once
#include <memory>

class StateMachine;

class GameState
{

    public:
        GameState();
        virtual ~GameState() = default;

    virtual void init() = 0;
    virtual void handleInput() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    
    // Método para renderizar UI global (FPS, debug info, etc.)
    virtual void renderGlobalUI();

    virtual void pause() = 0;
    virtual void resume() = 0;        void setStateMachine(StateMachine* stt_mch) {state_machine = stt_mch;}

    protected:
        StateMachine* state_machine;
};