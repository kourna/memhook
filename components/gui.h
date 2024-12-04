#pragma once

#include <thread>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
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
  int mouse_x,mouse_y,win_x,win_y;
  
  window_layout *active_layout;
  
  void draw_window(window_layout_struct* layout) {

    for(unsigned int i = 0; i < layout->id.size(); ++i) {

      switch(layout->type[i]) {

      case BUTTON:
	draw_dynamic_box_with_text(display, window, gc, layout->id[i], font , active_layout->layout);
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

  void clip_cursor_position(unsigned int mouse_x, unsigned int mouse_y) {

    window_layout_struct* active_struct = active_layout->layout;
    
    for(int i = 0; i < active_struct->id.size(); ++i) {

      if(
	 mouse_x > active_struct->anchor_x[i] &&
	 mouse_x < active_struct->size_x[i]+active_struct->anchor_x[i] &&
	 mouse_y > active_struct->anchor_y[i] &&
	 mouse_y < active_struct->size_y[i]+active_struct->anchor_y[i]
	 )
	{
	  std::cout << "clicked on button with id: " << active_struct->id[i] << std::endl;
	}      
      
    }

    return;

  }
  
  void window_runtime_helper(XFontStruct* font) {
    
    std::cout << __func__ << " loading fonts..." << std::endl;
    
    XEvent event;
    int cur_line;
    
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
	
	unsigned int psychiose;
	
	XQueryPointer(display, window, &root, &child, &win_x, &win_y, &mouse_x, &mouse_y, &psychiose );
	
      }
      
      if (event.type == KeyPress) {
	
	char buffer[1];
	KeySym keysym;
	XLookupString(&event.xkey, buffer, sizeof(buffer), &keysym, NULL);
	printf("Key pressed: %c\n", buffer[0]);

	if(buffer[0] == 'x')
	  break;
	
	if(buffer[0] == 'w') {
	  
	  active_layout->add_element(BUTTON,10,cur_line,100,10, "Crazy Button");
	  cur_line=cur_line+10;
	  
	}
	
	if(buffer[0] == 's') {
	  
	  clip_cursor_position(mouse_x,mouse_y);
	  
	}

	draw_window(active_layout->get_window_layout());
	
      }
      
    }
    
    return;

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

    window_runtime_helper(font);

    //Cleanup shoutout to feroxtard
    XUnloadFont(display, font->fid);
    XUnmapWindow(display, window);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;

  }

};

