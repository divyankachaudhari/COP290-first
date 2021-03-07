CC = g++
CFLAGS = -g -Wall
TARGET = subtask2
THREAD = -pthread
LIBS = pkg-config --cflags --libs opencv

all: $(TARGET)


$(TARGET): $(TARGET).cpp
	$(CC) $(TARGET).cpp warpAndCrop.cpp bsfilter.cpp densities.cpp -o $(TARGET) $(CFLAGS) $(THREAD) `$(LIBS)`

clean:
	$(RM) $(TARGET)
