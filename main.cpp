#include <iostream>
#include <stdlib.h>

#include "dragon_fractal.h"
#include "p-adic_draw.h"

#include "png_utils/PNG.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << "Usage: \n\t./image dragon <width>\n\t./image p_adic <width> <p> <children>" << std::endl;
    exit(1);
  }

  if (!strcmp(argv[1], "dragon")) {
    unsigned int width = (unsigned int) atoi(argv[2]);
    png_utils::PNG png = dragon_fractal(width, width);
    png.writeToFile("dragon.png");
  }
  
  if (!strcmp(argv[1], "p_adic")) {
    unsigned int width = (unsigned int) atoi(argv[2]);
    unsigned int p = (unsigned int) atoi(argv[3]);
    unsigned int children = (unsigned int) atoi(argv[4]);
    png_utils::PNG png = p_adic_draw(width, width, p, children);
    png.writeToFile("p_adic.png");
  }
  
  return 0;
}
