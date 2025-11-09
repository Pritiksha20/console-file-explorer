# Compiler & flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -O2

# If you use GCC 8, uncomment the next line:
# LDLIBS := -lstdc++fs

SRC := main.cpp explorer.cpp
OBJ := $(SRC:.cpp=.o)
TARGET := explorer

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDLIBS)

%.o: %.cpp explorer.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
.PHONY: all run clean
