CXX      = g++
CXXFLAGS = -Wall -std=c++20 -fexceptions
INC      = -I../engine

# Configurazioni Debug/Release
ifdef DEBUG
    CXXFLAGS += -g -D_DEBUG
    LIB_DIR   = ../engine/bin/Debug
    OUT_DIR   = bin/Debug
else
    CXXFLAGS += -O2
    LIB_DIR   = ../engine/bin/Release
    OUT_DIR   = bin/Release
endif

# Linker Flags
# Linka libengine esattamente come fa il client
LDFLAGS = -L$(LIB_DIR) -Wl,-rpath=\$$ORIGIN/../../$(LIB_DIR)
LIBS    = -lengine -lfreeimage -lglut -lGL -lGLU

SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=obj/%.o)
TARGET = $(OUT_DIR)/TestEngine2

.PHONY: all clean directories run

all: directories $(TARGET)

# Compilazione dell'eseguibile di test
# AGGIUNTA: Creo la cartella di output qui per sicurezza
$(TARGET): $(OBJ)
	@mkdir -p $(OUT_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

# Regola per ESEGUIRE i test (Ne teniamo solo una, quella in fondo)
# Se ./$(TARGET) ritorna errore (exit code != 0), il make si ferma qui.
run: $(TARGET)
	@echo "--- Executing Tests ---"
	@./$(TARGET)

# Compilazione oggetti
obj/%.o: %.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Questa rimane per chi lancia "make all"
directories:
	@mkdir -p $(OUT_DIR)
	@mkdir -p obj

clean:
	rm -rf obj
	rm -rf bin