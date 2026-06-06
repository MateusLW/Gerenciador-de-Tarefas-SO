ifeq ($(OS),Windows_NT)
    PLATFORM = win
    RM = del /Q
    TARGET = simulador.exe
    LDFLAGS = -Llib/$(PLATFORM) -lraylib -lopengl32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = linux
        TARGET = simulador
        LDFLAGS = -Llib/$(PLATFORM) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
    ifeq ($(UNAME_S),Darwin)
        PLATFORM = macos
        TARGET = simulador
        LDFLAGS = -Llib/$(PLATFORM) -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
    endif
    RM = rm -f
endif

CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude

# Pega todos os .cpp da pasta src automaticamente
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	$(RM) $(TARGET)
	$(RM) src\*.o
else
	$(RM) $(TARGET)
	$(RM) src/*.o
endif