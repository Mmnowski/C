#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSZ     27

char* calibrateColors(int red, int green, int blue){
  //simple dec > hex conversion
  static char red_hex[10], green_hex[10], blue_hex[10], rgb_hex[12];
  char temp[10];
  int red_value = red, green_value = green, blue_value = blue;
  int length;
  sprintf(&red_hex, "%x", red_value);
  strcpy(temp, red_hex);
  length = strlen(temp);
  printf("%d\n", length);
  if (length == 1){
    strcpy(red_hex, "0");
    strcat(red_hex, temp);
  }
  sprintf(&green_hex, "%x", green_value);
  strcpy(temp, green_hex);
  length = strlen(temp);
  if (length == 1){
    strcpy(green_hex, "0");
    strcat(green_hex, temp);
  }
  sprintf(&blue_hex, "%x", blue_value);
  strcpy(temp, blue_hex);
  length = strlen(temp);
  if (length == 1){
    strcpy(blue_hex, "0");
    strcat(blue_hex, temp);
  }
  printf("%s %s %s\n", red_hex, green_hex, blue_hex);
  strcpy(rgb_hex, "rgb:");
  strcat(rgb_hex, red_hex);
  strcat(rgb_hex, "/");
  strcat(rgb_hex, green_hex);
  strcat(rgb_hex, "/");
  strcat(rgb_hex, blue_hex);
  printf("%s\n", rgb_hex);
  return rgb_hex;
};

int main(int argc, char *argv[])
{
  //shared memory allocation
  int shmid;
  key_t key;
  char *shm, *s;

  key = 5678;

  if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
    perror("shmget");
    return 1;
  }

  if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
    perror("shmat");
    return 1;
  }

  s = shm;
  //---------------------------------------------
  //xlib configuration
  Display *dis;
  int screen, buttonType;
  Window win;
  Window rgbWindow;
  Window redWindow, greenWindow, blueWindow;
  Window redSubWindow, greenSubWindow, blueSubWindow;
  XEvent event;

  int x, y, redWidth, blueWidth, greenWidth, height;
  Window button;
  Colormap colormap;
  XColor color;
  XColor red_color, green_color, blue_color, rgb_color;
  XGCValues gcv_red, gcv_green, gcv_blue;
  GC gc_red, gc_green, gc_blue;

  int red = 0;
  int green = 0;
  int blue = 0;

  height = 30;

  void getServerRGB(){
    char getRed[10], getGreen[10], getBlue[10];
    int redcounter = 0, greencounter = 0, bluecounter = 0;

    for(redcounter = 4; redcounter <= 6; redcounter++) getRed[redcounter-4] = s[redcounter];
    for(greencounter = 8; greencounter <= 11; greencounter++) getGreen[greencounter-8] = s[greencounter];
    for(bluecounter = 12; bluecounter <= 14; bluecounter++) getBlue[bluecounter-12] = s[bluecounter];

    printf("%s\n", getRed);
    red = atoi(getRed);
    green = atoi(getGreen);
    blue = atoi(getBlue);

    printf("%d XDD %d XDD %d\n", red, green, blue);
}
  char dispType[100];

  if (argc < 1){
  	dis = XOpenDisplay(NULL);
  } else {
	  dis = XOpenDisplay(argv[1]);
}


  if(dis == NULL)
  {
    fprintf(stderr, "Cannot open display");
    exit(1);
  }

  screen = DefaultScreen(dis);

  // main window
  win = XCreateSimpleWindow(dis, RootWindow(dis, screen),
    200, 200, 700, 450,
    1, BlackPixel(dis, screen), WhitePixel(dis, screen));
  XSelectInput(dis, win, ExposureMask | KeyPressMask);
  XMapWindow(dis, win);

  colormap = DefaultColormap(dis, screen);

  redWidth = red;
  if (redWidth < 1){
    redWidth = 1;
  }
  if (redWidth > 255){
    redWidth = 255;
  }
  greenWidth = green;
  if (greenWidth < 1){
    greenWidth = 1;
  }
  if (greenWidth > 255){
    greenWidth = 255;
  }
  blueWidth = blue;
  if (blueWidth < 1){
    blueWidth = 1;
  }
  if (blueWidth > 255){
    blueWidth = 255;
  }

  XParseColor(dis, colormap, calibrateColors(red,green,blue), &rgb_color);
  XAllocColor(dis, colormap, &rgb_color);

  // rgb color window
  rgbWindow = XCreateSimpleWindow(dis, win,
    10, 10, 250, 430,
    1, BlackPixel(dis, screen), rgb_color.pixel);
  XMapWindow(dis, rgbWindow);

  XParseColor(dis, colormap, "rgb:ff/0/0", &red_color);
  XAllocColor(dis, colormap, &red_color);

  XParseColor(dis, colormap, "rgb:0/ff/0", &green_color);
  XAllocColor(dis, colormap, &green_color);

  XParseColor(dis, colormap, "rgb:0/0/ff", &blue_color);
  XAllocColor(dis, colormap, &blue_color);

// red slider window
  redWindow = XCreateSimpleWindow(dis, win,
    395, 90, 255, height,
    1, BlackPixel(dis, screen), WhitePixel(dis, screen));
  XSelectInput(dis, redWindow, ButtonPressMask);
  XMapWindow(dis, redWindow);
  XGrabPointer(dis, redWindow, False, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

  redSubWindow = XCreateSimpleWindow(dis, win,
    396, 91, redWidth, height,
    0, BlackPixel(dis, screen), red_color.pixel);
  XSelectInput(dis, redSubWindow, ButtonPressMask);
  XMapWindow(dis, redSubWindow);
  XGrabPointer(dis, redSubWindow, False, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

// green slider window
  greenWindow = XCreateSimpleWindow(dis, win,
    395, 220, 255, height,
    1, BlackPixel(dis, screen), WhitePixel(dis, screen));
  XSelectInput(dis, greenWindow, ButtonPressMask);
  XMapWindow(dis, greenWindow);
  XGrabPointer(dis, greenWindow, False, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);


  greenSubWindow = XCreateSimpleWindow(dis, win,
      396, 221, greenWidth, height,
      0, BlackPixel(dis, screen), green_color.pixel);
  XSelectInput(dis, greenSubWindow, ButtonPressMask);
  XMapWindow(dis, greenSubWindow);
  XGrabPointer(dis, greenSubWindow, False, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);


// blue slider window
  blueWindow = XCreateSimpleWindow(dis, win,
    395, 340, 255, height,
    1, BlackPixel(dis, screen), WhitePixel(dis, screen));
  XSelectInput(dis, blueWindow, ButtonPressMask);
  XMapWindow(dis, blueWindow);
  XGrabPointer(dis, blueWindow, False, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);


  blueSubWindow = XCreateSimpleWindow(dis, win,
    396, 341, blueWidth, height,
    0, BlackPixel(dis, screen), blue_color.pixel);
  XSelectInput(dis, blueSubWindow, ButtonPressMask);
  XMapWindow(dis, blueSubWindow);
  XGrabPointer(dis, blueSubWindow, False, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
  //---------------------------------------------------
  //change color of RGB window
  void setRGBColor(){
    if(red > 255){
      red = 255;
    }

    if(green > 255){
      green = 255;
    }

    if(blue > 255){
      blue = 255;
    }

    // XDestroyWindow(dis, rgbWindow); //destroying window is a patchfix, cant get XSetWindowBackground to work
    XParseColor(dis, colormap, calibrateColors(red,green,blue), &rgb_color);
    XAllocColor(dis, colormap, &rgb_color);
    rgbWindow = XCreateSimpleWindow(dis, win,
      10, 10, 250, 430,
      1, BlackPixel(dis, screen), rgb_color.pixel);
    XMapWindow(dis, rgbWindow);
    // XSetWindowBackground(dis, rgbWindow, rgb_color.pixel);
  }
  //-----------------------------------------------------


  void setServerRGB(){
    sprintf(s, "RGB(%03d %03d %03d)", red, green, blue);
  }

  while (1) {
    getServerRGB();
    setRGBColor();
    if(XPending(dis)){
    XNextEvent(dis, &event);
    if(event.xany.window == redWindow){
      if(event.type == ButtonPress){
        if(event.xbutton.button = Button1){
          x = event.xbutton.x;
          redWidth = x+1;
          red=redWidth;
          // setRGBColor();
          setServerRGB();
          if (redWidth < 1){
            redWidth = 1;
          }
          if (redWidth > 255){
            redWidth = 255;
          }
          buttonType = Button1;
          printf("click at %d \n", x);
          XResizeWindow(dis, redSubWindow, redWidth, height);
        }
      }
    }
    if(event.xany.window == redSubWindow){
      if(event.type == ButtonPress){
        if(event.xbutton.button = Button1){
          x = event.xbutton.x;
          redWidth = x+1;
          red=redWidth;
          // setRGBColor();
          setServerRGB();
          if (redWidth < 1){
            redWidth = 1;
          }
          if (redWidth > 255){
            redWidth = 255;
          }
          buttonType = Button1;
          printf("click at %d \n", x);
          XResizeWindow(dis, redSubWindow, redWidth, height);
        }
      }
    }
    if(event.xany.window == greenWindow){
      if(event.type == ButtonPress){
        if(event.xbutton.button = Button1){
          x = event.xbutton.x;
          greenWidth = x+1;
          green=greenWidth;
          // setRGBColor();
          setServerRGB();
          if (greenWidth < 1){
            greenWidth = 1;
          }
          if (greenWidth > 255){
            greenWidth = 255;
          }
          buttonType = Button1;
          printf("click at %d \n", x);
          XResizeWindow(dis, greenSubWindow, greenWidth, height);

        }
      }
    }
    if(event.xany.window == greenSubWindow){
      if(event.type == ButtonPress){
        if(event.xbutton.button = Button1){
          x = event.xbutton.x;
          greenWidth = x+1;
          green = greenWidth;
          // setRGBColor();
          setServerRGB();
          if (greenWidth < 1){
            greenWidth = 1;
          }
          if (greenWidth > 255){
            greenWidth = 255;
          }
          buttonType = Button1;
          printf("click at %d \n", x);
          XResizeWindow(dis, greenSubWindow, greenWidth, height);

        }
      }
    }
    if(event.xany.window == blueWindow){
      if(event.type == ButtonPress){
        if(event.xbutton.button = Button1){
          x = event.xbutton.x;
          blueWidth = x+1;
          blue = blueWidth;
          // setRGBColor();
          setServerRGB();
          if (blueWidth < 1){
            blueWidth = 1;
          }
          if (blueWidth > 255){
            blueWidth = 255;
          }
          buttonType = Button1;
          printf("click at %d \n", x);
          XResizeWindow(dis, blueSubWindow, blueWidth, height);

        }
      }
    }
    if(event.xany.window == blueSubWindow){
      if(event.type == ButtonPress){
        if(event.xbutton.button = Button1){
          x = event.xbutton.x;
          blueWidth = x+1;
          blue = blueWidth;
          // setRGBColor();
          setServerRGB();
          if (blueWidth < 1){
            blueWidth = 1;
          }
          if (blueWidth > 255){
            blueWidth = 255;
          }
          buttonType = Button1;
          printf("click at %d \n", x);
          XResizeWindow(dis, blueSubWindow, blueWidth, height);

        }
      }
    }
	}
  }

  return 0;
}
