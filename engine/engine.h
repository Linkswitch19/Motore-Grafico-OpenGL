/**
 * @file		engine.h
 * @brief	Graphics engine main include file
 *
 * @author	Achille Peternier (C) SUPSI [achille.peternier@supsi.ch] << change this to your group members
 */
#pragma once


 
//////////////
// #INCLUDE //
//////////////

   // C/C++:         
#include <memory> 
#include "lib.h"
#include "node.h"
#include "textHUD.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>








///////////////
// NAMESPACE //
///////////////

namespace Eng {

	



//////////////
// #INCLUDE //
//////////////   

   // You can subinclude here other headers of your engine...



///////////////////////
// MAIN ENGINE CLASS //
///////////////////////

/**
 * @brief Base engine main class. This class is a singleton.
 */
	class ENG_API Base final
	{
		//////////
	public: //
		//////////	      

		   // Const/dest:
		Base(Base const&) = delete;
		~Base();
		std::vector<eng::TextHUD> hudList;

		// Operators:
		void operator=(Base const&) = delete;
		std::string currentMessage = "";

		// Singleton:
		static Base& getInstance();
		void updateAnimation(float deltaTime);
		using KeyboardCallback = void(*)(unsigned char key, int x, int y);
		void createOrb(float x, float y, float z);

		// Init/free:
		bool init(const char* title, unsigned int width, unsigned int height, int argc, char* argv[]);

		void clearWindow();
		void setBackgroundColor(float r, float g, float b);
		bool free();
		void swapBuffers();
		void setKeyboardCallback(KeyboardCallback cb);
		std::shared_ptr<eng::Node> getNode(const std::string& name);

		void onKeyPressed(unsigned char key, int x, int y);
		void startLoop();
		void onDisplay();
		void update();

		void setParent(const std::string& childName, const std::string& parentName);
		void setMessage(const std::string& msg);
		void addText(const eng::TextHUD& textObject);
		void clearHUD();
		void loadTexture(const std::string& textureFile, const std::string& objectName);


		void moveCamera(eng::Camera_Movement direction, float deltaTime);
		void rotateCamera(float angle);
		void setCameraPosition(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch);
		 





		///////////
	private: //
		///////////	

		   // Reserved:
		struct Reserved;
		std::unique_ptr<Reserved> reserved;

		float window_width;
		float window_height;
		bool fps_show;

		int fps = 0;
		int frames = 0;
		
		int detail = 1;


		bool wireframe = false;
		bool flat = false;
		bool culling = false;

		// Projection matrices:
		glm::mat4 perspective;
		glm::mat4 ortho;

		// Material properties:
		glm::vec4 materialAmbient{ 0.5f, 0.5f, 0.5f, 1.0f };
		glm::vec4 materialDiffuse{ 0.5f, 0.5f, 0.5f, 1.0f };
		glm::vec4 materialSpecular{ 1.0f, 1.0f, 1.0f, 1.0f };
		float materialShininess = 128.0f;


		// Light properties:
		glm::vec3 lightPosition{0.0f, -3.0f, -30.0f};
		glm::vec4 lightAmbient{ 0.5f, 0.5f, 0.5f, 1.0f };
		glm::vec4 lightDiffuse{ 0.5f, 0.5f, 0.5f, 1.0f };
		glm::vec4 lightSpecular{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec3 lightDirection{ 0.0f, 0.0f, 0.0f };
		float lightCutoff = 180.0f;

		// Stato del gioco
		std::vector<int> poles[3]; // 3 vettori che rappresentano i 3 pali
		int heldDisk = -1;         // -1 = mano vuota, altrimenti contiene l'ID del disco (grandezza)

   
	   size_t prev_time = 0;
	   KeyboardCallback keyboardCallback = nullptr;
	   
	   void initialize();
	   static void timerCallback(int value);
	   

	   

	   void  reshapeCallback(int width, int height);


   

   

   // Const/dest:
   Base();
};

}; // end of namespace Eng::

