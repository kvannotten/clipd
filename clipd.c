#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "utils.h"

Display *dis;
Window win;
int x11_fd;
fd_set in_fds;
ClipConfig config;

struct timeval tv;
XEvent ev;

void setup()
{
  dis = XOpenDisplay(NULL);
  int screen_num = DefaultScreen(dis);
  win = XCreateSimpleWindow(dis, RootWindow(dis, screen_num), 1, 1, 256, 256,
      0, BlackPixel (dis, screen_num), BlackPixel(dis, screen_num));

  // only listen to the events that interest us...
  XSelectInput(dis, win, PropertyChangeMask);

  x11_fd = ConnectionNumber(dis);
}

void mainLoop()
{
  // buffer var
  unsigned char *lastData = (unsigned char*)malloc(sizeof(char));

  while(1) {
    // Create a File Description Set containing x11_fd
    FD_ZERO(&in_fds);
    FD_SET(x11_fd, &in_fds);

    tv.tv_usec = 100000; // check every 100ms
    tv.tv_sec = 0;

    // wait for timer
    select(1, &in_fds, NULL, NULL, &tv);

    // get clipboard
    Atom a = XInternAtom(dis, "CLIPBOARD", False);
    XConvertSelection(dis, a, XA_STRING, XA_STRING, win, CurrentTime);

    // wait for clipboard event
    while(XPending(dis)) {
      XNextEvent(dis, &ev);
    }

    unsigned long nitems, bytes_left;
    Atom type;
    int res, format;
    unsigned char *data;

    if( ev.type == SelectionNotify ) {
      res = XGetWindowProperty(dis, win, ev.xselection.property, 0,
          XMaxRequestSize(dis), False,
          AnyPropertyType, &type, &format, &nitems,
          &bytes_left, &data);

      if (res == Success && strcmp(data, lastData) != 0) {
        lastData = (unsigned char*)malloc(sizeof(char) * strlen(data));
        strcpy(lastData, data);
        // store trimmed version
        trim(data);
        storeData(data, config.path);
      }
    }
  }
}

int main(int argc, char **argv)
{
  config = parseArgs(argc, argv);
  setup();
  mainLoop();
  return(0);
}
