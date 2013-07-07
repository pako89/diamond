# Target name
TARGET	= diamond
TESTS_DIR = ./unittests

# Compiler
CC	= g++

# Compiler flags
CFLAGS += -g
CFLAGS += -Iinclude
CFLAGS += -I/usr/local/cuda/include
CFLAGS += -Isrc/cl
cflags += $(INCDIR) 
CFLAGS += -DDEBUG
CFLAGS += -DCHECK_HUFFMAN
#CFLAGS += -DDUMP_BITSTREAM
#CFLAGS += -Os
CFLAGS += -DDEFAULT_MAX_PREDICTION=2
CFLAGS += -DDEFAULT_CL_SRC_FILE="\"src/cl/kernel.cl\""
CFLAGS += -DDEFAULT_PREDICTION_METHOD=PREDICTION_METHOD_MSE
CFLAGS += -DUSE_INTERPOLATION
CFLAGS += -DPREDICTION_INTERPOLATION_SCALE=2
CFLAGS += -DIMAGE_CHECK_INDEX
CFLAGS += -DCOMPONENT_CHECK_INDEX
#CFLAGS += -DCL_KERNEL_FINISH

# Linker flags
LDFLAGS	+= -L/usr/lib

# Libs
LIBS = -lOpenCL

# Source files
SRC += src/main.cpp
SRC += src/log.cpp
SRC += src/avlib.cpp
SRC += src/decoder.cpp
SRC += src/getopt.cpp
SRC += src/prediction.cpp
SRC += src/static_huffman.cpp
SRC += src/basic_decoder.cpp
SRC += src/DiamondApp.cpp
SRC += src/getopt_long.cpp
SRC += src/quantizer.cpp
SRC += src/static_rlc.cpp
SRC += src/basic_encoder.cpp 
SRC += src/dynamic_huffman.cpp
SRC += src/image.cpp
SRC += src/rlc.cpp
SRC += src/rlc_factory.cpp
SRC += src/transform.cpp
SRC += src/bitstream.cpp
SRC += src/component.cpp
SRC += src/dynamic_rlc.cpp
SRC += src/sequence.cpp
SRC += src/utils.cpp
SRC += src/dct.cpp
SRC += src/encoder.cpp
SRC += src/mtimer.cpp
SRC += src/shift.cpp
SRC += src/zigzag.cpp
SRC += src/interpolation.cpp
SRC += src/cl_base.cpp
SRC += src/cl_encoder.cpp
SRC += src/cl_parallel_encoder.cpp
SRC += src/cl_merged_encoder.cpp
SRC += src/cl_policy.cpp
SRC += src/cl_component.cpp
SRC += src/cl_image.cpp
SRC += src/cl_quantizer.cpp
SRC += src/cl_dct.cpp
SRC += src/cl_kernel.cpp
SRC += src/cl_zigzag.cpp
SRC += src/cl_device.cpp
SRC += src/cl_platform.cpp
SRC += src/cl_device_info.cpp
SRC += src/cl_platform_info.cpp
SRC += src/cl_shift.cpp
SRC += src/cl_prediction.cpp
SRC += src/cl_host.cpp
SRC += src/cl_dctqzz.cpp
SRC += src/cl_interpolation.cpp

# Objects
OBJ	= $(SRC:.cpp=.o)

default: $(TARGET)

all: $(TARGET) tests

$(TARGET): $(OBJ) 
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : tests tests_clean
tests:
	$(MAKE) -C $(TESTS_DIR)

tests_clean:
	$(MAKE) -C $(TESTS_DIR) clean

clean: tests_clean
	rm -rf $(TARGET) $(OBJ)
