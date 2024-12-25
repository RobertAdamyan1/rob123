CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2

SRC = file_reader.cpp
TARGET = file_reader

all: $(TARGET)

$(TARGET): $(SRC)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
    rm -f $(TARGET)

rebuild: clean all

