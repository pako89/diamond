# Target name
TARGET	= diamond

# Compiler
CC	= g++

# Compiler flags
INCDIR	= -Iinclude -I/usr/local/cuda/include
CFLAGS	= -g $(INCDIR) -DDEBUG

# Linker flags
LDFLAGS	= -L/usr/lib

# Libs
LIB = -lOpenCL

# Source files
SRC	= $(wildcard src/*.cpp)

# Objects
OBJ	= $(SRC:.cpp=.o)

default: $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIB)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJ)
