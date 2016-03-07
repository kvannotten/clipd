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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
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

private:
  Atom getAtom(const std::string &atom);
};


Atom
ClipboardEventSubscriber::getAtom(const std::string &which)
{
  Atom a;
  if((a = XInternAtom(display, which.c_str(), True)) == None) {
    printf("Can't find atom %s", which.c_str());
    exit(EXIT_FAILURE);
  }

  return a;
}

std::string
ClipboardEventSubscriber::getData(const std::string &atom)
{
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
    exit(EXIT_FAILURE);
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
    exit(EXIT_FAILURE);
  }
}

void
writeEntry(std::string &data, std::string &path)
{
  std::fstream myFile;
  myFile.open(path.c_str(), std::ios::out | std::ios::app);
  myFile << data.c_str() << std::endl;
  myFile.close();
}

void
eventLoop(std::string &display, std::string &path)
{
  ClipboardEventSubscriber ces(display);

  std::string lastData;
  for(;;) {
    std::string data, owner;

    data = ces.getData("CLIPBOARD");

    if (data == lastData || data.empty()) {
      sleep(1);
      continue;
    }

    if (data != lastData) {
      writeEntry(data, path);
    }

    lastData = data;
  }
}

int
main(int argc, char** argv)
{
  // set defaults
  std::string display = ":0";
  std::string path    = "/tmp/clipd.data";
  int c;

  while ((c = getopt(argc, argv, "d:p:")) != -1) {
    switch(c) {
      case 'd':
        display = optarg;
        break;
      case 'p':
        path    = optarg;
        break;
      default:
        exit(EXIT_FAILURE);
    }
  }

  // daemonize/fork the app
  pid_t pid, sid;

  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  // exit parent process if forked successfully
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  umask(0);

  sid = setsid();
  if (sid < 0) {
    exit(EXIT_FAILURE);
  }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  // main loop
  eventLoop(display, path);

  exit(EXIT_SUCCESS);
}
