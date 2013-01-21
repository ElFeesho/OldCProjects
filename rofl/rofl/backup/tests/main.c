#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
static int attributeList[] = { GLX_RGBA, None };

static Bool WaitForNotify(Display *d, XEvent *e, char *arg)
    { return(e->type == MapNotify) && (e->xmap.window == (Window)arg); }

int main(int argc, char**argv)
{
    Display *dpy;
    XVisualInfo *vi;
    Colormap cmap;
    XSetWindowAttributes swa;
    Window win;
    GLXContext cx;
    XEvent event;

/* get a connection   */
    dpy   = XOpenDisplay(0);
    if (!dpy) {
        fprintf(stderr, "Cannot open display.\n");
        exit(-1);
    }

/* get an appropriate visual */
    vi = glXChooseVisual(dpy, DefaultScreen(dpy),
         attributeList);
    if (!vi) {
        fprintf(stderr, "Cannot find visual with desired attributes.\n");
        exit(-1);
    }

/* create a GLX context */
    cx = glXCreateContext(dpy, vi, 0, GL_FALSE);
    if (!cx) {
        fprintf(stderr, "Cannot create context.\n");
        exit(-1);
    }

/* create a colormap -- AllocAll for color index mode */
    cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),
           vi->visual, AllocNone);
    if (!cmap) {
        fprintf(stderr, "Cannot allocate colormap.\n");
        exit(-1);
    }

    /* create a   window */
    swa.colormap = cmap;
    swa.border_pixel = 0;
    /* connect the context to the window */
    glXMakeCurrent(dpy, win, cx);

/* clear the buffer */
    glClearColor(1,1,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

/*wait for a while */
    sleep(10);
/* exit cleanly */
    XCloseDisplay(dpy);
 	return 0; 
}
