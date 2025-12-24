#include <systems/SignDictionary.hpp>
#include <systems/LocalizationManager.hpp>
#include <systems/SettingsManager.hpp>
#include <string>

std::string SignDictionary::getText(const std::string& id) {
    if (id == "SIGN_TUTORIAL_WELCOME") {
        SettingsManager* sm = SettingsManager::getInstance();
        
        std::string text = _("Hola, bienvenido a Elements, vamos a aprender lo básico de forma práctica... ¡Diviértete!\n\n");
        text += _("Controles:\n\n");
        
        // Construimos el string dinámicamente con las teclas actuales
        text += _("Jugador Agua: Movimiento básico: ") + sm->getKeyName(sm->getWaterLeft()) + ", " + sm->getKeyName(sm->getWaterJump()) + ", " + sm->getKeyName(sm->getWaterRight()) + "\n";
        text += _("Ataque: ") + sm->getKeyName(sm->getWaterAttack()) + "\n\n";
        
        text += _("Jugador Fuego: Movimiento básico: ") + sm->getKeyName(sm->getFireLeft()) + ", " + sm->getKeyName(sm->getFireJump()) + ", " + sm->getKeyName(sm->getFireRight()) + "\n";
        text += _("Ataque: ") + sm->getKeyName(sm->getFireAttack());
        
        return text;
    }
    
    if (id == "SIGN_SECRET_1") {
        return _("A veces encontraras secretos ocultos... ¡Anímate a explorar!");
    }
    return id;
}
