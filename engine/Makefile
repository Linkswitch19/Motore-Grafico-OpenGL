# engine/Makefile
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