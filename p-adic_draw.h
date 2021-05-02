#pragma once

#include "png_utils/PNG.h"

#include <string>
#include <vector>

// enum to determine which type of image to generate:
// classic makes the usual round broccoli-like tree
// spikey magnifies the center cosets
// swirl magnifies the 0 cosets
enum image_style { classic, spikey, swirl };

// used to specify where HSLAPixels drawn with will have bright or dull colors
enum color_type { bright, dull };

struct Node {
    unsigned int x;
    unsigned int y;
    Node* children;
};

// helper functions:

png_utils::PNG p_adic_draw(int width, int height, const int p, int children, image_style style);
double get_new_angle(double theta, int c, const int p);
std::vector<int> draw_line_angle(png_utils::PNG* image, int x, int y, double d, double theta, int hue, color_type ct, double start_width, double end_width);
double get_branch_length(double d, double theta, double new_angle, const int p);
void draw_branch(png_utils::PNG* image, int x, int y, double d, double theta, int hue, int children, const int p, Node* node, color_type ct);
void trace_sequence(png_utils::PNG* image, const int p, std::vector<unsigned> tuple, int len, double start_d, int hue);
void* thread_draw_branch_wrapper(void* job);
void draw_branch(png_utils::PNG* image, int x, int y, double d, double theta, int hue, int children, const int p, Node* node, color_type ct);
png_utils::PNG plot_power_series(int width, int height, const int p, int children, image_style s);
void* thread_process_image(void* in);

