/********************************** */
// Pantalla de menú principal  

extern "C" {
    #include <raylib.h>
}


#include <GameState.hpp>



class MainMenuState : public GameState
{

    public:
        MainMenuState();
        virtual ~MainMenuState() = default;

        virtual void init() override;
        virtual void handleInput() override;
        virtual void update(float deltaTime) override;
        virtual void render() override;

        virtual void pause(){};
        virtual void resume(){};

    private:
        

        
};