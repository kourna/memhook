#ifndef GUI_H
#define GUI_H

#include <X11/Xlib.h>
#include <unistd.h> 
#include <stdio.h>  
#include <iostream>
#include <cmath>

void draw_test_line(Display* display, Window window, GC gc);

int init_gui();

#endif
