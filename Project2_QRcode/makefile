CXX = g++

CXXFLAGS = -Wall -O2 `pkg-config --cflags opencv4`

LIBS = `pkg-config --libs opencv4`

TARGET = qrrecognition

SRC = qr_recognition.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
