#pragma once

#include <thread>
#include <X11/Xlib.h>
#include <unistd.h> 
#include <stdio.h>  
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>

void draw_box(Display* display, Window window, GC gc, unsigned int anchor_x, unsigned int anchor_y, unsigned int size_x, unsigned int size_y) {

  XDrawLine(display, window, gc, anchor_x, anchor_y, anchor_x+size_x, anchor_y);
  XDrawLine(display, window, gc, anchor_x+size_x, anchor_y, anchor_x+size_x, anchor_y+size_y);
  XDrawLine(display, window, gc, anchor_x+size_x, anchor_y+size_y, anchor_x, anchor_y+size_y);
  XDrawLine(display, window, gc, anchor_x, anchor_y+size_y, anchor_x, anchor_y);

  return;
}

void draw_dynamic_box_with_text(Display* display, Window window, GC gc, unsigned int anchor_x, unsigned int anchor_y, unsigned int size_x, unsigned int size_y, std::string data, XFontStruct* font) {

  std::string todraw = data;
  
  int font_height = font->ascent + font->descent;
  int font_ascent = font->ascent; 
  int font_descent = font->descent;
  int max_width = font->max_bounds.width;

  int total_text_width = max_width * todraw.length();
  
  XDrawLine(display, window, gc, anchor_x, anchor_y, anchor_x+total_text_width, anchor_y);
  XDrawLine(display, window, gc, anchor_x+total_text_width, anchor_y, anchor_x+total_text_width, anchor_y+size_y);
  XDrawLine(display, window, gc, anchor_x+total_text_width, anchor_y+size_y, anchor_x, anchor_y+size_y);
  XDrawLine(display, window, gc, anchor_x, anchor_y+size_y, anchor_x, anchor_y);
  
  XDrawString(display, window, gc, anchor_x, anchor_y+size_y, todraw.c_str() , todraw.length() );
  
  return;
}



