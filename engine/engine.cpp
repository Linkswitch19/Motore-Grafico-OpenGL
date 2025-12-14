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
     
void drawShinyOrb(float rotationAngle) {

    // 1. Impostazione del Materiale Lucente e FLUORESCENTE

    // Componente AMBIENT e DIFFUSE (Possono essere spenti o molto scuri se l'emissione domina)
    GLfloat ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Molto scuro
    GLfloat diffuse[] = { 0.1f, 0.1f, 0.1f, 1.0f }; // Quasi spento

    // Componente SPECULAR e SHININESS (Per avere riflessi se c'è luce, ma la sfera emette luce)
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininess = 100.0f;

    // !!! NUOVO: Componente EMISSIONE (Il colore che la sfera emette da sé)
    // Scegli un colore vivido (es. Verde acido)
    GLfloat emission[] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Verde acido brillante 
    // Per un effetto più "fluo", puoi anche usare { 0.5f, 1.0f, 0.0f, 1.0f }

    // Applica le proprietà del materiale
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    // !!! APPLICA L'EMISSIONE !!!
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission); // <--- QUESTA È LA CHIAVE

    // 2. Applicazione della Trasformazione (Rotazione e Posizione)
    glPushMatrix();

    // Posiziona l'oggetto nello spazio (0, 5, -30)
    glTranslatef(0.0f, 5.0f, -30.0f);

    // Applica la rotazione corrente
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

    // 3. Disegna la primitiva (Sfera)
    glutSolidSphere(2.0, 32, 32);

    glPopMatrix();

    // !!! IMPORTANTE: RESETTA L'EMISSIONE !!!
    // Se non resettiamo l'emissione a zero dopo aver disegnato l'oggetto fluo, 
    // TUTTI gli oggetti successivi (inclusa la tua torre OVO) saranno disegnati 
    // con lo stesso colore emissivo, rovinando l'illuminazione del resto della scena.
    GLfloat zero_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zero_emission); // <--- RESETTA

}


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
    GLfloat redColor[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // Colore Luce
    GLfloat ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Nessuna luce ambientale diffusa

    glEnable(lightID); // Accendi l'interruttore
    glLightfv(lightID, GL_DIFFUSE, redColor);  // Luce che illumina gli oggetti opachi
    glLightfv(lightID, GL_SPECULAR, redColor); // Luce che fa i riflessi lucidi
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


    // 1. Carica la texture (FreeImage gestirà .dds, .png, .jpg, ecc.)
    // Assicurati che il file sia nella cartella dell'eseguibile!
    auto myTexture = std::make_shared<eng::Texture>("Wood094_1K-PNG_Color.dds");

    // 2. Applica la texture alla mesh.
    // Metodo A: Se sai che 'sceneRoot' è direttamente la Mesh:
    auto meshPtr = std::dynamic_pointer_cast<eng::Mesh>(reserved->sceneRoot);
    if (meshPtr) {
        if (!meshPtr->get_material()) meshPtr->set_material(std::make_shared<eng::Material>());
        meshPtr->get_material()->set_texture(myTexture);
        std::cout << "Texture applicata alla radice!" << std::endl;
    }
    // Metodo B: Se la mesh è figlia della radice (probabile con i file OVO complessi)
    else {
        for (auto& child : reserved->sceneRoot->get_children()) {
            auto childMesh = std::dynamic_pointer_cast<eng::Mesh>(child);
            if (childMesh) {
                if (!childMesh->get_material()) childMesh->set_material(std::make_shared<eng::Material>());
                childMesh->get_material()->set_texture(myTexture);
                std::cout << "Texture applicata al figlio: " << child->get_name() << std::endl;
                // Togli il break se vuoi texturizzare tutti i figli
                // break; 
            }
        }
    }

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
    // -----------------------------------------------------------
    // 1. PULIZIA E CALCOLI PRELIMINARI
    // -----------------------------------------------------------

    // Clear the screen:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calcolo Delta Time e aggiornamento animazione
    static int prev_t = glutGet(GLUT_ELAPSED_TIME);
    int curr_t = glutGet(GLUT_ELAPSED_TIME);
    float dt = (curr_t - prev_t) / 1000.0f;
    prev_t = curr_t;

    updateAnimation(dt); // Aggiorna l'angolo di rotazione


    // -----------------------------------------------------------
    // 2. SETUP DELLA TELECAMERA (VIEW MATRIX)
    // -----------------------------------------------------------
    // È fondamentale farlo PRIMA di disegnare qualsiasi oggetto 3D

    // Set perspective matrix:
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(perspective));

    // Set ModelView mode
    glMatrixMode(GL_MODELVIEW);

    // Ottieni e carica la matrice della Camera
    glm::mat4 viewMatrix = glm::mat4(1.0f); // Default Identity
    if (reserved->camera) {
        viewMatrix = reserved->camera->GetViewMatrix();
    }
    glLoadMatrixf(glm::value_ptr(viewMatrix)); // <--- LA TELECAMERA È ORA ATTIVA


    // -----------------------------------------------------------
    // 3. DISEGNO SFERE FLUO E POSIZIONAMENTO LUCI
    // -----------------------------------------------------------
    // Ora che la telecamera è settata, possiamo disegnare le sfere nella posizione giusta
    glDisable(GL_TEXTURE_2D);
    if (!reserved->orbsList.empty()) {

        // Colore FLUO della sfera (visivo)
        GLfloat neonRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

        // Attiva effetto NEON (Emissione)
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, neonRed);

        for (int i = 0; i < reserved->orbsList.size(); i++) {
            int lightID = GL_LIGHT0 + (i + 1);
            glm::vec3 pos = reserved->orbsList[i];

            // --- CALCOLO OSCILLAZIONE (BOBBING) ---
            // sin(tempo * velocità) * ampiezza
            // Velocità 2.0f = oscilla abbastanza veloce
            // Ampiezza 3.0f = sale e scende di 3 unità
            float verticalOffset = sin(reserved->totalTime * 2.0f) * 3.0f;

            glPushMatrix();

            // --- Spostamento ---
            // Aggiungiamo verticalOffset alla posizione Y originale
            glTranslatef(pos.x, pos.y + verticalOffset, pos.z);

            // --- Rotazione ---
            glRotatef(reserved->rotationAngleY, 0.0f, 1.0f, 0.0f);

            // --- Posizionamento Luce Reale ---
            // La luce segue la sfera (perché abbiamo traslato prima)
            GLfloat lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glLightfv(lightID, GL_POSITION, lightPos);

            // --- Disegno Sfera Fisica ---
            glutSolidSphere(2.0, 32, 32);

            glPopMatrix();
        }

        // Spegni effetto NEON (Reset obbligatorio per non rovinare la torre)
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
    }


    // -----------------------------------------------------------
    // 4. DISEGNO SCENA PRINCIPALE (OVO)
    // -----------------------------------------------------------

    // Set material properties (Valori di default per la scena)
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(materialAmbient));
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(materialDiffuse));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(materialSpecular));

    if (reserved->sceneRoot) {
        // Crea la matrice del Modello (Dove sta la torre nel mondo)
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, -40.0f));

        // ModelView = View * Model
        glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;

        // Chiama la funzione ricorsiva
        renderOvoNode(reserved->sceneRoot, modelViewMatrix);
    }


    // -----------------------------------------------------------
    // 5. INTERFACCIA 2D (HUD, TESTI, FPS)
    // -----------------------------------------------------------

    // Set orthographic projection:
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(ortho));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));

    // Disable lighting before rendering 2D text:
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Colore del testo (Bianco)
    glColor3f(1.0f, 1.0f, 1.0f);

    char buffer[128];
    float w = (float)glutGet(GLUT_WINDOW_WIDTH);
    float h = (float)glutGet(GLUT_WINDOW_HEIGHT);
    float textX = w - 90.0f;
    float textY = h - 50.0f;
    float stepY = 15.0f;

    // Stampa FPS
    sprintf_s(buffer, sizeof(buffer), "FPS: %d", fps);
    glRasterPos2f(textX, textY);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);

    // Stampa LOD
    textY += stepY;
    snprintf(buffer, sizeof(buffer), "LOD: %d", detail);
    glRasterPos2f(textX, textY);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);

    // Lista Comandi HUD
    for (auto& textItem : hudList) {
        textItem.render();
    }

    // Messaggio centrale (Giallo)
    if (!currentMessage.empty()) {
        glColor3f(1.0f, 1.0f, 0.0f); // Giallo
        glRasterPos2f(window_width / 2.0f - 50.0f, window_height - 50.0f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)currentMessage.c_str());
    }

    // -----------------------------------------------------------
    // 6. CHIUSURA FRAME
    // -----------------------------------------------------------

    // Reactivate lighting for next frame
    glEnable(GL_LIGHTING);

    // Swap buffers
    frames++;
    glutSwapBuffers();

    // Force rendering refresh
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

void ENG_API Eng::Base::addText(const eng::TextHUD& textObject)
{
    this->hudList.push_back(textObject);
}

void ENG_API Eng::Base::clearHUD()
{
    this->hudList.clear();
}











