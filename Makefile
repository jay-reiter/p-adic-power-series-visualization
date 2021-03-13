#Name .exe file!
EXENAME = image
#List all the .o files that get created (one for each .cpp file)
OBJS = main.o PNG.o HSLAPixel.o lodepng.o dragon_fractal.o p-adic_draw.o

CXX = clang++
CXXFLAGS = $(png_utils) -std=c++1y -stdlib=libc++ -c -g -O0 -Wall -Wextra -pedantic
LD = clang++
LDFLAGS = -std=c++1y -stdlib=libc++ -lc++abi -lm


all : $(EXENAME)

$(EXENAME) : output_msg $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXENAME)

# main.o lists itself and all .h files to be compiled
main.o : main.cpp dragon_fractal.h png_utils/HSLAPixel.h png_utils/PNG.h png_utils/RGB_HSL.h p-adic_draw.h
	$(CXX) $(CXXFLAGS) main.cpp

# Each .o file here lists the relevant .cpp and .h files
dragon_fractal.o : dragon_fractal.cpp dragon_fractal.h
	$(CXX) $(CXXFLAGS) dragon_fractal.cpp

p-adic_draw.o : p-adic_draw.cpp p-adic_draw.h
	$(CXX) $(CXXFLAGS) p-adic_draw.cpp

# For files within another directory, put the file path. Nothing can be above the directory with Makefile.
PNG.o : png_utils/PNG.cpp png_utils/PNG.h png_utils/HSLAPixel.h png_utils/lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) png_utils/PNG.cpp

HSLAPixel.o : png_utils/HSLAPixel.cpp png_utils/HSLAPixel.h
	$(CXX) $(CXXFLAGS) png_utils/HSLAPixel.cpp

lodepng.o : png_utils/lodepng/lodepng.cpp png_utils/lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) png_utils/lodepng/lodepng.cpp

clean : 
	rm *.o


# Stuff for test file:
# Custom Clang version enforcement Makefile rule:
ccred=$(shell echo -e "\033[0;31m")
ccyellow=$(shell echo -e "\033[0;33m")
ccend=$(shell echo -e "\033[0m")

IS_EWS=$(shell hostname | grep "ews.illinois.edu")
IS_CORRECT_CLANG=$(shell clang -v 2>&1 | grep "version 6")
ifneq ($(strip $(IS_EWS)),)
ifeq ($(strip $(IS_CORRECT_CLANG)),)
CLANG_VERSION_MSG = $(error $(ccred) On EWS, please run 'module load llvm/6.0.1' first when running png_utils assignments. $(ccend))
endif
else
CLANG_VERSION_MSG = $(warning $(ccyellow) Looks like you are not on EWS. Be sure to test on EWS before the deadline. $(ccend))
endif

output_msg: ; $(CLANG_VERSION_MSG)

# Be sure to add output_msg as dependency target for your `dragon_fractal` binary

# test: output_msg tests.o PNG.o HSLAPixel.o lodepng.o dragon_fractal.o
# 	$(LD) tests.o PNG.o HSLAPixel.o lodepng.o dragon_fractal.o $(LDFLAGS) -o test

# tests.o: tests/tests.cpp tests/catch.hpp png_utils/PNG.h png_utils/HSLAPixel.h
# 	$(CXX) $(CXXFLAGS) tests/tests.cpp
    
.PHONY: output_msg