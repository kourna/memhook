#pragma once

#include <thread>
#include <X11/Xlib.h>
#include <unistd.h> 
#include <stdio.h>  
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
  
void mouse_handler(Display* display, Window window, GC gc) {
  
  XEvent event;
  
  while (true) {
    
    XNextEvent(display, &event);
    
    if(event.type == MotionNotify) {	
      
      Window root;

      int mouse_x,mouse_y,win_x,win_y;
      
      XQueryPointer(display, window, &root, &root, &mouse_x, &mouse_y, &win_x, &win_y, nullptr);
      
      std::cout << "Mouse position: (" << mouse_x << ", " << mouse_y << ")" << std::endl;
      
    }
    
  }
  
  
  return;
}
