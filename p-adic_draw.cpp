#include "png_utils/PNG.h"
#include "png_utils/HSLAPixel.h"

#include "p-adic_draw.h"

#include <string>
#include <iostream>
#include <cmath>
#include <vector>

static const double PI = 3.1415926535897;
static double r;

/**
 * colors all the points in an epsilon neighborhood of x and y
 */
void color_neighborhood(png_utils::PNG & image, double x, double y, double eps, int hue) {
    // always color the point closest to (x,y)
    png_utils::HSLAPixel* pixel = &image.getPixel(std::floor(x), std::floor(y));
    *pixel = png_utils::HSLAPixel(hue, 1, 0.5);
    
    // iterate over square containing circle
    for (int x_hat = std::floor(x - eps); x_hat < std::ceil(x + eps); x_hat++) {
        for (int y_hat = std::floor(y - eps); y_hat < std::ceil(y + eps); y_hat++) {
            // if the point is in an epsilon neighborhood of (x,y), we color it
            if (std::pow(x_hat - x, 2) + std::pow(y_hat - y, 2) < eps * eps) {
                pixel = &image.getPixel(x_hat, y_hat);
                *pixel = png_utils::HSLAPixel(hue, 1, 0.5);
            }
        }
    }
}

/**
 * draws a line starting at (x,y) of length d in the direction theta (measured counterclockwise from positive x-axis)
 * theta should be in radians
 * also takes an input "hue" to determine the color of the line
 * returns the end point of the line segment drawn as a vector
 */
std::vector<int> draw_line_angle(png_utils::PNG & image, int x, int y, double d, double theta, int hue, int width) {
    // unit vector in the direction theta
    double v_hat[2] = {std::cos(theta), std::sin(theta)};

    double curr_point[2] = {(double) x, (double) y};

    // start at (x,y) and color all lattice points near (x,y)+k*v_hat for 0<k<d
    for (int k = 0; k < d; k++) {
        // increment current point by v_hat
        curr_point[0] += v_hat[0];
        curr_point[1] += v_hat[1];

        color_neighborhood(image, curr_point[0], curr_point[1], width, hue);
    }
    std::vector<int> ret = {(int) std::floor(curr_point[0]), (int) std::floor(curr_point[1])};
    return ret;
}

void draw_branch(png_utils::PNG & image, int x, int y, double d, double theta, int hue, int children, int p) {
    // this is a leaf
    if (children == -1) {
        return;
    }
    // TODO: store this point in a p^n coset vector somewhere so we can use its coordinates for calculations later; may need to use a struct?
    std::vector<int> new_center = draw_line_angle(image, x, y, d, theta, hue, std::log10(d) * children);

    // recursively call p more branches off the end of this one
    for (int c = 0; c < p; c++) {

        double new_angle = get_new_angle(theta, c, p);
        double new_d = get_branch_length(d, theta, new_angle, p);

        draw_branch(image, new_center[0], new_center[1], new_d, new_angle, (hue + 50) % 360, children - 1, p);
    }
}

/**
 * calculates a new angle for a branch to stem from based on parent angle and branch coset
 */
double get_new_angle(double theta, int c, int p) {
    double new_angle = theta + ((c * 2 * PI) / p);
    // reduce modulo PI
    new_angle = std::cos(theta - new_angle) > 0 ? new_angle : new_angle - PI;
    
    return new_angle;
}

/**
 * calculates new branch length based on angle relative to parent angle
 */
double get_branch_length(double d, double theta, double new_angle, int p) {
    // makes branch length depend on angle--useful for avoiding branhc collisions
    r = 0.6;
    return d * r * std::abs(1 - std::abs(std::fmod(theta - new_angle, PI)) / ((PI / 2)));

    // makes branch length the same across cosets; only decreases with generation (power of p coset)
    // r = 0.2;
    // return d * r;
}

/**
 * Wrapper function for generating image of Z_p with children
 * will eventually use multithreading
 */
png_utils::PNG p_adic_draw(int width, int height, int p, int children) {
    
    png_utils::PNG png(width, height);
    
    const int START_HUE = 200;
    r = 0.5;

    //Set the background to a cool gradient
    for (unsigned int x = 0; x < png.width(); x++){
        for (unsigned int y = 0; y < png.height(); y++){
        png_utils::HSLAPixel & pixel = png.getPixel(x, y);
        pixel.l = 0.2;
        pixel.h = 220 + std::floor(80 * y / png.height());
        pixel.s = 0.4;
        }
    }

    // initial length of branch components; will make tree go all the way to the edge of the png
    double start_d = (1 - r) * width / 2.;
    int center[2] = {width / 2, (p - 1) * height / p};

    // TODO: make each iteration of the loop call its own thread
    double theta = 3 * PI / 2;
    for (int c = 0; c < p; c++) {
        double new_angle = get_new_angle(theta, c, p);
        double new_d = get_branch_length(start_d / r, theta, new_angle, p);
        draw_branch(png, center[0], center[1], new_d, new_angle, START_HUE, children, p);
    }

  return png;
}