/**
 * @file		engine.cpp
 * @brief	Graphics engine main file
 *
 * @author	Achille Peternier (C) SUPSI [achille.peternier@supsi.ch] << change this to your group members
 */



//////////////
// #INCLUDE //
//////////////

   // Main include:
    #include "engine.h"
    #include "light.h"
    #include <glm/glm.hpp>
    #include <glm/gtc/type_ptr.hpp>
    #include<glm/gtc/matrix_transform.hpp>
    #include <GL/freeglut.h>
    #include "ovo_parser.h"
    #include "mesh.h"      
    #include "material.h"  
    #include "texture.h"   
    #include "camera.h"
    #include <tuple>       
   

    

   
   // C/C++:
       #include <iostream>   
       #include <source_location>
     



/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Base class reserved structure (using PIMPL/Bridge design pattern https://en.wikipedia.org/wiki/Opaque_pointer).
 */
struct Eng::Base::Reserved
{
   // Flags:
   bool initFlag;
   float clearColor[3] = { 0.f, 0.f, 0.f };
   int windowId;
   std::shared_ptr<eng::Node> sceneRoot = nullptr;
   std::unique_ptr<eng::Camera> camera = nullptr;
   
   

   /**
    * Constructor.
    */
   Reserved() : initFlag{ false } 
   {}
};



////////////////////////
// BODY OF CLASS Base //
////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Base::Base() : reserved(std::make_unique<Eng::Base::Reserved>())
{  
#ifdef _DEBUG   
   std::cout << "[+] " << std::source_location::current().function_name() << " invoked" << std::endl;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Base::~Base()
{
#ifdef _DEBUG
   std::cout << "[-] " << std::source_location::current().function_name() << " invoked" << std::endl;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets a reference to the (unique) singleton instance.
 * @return reference to singleton instance
 */
Eng::Base ENG_API &Eng::Base::getInstance()
{
   static Base instance;
   return instance;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Init internal components. 
 * @return TF
 */
bool ENG_API Eng::Base::init(const char* title, unsigned int width, unsigned int height, int argc, char* argv[])
{
   
   // Already initialized?
   if (reserved->initFlag)
   {
      std::cout << "ERROR: engine already initialized" << std::endl;
      return false;
   }



    glutInit(&argc, argv); 
    std::cout << "ENGINE INITIALIZED" << std::endl;
   
   
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowPosition(100, 100);

   this->window_width = width;
   this->window_height = height;
   this->fps_show = false;
   glutInitWindowSize(width, height);  // dimensione finestra
   reserved->windowId = glutCreateWindow(title);
   std::cout << "   renderer . . : " << glGetString(GL_RENDERER) << std::endl;

   
   glutDisplayFunc([](void) { Base::getInstance().onDisplay(); });
   glutReshapeFunc([](int w, int h) {Base::getInstance().reshapeCallback(w,h);});

   glutTimerFunc(1000, timerCallback, 0);

   //flag opzionali

   glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
   glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f); // default 0

   glEnable(GL_DEPTH_TEST);
   glDisable(GL_LIGHTING);
   glEnable(GL_NORMALIZE);
   glEnable(GL_CULL_FACE);
   glEnable(GL_TEXTURE_2D);
   //vettore per la luce ambientale
   glm::vec4 gAmbient(1.0f, 1.0f, 1.0f, 1.0f);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, glm::value_ptr(gAmbient));

   glShadeModel(GL_SMOOTH);

   glutKeyboardFunc([](unsigned char key, int x, int y) {
       Base::getInstance().onKeyPressed(key, x, y);
       });



   // Here you can initialize most of the graphics engine's dependencies and default settings...
   
   
   
   glutDisplayFunc([](void) { Base::getInstance().onDisplay(); });

   glutIdleFunc([]() { glutPostRedisplay(); });
   // Done:
   std::cout << "[>] " << LIB_NAME << " initialized" << std::endl;
   reserved->initFlag = true;
   initialize();
   
   
   std::cout << "[application terminated]" << std::endl;
   return true;
}



void ENG_API Eng::Base::startLoop() {
    glutMainLoop();
}

void ENG_API Eng::Base::initialize()

{
    glClearColor(0.1, 0.1, 0.1, 5.0); //Background Color
    glShadeModel(GL_SMOOTH);		  //SMOOTH Shading
    glEnable(GL_DEPTH_TEST);		  //Enabling Depth Test

    //Setting Light0 parameters
    GLfloat light0_pos[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // A positional light
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    this->reserved->camera = std::make_unique<eng::Camera>(glm::vec3(0.0f, 10.0f, 50.0f));


    // Carica OVO
    reserved->sceneRoot = eng::OVOParser::from_file("torretina.ovo");
    if (!reserved->sceneRoot) std::cout << "Errore caricamento OVO" << std::endl;

    glEnable(GL_LIGHTING);			  //Enabling Lighting
    glEnable(GL_LIGHT0);		      //Enabling Light0	



    //Globals initializations
    prev_time = glutGet(GLUT_ELAPSED_TIME);

   
}


void ENG_API Eng::Base::clearWindow()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ENG_API Eng::Base::setBackgroundColor(float r, float g, float b)
{
    reserved->clearColor[0] = r;
    reserved->clearColor[1] = g;
    reserved->clearColor[2] = b;
   
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Free internal components.
 * @return TF
 */
bool ENG_API Eng::Base::free()
{
   // Not initialized?
   if (!reserved->initFlag)
   {
      std::cout << "ERROR: engine not initialized" << std::endl;
      return false;
   }

   // Here you can properly dispose of any allocated resource (including third-party dependencies)...

   // Done:
   std::cout << "[<] " << LIB_NAME << " deinitialized" << std::endl;
   reserved->initFlag = false;
   return true;
}

void ENG_API Eng::Base::swapBuffers()
{
    glutSwapBuffers();
    glutPostRedisplay();
}

void ENG_API Eng::Base::setKeyboardCallback(KeyboardCallback cb)
{
    this->keyboardCallback = cb;
}

void ENG_API Eng::Base::onKeyPressed(unsigned char key, int x, int y) {
    glutPostRedisplay(); // <--- FORZA L'AGGIORNAMENTO DELLO SCHERMO


    // Velocità di movimento (o delta time se lo calcoli)
    float dt = 1.0f; // Valore arbitrario, se hai un calcolo del deltaTime usalo qui

    // Gestione Camera WASD
    if (reserved->camera) {
        // Usa tolower per accettare sia maiuscole che minuscole
        switch (tolower(key)) {
        case 'w':
            reserved->camera->ProcessKeyboard(eng::FORWARD, dt);
            break;
        case 's':
            reserved->camera->ProcessKeyboard(eng::BACKWARD, dt);
            break;
        case 'a':
            reserved->camera->ProcessKeyboard(eng::LEFT, dt);
            break;
        case 'd':
            reserved->camera->ProcessKeyboard(eng::RIGHT, dt);
            break;
        case 'q': // Opzionale: Sali
            reserved->camera->ProcessKeyboard(eng::UP, dt);
            break;
        case 'e': // Opzionale: Scendi
            reserved->camera->ProcessKeyboard(eng::DOWN, dt);
            break;


        case 'j':
        {
            // Invece di ruotare solo la testa, ci spostiamo DI LATO alla torre
            // La torre è a Z = -40. 
            // Posizione Nuova: X = 50, Z = -40 (Di lato alla torre)
            // Yaw = -180 (Guarda verso sinistra, verso la torre)

            static bool isSideView = false;

            if (!isSideView) {
                // Vai di lato
                reserved->camera = std::make_unique<eng::Camera>(
                    glm::vec3(60.0f, 10.0f, -40.0f), // Posizionati a destra della torre
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    -180.0f, // Guarda verso sinistra (dove sta la torre)
                    0.0f
                );
                isSideView = true;
            }
            else {
                // Torna frontale
                reserved->camera = std::make_unique<eng::Camera>(
                    glm::vec3(0.0f, 10.0f, 50.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    -90.0f,
                    0.0f
                );
                isSideView = false;
            }
            break;
        }

        case 'k':
            // Ruota di 90 gradi verso destra
            reserved->camera->RotateBy(-90.0f);
            std::cout << "Ruotato di 90 gradi a destra" << std::endl;
            break;
        case 'u':
        {
            // Variabile statica: mantiene il valore tra una pressione e l'altra
            static bool isTopView = false;

            if (!isTopView) {
                // --- ATTIVA VISTA DALL'ALTO ---
                reserved->camera = std::make_unique<eng::Camera>(
                    glm::vec3(0.0f, 60.0f, -40.0f), // Sopra la torre
                    glm::vec3(0.0f, 1.0f, 0.0f),
                    -90.0f,
                    -89.0f  // Guarda in giù
                );
                std::cout << "Visuale: ALTO" << std::endl;
                isTopView = true;
            }
            else {
                // --- RESETTA VISTA NORMALE ---
                // Torna alla posizione definita in initialize()
                reserved->camera = std::make_unique<eng::Camera>(
                    glm::vec3(0.0f, 10.0f, 50.0f)
                    // I parametri di default (Yaw -90, Pitch 0) raddrizzano la visuale
                );
                std::cout << "Visuale: NORMALE" << std::endl;
                isTopView = false;
            }
            break;
        }


        }


    }


   


    if (keyboardCallback) keyboardCallback(key, x, y);
}




void renderOvoNode(std::shared_ptr<eng::Node> node, glm::mat4 parentMatrix) {
    if (!node) return;

    // 1. Calcola matrice globale
    glm::mat4 globalMatrix = parentMatrix * node->get_base_matrix();

    // 2. Controlla se è una Mesh
    std::shared_ptr<eng::Mesh> mesh = std::dynamic_pointer_cast<eng::Mesh>(node);

    if (mesh) {
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(globalMatrix));

        // Setup Materiale
        if (mesh->get_material()) {
            auto mat = mesh->get_material();

            // Imposta colori
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(glm::vec4(mat->get_ambient_color(), 1.0f)));
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(glm::vec4(mat->get_diffuse_color(), 1.0f)));
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(glm::vec4(mat->get_specular_color(), 1.0f)));
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat->get_shininess());

            // Gestione Texture
            if (mat->get_texture()) {
                glEnable(GL_TEXTURE_2D);
                // IMPORTANTE: Assicurati che la tua classe Texture abbia un metodo per ottenere l'ID
                // Se la texture non si vede, potrebbe mancare il bind qui, es:
                // glBindTexture(GL_TEXTURE_2D, mat->get_texture()->get_texture_id()); 
            }
            else {
                glDisable(GL_TEXTURE_2D);
            }
        }

        // Setup Geometry
        const auto& verts = mesh->get_vertices();
        const auto& norms = mesh->get_normals();
        const auto& uvs = mesh->get_uvs();
        const auto& faces = mesh->get_faces();

        glBegin(GL_TRIANGLES);
        for (const auto& face : faces) {
            // Estrai indici
            uint32_t i0 = std::get<0>(face);
            uint32_t i1 = std::get<1>(face);
            uint32_t i2 = std::get<2>(face);

            // Vertice 0
            if (i0 < norms.size()) glNormal3fv(glm::value_ptr(norms[i0]));
            if (i0 < uvs.size()) glTexCoord2fv(glm::value_ptr(uvs[i0]));
            if (i0 < verts.size()) glVertex3fv(glm::value_ptr(verts[i0]));

            // Vertice 1
            if (i1 < norms.size()) glNormal3fv(glm::value_ptr(norms[i1]));
            if (i1 < uvs.size()) glTexCoord2fv(glm::value_ptr(uvs[i1]));
            if (i1 < verts.size()) glVertex3fv(glm::value_ptr(verts[i1]));

            // Vertice 2
            if (i2 < norms.size()) glNormal3fv(glm::value_ptr(norms[i2]));
            if (i2 < uvs.size()) glTexCoord2fv(glm::value_ptr(uvs[i2]));
            if (i2 < verts.size()) glVertex3fv(glm::value_ptr(verts[i2]));
        }
        glEnd();

        glPopMatrix();
    }

    // 3. Ricorsione sui figli
    for (const auto& child : node->get_children()) {
        renderOvoNode(child, globalMatrix);
    }
}




void ENG_API Eng::Base::onDisplay()
{
    // Clear the screen:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    ////////////////
    // 3D rendering:

    // Set perpsective matrix:
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(perspective));
    glMatrixMode(GL_MODELVIEW);

    // 1. Ottieni la matrice della Camera (View Matrix)
    glm::mat4 viewMatrix = glm::mat4(1.0f); // Default Identity
    if (reserved->camera) {
        viewMatrix = reserved->camera->GetViewMatrix();
    }
    if (!currentMessage.empty()) {
    glColor3f(1.0f, 1.0f, 0.0f); // Giallo
    glRasterPos2f(window_width / 2.0f - 50.0f, window_height - 50.0f); // Posizione (Centrato in alto)
    
    // Disegna la stringa
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)currentMessage.c_str());
}

   

  

    // Set material properties:
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(materialAmbient));
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(materialDiffuse));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(materialSpecular));


  


    // --- NUOVO DISEGNO ---
    if (reserved->sceneRoot) {
        // 2. Crea la matrice del Modello (Dove sta la torre nel mondo)
        // La spostiamo a (0, -5, -40)
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, -40.0f));

        // 3. CALCOLO FONDAMENTALE: ModelView = View * Model
        // Moltiplichiamo la camera per il modello. 
        // In questo modo passiamo a renderOvoNode una matrice che contiene SIA la posizione della torre SIA la posizione della camera.
        glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;

        // Chiama la funzione ricorsiva passando la matrice combinata
        renderOvoNode(reserved->sceneRoot, modelViewMatrix);
    }



  

    //////////////////////////
    // Switch to 2D rendering:

    // Set orthographic projection:
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(ortho));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));

    // Disable lighting before rendering 2D text:
    glDisable(GL_LIGHTING);

    // Colore del testo (Bianco)
    glColor3f(1.0f, 1.0f, 1.0f);

    char buffer[128];
    float textX = 10.0f; // Margine sinistro
    float textY = 20.0f; // Altezza iniziale dal basso
    float stepY = 15.0f; // Spazio tra le righe

    // 1. Stampa FPS (In basso)
    sprintf_s(buffer, sizeof(buffer), "FPS: %d", fps);
    glRasterPos2f(textX, textY);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);

    // 2. Stampa LOD (Sopra FPS)
    textY += stepY;
    snprintf(buffer, sizeof(buffer), "LOD: %d", detail);
    glRasterPos2f(textX, textY);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);
    // Reactivate lighting:
    glEnable(GL_LIGHTING);

    // Swap this context's buffer:
    frames++;
    if (!currentMessage.empty()) {
        glColor3f(1.0f, 1.0f, 0.0f); // Giallo
        glRasterPos2f(window_width / 2.0f - 50.0f, window_height - 50.0f); // Posizione (Centrato in alto)

        // Disegna la stringa
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)currentMessage.c_str());
    }

    glutSwapBuffers();

    // Force rendering refresh:
    glutPostWindowRedisplay(this->reserved->windowId);
}

void ENG_API Eng::Base::update()
{
    glutMainLoopEvent();
}

std::shared_ptr<eng::Node> findParentOf(std::shared_ptr<eng::Node> current, const std::string& childName) {
    if (!current) return nullptr;

    for (auto& child : current->get_children()) {
        // Se questo child è quello che cerchiamo, allora 'current' è il padre!
        if (child->get_name() == childName) {
            return current;
        }

        // Altrimenti cerca nei nipoti (ricorsione)
        auto result = findParentOf(child, childName);
        if (result) return result;
    }
    return nullptr;
}

void ENG_API Eng::Base::setParent(const std::string& childName, const std::string& parentName)
{
    if (!reserved->sceneRoot) return;

    // 1. Trova il nodo figlio e il suo ATTUALE padre
    auto currentParent = findParentOf(reserved->sceneRoot, childName);
    if (!currentParent) return; // Nodo non trovato o orfano

    // 2. Trova il puntatore al nodo figlio
    std::shared_ptr<eng::Node> childNode = nullptr;
    auto& siblings = currentParent->get_children();

    // Rimuovi dal vecchio padre
    for (auto it = siblings.begin(); it != siblings.end(); ) {
        if ((*it)->get_name() == childName) {
            childNode = *it;
            it = siblings.erase(it); // <--- LO STACCHI QUI
            break;
        }
        else {
            ++it;
        }
    }

    if (!childNode) return; // Errore strano, non trovato nella lista

    // 3. Trova il NUOVO padre
    std::shared_ptr<eng::Node> newParentNode = getNode(parentName);
    if (!newParentNode) {
        // Se il nuovo padre non esiste, riattacca al vecchio per non rompere tutto
        currentParent->add_child(childNode);
        std::cout << "Errore: Nuovo padre " << parentName << " non trovato!" << std::endl;
        return;
    }

    // 4. ATTACCA AL NUOVO PADRE
    newParentNode->add_child(childNode);
    std::cout << "Reparenting: " << childName << " ora è figlio di " << parentName << std::endl;
}

void ENG_API Eng::Base::timerCallback(int value)
{
    Eng::Base& eng = Eng::Base::getInstance();

    // Aggiorna FPS
    eng.fps = eng.frames;
    eng.frames = 0;

    // Register the next update:
    glutTimerFunc(1000, timerCallback, 0);
}
std::shared_ptr<eng::Node> findNodeRecursive(std::shared_ptr<eng::Node> current, const std::string& name) {
    if (current->get_name() == name) return current;
    for (auto& child : current->get_children()) {
        auto res = findNodeRecursive(child, name);
        if (res) return res;
    }
    return nullptr;
}
std::shared_ptr<eng::Node> ENG_API Eng::Base::getNode(const std::string& name) {
    if (!reserved->sceneRoot) return nullptr;
    return findNodeRecursive(reserved->sceneRoot, name);
}




void ENG_API Eng::Base::reshapeCallback(int width, int height)
{
    // Update viewport size:
    glViewport(0, 0, width, height);

    // Refresh projection matrices:
    perspective = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, 100.0f);
    ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
}

void ENG_API Eng::Base::setMessage(const std::string& msg) {
    this->currentMessage = msg;
}







