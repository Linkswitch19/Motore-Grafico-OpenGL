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
   glutMainLoop();
   
   std::cout << "[application terminated]" << std::endl;
   return true;
}

void ENG_API Eng::Base::initialize()

{
    glClearColor(0.1, 0.1, 0.1, 5.0); //Background Color
    glShadeModel(GL_SMOOTH);		  //SMOOTH Shading
    glEnable(GL_DEPTH_TEST);		  //Enabling Depth Test

    //Setting Light0 parameters
    GLfloat light0_pos[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // A positional light
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);

    // Carica OVO
    reserved->sceneRoot = eng::OVOParser::from_file("torretina.ovo");
    if (!reserved->sceneRoot) std::cout << "Errore caricamento OVO" << std::endl;

    glEnable(GL_LIGHTING);			  //Enabling Lighting
    glEnable(GL_LIGHT0);		      //Enabling Light0	


    // --- INIZIALIZZAZIONE HANOI ---
    // Svuota tutto per sicurezza
    poles[0].clear();
    poles[1].clear();
    poles[2].clear();
    heldDisk = -1;

    // Riempiamo il primo palo (Palo 0) con 5 dischi
    // Li inseriamo dal più grande (0) al più piccolo (4) o viceversa.
    // Usiamo numeri interi per rappresentare la grandezza: 0 = più piccolo, 4 = più grande
    int numDiscs = 9;
    for (int i = numDiscs - 1; i >= 0; i--) {
        poles[0].push_back(i);
    }

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

    // Logica di gioco Hanoi
    int poleIndex = -1;

    if (key == '1') poleIndex = 0;
    else if (key == '2') poleIndex = 1;
    else if (key == '3') poleIndex = 2;

    if (poleIndex != -1)
    {
        // CASO 1: Mano vuota, provo a PRENDERE un disco
        if (heldDisk == -1)
        {
            if (!poles[poleIndex].empty())
            {
                heldDisk = poles[poleIndex].back(); // Prendo il disco in cima
                poles[poleIndex].pop_back();        // Lo rimuovo dal palo
                std::cout << "Preso disco " << heldDisk << " dal palo " << poleIndex + 1 << std::endl;
            }
        }
        // CASO 2: Ho un disco, provo a POSARLO
        else
        {
            // Regola: posso posare se il palo è vuoto OPPURE se il disco sotto è più grande
            // (Assumiamo che numeri più grandi siano dischi più grandi)
            if (poles[poleIndex].empty() || poles[poleIndex].back() > heldDisk)
            {
                poles[poleIndex].push_back(heldDisk);
                std::cout << "Posato disco " << heldDisk << " sul palo " << poleIndex + 1 << std::endl;
                heldDisk = -1; // Mano vuota
            }
            else
            {
                std::cout << "Mossa non valida! Il disco sotto e' troppo piccolo." << std::endl;
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

   

  

    // Set material properties:
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(materialAmbient));
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(materialDiffuse));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(materialSpecular));

    // Position and render the grid:
    glm::mat4 transGrid = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -10.0f, -50.0f));
    glLoadMatrixf(glm::value_ptr(transGrid));
    drawGrid(75.0f, (int)pow(8, detail));

    // Reimposta la matrice Identity per posizionare la Torre in coordinate mondo
    glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));


    // --- NUOVO DISEGNO ---
    if (reserved->sceneRoot) {
        // Sposta la torre dove vuoi (es. un po' indietro e in basso come l'originale)
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, -40.0f));

        // Chiama la funzione ricorsiva
        renderOvoNode(reserved->sceneRoot, modelMatrix);
    }



    // Disegna la Torre di Hanoi con 5 dischi
    //drawHanoi(9);

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

    // 3. Stampa ISTRUZIONI (Sopra LOD)
    textY += stepY;
    snprintf(buffer, sizeof(buffer), "Comandi: Premi 1, 2, 3 per spostare i dischi ogni numero corrisponde ad un palo");
    glRasterPos2f(textX, textY);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);


    // 4. Stampa STATO MANO (Sopra istruzioni - Molto utile!)
    textY += stepY;
    if (heldDisk != -1) {
        // Se hai un disco, scriviamo quale (Aggiungiamo un colore verde per evidenziare)
        glColor3f(0.5f, 1.0f, 0.5f);
        snprintf(buffer, sizeof(buffer), "Stato: Hai in mano il disco %d", heldDisk+1);
    }
    else {
        // Se la mano è vuota
        glColor3f(1.0f, 1.0f, 1.0f);
        snprintf(buffer, sizeof(buffer), "Stato: Mano vuota - Seleziona un palo");
    }
    glRasterPos2f(textX, textY);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);

    // Reactivate lighting:
    glEnable(GL_LIGHTING);

    // Swap this context's buffer:
    frames++;
    glutSwapBuffers();

    // Force rendering refresh:
    glutPostWindowRedisplay(this->reserved->windowId);
}

void ENG_API Eng::Base::update()
{
    glutMainLoopEvent();
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

/**
 * Renders a square grid on plane XZ made of triangles.
 * @param size size of the grid
 * @param tesselation number of triangles to use on each direction
 */
void ENG_API Eng::Base::drawGrid(float size, int tesselation)
{
    // Compute starting coordinates and step size:
    float startX = -size / 2.0f;
    float startZ = size / 2.0f;
    float triangleSize = size / (float)tesselation;

    // Normal is just one, set it now:
    glNormal3f(0.0f, 1.0f, 0.0f);

    // Go over XZ and draw triangles:
    for (int curZ = 0; curZ < tesselation; curZ++)
    {
        for (int curX = 0; curX < tesselation; curX++)
        {
            glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(startX, 0.0f, startZ);
            glVertex3f(startX + triangleSize, 0.0f, startZ);
            glVertex3f(startX, 0.0f, startZ - triangleSize);
            glVertex3f(startX + triangleSize, 0.0f, startZ - triangleSize);
            glEnd();

            startX += triangleSize;
        }
        startX = -size / 2.0f;
        startZ -= triangleSize;
    }
}


void ENG_API Eng::Base::reshapeCallback(int width, int height)
{
    // Update viewport size:
    glViewport(0, 0, width, height);

    // Refresh projection matrices:
    perspective = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, 100.0f);
    ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
}


// Funzione helper locale per disegnare un cilindro pieno orientato verso l'alto
void ENG_API Eng::Base::drawSolidCylinder(float radius, float height, int slices) {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    glPushMatrix();
    // Ruota per allineare all'asse Y (alto) invece che Z
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    // Disegna il tubo
    gluCylinder(quad, radius, radius, height, slices, 1);

    // Disegna il tappo inferiore (base)
    glPushMatrix();
    glScalef(-1, 1, 1); // Inverte le normali per il tappo sotto
    gluDisk(quad, 0.0, radius, slices, 1);
    glPopMatrix();

    // Disegna il tappo superiore
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0, radius, slices, 1);
    glPopMatrix();
    glPopMatrix();

    gluDeleteQuadric(quad);
}


void ENG_API Eng::Base::drawHanoi(int numDiscs) // numDiscs qui è ignorato, usiamo i vector
{
    float poleHeight = 10.0f;
    float poleRadius = 0.3f;
    float discHeight = 1.0f;
    float maxDiscRadius = 4.0f;
    glm::vec3 basePos(0.0f, -5.0f, -40.0f);

    // Colori
    glm::vec4 discColors[] = {
        {1.0f, 0.0f, 0.0f, 1.0f}, // Rosso (piccolo)
        {0.0f, 1.0f, 0.0f, 1.0f}, // Verde
        {0.0f, 0.0f, 1.0f, 1.0f}, // Blu
        {1.0f, 1.0f, 0.0f, 1.0f}, // Giallo
        {0.0f, 1.0f, 1.0f, 1.0f}, // Ciano (grande)
    };

    // 1. Disegna i 3 pali (STRUTTURA FISSA)
    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        glm::vec3 polePos = basePos;
        polePos.x += (i - 1) * 12.0f;
        glTranslatef(polePos.x, polePos.y, polePos.z);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glm::value_ptr(glm::vec4(0.6f, 0.4f, 0.2f, 1.0f)));
        drawSolidCylinder(poleRadius, poleHeight, 20);
        glPopMatrix();
    }

    // 2. Disegna i dischi presenti SUI PALI
    // Iteriamo sui 3 pali
    for (int p = 0; p < 3; p++)
    {
        float pX = basePos.x + (p - 1) * 12.0f; // Posizione X del palo corrente

        // Iteriamo sui dischi di questo palo
        for (size_t i = 0; i < poles[p].size(); i++)
        {
            int discID = poles[p][i]; // 0 è piccolo, 4 è grande

            // Calcolo raggio: discID 0 = raggio min, discID 4 = raggio max
            // Mappiamo: Raggio = Base + (ID * fattore)
            float currentRadius = 1.5f + (discID * 0.6f);

            glPushMatrix();
            // Y = base + altezza disco * indice nello stack
            float yPos = basePos.y + (i * discHeight);

            glTranslatef(pX, yPos, basePos.z);

            // Colore basato sull'ID del disco
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glm::value_ptr(discColors[discID % 5]));

            drawSolidCylinder(currentRadius, discHeight, 30);
            glPopMatrix();
        }
    }

    // 3. Disegna il disco "IN MANO" (se presente)
    if (heldDisk != -1)
    {
        float currentRadius = 1.5f + (heldDisk * 0.6f);
        glPushMatrix();
        // Lo disegniamo in alto al centro (o sopra l'ultimo palo usato, ma centro è più semplice)
        // Y = 10.0f (fluttuante in aria)
        glTranslatef(0.0f, 10.0f, basePos.z);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glm::value_ptr(discColors[heldDisk % 5]));
        drawSolidCylinder(currentRadius, discHeight, 30);
        glPopMatrix();
    }
}

