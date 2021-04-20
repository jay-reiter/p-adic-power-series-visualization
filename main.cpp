#include <iostream>
#include <stdlib.h>

#include "dragon_fractal.h"
#include "p-adic_draw.h"
#include "p_adic.h"

#include "png_utils/PNG.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>

void math_test();

int main(int argc, char *argv[]) {
  if (argc == 1) {
    // usage message
    std::cout << "Usage: \n\t./image dragon <width>\n\t./image p_adic <width> <p> <children> [target_file_name]\n";
    exit(1);
  }

  if (!strcmp(argv[1], "dragon")) {
    // dragon fractal functionality
    unsigned int width = (unsigned int) atoi(argv[2]);
    png_utils::PNG png = dragon_fractal(width, width);
    png.writeToFile("dragon.png");
  }
  
  if (!strcmp(argv[1], "p_adic")) {
    // parse arguments
    unsigned int width = (unsigned int) atoi(argv[2]);
    unsigned int p = (unsigned int) atoi(argv[3]);
    unsigned int children = (unsigned int) atoi(argv[4]);

    // TODO: make changing the enum input here easier
    png_utils::PNG png = p_adic_draw(width, width, p, children, swirl);
    
    // if there is a user-specified target file
    if (argc == 6) {
      // construct file name
      char* target_name = (char*) malloc(strlen(argv[5]) + 1 + strlen(".png"));
      strcpy(target_name, argv[5]);
      strcat(target_name, ".png");

      png.writeToFile(target_name);
      
      free(target_name);
    }
    else {
      png.writeToFile("p_adic.png");
    }
  }

  if (!strcmp(argv[1], "math")) {
    math_test();
  }
  
  return 0;
}

void math_test() {
  // p_adic<7> zero = p_adic<7>();
  p_adic<7> a = p_adic<7>({1,2,1});
  p_adic<7> b = p_adic<7>({4,3,2});
  std::cout << "Here are the numbers:\n" << a << "\n" << b << std::endl;
  std::cout << "a + b = " << a + b << std::endl; 
}
