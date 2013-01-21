
extern char controls_enumerate(int fd);

extern void controls_set_gain(int fd, int val);
extern void controls_set_exposure(int fd, int val);

extern int  controls_get_gain(int fd);

extern int  controls_get_max_gain(int fd);

extern void controls_set_jpegcompr(int fd, int v);

extern int  controls_get_jpegcompr(int fd);


