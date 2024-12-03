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

      }

      XFlush(display);

    }

  }
  
  void window_runtime_helper(Display* display, Window window, GC gc, XFontStruct* font) {

    std::cout << __func__ << " loading fonts..." << std::endl;
    
    XEvent event;
    
    XSetFont(display, gc, font->fid);
    
    //main window rendering loop :-)
    while(!shutdown) {
      
      XNextEvent(display, &event);

      if (event.type == Expose) {
       
	draw_window(active_layout->get_window_layout());
	
      }

      if (event.type == ConfigureNotify) {

	std::cout << "Window resized or moved." << std::endl;
	
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

    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
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

    std::thread t(&wruff_gui::window_runtime_helper, this, display, window, gc, font);
    
    sleep(60);

    shutdown = true;
    
    t.join();

    //Cleanup shoutout to feroxtard
    XUnloadFont(display, font->fid);
    XUnmapWindow(display, window);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;

  }

};


class input_handler {

public: 

  unsigned int mouseX, mouseY;
  
  void mouse_handler() {

    //XQueryPointer(display, rootWindow, &rootReturn, &childReturn, &mouseX, &mouseY, nullptr, nullptr, nullptr);

    return;
  }
  
};


