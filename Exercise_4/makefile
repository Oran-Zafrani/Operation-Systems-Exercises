# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -g  # Added -g for debugging support

# Output file
OUTPUT = ex4.out

# Source files
SRCS = Main.cpp BoundedBuffer.cpp CoEditor.cpp Dispatcher.cpp Producer.cpp ScreenManager.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(OUTPUT)

# Rule to build the final executable
$(OUTPUT): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(OUTPUT) $(OBJS)

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files only (keep the executable)
clean:
	rm -f $(OBJS)

# Clean up everything, including the executable
clean_all:
	rm -f $(OBJS) $(OUTPUT)

# Phony targets
.PHONY: all clean clean_all