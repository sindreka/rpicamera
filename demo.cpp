
#include <libcamera/libcamera.h>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#include "rpicamera.h"

constexpr const double R = 0.299;
constexpr const double G = 0.587;
constexpr const double B = 0.114;

void savePGM(int *image, const int width, const int height, std::string filename) {
  
  std::ofstream file(filename + ".pgm");
  file << "P2\n" << width << " " << height << " 255";

  for (int pixel_index = 0; pixel_index < height*width; ++pixel_index) {
    if (!(pixel_index%(width))) {
      file << "\n";
    }
    file << image[pixel_index] << " " ;
  }
}

void downsizeMonoExtract(int *output, uint8_t *input, const size_t height, const size_t width, const size_t stride, const size_t factor) {
  for (int y = 0; y < height; y += factor) {
    for (int x = 0; x < width; x += factor) {
    
      const int out_index = int(y/factor) * int(width/factor) + int(x/factor);
      int in_index = y*stride + 3*x;
      output[out_index] = int((double(input[in_index + 0])*R + double(input[in_index + 1])*G + double(input[in_index + 2])*B));
    }
  }
}

int main() {
  const double fps = 1.0;
  int width = 1920;
  int height = 1080;
  
  auto camera = rpic::rpiCamera();
  camera.setWidth(width); 
  camera.setHeight(height);
  camera.setFormat(libcamera::formats::RGB888);
  camera.setRotation(libcamera::Orientation::Rotate0Mirror);

  const double period = 1000.0/fps;

  camera.open();
  
  int stride;
  camera.getSize(height,width,stride);
  int *image = new int [width * height];
  const int factor = 6;

  int index = 0;
  while (index < 10) {
    ++index;
    camera.grab();
    uint8_t *data = camera.retrieve();
    downsizeMonoExtract(image,data,height,width,stride,factor);
    savePGM(image,width/factor,height/factor,"image_index_" + std::to_string(index));
    std::chrono::duration<double, std::milli> delta_ms(period);
  }
    
    return 0;
}

