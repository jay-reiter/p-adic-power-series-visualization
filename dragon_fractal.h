#pragma once

#include "png_utils/PNG.h"

#include <string>

void rotate(std::string inputFile, std::string outputFile);
png_utils::PNG dragon_fractal(unsigned int width, unsigned int height);
