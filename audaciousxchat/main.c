#include <xchat/xchat-plugin.h>

#include <audacious/audctrl.h>
#include <audacious/dbus.h>

#define PNAME "Audacious XChat Plugin"
#define PDESC "Enables you to print out the currently playing track on audacious"
#define PVERSION "0.1"

static xchat_plugin *ph;   /* plugin handle */
static int enable = 1;

static DBusGProxy *proxy = NULL;

static int track_cb(char **word, char **word_eol, void *userdata)
{
	if(!audacious_remote_is_playing(proxy))
	{
		xchat_print(ph, "Audacious is not currently playing\n");
	}
	else
	{
		gchar **tuples = audacious_remote_get_tuple_fields(proxy);
		while(*tuples!=NULL)
		{
			xchat_print(ph, *tuples);
			xchat_print(ph, "\n");
			tuples++;
		}
	}
	return XCHAT_EAT_ALL;
}

void xchat_plugin_get_info(char **name, char **desc, char **version, void **reserved)
{
   *name = PNAME;
   *desc = PDESC;
   *version = PVERSION;
}

int xchat_plugin_init(xchat_plugin *plugin_handle,
                      char **plugin_name,
                      char **plugin_desc,
                      char **plugin_version,
                      char *arg)
{
	proxy = audacious_get_dbus_proxy();

   /* we need to save this for use with any xchat_* functions */
   ph = plugin_handle;

   /* tell xchat our info */
   *plugin_name = PNAME;
   *plugin_desc = PDESC;
   *plugin_version = PVERSION;

   xchat_hook_command(ph, "/track", XCHAT_PRI_NORM, track_cb, "Usage: /track", 0);

   xchat_print(ph, "Audacious XChat plugin loaded successfully!\n");

   return 1;       /* return 1 for success */
}
