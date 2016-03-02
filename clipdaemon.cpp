/*
  clipd: a very simple/lightweight clipboard manager for X11
  Copyright (C) 2016  Kristof Vannotten

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <unistd.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

class ClipboardEventSubscriber {
  Display *display;
  std::string displayName;
  Window myWindow;

public:
  ClipboardEventSubscriber(const std::string &display);
  std::string getData(const std::string &atom);
  std::string getOwner(const std::string &atom);

private:
  Atom getAtom(const std::string &atom);
};


Atom ClipboardEventSubscriber::getAtom(const std::string &which) {
  Atom a;
  if((a = XInternAtom(display, which.c_str(), True)) == None) {
    printf("Can't find atom %s", which);
    exit(4);
  }

  return a;
}

std::string ClipboardEventSubscriber::getOwner(const std::string &which) {
  Atom atom = getAtom(which);
  std::string atomName = XGetAtomName(display, atom);
  Window win;
  char *windowName;

  if ( None == (win = XGetSelectionOwner(display, atom)) ) {
    printf("Can't get selection owner");
    exit(5);
  }

  XFetchName(display, win, &windowName);
  std::string ret(windowName);
  XFree(windowName);
  return ret;
}

std::string ClipboardEventSubscriber::getData(const std::string &atom) {
  XConvertSelection(display,
                    getAtom(atom),
                    XA_STRING,
                    XA_STRING,
                    myWindow,
                    CurrentTime);

  XEvent report;
  unsigned char *buf = 0;
  Atom type;
  int format;
  unsigned long nitems, bytes;
  for (;;) {
    XNextEvent(display, &report);
    switch( report.type) {
      case SelectionNotify:
        if (report.xselection.property == None) {
          return "";
        }
        XGetWindowProperty(display,
                            myWindow,
                            report.xselection.property,
                            0,
                            (~0L),
                            False,
                            AnyPropertyType,
                            &type,
                            &format,
                            &nitems,
                            &bytes,
                            &buf);
        return (const char*)buf;
      default:
        // NOOP
        ;
    }
  }

}

ClipboardEventSubscriber::ClipboardEventSubscriber(const std::string &displ) : displayName(displ) {
  const char *display_cstr = NULL;
  if(!displayName.empty()) {
    display_cstr = displayName.c_str();
  }

  if (!(display = XOpenDisplay(display_cstr))) {
    printf("Error while opening display");
    exit(2);
  }

  int screen_num = DefaultScreen(display);
  myWindow = XCreateSimpleWindow(display,
                                  RootWindow(display, screen_num),
                                  100, 100,
                                  100, 100,
                                  10,
                                  BlackPixel(display, screen_num),
                                  WhitePixel(display, screen_num));

  if (!myWindow) {
    printf("Failed to create window");
    exit(3);
  }
}

void writeEntry(std::string data) {
  std::fstream myFile;
  myFile.open("/tmp/clipd.data", std::ios::out | std::ios::app);
  myFile << data.c_str() << std::endl;
  myFile.close();
}

void eventLoop(std::string &display) {
  ClipboardEventSubscriber ces(":0");

  std::string lastData;
  for(;;) {
    std::string data, owner;

    data = ces.getData("CLIPBOARD");

    if (data == lastData) {
      sleep(1);
      continue;
    }

    owner = ces.getOwner("CLIPBOARD");

    if (data != lastData) {
      writeEntry(data);
    }

    lastData = data;
  }
}

int main() {
    std::string display;

    eventLoop(display);
}
