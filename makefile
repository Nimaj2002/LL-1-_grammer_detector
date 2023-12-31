# Makefile for a C++ program

# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++17

# Source files and executable name
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = llone

# Default target (the one that will be built if you just run 'make')
all: $(EXEC)

# Rule to build the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXEC)

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(EXEC) -g

# Clean up generated files
clean:
	rm -f $(OBJS) $(EXEC)