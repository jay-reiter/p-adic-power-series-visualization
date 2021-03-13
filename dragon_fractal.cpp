#include "png_utils/PNG.h"
#include "png_utils/HSLAPixel.h"

#include <string>
#include <iostream>
#include <cmath>


std::string dragonString(int iterations);
void drawLine(png_utils::PNG & image, int x1, int y1, int x2, int y2, int hue, int step);
std::string reverseStr(std::string str);
int newDirection(int currentDirection, char turn);

//Rotates png stored at inputFile and places it in outputFile 
void rotate(std::string inputFile, std::string outputFile) {
  // TODO: Part 2

  png_utils::PNG input = png_utils::PNG();
  input.readFromFile(inputFile);

  double yCenter = std::ceil(input.height() / 2);
  double xCenter = std::ceil(input.width() / 2);

  for (unsigned x = 0; x < input.width(); x++) {
    for (unsigned y = 0; y < yCenter; y++) {

      png_utils::HSLAPixel & pixel_1 = input.getPixel(x, y);
      png_utils::HSLAPixel & pixel_2 = input.getPixel(input.width() - 1 - x, input.height() - 1 - y);

      png_utils::HSLAPixel pixel_temp = pixel_1;
      pixel_1 = pixel_2;
      pixel_2 = pixel_temp;
      
    }
  }

  //if picture has odd height, go through half of the middle row
  if (input.height() % 2 == 1){
    for (unsigned x = 0; x < xCenter; x++){
      png_utils::HSLAPixel & pixel_1 = input.getPixel(x, yCenter);
      png_utils::HSLAPixel & pixel_2 = input.getPixel(input.width() - 1 - x, yCenter);
      
      png_utils::HSLAPixel pixel_temp = pixel_1;
      pixel_1 = pixel_2;
      pixel_2 = pixel_temp;
    }
  } 

  input.writeToFile(outputFile);  
}

//draws a colored line on the given PNG; must have x1 == x2 or y1 == y2
void drawLine(png_utils::PNG & image, int x1, int y1, int x2, int y2, int hue, int step){

  //std::cout << "Drawing from (" << x1 << "," << y1 << ") to (" << x2 << "," << y2 << ")";

  x1 = x1 % image.width();
  x2 = x2 % image.width();
  y1 = y1 % image.height();
  y2 = y2 % image.height();

  //avoids lines that cross the whole picture
  if ((std::abs(x1 - x2) > step) || (std::abs(y1 - y2) > step))
    return;

  if (x1 == x2){
    if (y1 < y2){
      for (unsigned y = y1; y < y2; y++) {
        png_utils::HSLAPixel & pixel = image.getPixel(x1, y);
        pixel = png_utils::HSLAPixel(hue, 1, .5);
      }
    }
    else{
      for (unsigned y = y2; y < y1; y++) {
        png_utils::HSLAPixel & pixel = image.getPixel(x1, y);
        pixel = png_utils::HSLAPixel(hue, 1, .5);
      }
    }
  }
  else{
    if (x1 < x2){
      for (unsigned x = x1; x < x2; x++) {
        png_utils::HSLAPixel & pixel = image.getPixel(x, y1);
        pixel = png_utils::HSLAPixel(hue, 1, .5);
      }
    }
    else{
      for (unsigned x = x2; x < x1; x++) {
        png_utils::HSLAPixel & pixel = image.getPixel(x, y1);
        pixel = png_utils::HSLAPixel(hue, 1, .5);
      }
    }
  }
}

//reverses a string; "Hello" -> "olleH"
std::string reverseStr(std::string str){
  std::string revStr = "";
  for (int i = str.length() - 1; i >= 0; i--)
    revStr += str[i];
  return revStr;
}

//generates string for dragon fractal
std::string dragonString(int iterations){
  using std::string;
  
  const string r = "r";
  const string l = "l";
  
  string oldStr = r;
  string newStr = oldStr;
  string tempStr;
  int cycle = 1;

  while (cycle < iterations){
    newStr = oldStr + r;
    oldStr = reverseStr(oldStr);

    tempStr = "";
    for (int i = 0; i < oldStr.length(); i++){
      if (oldStr.at(i) == 114)
        tempStr += l;
      else if (oldStr.at(i) == 108)
        tempStr += r;
    }
    oldStr = tempStr;

    newStr = newStr + oldStr;
    oldStr = newStr;
    cycle++;
  }
  return newStr;
}

//decides new directon for cursor based on current orientation and turn; N = 0, E = 1, S = 2, W = 3;
int newDirection(int currentDirection, char turn){
  if (turn == 114) //if right turn
    return (currentDirection + 1) % 4;
  else if (turn == 108) //if left turn
    return (currentDirection + 3) % 4;
}

//makes ART
png_utils::PNG dragon_fractal(unsigned int width, unsigned int height) {
  png_utils::PNG png(width, height);
  // TODO: Part 3

  const int illiniBlue = 216;
  const int illiniOrange = 22;

  //Set the background to white
  for (unsigned int x = 0; x < png.width(); x++){
    for (unsigned int y = 0; y < png.height(); y++){
      png_utils::HSLAPixel & pixel = png.getPixel(x, y);
      pixel.l = 0.2;
      pixel.h = illiniBlue + std::floor(80 * y / png.height());
      pixel.s = 0.4;
    }
  }

  //origin point
  const int x0 = (int) std::floor(7 * width / 12);
  const int y0 = (int) std::floor(9 * height / 40);

  //cursor location
  int xLoc = x0;
  int yLoc = y0;

  //cursor destination
  int xDes, yDes;

  int currentDirection = 0; //start facing north
  int step = 2; //step size in pixels

  std::string dragonStr = "";
  dragonStr += dragonString(17);
  for (int i = 0; i < dragonStr.length(); i++){
    if (currentDirection == 0){
      xDes = xLoc;
      yDes = yLoc + step;
    }
    else if (currentDirection == 1){
      xDes = xLoc + step;
      yDes = yLoc;
    }
    else if (currentDirection == 2){
      xDes = xLoc;
      yDes = yLoc - step;
    }
    else if (currentDirection == 3){
      xDes = xLoc - step;
      yDes = yLoc;
    }

    int newHue = ((int) std::floor(0.002 * i) % 69);
    drawLine(png, xLoc, yLoc, xDes, yDes, newHue, step);

    //update position;
    xLoc = xDes;
    yLoc = yDes;

    currentDirection = newDirection(currentDirection, dragonStr.at(i));
  }
  
  return png;
}


