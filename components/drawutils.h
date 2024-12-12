#pragma once

#define MARGIN_X 3
#define MARGIN_Y 3

#include <thread>
#include <X11/Xlib.h>
#include <unistd.h> 
#include <stdio.h>  
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>

#include "bmp_decode.h"

struct layout_struct;

uint32_t merge_to_rgb(uint8_t r, uint8_t g, uint8_t b) {
  return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | static_cast<uint32_t>(b);
}

void draw_image(Display* display, Window window, GC gc, unsigned int anchor_x, unsigned int anchor_y, std::vector<std::vector<std::array<uint8_t, 3>>> pixel_data) {
  
  size_t width = pixel_data[0].size();
  size_t height = pixel_data.size();
    
  for(int i = 0; i < width; ++i) {
    
    for(int j = 0; j < height; ++j) {
	
      uint32_t color = merge_to_rgb(pixel_data[i][j][0],pixel_data[i][j][2],pixel_data[i][j][2]);
      
      XSetForeground(display, gc, color);

      XDrawPoint(display, window, gc, j + anchor_x , i + anchor_y);
      
    }

  }

  return;
  
}

void draw_box(Display* display, Window window, GC gc, unsigned int anchor_x, unsigned int anchor_y, unsigned int size_x, unsigned int size_y) {

  XDrawLine(display, window, gc, anchor_x, anchor_y, anchor_x+size_x, anchor_y);
  XDrawLine(display, window, gc, anchor_x+size_x, anchor_y, anchor_x+size_x, anchor_y+size_y);
  XDrawLine(display, window, gc, anchor_x+size_x, anchor_y+size_y, anchor_x, anchor_y+size_y);
  XDrawLine(display, window, gc, anchor_x, anchor_y+size_y, anchor_x, anchor_y);

  return;
}

void draw_dynamic_box_with_text(Display* display, Window window, GC gc, unsigned int id , XFontStruct* font, unsigned int anchor_x, unsigned int anchor_y, unsigned int *size_x, unsigned int *size_y, std::string label) {

  std::string todraw = label;
  
  int font_height = font->ascent + font->descent;
  int font_ascent = font->ascent; 
  int font_descent = font->descent;
  int max_width = font->max_bounds.width;
  
  int total_text_width = (max_width * todraw.length()) + 2*MARGIN_X;
  int total_text_height = (font_height);

  *size_x = total_text_width;
  *size_y = total_text_height;

  XDrawLine(display, window, gc, anchor_x, anchor_y, anchor_x+total_text_width, anchor_y);
  XDrawLine(display, window, gc, anchor_x+total_text_width, anchor_y, anchor_x+total_text_width, anchor_y+total_text_height);
  XDrawLine(display, window, gc, anchor_x+total_text_width, anchor_y+total_text_height, anchor_x, anchor_y+total_text_height);
  XDrawLine(display, window, gc, anchor_x, anchor_y+total_text_height, anchor_x, anchor_y);
  
  XDrawString(display, window, gc, anchor_x+MARGIN_X, anchor_y+total_text_height-MARGIN_Y+1, todraw.c_str() , todraw.length() );

  XFlush(display);
  
  return;
}

void draw_dynamic_text(Display* display, Window window, GC gc, unsigned int id , XFontStruct* font, unsigned int anchor_x, unsigned int anchor_y, unsigned int *size_x, unsigned int *size_y, std::string label) {

  std::string todraw = label;
  
  int font_height = font->ascent + font->descent;
  int font_ascent = font->ascent; 
  int font_descent = font->descent;
  int max_width = font->max_bounds.width;
  
  int total_text_width = (max_width * todraw.length()) + 2*MARGIN_X;
  int total_text_height = (font_height);

  *size_x = total_text_width;
  *size_y = total_text_height;

  XDrawString(display, window, gc, anchor_x+MARGIN_X, anchor_y+total_text_height-MARGIN_Y+1, todraw.c_str() , todraw.length() );

  XFlush(display);
  
  return;
}

void draw_dynamic_underlined_text(Display* display, Window window, GC gc, unsigned int id , XFontStruct* font, unsigned int anchor_x, unsigned int anchor_y, unsigned int *size_x, unsigned int *size_y, std::string label) {

  std::string todraw = label;
  
  int font_height = font->ascent + font->descent;
  int font_ascent = font->ascent; 
  int font_descent = font->descent;
  int max_width = font->max_bounds.width;
  
  int total_text_width = (max_width * todraw.length()) + 2*MARGIN_X;
  int total_text_height = (font_height);

  *size_x = total_text_width;
  *size_y = total_text_height;

  XDrawLine(display, window, gc, anchor_x+total_text_width, anchor_y+total_text_height, anchor_x, anchor_y+total_text_height);
  
  XDrawString(display, window, gc, anchor_x+MARGIN_X, anchor_y+total_text_height-MARGIN_Y+1, todraw.c_str() , todraw.length() );

  XFlush(display);
  
  return;
}

void draw_dynamic_window_border(Display* display, Window window, GC gc, unsigned int width) {
  
  XWindowAttributes window_attributes;

  if (XGetWindowAttributes(display, window, &window_attributes)) {

    for(int i = 0; i<width; ++i) {

      draw_box(display,window,gc,0+i,0+i,window_attributes.width-(i*2)-1, window_attributes.height-(i*2)-1);
   
    }
    
  } else {

    std::cout << "Failed  to get window info." << std::endl;

  }

  return;

}
