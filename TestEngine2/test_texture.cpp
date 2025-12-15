#include "test_texture.h"
#include "texture.h"
#include <iostream>
#include <cassert>

void test_texture_system() {
    std::cout << "[RUNNING] Texture System (External File) ... ";

   
    std::string filename = "rinGrafica.dds";

    // Proviamo a caricare la texture
    // NOTA: Richiede che init() sia stato chiamato nel main per avere OpenGL attivo!
    eng::Texture tex(filename);

    // Verifiche
    if (tex.get_texture_id() == 0) {
        // Se l'ID è 0, il caricamento è fallito.
        // Probabilmente il file non è nella cartella giusta.
        std::cout << "FALLITO!" << std::endl;
        std::cout << "ERRORE: Non trovo il file '" << filename << "'." << std::endl;
        std::cout << "Assicurati che l'immagine sia nella cartella del progetto o dell'eseguibile." << std::endl;
        assert(false && "Texture non caricata (File non trovato o errore FreeImage)");
    }

    // Se siamo qui, la texture è in memoria GPU
    assert(tex.get_path() == filename && "Errore: Path non memorizzato");

    std::cout << "OK (ID: " << tex.get_texture_id() << ")" << std::endl;
}