#pragma once

#include "png_utils/PNG.h"

#include <string>
#include <vector>

// enum to determine which type of image to generate:
// classic makes the usual round broccoli-like tree
// spikey magnifies the center cosets
// swirl magnifies the 0 cosets
enum image_style { classic, spikey, swirl };

// helper functions:

png_utils::PNG p_adic_draw(int width, int height, int p, int children, image_style style);

double get_new_angle(double theta, int c, int p);

std::vector<int> draw_line_angle(png_utils::PNG* image, int x, int y, double d, double theta, int hue, int width);

double get_branch_length(double d, double theta, double new_angle, int p);

void draw_branch(png_utils::PNG* image, int x, int y, double d, double theta, int hue, int children, int p);

void* thread_draw_branch_wrapper(void* job);