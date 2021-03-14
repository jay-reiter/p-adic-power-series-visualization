#include "png_utils/PNG.h"
#include "png_utils/HSLAPixel.h"

#include "p-adic_draw.h"

#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <pthread.h>

static const double PI = 3.1415926535897;
static double r;

// array to store p hues used for each coset
int* hues;

image_style style;

// struct used for passing info on a branch job to threads
typedef struct thread_branch_info_ {
    png_utils::PNG* png;
    int x;
    int y;
    double d;
    double theta;
    double hue;
    double children;
    double p;
} thread_branch_info;

// lock for thread safety
pthread_mutex_t lock;

/**
 * colors all the points in an epsilon neighborhood of x and y
 */
void color_neighborhood(png_utils::PNG* image, double x, double y, double eps, int hue) {
    // always color the point closest to (x,y)
    pthread_mutex_lock(&lock);
    png_utils::HSLAPixel & pixel = image->getPixel(std::floor(x), std::floor(y));
    pixel = png_utils::HSLAPixel(hue, 1, 0.5);
    pthread_mutex_unlock(&lock);

    // iterate over square containing circle
    for (int x_hat = std::floor(x - eps); x_hat < std::ceil(x + eps); x_hat++) {
        for (int y_hat = std::floor(y - eps); y_hat < std::ceil(y + eps); y_hat++) {

            // if the point is in an epsilon neighborhood of (x,y), we color it
            if (std::pow(x_hat - x, 2) + std::pow(y_hat - y, 2) < eps * eps) {
                pthread_mutex_lock(&lock);
                png_utils::HSLAPixel & pixel = image->getPixel(x_hat, y_hat);
                pixel = png_utils::HSLAPixel(hue, 1, 0.5);
                pthread_mutex_unlock(&lock);
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
std::vector<int> draw_line_angle(png_utils::PNG* image, int x, int y, double d, double theta, int hue, int width) {
    // unit vector in the direction theta
    double v_hat[2] = {std::cos(theta), std::sin(theta)};

    double curr_point[2] = {(double) x, (double) y};

    // start at (x,y) and color all lattice points near (x,y)+k*v_hat for 0<k<d
    for (int k = 0; k < d; k++) {
        // increment current point by v_hat
        // TODO: increment by width/2*v_hat insdead to improve performance
        curr_point[0] += v_hat[0];
        curr_point[1] += v_hat[1];

        color_neighborhood(image, curr_point[0], curr_point[1], width, hue);
    }
    std::vector<int> ret = {(int) std::floor(curr_point[0]), (int) std::floor(curr_point[1])};
    return ret;
}

void draw_branch(png_utils::PNG* image, int x, int y, double d, double theta, int hue, int children, int p) {
    if (children == -1) {
        // this is a leaf
        return;
    }

    double width = p * std::log10(d) * children / 10;

    // TODO: store this point in a p^n coset vector somewhere so we can use its coordinates for calculations later; may need to use a struct?
    std::vector<int> new_center = draw_line_angle(image, x, y, d, theta, hue, width);

    // recursively call p more branches off the end of this one
    for (int c = 0; c < p; c++) {

        double new_angle = get_new_angle(theta, c, p);
        double new_d = std::abs(get_branch_length(d, theta, new_angle, p));

        draw_branch(image, new_center[0], new_center[1], new_d, new_angle, hues[c], children - 1, p);
    }
}

/**
 * calculates a new angle for a branch to stem from based on parent angle and branch coset
 */
double get_new_angle(double theta, int c, int p) {
    // rotate all the way around each vertex
    if (style == classic) {
        double new_angle = theta + ((c * 2 * PI) / p);
        return new_angle;
    }
    // TODO: rotate most of the way around each vertex
    if (style == swirl) {
        double k = c - std::floor(p * 0.5);
        double new_angle = theta - k * 2 * PI / (p + 1.);

        return new_angle;
    }
    // branches only come out in 180 degree range
    else {
        double new_angle = theta + ((c * 2 * PI) / p);
        // reduce modulo PI
        new_angle = std::cos(theta - new_angle) > 0 ? new_angle : new_angle - PI;
        return new_angle;
    }
}

/**
 * calculates new branch length based on angle relative to parent angle
 */
double get_branch_length(double d, double theta, double new_angle, int p) {
    if (style == spikey) {
        return d * r * std::abs(1 - std::abs(std::fmod(theta - new_angle, PI)) / ((PI / 2)));
    }

    if (style == swirl) {
        double scale_factor = std::fmod(theta - std::fmod(new_angle, 2 * PI) + PI, 2 * PI) / (2 * PI);
        scale_factor = std::pow(scale_factor, 1.5);
        return d * (0.08 + 0.27 * scale_factor) * 1.5;
    }

    // makes branch length the same across cosets; only decreases with generation (power of p coset)
    if (style == classic) {
        return d * r;
    }
}

void* thread_draw_branch_wrapper(void* job) {
    thread_branch_info* _job = (thread_branch_info*) job;

    draw_branch(_job->png, _job->x, _job->y, _job->d, _job->theta, _job->hue, _job->children, _job->p);
    return NULL;
}

/**
 * Wrapper function for generating image of Z_p with children
 * will eventually use multithreading
 */
png_utils::PNG p_adic_draw(int width, int height, int p, int children, image_style s) {
    
    // make png object to draw on
    png_utils::PNG image(width, height);

    // initialize constants
    style = s;
    // initial length of branch components; will make tree go all the way to the edge of the png
    double start_d;
    int center[2] = {width / 2, 0};
    switch (style) {

        case classic:
            r = 0.2; 
            center[1] = width / 2;
            start_d = (1 - r) * width / 2.;
            break;

        case spikey:
            r = 0.6; 
            center[1] = (p - 1) * height / p;
            start_d = (1 - r) * width / 2.;
            break;

        case swirl:
            r = 0.6; 
            center[1] = width / 2;
            start_d = (0.5) * width;
            break;
    }

    hues = (int*) calloc(p, sizeof(int));
    for (int i = 0; i < p; i++) {
        hues[i] = (int) std::fmod(i * (360 / p), 360);
        // std::cout << "hues[" << i << "] = " << hues[i] << std::endl;
    }

    // array to store thread ids; one thread per root branch
    pthread_t threads[p];
    thread_branch_info* jobs[p];
    pthread_mutex_init(&lock, NULL);

    //Set the background to a cool gradient
    for (unsigned int x = 0; x < image.width(); x++){
        for (unsigned int y = 0; y < image.height(); y++){
        png_utils::HSLAPixel & pixel = image.getPixel(x, y);
        pixel.l = 0.2;
        pixel.h = 220 + std::floor(80 * y / image.height());
        pixel.s = 0.4;
        }
    }

    double theta = 3 * PI / 2;
    // create threads and send them on jobs
    for (int c = 0; c < p; c++) {

        jobs[c] = new thread_branch_info();

        jobs[c]->png = &image;
        jobs[c]->children = children;
        jobs[c]->hue = hues[c];
        jobs[c]->theta = get_new_angle(theta, c, p);
        jobs[c]->d = get_branch_length(start_d / r, theta, jobs[c]->theta, p);
        jobs[c]->x = center[0];
        jobs[c]->y = center[1];
        jobs[c]->p = p;
    }

    // start threads
    for (int c = 0; c < p; c++) {
        pthread_create(threads + c, NULL, thread_draw_branch_wrapper, (void *) jobs[c]);
    }

    // wait for all the threads to finish
    for (int c = 0; c < p; c++) {
        pthread_join(threads[c], NULL);
    }

  return image;
}