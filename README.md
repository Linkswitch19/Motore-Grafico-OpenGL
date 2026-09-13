<div align="center">

# 🎮 Motore Grafico OpenGL

**Motore grafico 3D real-time scritto in C++20, con scene graph gerarchico, illuminazione Phong e un gioco dimostrativo — la Torre di Hanoi in 3D.**

[![Language](https://img.shields.io/badge/C%2B%2B-20-blue.svg?logo=c%2B%2B)](https://en.cppreference.com/w/cpp/20)
[![Graphics API](https://img.shields.io/badge/OpenGL-Core-5586A4.svg?logo=opengl)](https://www.opengl.org/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg)]()
[![Build](https://img.shields.io/badge/CI-GitLab%20Pipeline-orange.svg?logo=gitlab)]()
[![Status](https://img.shields.io/badge/status-completato-brightgreen.svg)]()

</div>

---

## 📑 Indice

- [Panoramica](#-panoramica)
- [Struttura del progetto](#-struttura-del-progetto)
- [Funzionalità del motore](#-funzionalità-del-motore)
- [Applicazione dimostrativa: Torre di Hanoi 3D](#-applicazione-dimostrativa-torre-di-hanoi-3d)
- [Architettura](#-architettura)
- [Dipendenze](#-dipendenze)
- [Compilazione](#-compilazione)
- [Esecuzione](#-esecuzione)
- [Test](#-test)
- [Integrazione continua](#-integrazione-continua)
- [Autori](#-autori)

---

## 📖 Panoramica

**Motore-Grafico-OpenGL** è una libreria grafica 3D sviluppata da zero sopra le API OpenGL, come progetto del corso di *Computer Graphics* (Laboratorio di Ingegneria del Software) alla **SUPSI**. Espone un'interfaccia singleton (`Eng::Base`) che nasconde al client tutta la complessità di rendering, scene graph, illuminazione e gestione risorse, mantenendo un'API pubblica compatta e semplice da integrare.

Il repository include tre componenti principali:

| Componente | Descrizione |
|---|---|
| 🧩 `engine/` | Libreria del motore grafico (compilata come `libengine.so` / `.dll`) |
| 🕹️ `client/` | Applicazione dimostrativa: Torre di Hanoi in 3D |
| ✅ `TestEngine2/` | Suite di test automatici per la validazione del motore |

---

## 🗂 Struttura del progetto

```
Motore-Grafico-OpenGL/
├── engine/             # Libreria del motore grafico
│   ├── engine.h/.cpp       → Facade singleton (Eng::Base)
│   ├── node.h/.cpp         → Scene graph (Node)
│   ├── camera.h/.cpp       → Camera fly-cam prospettica/ortografica
│   ├── mesh.h/.cpp         → Geometria e mesh renderizzabili
│   ├── material.h/.cpp     → Materiali Phong/Blinn-Phong
│   ├── texture.h/.cpp      → Gestione texture (FreeImage)
│   ├── light.h/.cpp        → Luci (base)
│   ├── point_light / spot_light / directional_light
│   ├── light_manager.h/.cpp
│   ├── scene_manager.h/.cpp
│   ├── ovo_parser.h/.cpp   → Import mesh/scene in formato .ovo
│   ├── renderer.h/.cpp     → Pipeline di rendering
│   └── hud_manager / textHUD → HUD testuale a schermo
│
├── client/             # Gioco dimostrativo: Torre di Hanoi 3D
│   └── main.cpp
│
├── TestEngine2/        # Unit test automatici del motore
│   └── main.cpp
│
├── dependencies/       # Librerie di terze parti
│   ├── glm/                → Algebra vettoriale/matriciale
│   ├── freeglut/            → Finestre e input
│   └── freeimage/           → Caricamento texture
│
├── hanoi.sln           # Soluzione Visual Studio
└── .gitlab-ci.yml       # Pipeline di build e test
```

---

## ⚙️ Funzionalità del motore

- 🌳 **Scene graph gerarchico** — nodi (`Node`) con matrici di trasformazione locali, figli e parent, per comporre scene complesse da mesh, luci e gruppi.
- 📦 **Import mesh `.ovo`** — parser dedicato (`ovo_parser`) per caricare geometrie, materiali e gerarchie esportate in questo formato.
- 🎨 **Materiali Phong / Blinn-Phong** — coefficienti ambient, diffuse, specular, emission e shininess per superficie, con texture opzionale.
- 🖼️ **Texture mapping** tramite FreeImage (`.dds`, `.png`, ecc.).
- 💡 **Illuminazione multipla** — luci direzionali, puntiformi (point light) e spot, tutte derivate da `Light` e coordinate da un `light_manager`.
- 🌑 **Ombre** proiettate e gestione di "orb" di luce nella scena.
- 🎥 **Camera fly-cam** — movimento in prima persona (WASD + mouse look, yaw/pitch), con proiezione prospettica e ortografica commutabile a runtime.
- 🔠 **HUD testuale** — overlay di testo e messaggi a schermo tramite `hud_manager` / `textHUD`.
- 📊 **Statistiche di rendering** — conteggio FPS e livelli di dettaglio (LOD) configurabili.

L'intera implementazione interna è nascosta dietro un pattern **PIMPL**, così il client dipende solo da un'API pubblica minimale e stabile.

---

## 🕹️ Applicazione dimostrativa: Torre di Hanoi 3D

`client/main.cpp` implementa una versione 3D del classico puzzle, usata come banco di prova del motore:

- selezione interattiva di dischi e pali tramite lo scene graph del motore;
- **calibrazione automatica** delle posizioni dei pali a partire dai nodi caricati dalla scena;
- **cronologia delle mosse** con supporto a undo/redo.

---

## 🏗️ Architettura

```
        ┌──────────────────────┐
        │   client (Hanoi 3D)  │
        └──────────┬───────────┘
                    │  Eng::Base::getInstance()
        ┌──────────▼───────────┐
        │   engine (facade)    │
        │  ─────────────────   │
        │  scene_manager        │
        │  light_manager        │
        │  renderer              │
        │  hud_manager            │
        └──────────┬────────────┘
                    │
   ┌────────────────┼─────────────────┐
   ▼                ▼                 ▼
 Node graph      Material/Texture    Camera
 (mesh, luci,    (Phong shading,     (fly-cam,
  gruppi)         FreeImage)          proiezioni)
```

---

## 📦 Dipendenze

| Libreria | Ruolo |
|---|---|
| **OpenGL / GLU** | Rendering grafico low-level |
| **FreeGLUT** | Creazione finestra e gestione input |
| **FreeImage** | Caricamento e decodifica texture |
| **GLM** | Algebra vettoriale e matriciale |

### Installazione su Linux (Ubuntu 24.04)

```bash
sudo apt update
sudo apt install -y build-essential cppcheck \
    libopengl-dev libgl-dev libglm-dev \
    libgl1-mesa-dev libglu1-mesa-dev \
    libfreeimage-dev freeglut3-dev
```

### Windows

Le versioni precompilate di FreeGLUT e FreeImage sono già incluse in `dependencies/`, pronte per l'uso con la soluzione Visual Studio `hanoi.sln`.

---

## 🔨 Compilazione

### Linux (Makefile)

```bash
# 1. Motore → genera engine/bin/Release/libengine.so
cd engine
make engine

# 2. Test del motore
make test

# 3. Client → genera client/bin/Release/client
cd ../client
make client
```

> Per una build di debug: aggiungere `DEBUG=1`, ad esempio `make engine DEBUG=1`.

### Windows (Visual Studio)

Aprire `hanoi.sln` e compilare i progetti `engine`, `client` e `TestEngine2`.

### Code::Blocks

Sono disponibili anche i progetti `.cbp` per `engine`, `client` e `TestEngine2`.

---

## ▶️ Esecuzione

```bash
cd client/bin/Release
./client
```

---

## ✅ Test

La suite `TestEngine2` copre, tra gli altri:

- inizializzazione del singleton dell'engine,
- matematica della camera,
- parsing dei file `.ovo`,
- manipolazione e ricerca nello scene graph,
- robustezza e gestione risorse,
- integrazione camera ↔ engine.

```bash
cd engine
make test
```

---

## 🚀 Integrazione continua

Pipeline **GitLab CI** (`.gitlab-ci.yml`) su immagine Ubuntu 24.04, organizzata in 4 stage:

| Stage | Descrizione |
|---|---|
| 1️⃣ `structure` | Verifica che esistano le cartelle `engine` e `client` |
| 2️⃣ `engine` | Build e test del motore → pubblica `libengine.so` come artifact |
| 3️⃣ `client` | Build del client → pubblica l'eseguibile come artifact |
| 4️⃣ `package` | Crea l'archivio `GraficaLinky_Release.tar.gz` pronto alla distribuzione |

---

## 👥 Autori

Progetto sviluppato da Andre per il corso di *Computer Graphics* (Laboratorio di Ingegneria del Software), **SUPSI** — anno accademico 2025-2026.

<div align="center">

---

Fatto con 🎓 alla SUPSI

</div>
