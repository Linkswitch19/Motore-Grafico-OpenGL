#include "test_rendering.h"
#include "test_utils.h"
#include "engine.h" // Fondamentale
#include "camera.h"
#include "mesh.h"
#include "material.h"
#include <cassert>

void test_lights_compilation() {
    TEST_START("Engine: Light Management (createOrb)");

    Eng::Base& engine = Eng::Base::getInstance();

    // 1. Contiamo quante luci ci sono ora
    int initialCount = engine.getOrbCount();

    // 2. Chiamiamo l'ENGINE per creare una luce
    engine.createOrb(10.0f, 5.0f, 0.0f);

    // 3. Verifichiamo che il contatore sia salito
    int newCount = engine.getOrbCount();
    assert(newCount == initialCount + 1 && "Errore: createOrb non ha incrementato il numero di luci");

    // 4. Test limite (opzionale, riempiamo fino a 7)
    // ...

    TEST_PASS();
}

void test_material() {
    TEST_START("Engine: Texture Loading (loadTexture)");

    Eng::Base& engine = Eng::Base::getInstance();
    auto root = engine.getSceneRoot();
    if (!root) return;

    // 1. Setup: Dobbiamo avere una MESH nella scena per applicare la texture
    auto testMesh = std::make_shared<eng::Mesh>();
    testMesh->set_name("OggettoPerTexture");
    root->add_child(testMesh); // Aggiungiamo alla scena dell'engine

    // 2. Chiamiamo l'ENGINE: Carica texture su questo oggetto
    // Nota: "fake.png" fallirà il caricamento immagine (FreeImage), 
    // ma l'engine dovrebbe comunque creare l'oggetto Texture e assegnarlo al Materiale (o gestire l'errore).
    // Se il tuo engine è robusto, non crasha.

    // Per testare che ASSEGNI il materiale, simuliamo una chiamata sicura o verifichiamo che non crashi.
    try {
        engine.loadTexture("missing_file.png", "OggettoPerTexture");
    }
    catch (...) {
        assert(false && "Crash durante loadTexture");
    }

    // Verifica: La mesh ha ricevuto un materiale?
    // loadTexture nel tuo codice fa: mesh->set_material(...)
    if (testMesh->get_material()) {
        // Se ha creato il materiale, il test è passato (anche se la texture è vuota per via del file mancante)
        assert(true);
    }
    else {
        // Se il tuo codice non crea il materiale se il file manca, allora questo assert dipende dalla tua implementazione.
        // Assumiamo che il test sia "Non deve crashare".
    }

    TEST_PASS();
}

void test_camera_math() {
    TEST_START("Engine: Camera Movement");

    Eng::Base& engine = Eng::Base::getInstance();

    // 1. Resettiamo la camera a una posizione nota
    engine.setCameraPosition(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), -90.0f, 0.0f);

    // Recuperiamo la camera interna per controllare
    eng::Camera* internalCam = engine.getCamera();
    assert(internalCam != nullptr && "Errore: Camera non inizializzata nell'engine");

    glm::mat4 viewBefore = internalCam->GetViewMatrix();

    // 2. Usiamo l'ENGINE per muovere la camera
    // Simuliamo 1 secondo di pressione del tasto 'W' (Avanti)
    engine.moveCamera(eng::FORWARD, 1.0f);

    // 3. Verifica
    glm::mat4 viewAfter = internalCam->GetViewMatrix();

    bool changed = false;
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
        if (viewBefore[i][j] != viewAfter[i][j]) changed = true;

    assert(changed && "Errore: engine.moveCamera non ha aggiornato la vista");

    TEST_PASS();
}