/********************************** */
// Pantalla de menú principal  

extern "C" {
    #include <raylib.h>
}


#include <GameState.hpp>
#include <StateMachine.hpp>
#include <MainGameState.hpp>



class MainMenuState : public GameState
{

    public:
        MainMenuState();
        virtual ~MainMenuState();

        virtual void init() override;
        virtual void handleInput() override;
        virtual void update(float deltaTime) override;
        virtual void render() override;

        virtual void pause(){};
        virtual void resume(){};

    private:
        // Variables para los botones y manejar el raton
        Vector2 mousePos;
        Rectangle playButton;
        Rectangle optionsButton;
        Rectangle exitButton;
        bool mouseOverPlayButton = false;
        bool mouseOverOptionsButton = false;
        bool mouseOverExitButton = false;

        bool mouseExitedPlayButton = true;
        bool mouseExitedOptionsButton = true;
        bool mouseExitedExitButton = true;

        // Logo
        Texture2D logo;
        
};