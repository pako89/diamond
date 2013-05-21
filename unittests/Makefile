UNIT_DIR_RLC = rlc
UNIT_DIR_STATIC_HUFFMAN = static_huffman

CFLAGS += -g

default: all

all: rlc static_huffman

clean: all_clean

all_clean: rlc_clean static_huffman_clean

.PHONY: rlc rlc_clean
rlc:
	$(MAKE) -C $(UNIT_DIR_RLC) 


rlc_clean:
	$(MAKE) -C $(UNIT_DIR_RLC) clean

.PHONY: static_huffman static_huffman_clean
static_huffman:
	$(MAKE) -C $(UNIT_DIR_STATIC_HUFFMAN)

static_huffman_clean:
	$(MAKE) -C $(UNIT_DIR_STATIC_HUFFMAN) clean

