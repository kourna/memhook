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
  
};

class wruff_gui {

public:

  Display* display;
  Window window;
  GC  gc;
  bool shutdown = false;
  XFontStruct* font;
  
  //Not sure if this is good impl but it works xddd
  void draw_window() {

    draw_test_line(display,window,gc);
    
    XDrawString(display, window, gc, 10, 50, "This gui gonna SUCK X3 !", 12);
    
    XFlush(display);

    return;
  }
  
  void window_runtime_helper(Display* display, Window window, GC gc, XFontStruct* font) {

    XEvent event;
    
    XSetFont(display, gc, font->fid);
    
    //main window rendering loop :-)
    while(!shutdown) {
      
      XNextEvent(display, &event);

      if (event.type == Expose) {
       
	draw_window();
	
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

    XFlush(display);

    draw_test_line(display,window,gc);

    XFlush(display);

    // Handle window resizes dynamically in a seperate thread to avoid blocking.
    std::thread t(&wruff_gui::window_runtime_helper, this, display, window, gc, font);
    
    sleep(10);

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


class window_layout {

public:

  window_layout_struct* layout;

  unsigned int element_count;
  
  bool add_element(valid_element_types type, unsigned int anchor_x, unsigned int anchor_y, unsigned int size_x, unsigned int size_y, unsigned int id) {

    layout->type.push_back(type);
    layout->anchor_x.push_back(anchor_x);
    layout->anchor_y.push_back(anchor_y);
    layout->size_x.push_back(size_x);
    layout->size_y.push_back(size_y);
    layout->id.push_back(element_count);

    element_count++;
    
    return true;
    
  }

  bool validate_window_layout_struct() {

    if((layout->type.size() + layout->anchor_x.size() + layout->anchor_y.size() + layout->size_x.size() + layout->size_y.size() + layout->id.size()) / 6 == layout->id.size() ) {
      return true; } else { return false;}

  }
  
};
