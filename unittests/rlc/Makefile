TARGET=ut_rlc

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
CFLAGS += -DCHECK_HUFFMAN

SRC=main.cpp
SRC += $(SRCDIR)/static_huffman.cpp
SRC += $(SRCDIR)/static_rlc.cpp
SRC += $(SRCDIR)/rlc.cpp
SRC += $(SRCDIR)/utils.cpp
SRC += $(SRCDIR)/avlib.cpp
SRC += $(SRCDIR)/mtimer.cpp
SRC += $(SRCDIR)/bitstream.cpp

OBJ = $(SRC:.cpp=.o)

default: $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)

