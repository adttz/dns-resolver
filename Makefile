CXX := g++
CXXFLAGS := -std=c++17 -Wall -Iinclude

SRC := src/main.cpp src/dns.cpp
OBJ := $(SRC:src/%.cpp=build/%.o)
BIN := build/dns_resolver

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir build

clean:
	rm -rf build

.PHONY: all clean
