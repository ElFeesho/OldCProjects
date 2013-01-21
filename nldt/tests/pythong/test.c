#include <gtk/gtk.h>
#include <Python.h>

static PyObject *pClassInst = NULL;


gboolean canvas_on_click(GtkWidget *canvas, GdkEventButton *ev, gpointer user_data)
{
	PyObject *on_mouse = PyObject_GetAttrString(pClassInst,"on_click");
	PyObject *pArgs;
	PyObject *pValue;
	pArgs = PyTuple_New(2);
	pValue = PyFloat_FromDouble(ev->x);
	PyTuple_SetItem(pArgs, 0, pValue);
	pValue = PyFloat_FromDouble(ev->y);
	PyTuple_SetItem(pArgs, 1, pValue);
	PyObject_CallObject(on_mouse, pArgs);
	return TRUE;
}


gboolean interp(GtkWidget *canvas)
{
	static GdkPixmap *bg_buf = NULL;
	if(bg_buf == NULL)
	{
		bg_buf = gdk_pixmap_new(NULL,500,400,24);
	}
	static PyObject *pName = NULL;
	static PyObject *pModule = NULL;
	static PyObject *pDict = NULL;
	static PyObject *pClassObj = NULL;
	static PyObject *pArgs = NULL;
	static PyObject *pValue = NULL;

	if(pName == NULL && pModule == NULL)
	{
		pName = PyString_FromString("testobj");
		if(pName == NULL)
			printf("Failed loading module name\n");
		pModule = PyImport_Import(pName);
		if(pModule == NULL)
			printf("Failed loading module\n");
		Py_DECREF(pName);
	}
	if(pDict == NULL)
	{
		pDict = PyModule_GetDict(pModule);
		pClassObj = PyDict_GetItemString(pDict, "TestObj");
		if(pClassObj == NULL)
		{	
			printf("Failed loading class\n");
		}
	}

	if (pClassObj && PyClass_Check(pClassObj)) 
	{
		if(pClassInst == NULL)
		{
			pArgs = PyTuple_New(4);
			int num = 10;
			pValue = PyInt_FromLong(num);
			PyTuple_SetItem(pArgs, 0, pValue);

			num = 300;
			pValue = PyInt_FromLong(num);
			PyTuple_SetItem(pArgs, 1, pValue);
			
			num = 30;
			pValue = PyInt_FromLong(num);
			PyTuple_SetItem(pArgs, 2, pValue);
			PyTuple_SetItem(pArgs, 3, pValue);

			pClassInst = PyInstance_New(pClassObj,pArgs,NULL);
			Py_DECREF(pArgs);
			Py_DECREF(pValue);

			if(pClassInst == NULL)
			{
				printf("Failed to create instance\n");
				return FALSE;
			}
		}
	}
	if(pClassInst!=NULL)
	{
		PyObject *on_update = PyObject_GetAttrString( pClassInst, "on_update" );
		if(on_update!=NULL)
		{
			PyObject_CallObject(on_update, NULL);
			PyObject *xpos = PyObject_GetAttrString( pClassInst, "x");
			PyObject *ypos = PyObject_GetAttrString( pClassInst, "y");
			PyObject *width = PyObject_GetAttrString( pClassInst, "width");
			PyObject *height = PyObject_GetAttrString( pClassInst, "height");
			
			double x = PyFloat_AsDouble(xpos);
			double y = PyFloat_AsDouble(ypos);
			double w = PyFloat_AsDouble(width);
			double h = PyFloat_AsDouble(height);
			GdkGC *gc = gdk_gc_new(bg_buf);
			gdk_draw_rectangle(bg_buf,gc,1,0,0,500,400);

			GdkColor red;
			gdk_color_parse("#ff0000",&red);
			gdk_gc_set_rgb_fg_color(gc,&red);
			
			gdk_draw_rectangle(bg_buf,gc,1,x,y,w,h);
			gdk_draw_drawable(canvas->window,gdk_gc_new(canvas->window),bg_buf,0,0,0,0,500,400);
		}
		else
			printf("failed to get update func\n");
	}	
	return TRUE;
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);

	GtkWidget *window, *canvas;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	canvas = gtk_drawing_area_new();
	gtk_widget_add_events(canvas,GDK_BUTTON_PRESS_MASK);
	gtk_widget_set_size_request(canvas,500,400);

	gtk_container_add(GTK_CONTAINER(window),canvas);

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),NULL);
	g_signal_connect(G_OBJECT(canvas),"button-press-event",G_CALLBACK(canvas_on_click),NULL);
	gtk_widget_show_all(window);
	//Initialise Python Interp... right?
	Py_Initialize();

	g_timeout_add(20,(GSourceFunc)interp,canvas);

	gtk_main();
	Py_Finalize();
	return 0;
}
