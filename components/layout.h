#pragma once

#include <thread>
#include <X11/Xlib.h>
#include <unistd.h> 
#include <stdio.h>  
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>

enum valid_element_types {

  BUTTON,
  TEXT_INPUT_BOX,
  TEXT_BOX,
  BUTTON_LABELED,
  IMAGE,
  SLIDER,
  SLIDER_LABELED,
  TABLE,
  BORDER

};

struct window_layout_struct {

  std::vector<valid_element_types> type;
  std::vector<unsigned int> anchor_x;
  std::vector<unsigned int> anchor_y;
  std::vector<unsigned int> size_x;
  std::vector<unsigned int> size_y;
  std::vector<unsigned int> id;
  std::vector<std::string> data;
};


class window_layout {

public:

  window_layout_struct* layout;

  unsigned int element_count;

  window_layout() {

    layout = new window_layout_struct();
    
    element_count = 0;

  }

  ~window_layout() {

    delete layout;

  }
  
  bool add_element(valid_element_types type, unsigned int anchor_x, unsigned int anchor_y, unsigned int size_x, unsigned int size_y, std::string data) {

    layout->type.push_back(type);
    layout->anchor_x.push_back(anchor_x);
    layout->anchor_y.push_back(anchor_y);
    layout->size_x.push_back(size_x);
    layout->size_y.push_back(size_y);
    layout->id.push_back(element_count);
    layout->data.push_back(data);

    element_count++;
    
    return true;
    
  }

  bool validate_window_layout_struct() {

    if((layout->type.size() + layout->anchor_x.size() + layout->anchor_y.size() + layout->size_x.size() + layout->size_y.size() + layout->id.size()) / 6 == layout->id.size() ) {
      return true; } else { return false;}

  }

  window_layout_struct* get_window_layout() {

    return this->layout;

  }
  
};
