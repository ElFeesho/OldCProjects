#ifndef __MANIPULATE_H__
#define __MANIPULATE_H__

extern void manip_decompress_init();
extern int manip_decompress(int width, int height, unsigned char *inp, unsigned char *outp);
extern void manip_check_diffs(unsigned char *image, int *xp, int *yp, int *xp2, int *yp2);
#endif
