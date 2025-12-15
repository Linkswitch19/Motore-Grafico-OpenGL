// test_ovo.cpp
#include "test_ovo.h"
#include "ovo_parser.h"
#include "node.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <glm/glm.hpp>
#include <cstdio> // per remove()

// --- Helper per creare file OVO finti ---
class OvoWriter {
    std::string filename;
    std::ofstream file;

public:
    OvoWriter(const std::string& fname) : filename(fname) {
        file.open(filename, std::ios::binary);
    }

    ~OvoWriter() {
        if (file.is_open()) file.close();
    }

    void writeChunkHeader(uint32_t type, uint32_t size) {
        file.write(reinterpret_cast<const char*>(&type), sizeof(uint32_t));
        file.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
    }

    void writeInt(uint32_t val) {
        file.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
    }

    void writeString(const std::string& str) {
        file.write(str.c_str(), str.size() + 1); // +1 per null terminator
    }

    void writeMat4(const glm::mat4& mat) {
        file.write(reinterpret_cast<const char*>(&mat), sizeof(glm::mat4));
    }

    // Helper specifico per scrivere un Chunk Nodo (Type 1)
    void writeNodeChunk(const std::string& name, int childrenCount) {
        // Calcoliamo la dimensione del payload dati
        // Payload: String (len+1) + Mat4 (64 bytes) + ChildrenCount (4 bytes)
        uint32_t size = (uint32_t)(name.size() + 1 + sizeof(glm::mat4) + sizeof(uint32_t));

        writeChunkHeader(1, size); // Type 1 = Node
        writeString(name);
        writeMat4(glm::mat4(1.0f)); // Matrice identità
        writeInt(childrenCount);
    }

    // Helper per scrivere il Version Chunk (Type 0)
    void writeVersionChunk() {
        uint32_t version = 1;
        writeChunkHeader(0, sizeof(uint32_t));
        writeInt(version);
    }
};

// --- Test Case ---

void test_ovo_parsing() {
    std::cout << "[RUNNING] OVO Parser Test (Binary Read/Write) ... ";

    const std::string tempFile = "unit_test_scene.ovo";

    // 1. CREAZIONE FILE BINARIO TEMPORANEO
    {
        OvoWriter writer(tempFile);

        // Scriviamo Chunk Versione
        writer.writeVersionChunk();

        // Scriviamo un Nodo "Padre" che ha 1 figlio
        // NOTA: Il parser usa uno stack. Quando diciamo che un nodo ha figli, 
        // si aspetta che il chunk successivo sia il figlio.
        writer.writeNodeChunk("RootNode", 1);

        // Scriviamo il Nodo "Figlio" (0 figli)
        writer.writeNodeChunk("ChildNode", 0);
    }

    // 2. ESECUZIONE PARSER
    // Il parser deve leggere il file appena creato e restituire la root
    std::shared_ptr<eng::Node> resultRoot = nullptr;
    try {
        resultRoot = eng::OVOParser::from_file(tempFile);
    }
    catch (...) {
        assert(false && "Errore: Il parser e' andato in crash leggendo il file di test.");
    }

    // 3. VERIFICA STRUTTURA SCENA
    // La struttura attesa è:
    // Scene Root (creata default dal parser)
    //   -> RootNode (dal file)
    //        -> ChildNode (dal file)

    assert(resultRoot != nullptr && "Errore: Il parser ha restituito nullptr");

    // Il parser crea sempre un nodo radice fittizio chiamato "Scene Root"
    // Quindi ci aspettiamo che il nostro "RootNode" sia figlio di questo.
    assert(resultRoot->get_children().size() > 0 && "Errore: Nessun nodo caricato sotto la root");

    auto loadedParent = resultRoot->get_children()[0];
    assert(loadedParent->get_name() == "RootNode" && "Errore: Nome nodo padre errato");

    assert(loadedParent->get_children().size() == 1 && "Errore: Il figlio non è stato attaccato al padre");

    auto loadedChild = loadedParent->get_children()[0];
    assert(loadedChild->get_name() == "ChildNode" && "Errore: Nome nodo figlio errato");

    // 4. CLEANUP
    // Cancelliamo il file temporaneo
    std::remove(tempFile.c_str());

    std::cout << "OK" << std::endl;
}
