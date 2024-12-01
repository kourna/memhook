#include <X11/Xlib.h>
#include <unistd.h> 
#include <stdio.h>  
#include <iostream>
#include <cmath>

struct availableColors {

  XColor customBlue;
  XColor customRed;

};

struct eXPoints {

  XPoint* points;
  int nOfPoints;

};

void draw_test_line(Display* display, Window window, GC gc) {

  XDrawLine(display, window, gc, 10, 10, 100, 10);
   
}

int init_gui(void) {

  Display* display = XOpenDisplay(NULL);

  if (!display)
  {
    fprintf(stderr, "error:"
      "	Can't open connection to display server."
      " Probably X server is not started.\n");
    return 1;
  }    

  int blackColor = BlackPixel(display, DefaultScreen(display));
  int whiteColor = WhitePixel(display, DefaultScreen(display));
  
  Window window = XCreateSimpleWindow(
				      display,
				      DefaultRootWindow(display),// parent window
				      0,//x
				      0,//y
				      720,//width
				      480,//height
				      2, //border width         
				      whiteColor,//border color
				      whiteColor); // background color

  XSelectInput(display, window, StructureNotifyMask);

  XMapWindow(display, window);                    
  
  GC gc = XCreateGC(display, window, 0, NULL); // graphic context for render?

  XSetForeground(display, gc, blackColor);

  XStoreName(display, window, "wruff-tools");
  
  for(;;) {
    XEvent e;
    XNextEvent(display, &e);
    if (e.type == MapNotify)
      break;
  }
  
  XFlush(display);

  sleep(50);

  XUnmapWindow(display, window);// close window, connection and free resources shoutout to ferox
  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return 0;
  
}
