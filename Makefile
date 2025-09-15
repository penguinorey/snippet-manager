CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = sn
SRC = sn.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

clean:
	rm -f $(TARGET)

.PHONY: all install clean
