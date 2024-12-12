#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <array>

#pragma pack(push, 1) // why does padding exist :(
struct BMPHeader {
  uint16_t fileType;      
  uint32_t fileSize;      
  uint16_t reserved1;     
  uint16_t reserved2;     
  uint32_t offsetData;    
  
  uint32_t size;          
  int32_t width;          
  int32_t height;         
  uint16_t planes;        
  uint16_t bitCount;      
  uint32_t compression;   
  uint32_t sizeImage;     
  int32_t xPixelsPerMeter; 
  int32_t yPixelsPerMeter; 
  uint32_t colorsUsed;     
  uint32_t colorsImportant; 
};
#pragma pack(pop)

std::vector<std::vector<std::array<uint8_t, 3>>> read_bmp(const std::string& filePath) {
  std::ifstream file(filePath, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Could not open the file!");
  }
  
  BMPHeader header;
  file.read(reinterpret_cast<char*>(&header), sizeof(header));
  
  
  if (header.fileType != 0x4D42) { 
    throw std::runtime_error("Not a valid BMP file!");
  }
  
  
  if (header.bitCount != 24) {
    throw std::runtime_error("Only 24-bit BMP files are supported!");
  }
  
  
  file.seekg(header.offsetData, std::ios::beg);
  
  
  std::vector<std::vector<std::array<uint8_t, 3>>> pixelData(header.height, std::vector<std::array<uint8_t, 3>>(header.width));
  
  
  for (int32_t y = header.height - 1; y >= 0; --y) {
    for (int32_t x = 0; x < header.width; ++x) {
      std::array<uint8_t, 3> pixel;
      file.read(reinterpret_cast<char*>(pixel.data()), pixel.size());
      pixelData[y][x] = pixel;
    }
    
    file.ignore((4 - (header.width * 3) % 4) % 4);
  }
  
  return pixelData;
}
