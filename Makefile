# Target name
TARGET	= diamond

# Compiler
CC	= g++

# Compiler flags
INCDIR	= -Iinclude
CFLAGS	= -g $(INCDIR)

# Linker flags
LDFLAGS	= -Llib

# Source files
SRC	= $(wildcard src/*.cpp)

# Objects
OBJ	= $(SRC:.cpp=.o)

default: $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJ)
