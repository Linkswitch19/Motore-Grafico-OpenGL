#pragma once
#include <string>
#include <iostream>
#include "lib.h"
namespace eng {
	class ENG_API TextHUD
	{

    private:
        std::string content;
        float x, y;
        float r, g, b;

    public:
        
        TextHUD(std::string content, float x, float y, float r = 1.0f, float g = 1.0f, float b = 1.0f);

        // Metodo per aggiornare il contenuto (es. se vuoi cambiare il punteggio)
        void setContent(std::string newContent);

        
        void render();
	};
}


