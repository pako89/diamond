TARGET=ut_bitstream

CC = g++
CP = cp -v
RM = rm -rf

SRCDIR=../../src
INCDIR=../../include

CFLAGS += -g
CFLAGS += -I.
CFLAGS += -I..
CFLAGS += -I$(INCDIR)
CFLAGS += -DDEBUG

SRC=main.cpp
SRC += $(SRCDIR)/bitstream.cpp

OBJ = $(SRC:.cpp=.o)

default: clean $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)

