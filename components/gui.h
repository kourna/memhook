#pragma once

#include <thread>
#include <X11/Xlib.h>
#include <unistd.h> 
#include <stdio.h>  
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>

#include "layout.h"
#include "drawutils.h"

class wruff_gui {

public:

  Display* display;
  Window window;
  GC  gc;

  bool shutdown = false;

  XFontStruct* font;

  window_layout *active_layout;
  
  //obsolete
  void draw_window_old() {

    draw_test_line(display,window,gc);
        
    XFlush(display);

    return;
  }
  
  //new
  void draw_window(window_layout_struct* layout) {

    for(unsigned int i = 0; i < layout->id.size(); ++i) {

      switch(layout->type[i]) {

      case BUTTON:
	draw_dynamic_box_with_text(display, window, gc, layout->anchor_x[i], layout->anchor_y[i], layout->size_x[i], layout->size_y[i], layout->data[i], font);
	break;
      case TEXT_BOX:
	draw_box(display, window, gc, layout->anchor_x[i], layout->anchor_y[i], layout->size_x[i], layout->size_y[i]);
	break;
      case BORDER:
	draw_dynamic_window_border(display,window,gc,layout->anchor_x[i]);
	break;
      case TABLE:
	//	array_to_table(display,window,); how tf am i gonna do this lol
	break;
      }

      XFlush(display);

    }

  }
  
  void window_runtime_helper(Display* input_display, Window input_window, GC input_gc, XFontStruct* font) {

    Display* func_display = input_display;
    Window func_window = input_window;
    GC func_gc = input_gc;
    
    std::cout << __func__ << " loading fonts..." << std::endl;
    
    XEvent event;
    
    XSetFont(display, gc, font->fid);
    
    while(!shutdown) {
      
      XNextEvent(display, &event);

      if (event.type == Expose) {
       
	draw_window(active_layout->get_window_layout());
	
      }

      if (event.type == ConfigureNotify) {

	std::cout << "Window resized or moved." << std::endl;
	
      }

      if(event.type == MotionNotify) {	
	
	Window root;
	Window child;
	
	int mouse_x,mouse_y,win_x,win_y;

	std::cout << "help me please" << std::endl;
	
	if(XQueryPointer(func_display, func_window, &root, &child, &mouse_x, &mouse_y, &win_x, &win_y, nullptr))
	  std::cout << "cry";
	std::cout << "Mouse position: (" << mouse_x << ", " << mouse_y << ")" << std::endl;
	
      }
      
    }
    
    return;

  } 
  
  void draw_test_line(Display* display, Window window, GC gc) {

    XDrawLine(display, window, gc, 10, 10, 100, 10);

  }

  int init_gui(void) {

    std::cout << "Trying to allocate GUI." << std::endl;
      
    display = XOpenDisplay(NULL);

    if (!display)
      {
        fprintf(stderr, "error:"
                "       Can't open connection to display server."
                " Probably X server is not started.\n");
        return 1;
      }

    int blackColor = BlackPixel(display, DefaultScreen(display));
    int whiteColor = WhitePixel(display, DefaultScreen(display));

    window = XCreateSimpleWindow(
                                 display,
                                 DefaultRootWindow(display),// parent window
                                 0,//x
                                 0,//y
                                 720,//width
                                 480,//height
                                 2, //border width
                                 whiteColor,//border color
                                 whiteColor); // background color

    XSelectInput(display, window, ExposureMask | PointerMotionMask | KeyPressMask | StructureNotifyMask);
    // Font loader
    font = XLoadQueryFont(display, "fixed");
    if (!font) {
      std::cerr << "Unable to load font" << std::endl;
      return 1;
    }
    
    XMapWindow(display, window);

    gc = XCreateGC(display, window, 0, NULL); // graphic context for render?

    XSetForeground(display, gc, blackColor);

    XStoreName(display, window, "wruff-tools");


    for(;;) {
      XEvent e;
      XNextEvent(display, &e);
      if (e.type == MapNotify)
        break;
    }

    std::cout << "Window allocation successful!" << std::endl;
    
    //================================= MAIN LAYOUT CONFIG SPACE ================================= 

    std::cout << "Loading Layouts..." << std::endl;

    
    active_layout = new window_layout();

    active_layout->add_element(BUTTON,10,10,100,10, "Crazy Button");
    active_layout->add_element(BUTTON, 10, 50, 50, 10, "Another Button");
    active_layout->add_element(BORDER,5,0,0,0,"border");
    
    //================================= WINDOW HANDLER =================================
    
    std::cout << "Launching window runtime helper..." << std::endl;

    std::thread win_runtime(&wruff_gui::window_runtime_helper, this, display, window, gc, font);


    

    
    sleep(60);

    shutdown = true;
    
    win_runtime.join();

    //Cleanup shoutout to feroxtard
    XUnloadFont(display, font->fid);
    XUnmapWindow(display, window);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;

  }

};

