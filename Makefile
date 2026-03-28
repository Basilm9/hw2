CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

TARGET = fp_overflow_checker
SRC    = fp_overflow_checker.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
