# Target name
TARGET	= diamond
TESTS_DIR = ./unittests

# Compiler
CC	= g++

# Compiler flags
INCDIR	= -Iinclude -I/usr/local/cuda/include
CFLAGS	= -g $(INCDIR) 
CFLAGS += -DDEBUG
CFLAGS += -DCHECK_HUFFMAN

# Linker flags
LDFLAGS	= -L/usr/lib

# Libs
LIB = -lOpenCL

# Source files
SRC	= $(wildcard src/*.cpp)

# Objects
OBJ	= $(SRC:.cpp=.o)

default: $(TARGET)

all: $(TARGET) tests

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIB)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : tests tests_clean
tests:
	$(MAKE) -C $(TESTS_DIR)

tests_clean:
	$(MAKE) -C $(TESTS_DIR) clean

clean: tests_clean
	rm -rf $(TARGET) $(OBJ)
