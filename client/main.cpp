/**
 * @file		main.cpp
 * @brief	Client application (that uses the graphics engine)
 *
 * @author	Achille Peternier (C) SUPSI [achille.peternier@supsi.ch] << change this to your group members
 */



//////////////
// #INCLUDE //
//////////////

   // Library header:
   #include "engine.h"

   // C/C++:
   #include <iostream>



#include <Windows.h>                              // Header File For Windows

#include <math.h>

#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
#define _CRT_SECURE_NO_WARNINGS




void keyboardCallback(unsigned char key, int x, int y) {
    //esc
    if (key == 27) {
        std::cout << "Uscita richiesta..." << std::endl;
        exit(0);
        
    }
    
    // Esempio movimento (W)
    if (key == 'w' || key == 'W') {
        std::cout << "Avanti!" << std::endl;
       
    }




}



//////////
// MAIN //
//////////

/**
 * Application entry point.
 * @param argc number of command-line arguments passed
 * @param argv array containing up to argc passed arguments
 * @return error code (0 on success, error code otherwise)
 */
int main(int argc, char *argv[])
{
   // Credits:
   std::cout << "Client application example, A. Peternier (C) SUPSI" << std::endl;
   std::cout << std::endl;

   // Init engine:
   Eng::Base &eng = Eng::Base::getInstance();
 
   eng.setKeyboardCallback(keyboardCallback);
   eng.init("My OpenGL window", 640, 480,argc,argv);
  

 
   

   // Release engine:
   eng.free();

   // Done:
   std::cout << "\n[application terminated]" << std::endl;
   return 0;
}








