#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <sstream>

#include "layout.h"

enum valid_element_types;

struct layout_struct;

valid_element_types string_to_element_type(const std::string& typeStr) {
    if (typeStr == "BUTTON") return BUTTON;
    if (typeStr == "TABLE") return TABLE;
    if (typeStr == "TEXT") return TEXT;
    if (typeStr == "TEXT_UNDERLINED") return TEXT_UNDERLINED;
    if (typeStr == "TEXT_INPUT_BOX") return TEXT_INPUT_BOX;
    if (typeStr == "TEXT_BOX") return TEXT_BOX;
    if (typeStr == "BUTTON_LABELED") return BUTTON_LABELED;
    if (typeStr == "IMAGE") return IMAGE;
    if (typeStr == "SLIDER") return SLIDER;
    if (typeStr == "SLIDER_LABELED") return SLIDER_LABELED;
    if (typeStr == "BORDER") return BORDER;
    return UNKNOWN; 
}

layout_struct* deserialize_layout_file(std::string filename) {

  layout_struct *constructed_layout_struct = new layout_struct;
  
  std::vector<std::string> lines;
  std::ifstream file(filename);
  
  if (!file.is_open()) {

    std::cout << "Error: Could not open the file " << filename << std::endl;
    return constructed_layout_struct;

  }
  
  std::string line;
  while (std::getline(file, line)) { 

    lines.push_back(line);

  }
  
  file.close();

  for(int i = 0; i < lines.size(); ++i) {

    std::vector<std::string> result;

    std::cout << lines[0] << std::endl;
    
    std::stringstream ss(lines[i]); 
    std::string item;
    
    result.clear();
    
    unsigned int slot = 0;
    
    while (std::getline(ss, item, ',')) {
      result.push_back(item); 
    }

    unsigned int next_free_slot = constructed_layout_struct->type.size();
    
    constructed_layout_struct->type.emplace_back(string_to_element_type(result[0]));
    constructed_layout_struct->anchor_x.emplace_back(std::stoi(result[1]));
    constructed_layout_struct->anchor_y.emplace_back(std::stoi(result[2]));
    constructed_layout_struct->size_x.emplace_back(std::stoi(result[3]));
    constructed_layout_struct->size_y.emplace_back(std::stoi(result[4]));
    constructed_layout_struct->label.emplace_back(result[5]);
    constructed_layout_struct->callback.emplace_back(std::stoi(result[6]));
    constructed_layout_struct->id.emplace_back(next_free_slot);
    
  }

  return constructed_layout_struct;

}
