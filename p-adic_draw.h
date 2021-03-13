#pragma once

#include "png_utils/PNG.h"

#include <string>
#include <vector>

png_utils::PNG p_adic_draw(int width, int height, int p, int children);
double get_new_angle(double theta, int c, int p);
std::vector<int> draw_line_angle(png_utils::PNG & image, int x, int y, double d, double theta, int hue, int width);
double get_branch_length(double d, double theta, double new_angle, int p);