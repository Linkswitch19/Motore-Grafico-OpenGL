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
    #define FREEIMAGE_LIB 
    #include <FreeImage.h>
    #include "ovo_parser.h"
    #include "mesh.h"      
    #include "material.h"  
    #include "texture.h"   
    #include "camera.h"
    #include <tuple>      
    #include <vector>
   

    

   
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
   std::vector<glm::vec3> orbsList;
   float rotationAngleY = 0.0f;
   float totalTime = 0.0f;
   
   

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

void ENG_API Eng::Base::updateAnimation(float deltaTime) {
    reserved->rotationAngleY += 50.0f * deltaTime;
    reserved->totalTime += deltaTime;
}



void ENG_API Eng::Base::createOrb(float x, float y, float z)
{
    // OpenGL supporta max 8 luci (GL_LIGHT0 -> GL_LIGHT7).
    // Usiamo GL_LIGHT0 per il sole, quindi abbiamo spazio per 7 sfere (1-7).
    if (reserved->orbsList.size() >= 7) {
        std::cout << "MAX LUCI RAGGIUNTO" << std::endl;
        return;
    }

    // Aggiungi posizione alla lista
    reserved->orbsList.push_back(glm::vec3(x, y, z));

    // Calcola ID luce: La prima sfera sarà GL_LIGHT1, la seconda GL_LIGHT2...
    int lightID = GL_LIGHT0 + reserved->orbsList.size();

    // Configura la luce ROSSA
    GLfloat Color[] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Colore Luce
    GLfloat ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Nessuna luce ambientale diffusa

    glEnable(lightID); // Accendi l'interruttore
    glLightfv(lightID, GL_DIFFUSE, Color);  // Luce che illumina gli oggetti opachi
    glLightfv(lightID, GL_SPECULAR, Color); // Luce che fa i riflessi lucidi
    glLightfv(lightID, GL_AMBIENT, ambient);

    // Imposta attenuazione (la luce diminuisce con la distanza)
    glLightf(lightID, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(lightID, GL_LINEAR_ATTENUATION, 0.1f);
    glLightf(lightID, GL_QUADRATIC_ATTENUATION, 0.02f);

    std::cout << "Sfera creata. Luce attiva ID: " << (lightID - GL_LIGHT0) << std::endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Init internal components. 
 * @return TF
 */
bool ENG_API Eng::Base::init(const char* title, unsigned int width, unsigned int height, int argc, char* argv[])
{
    FreeImage_Initialise();
    
   
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
    glClearColor(0.1, 0.1, 0.1, 5.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    GLfloat light0_pos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    this->reserved->camera = std::make_unique<eng::Camera>(glm::vec3(0.0f, 10.0f, 50.0f));

    // Carica OVO
    reserved->sceneRoot = eng::OVOParser::from_file("torretina.ovo");
    if (!reserved->sceneRoot) std::cout << "Errore caricamento OVO" << std::endl;

    // --- NOTA: Ho tolto tutto il codice della texture da qui! ---

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

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

   FreeImage_DeInitialise();

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
  // 1. Forza aggiornamento grafico
    glutPostRedisplay(); 

    // 2. Se l'utente (il main) ha definito una funzione per i tasti, chiamala!
    if (keyboardCallback) {
        keyboardCallback(key, x, y);
    }

   
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
               
                glBindTexture(GL_TEXTURE_2D, mat->get_texture()->get_texture_id()); 
                glColor3f(1.0f, 1.0f, 1.0f);
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
    // 1. Pulizia Schermo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Calcolo Delta Time
    static int prev_t = glutGet(GLUT_ELAPSED_TIME);
    int curr_t = glutGet(GLUT_ELAPSED_TIME);
    float dt = (curr_t - prev_t) / 1000.0f;
    prev_t = curr_t;

    updateAnimation(dt);

    // 3. Fase 3D
    setupCamera();  // Imposta Proiezione e Vista
    renderOrbs();   // Disegna le luci/sfere
    renderScene();  // Disegna il livello (OVO)

    // 4. Fase 2D
    renderHUD();    // Disegna testi e interfaccia

    // 5. Chiusura Frame
    frames++;
    glutSwapBuffers();
    glutPostWindowRedisplay(this->reserved->windowId);
}



void Eng::Base::setupCamera() {
    // Set perspective matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(perspective));

    // Set ModelView matrix
    glMatrixMode(GL_MODELVIEW);

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    if (reserved->camera) {
        viewMatrix = reserved->camera->GetViewMatrix();
    }
    glLoadMatrixf(glm::value_ptr(viewMatrix));
}

void Eng::Base::renderOrbs() {
    // Importante: Spegni le texture per gli oggetti "puri" come le luci
    glDisable(GL_TEXTURE_2D);

    if (reserved->orbsList.empty()) return;

    GLfloat neon[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    // Attiva emissione
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, neon);

    for (int i = 0; i < reserved->orbsList.size(); i++) {
        int lightID = GL_LIGHT0 + (i + 1);
        glm::vec3 pos = reserved->orbsList[i];

        // Bobbing animation
        float verticalOffset = sin(reserved->totalTime * 2.0f) * 3.0f;

        glPushMatrix();
        glTranslatef(pos.x, pos.y + verticalOffset, pos.z);
        glRotatef(reserved->rotationAngleY, 0.0f, 1.0f, 0.0f);

        // Posiziona la luce fisica
        GLfloat lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightfv(lightID, GL_POSITION, lightPos);

        // Disegna la sfera
        glutSolidSphere(2.0, 32, 32);
        glPopMatrix();
    }

    // Resetta emissione
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
}

void Eng::Base::renderScene() {
    // Riabilita il lighting (nel caso fosse stato spento)
    glEnable(GL_LIGHTING);

    // Proprietà materiale di default
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(materialAmbient));
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(materialDiffuse));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(materialSpecular));

    if (reserved->sceneRoot) {
        // Matrice di posizione del mondo
        // Se hai la matrice View già caricata in setupCamera(), qui serve solo la Model
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, -40.0f));

        // OpenGL Fixed Pipeline lavora moltiplicando la matrice corrente.
        // Poiché in setupCamera abbiamo caricato la VIEW, qui carichiamo la MODEL.
        // Per renderOvoNode che usa glLoadMatrix, dobbiamo passargli View * Model.

        // Recuperiamo la View Matrix corrente (salvata nel camera)
        glm::mat4 viewMatrix = glm::mat4(1.0f);
        if (reserved->camera) viewMatrix = reserved->camera->GetViewMatrix();

        glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;

        renderOvoNode(reserved->sceneRoot, modelViewMatrix);
    }
}

void Eng::Base::renderHUD() {
    // Setup 2D
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(ortho));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));

    // Disabilita effetti 3D
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D); // FONDAMENTALE

    glColor3f(1.0f, 1.0f, 1.0f); // Bianco

    char buffer[128];
    float w = (float)glutGet(GLUT_WINDOW_WIDTH);
    float h = (float)glutGet(GLUT_WINDOW_HEIGHT);
    float textX = w - 90.0f;
    float textY = h - 50.0f;
    float stepY = 15.0f;

    // FPS
    sprintf_s(buffer, sizeof(buffer), "FPS: %d", fps);
    glRasterPos2f(textX, textY);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);

    // LOD
    textY += stepY;
    snprintf(buffer, sizeof(buffer), "LOD: %d", detail);
    glRasterPos2f(textX, textY);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);

    // Lista HUD personalizzata
    for (auto& textItem : hudList) {
        textItem.render();
    }

    // Messaggio centrale
    if (!currentMessage.empty()) {
        glColor3f(1.0f, 1.0f, 0.0f); // Giallo
        glRasterPos2f(w / 2.0f - 50.0f, h - 50.0f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)currentMessage.c_str());
    }

    // Ripristina stato per il prossimo frame 3D
    glEnable(GL_LIGHTING);
}



void ENG_API Eng::Base::update()
{
    glutMainLoopEvent();
}
// per scegliere il palo giusto
std::shared_ptr<eng::Node> findParentOf(std::shared_ptr<eng::Node> current, const std::string& childName) {
    if (!current) return nullptr;

    for (auto& child : current->get_children()) {
        
        if (child->get_name() == childName) {
            return current;
        }

       
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
    perspective = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, 500.0f); //quanto si puo vedere
    ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
}

void ENG_API Eng::Base::setMessage(const std::string& msg) {
    this->currentMessage = msg;
}

void ENG_API Eng::Base::addText(const eng::TextHUD& textObject)
{
    this->hudList.push_back(textObject);
}

void ENG_API Eng::Base::clearHUD()
{
    this->hudList.clear();
}

void ENG_API Eng::Base::loadTexture(const std::string& textureFile, const std::string& objectName)
{
    // 1. Cerca l'oggetto nella scena per nome
    auto node = getNode(objectName);

    if (!node) {
        std::cout << "[ERRORE] Nodo non trovato: " << objectName << std::endl;
        return;
    }

    // 2. Controlla che sia una Mesh (solo le mesh hanno materiali)
    auto mesh = std::dynamic_pointer_cast<eng::Mesh>(node);
    if (!mesh) {
        std::cout << "[ERRORE] L'oggetto '" << objectName << "' non e' una Mesh (forse e' un gruppo/nodo vuoto?)" << std::endl;
        return;
    }

    // 3. Carica la Texture
    auto newTexture = std::make_shared<eng::Texture>(textureFile);

    // 4. Assegna il materiale
    if (!mesh->get_material()) {
        mesh->set_material(std::make_shared<eng::Material>());
    }

    mesh->get_material()->set_texture(newTexture);

    // Imposta il colore a bianco per far vedere bene la texture
    mesh->get_material()->set_diffuse_color(glm::vec3(1.0f, 1.0f, 1.0f));

    std::cout << "[OK] Texture " << textureFile << " applicata a: " << objectName << std::endl;
}


void ENG_API Eng::Base::moveCamera(eng::Camera_Movement direction, float deltaTime) {
    if (reserved->camera) {
        reserved->camera->ProcessKeyboard(direction, deltaTime);
    }
}

void ENG_API Eng::Base::rotateCamera(float angle) {
    if (reserved->camera) {
        reserved->camera->RotateBy(angle);
    }
}

void ENG_API Eng::Base::setCameraPosition(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch) {
    
    reserved->camera = std::make_unique<eng::Camera>(position, up, yaw, pitch);
}











