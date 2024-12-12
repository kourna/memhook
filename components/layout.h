#pragma once

#include <variant>
#include <thread>
#include <X11/Xlib.h>
#include <unistd.h> 
#include <stdio.h>  
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <memory>

#include "drawutils.h"

enum valid_element_types {

  BUTTON,
  TABLE,
  TEXT_INPUT_BOX,
  TEXT_BOX,
  TEXT,
  TEXT_UNDERLINED,
  BUTTON_LABELED,
  IMAGE,
  SLIDER,
  SLIDER_LABELED,
  BORDER,
  UNKNOWN

};

struct style_rules {


};

struct layout_struct {

  std::vector<valid_element_types>type;
  std::vector<unsigned int>anchor_x;
  std::vector<unsigned int>anchor_y;
  std::vector<unsigned int>size_x;
  std::vector<unsigned int>size_y;
  std::vector<std::string>label;
  std::vector<unsigned int>callback;
  std::vector<unsigned int>id;

};

void add_element(layout_struct *mod_layout_struct, valid_element_types ntype, unsigned int nanchor_x, unsigned int nanchor_y, unsigned int nsize_x, unsigned int nsize_y, std::string nlabel, unsigned int ncallback) {

  unsigned int next_free_slot = mod_layout_struct->id.size();
  
  mod_layout_struct->type.emplace_back(ntype);
  mod_layout_struct->anchor_x.emplace_back(nanchor_x);
  mod_layout_struct->anchor_y.emplace_back(nanchor_y);
  mod_layout_struct->size_x.emplace_back(nsize_x);
  mod_layout_struct->size_y.emplace_back(nsize_y);
  mod_layout_struct->label.emplace_back(nlabel);
  mod_layout_struct->callback.emplace_back(ncallback);
  mod_layout_struct->id.emplace_back(next_free_slot);

  return;
  
}
