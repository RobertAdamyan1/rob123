
CC = g++
CFLAGS = -Wall -std=c++11
TARGET = do-command

all: $(TARGET)

$(TARGET): do-command.cpp
	$(CC) $(CFLAGS) -o $(TARGET) do-command.cpp

clean:
	rm -f $(TARGET)

rebuild: clean all
