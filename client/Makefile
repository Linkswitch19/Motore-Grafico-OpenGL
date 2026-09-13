# client/Makefile
CXX      = g++
CXXFLAGS = -Wall -std=c++20 -fexceptions
INC      = -I../engine

# --- AGGIUNTA PER GITLAB CI ---
.PHONY: client
# Quando la CI scrive "make client", noi compiliamo tutto (all)
client: all

# ------------------------------

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
# -Wl,-rpath tells the loader where to look for libengine.so at runtime
LDFLAGS = -L$(LIB_DIR) -Wl,-rpath=\$$ORIGIN/../../../engine/bin/Release:\$$ORIGIN
LIBS     = -lengine -lfreeimage -lglut -lGL -lGLU

SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=obj/%.o)
TARGET = $(OUT_DIR)/client

.PHONY: all clean directories

all: directories $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

obj/%.o: %.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

directories:
	@mkdir -p $(OUT_DIR)
	@mkdir -p obj

clean:
	rm -rf obj
	rm -rf bin