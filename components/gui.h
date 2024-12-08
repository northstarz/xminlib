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

class x_gui {

public:

  Display* display;
  Window window;
  GC  gc;

  bool shutdown = false;

  XFontStruct* font;
    
  void window_runtime_helper(XFontStruct* font, layout active_layout) {
    
    std::cout << __func__ << " loading fonts..." << std::endl;
    
    XEvent event;
    int cur_line = 10;
    
    XSetFont(display, gc, font->fid);
    
    while(!shutdown) {
      
      XNextEvent(display, &event);

      if (event.type == Expose) {
       
	active_layout.drawAll(display);
	
      }

      if (event.type == ConfigureNotify) {

	std::cout << "Window resized or moved." << std::endl;

	//	active_layout.drawAll();

	XFlush(display);
	
      }

      if(event.type == MotionNotify) {	
	
	Window root;
	Window child;
	
	unsigned int psychiose;

	int mouse_x,mouse_y,win_x,win_y;
	
	std::cout << "calling xqery" << std::endl;
	XQueryPointer(display, window, &root, &child, &win_x, &win_y, &mouse_x, &mouse_y, &psychiose );
	
      }

      if (event.type == ButtonPress && event.xbutton.button == 1) { 

	printf("Left mouse button pressed at (%d, %d)\n", event.xbutton.x, event.xbutton.y);

      } else if (event.type == ButtonRelease && event.xbutton.button == 1) {

	printf("Left mouse button released at (%d, %d)\n", event.xbutton.x, event.xbutton.y);

      }
                
      if (event.type == KeyPress) {
	
	char buffer[1];

	KeySym keysym;

	printf("before loopup");
	
	XLookupString(&event.xkey, buffer, sizeof(buffer), &keysym, NULL);
	printf("Key pressed: %c\n", buffer[0]);

	printf("after loopup");
	
	if(buffer[0] == 'x')
	  break;
	
	if(buffer[0] == 'w') {
	  
	  //  active_layout->add_element(BUTTON,10,cur_line,100,10, "Crazy Button");

	  //  cur_line=cur_line+active_layout->layout->size_y[1];
	  

	}
	
	if(buffer[0] == 's') {
	  
	  // clip_cursor_position(mouse_x,mouse_y);


	}

	// draw_window(active_layout->get_window_layout());
	
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

    layout active_layout;
    
    active_layout.addElement(std::make_unique<element_button>(display,window,gc,10,10,10,10,"button",1));
    active_layout.addElement(std::make_unique<element_label>("label"));
    active_layout.addElement(std::make_unique<element_text_box>(display,window,gc,10,10,10,10,"username"));

    std::cout << "trying to render shit..." << std::endl;
    
    active_layout.drawAll(display);
    
    //================================= WINDOW HANDLER =================================
    
    std::cout << "Launching window runtime helper..." << std::endl;

    window_runtime_helper(font,active_layout);

    //Cleanup shoutout to feroxtard
    XUnloadFont(display, font->fid);
    XUnmapWindow(display, window);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;

  }

};

