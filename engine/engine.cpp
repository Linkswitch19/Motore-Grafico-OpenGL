//////////////
// #INCLUDE //
//////////////

#include "engine.h"
#include "camera.h"

#include <GL/freeglut.h>
#define FREEIMAGE_LIB 
#include <FreeImage.h>
#include <iostream>
#include <source_location>
#include <glm/gtc/type_ptr.hpp>

// Includi i tuoi nuovi manager
#include "scene_manager.h"
#include "light_manager.h"
#include "hud_manager.h"
#include "renderer.h"

//xd


/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Struttura privata (Pimpl idiom) per nascondere l'implementazione del motore.
 * Contiene i flag di stato, i manager dei sottosistemi, e le variabili della telecamera.
 */
struct Eng::Base::Reserved
{
    bool initFlag = false;
    int windowId = -1;
    float clearColor[3] = { 0.1f, 0.1f, 0.1f };

    // Sottosistemi delegati
    std::unique_ptr<eng::SceneManager> sceneManager;
    std::unique_ptr<eng::LightManager> lightManager;
    std::unique_ptr<eng::HUDManager>   hudManager;
    std::unique_ptr<eng::Renderer>     renderer;

    // Telecamera e animazioni
    std::unique_ptr<eng::Camera> camera;
    float rotationAngleY = 0.0f;
    float totalTime = 0.0f;

    Reserved() {
        sceneManager = std::make_unique<eng::SceneManager>();
        lightManager = std::make_unique<eng::LightManager>();
        hudManager = std::make_unique<eng::HUDManager>();
        renderer = std::make_unique<eng::Renderer>();
    }
};


////////////////////////
// BODY OF CLASS Base //
////////////////////////

/**
 * @brief Costruttore del motore. Inizializza la struttura riservata.
 */
ENG_API Eng::Base::Base() : reserved(std::make_unique<Eng::Base::Reserved>()) {
#ifdef _DEBUG    
    DEBUG("%s invoked", std::source_location::current().function_name());
#endif
}

/**
 * @brief Distruttore del motore.
 */
ENG_API Eng::Base::~Base() {
#ifdef _DEBUG
    DEBUG("%s invoked", std::source_location::current().function_name());
#endif
}

/**
 * @brief Restituisce l'istanza Singleton del motore grafico.
 * @return Eng::Base& Riferimento all'unica istanza di Base.
 */
Eng::Base ENG_API& Eng::Base::getInstance() {
    static Base instance;
    return instance;
}

/**
 * @brief Inizializza il motore, le librerie (FreeImage, GLUT) e la finestra OpenGL.
 * @param title Il titolo della finestra.
 * @param width La larghezza della finestra in pixel.
 * @param height L'altezza della finestra in pixel.
 * @param argc Argomenti da riga di comando per GLUT.
 * @param argv Argomenti da riga di comando per GLUT.
 * @return true Se l'inizializzazione è avvenuta con successo, false altrimenti.
 */
bool ENG_API Eng::Base::init(const char* title, unsigned int width, unsigned int height, int argc, char* argv[]) {
    FreeImage_Initialise();

    if (reserved->initFlag) {
        ERROR("Engine already initialized");
        return false;
    }

    glutInit(&argc, argv);
    std::cout << "[INFO] Engine Initialized" << std::endl;

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowPosition(100, 100);

    this->window_width = width;
    this->window_height = height;
    this->fps_show = false;
    glutInitWindowSize(width, height);
    reserved->windowId = glutCreateWindow(title);

    std::cout << "[INFO] Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // Callbacks setup
    glutDisplayFunc([](void) { Base::getInstance().onDisplay(); });
    glutReshapeFunc([](int w, int h) {Base::getInstance().reshapeCallback(w, h); });
    glutTimerFunc(1000, timerCallback, 0);

    // OpenGL State configuration
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_NORMALIZE); // opgl normalizza tutto se no scena rip
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    // luce ambientale
    glm::vec4 gAmbient(0.5f, 0.5f, 0.5f, 1.0f);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, glm::value_ptr(gAmbient));
    glShadeModel(GL_SMOOTH);

    glutKeyboardFunc([](unsigned char key, int x, int y) {
        Base::getInstance().onKeyPressed(key, x, y);
        });
    glutIdleFunc([]() { glutPostRedisplay(); });

    std::cout << "[>] " << LIB_NAME << " initialized" << std::endl;
    reserved->initFlag = true;
    initialize();

    return true;
}

/**
 * @brief Imposta lo stato iniziale interno del motore, crea la telecamera di default e carica la scena base.
 */
void ENG_API Eng::Base::initialize() {
    glClearColor(reserved->clearColor[0], reserved->clearColor[1], reserved->clearColor[2], 1.0f);

    reserved->camera = std::make_unique<eng::Camera>(glm::vec3(0.0f, 10.0f, 50.0f));

    // DELEGA: Caricamento scena (assicurati che il file OVO esista in questa cartella)
    reserved->sceneManager->loadScene("torretina.ovo");

    glEnable(GL_LIGHTING);


    prev_time = glutGet(GLUT_ELAPSED_TIME);
}

/**
 * @brief Avvia il ciclo principale (main loop) di GLUT.
 */
void ENG_API Eng::Base::startLoop() {
    glutMainLoop();
}

/**
 * @brief De-inizializza il motore e libera le risorse allocate (es. FreeImage).
 * @return true Se de-inizializzato correttamente, false se non era inizializzato.
 */
bool ENG_API Eng::Base::free() {
    if (!reserved->initFlag) return false;
    FreeImage_DeInitialise();
    std::cout << "[<] " << LIB_NAME << " deinitialized" << std::endl;
    reserved->initFlag = false;
    return true;
}


// --- DELEGAZIONI PER LUCI E OMBRE ---

/**
 * @brief Delega la creazione di una sfera luminosa rotante (Orb) al LightManager.
 */
void ENG_API Eng::Base::createOrb(float x, float y, float z) {
    reserved->lightManager->createOrb(x, y, z);
}

/**
 * @brief Delega l'aggiunta di una luce fissa al LightManager.
 */
void ENG_API Eng::Base::addLight(float x, float y, float z, float r, float g, float b, float a) {
    reserved->lightManager->addLight(x, y, z, r, g, b, a);
}

/**
 * @brief Aggiunge una richiesta di proiezione d'ombra per un nodo specifico delegando al LightManager.
 */
void ENG_API Eng::Base::addShadow(const std::string& nodeName, float x, float y, float z) {
    auto node = reserved->sceneManager->getNode(nodeName);
    if (!node) {

        std::cout << "[WARNING] addShadow: Node '" << nodeName << "' not found." << std::endl;
        return;
    }

    // matrici di posizione del ogetto
    glm::mat4 nodeMatrix = node->get_base_matrix();
    float nodeX = nodeMatrix[3][0];
    float nodeY = nodeMatrix[3][1];
    float nodeZ = nodeMatrix[3][2];


    float lightPosX = nodeX + x;
    float lightPosY = nodeY + y;
    float lightPosZ = nodeZ + z;


    reserved->lightManager->addShadow(nodeName, x, y, z);
}


// --- DELEGAZIONI PER SCENA E NODI ---

/**
 * @brief Recupera un nodo specifico della scena tramite il SceneManager.
 */
std::shared_ptr<eng::Node> ENG_API Eng::Base::getNode(const std::string& name) {
    return reserved->sceneManager->getNode(name);
}

/**
 * @brief Restituisce il nodo radice dell'intera scena caricata.
 */
std::shared_ptr<eng::Node> ENG_API Eng::Base::getSceneRoot() {
    return reserved->sceneManager->getRoot();
}

/**
 * @brief Delega lo spostamento di un nodo sotto un nuovo nodo genitore.
 */
void ENG_API Eng::Base::setParent(const std::string& childName, const std::string& parentName) {
    reserved->sceneManager->setParent(childName, parentName);
}

/**
 * @brief Ottiene l'altezza calcolata di una mesh specifica.
 */
float ENG_API Eng::Base::getMeshHeight(const std::string& nodeName) {
    return reserved->sceneManager->getMeshHeight(nodeName);
}


// --- DELEGAZIONI PER HUD E TESTI ---

/**
 * @brief Aggiunge un oggetto di testo all'HUD delegando all'HUDManager.
 */
void ENG_API Eng::Base::addText(const eng::TextHUD& textObject) {
    reserved->hudManager->addText(textObject);
}

/**
 * @brief Imposta un messaggio di sistema temporaneo o persistente sull'HUD.
 */
void ENG_API Eng::Base::setMessage(const std::string& msg) {
    reserved->hudManager->setMessage(msg);
}

/**
 * @brief Svuota l'HUD rimuovendo tutti i testi.
 */
void ENG_API Eng::Base::clearHUD() {
    reserved->hudManager->clearHUD();
}


// --- GESTIONE TELECAMERA E ANIMAZIONI ---

/**
 * @brief Aggiorna lo stato delle animazioni (es. rotazione sfere) basato sul tempo.
 * @param deltaTime Il tempo trascorso dall'ultimo fotogramma.
 */
 //rotazione sfere
void ENG_API Eng::Base::updateAnimation(float deltaTime) {
    reserved->rotationAngleY += 50.0f * deltaTime;
    reserved->totalTime += deltaTime;
}

/**
 * @brief Sposta la telecamera nella scena.
 * @param direction La direzione del movimento (Avanti, Indietro, Sinistra, Destra, ecc.).
 * @param deltaTime Tempo per livellare la velocità del frame.
 */
void ENG_API Eng::Base::moveCamera(eng::Camera_Movement direction, float deltaTime) {
    if (!reserved->camera) return;

    if (direction == eng::FORWARD || direction == eng::BACKWARD) {
        glm::vec3 frontXZ = reserved->camera->Front;
        frontXZ.y = 0.0f;
        frontXZ = glm::normalize(frontXZ);
        float velocity = reserved->camera->MovementSpeed * deltaTime;

        if (direction == eng::FORWARD)  reserved->camera->Position += frontXZ * velocity;
        if (direction == eng::BACKWARD) reserved->camera->Position -= frontXZ * velocity;
    }
    else {
        reserved->camera->ProcessKeyboard(direction, deltaTime);
    }
}

/**
 * @brief Ruota la telecamera orizzontalmente (Yaw).
 * @param angle L'angolo di rotazione in gradi.
 */
void ENG_API Eng::Base::rotateCamera(float angle) {
    if (reserved->camera) reserved->camera->RotateBy(angle);
}

/**
 * @brief Imposta istantaneamente la posizione e l'orientamento della telecamera.
 * Non distrugge l'istanza attuale se già presente, preservando lo stato interno (es. proiezione).
 */
void ENG_API Eng::Base::setCameraPosition(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch) {
    if (reserved->camera) {
        // Aggiorniamo la telecamera esistente senza distruggerla! Così non perde l'Aspect Ratio o la proiezione.
        reserved->camera->SetPositionAndOrientation(position, up, yaw, pitch);
    }
    else {
        // La creiamo solo se non esisteva proprio
        reserved->camera = std::make_unique<eng::Camera>(position, up, yaw, pitch);
    }
}

/**
 * @brief Ruota la telecamera verticalmente (Pitch).
 * @param angle L'angolo di inclinazione in gradi.
 */
void ENG_API Eng::Base::pitchCamera(float angle) {
    if (reserved->camera) {
        reserved->camera->PitchBy(angle);
    }
}

/**
 * @brief Alterna la proiezione della telecamera tra Prospettica e Ortografica (2D/3D).
 */
 //Prospettica 3d 2d orto
void ENG_API Eng::Base::toggleCameraProjection() {
    if (reserved->camera) {
        reserved->camera->ToggleProjection();


    }
}


// --- IL CUORE DEL RENDERING ---

/**
 * @brief Funzione callback principale richiamata ad ogni fotogramma per disegnare la scena.
 * Gestisce l'aggiornamento del tempo, il setup delle matrici, e delega il rendering
 * di luci, scena, ombre e interfaccia utente.
 */
void ENG_API Eng::Base::onDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    static int prev_t = glutGet(GLUT_ELAPSED_TIME);
    int curr_t = glutGet(GLUT_ELAPSED_TIME);
    float dt = (curr_t - prev_t) / 1000.0f;
    prev_t = curr_t;

    updateAnimation(dt);

    // Setup Proiezione e Vista richiesti alla Camera
    glMatrixMode(GL_PROJECTION);
    glm::mat4 projMatrix = reserved->camera ? reserved->camera->GetProjectionMatrix() : glm::mat4(1.0f);
    glLoadMatrixf(glm::value_ptr(projMatrix));

    glMatrixMode(GL_MODELVIEW);
    glm::mat4 viewMatrix = reserved->camera ? reserved->camera->GetViewMatrix() : glm::mat4(1.0f);
    glLoadMatrixf(glm::value_ptr(viewMatrix));

    // DELEGA 1: Rendering delle sfere luminose
    reserved->lightManager->renderOrbs(reserved->rotationAngleY, reserved->totalTime);

    // DELEGA 2: Rendering scena OVO
    reserved->renderer->renderScene(reserved->sceneManager->getRoot(), viewMatrix, detail);

    // DELEGA 3: Rendering delle ombre
    // Nota: Passiamo la root in modo che il Renderer possa trovare l'oggetto che proietta ombra
    reserved->renderer->renderShadows(reserved->sceneManager->getRoot(), reserved->lightManager->getShadowRequests(), viewMatrix);

    // DELEGA 4: Rendering HUD 2D
    reserved->hudManager->renderHUD(fps, detail, window_width, window_height);

    frames++;
    glutSwapBuffers();
    glutPostWindowRedisplay(this->reserved->windowId);
}



// --- ALTRE FUNZIONI UTILITY ---

/**
 * @brief Pulisce i buffer colore e profondità della finestra.
 */
void ENG_API Eng::Base::clearWindow() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

/**
 * @brief Imposta il colore di sfondo (clear color) per OpenGL.
 */
void ENG_API Eng::Base::setBackgroundColor(float r, float g, float b) {
    reserved->clearColor[0] = r; reserved->clearColor[1] = g; reserved->clearColor[2] = b;
}

/**
 * @brief Scambia i buffer (per il double buffering) e richiede il ridisegno.
 */
void ENG_API Eng::Base::swapBuffers() { glutSwapBuffers(); glutPostRedisplay(); }

/**
 * @brief Registra la funzione callback definita dall'utente per la gestione della tastiera.
 */
void ENG_API Eng::Base::setKeyboardCallback(KeyboardCallback cb) { this->keyboardCallback = cb; }

/**
 * @brief Metodo interno richiamato da GLUT alla pressione di un tasto, inoltra la chiamata all'utente.
 */
void ENG_API Eng::Base::onKeyPressed(unsigned char key, int x, int y) {
    glutPostRedisplay();
    if (keyboardCallback) keyboardCallback(key, x, y);
}

/**
 * @brief Processa un singolo evento del loop principale di GLUT.
 */
void ENG_API Eng::Base::update() { glutMainLoopEvent(); }

/**
 * @brief Callback per il calcolo dei Frames Per Second (FPS), richiamata ogni secondo.
 */
void ENG_API Eng::Base::timerCallback(int value) {
    Eng::Base& eng = Eng::Base::getInstance();
    eng.fps = eng.frames;
    eng.frames = 0;
    glutTimerFunc(1000, timerCallback, 0);
}


/**
 * @brief Callback invocata al ridimensionamento della finestra.
 * Ricalcola la viewport, aggiorna l'aspect ratio della telecamera e prepara la matrice per l'HUD.
 * @param width Nuova larghezza.
 * @param height Nuova altezza.
 */
void ENG_API Eng::Base::reshapeCallback(int width, int height) {
    glViewport(0, 0, width, height);

    // Aggiorniamo la telecamera
    if (reserved->camera) {
        reserved->camera->SetAspectRatio((float)width, (float)height);
    }

    // Manteniamo solo la matrice per l'HUD 2D
    ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
}

/**
 * @brief Restituisce il conteggio attuale delle sfere luminose tramite il LightManager.
 */
int ENG_API Eng::Base::getOrbCount() const { return reserved->lightManager->getOrbCount(); }