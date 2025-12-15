CXX      = g++
CXXFLAGS = -Wall -std=c++20 -fexceptions
INC      = -I../engine

# ... (resto del file) ...

# Regola run: compila ED esegue
run: $(TARGET)
	@echo "--- Eseguo i Test per la CI ---"
	@./$(TARGET)

# ... (resto del file) ...

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
LIBS     = -lengine -lfreeimage -lglut -lGL -lGLU

SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=obj/%.o)
TARGET = $(OUT_DIR)/TestEngine2

.PHONY: all clean directories run

all: directories $(TARGET)

# Compilazione dell'eseguibile di test
$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

# Regola per ESEGUIRE i test
# Se ./$(TARGET) ritorna errore (exit code != 0), il make si ferma qui.
run: $(TARGET)
	@echo "--- Executing Tests ---"
	@./$(TARGET)

obj/%.o: %.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

directories:
	@mkdir -p $(OUT_DIR)
	@mkdir -p obj

clean:
	rm -rf obj
	rm -rf bin