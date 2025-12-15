# engine/Makefile
# --- AGGIUNTA PER GITLAB CI ---
.PHONY: engine test
# 1. Quando la CI scrive "make engine", noi compiliamo tutto (all)
engine: all

# 2. Quando la CI scrive "make test", noi saltiamo nella cartella a fianco ed eseguiamo i test
test:
	@echo "=== CI sta chiedendo i test, delego a TestEngine2 ==="
	$(MAKE) -C ../TestEngine2 run

# ------------------------------
CXX      = g++
CXXFLAGS = -Wall -std=c++20 -fPIC -fexceptions
LDFLAGS  = -shared
LIBS     = -lfreeimage -lglut -lGL -lGLU

# Configurazioni Debug/Release
ifdef DEBUG
    CXXFLAGS += -g -D_DEBUG
    OUT_DIR   = bin/Debug
else
    CXXFLAGS += -O2
    OUT_DIR   = bin/Release
endif

# Trova tutti i file .cpp nella cartella corrente
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=obj/%.o)
TARGET = $(OUT_DIR)/libengine.so

.PHONY: all clean directories

all: directories $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

# Regola generica per compilare i .cpp in .o
obj/%.o: %.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

directories:
	@mkdir -p $(OUT_DIR)
	@mkdir -p obj

clean:
	rm -rf obj
	rm -rf bin