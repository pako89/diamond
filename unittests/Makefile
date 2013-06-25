UNIT_DIR_RLC = rlc
UNIT_DIR_STATIC_HUFFMAN = static_huffman
UNIT_DIR_BITSTREAM = bitstream

CFLAGS += -g

default: all

all: rlc static_huffman bitstream

clean: all_clean

all_clean: rlc_clean static_huffman_clean bitstream_clean

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

.PHONY: bitstream bitstream_clean
bitstream:
	$(MAKE) -C $(UNIT_DIR_BITSTREAM)

bitstream_clean:
	$(MAKE) -C $(UNIT_DIR_BITSTREAM) clean

