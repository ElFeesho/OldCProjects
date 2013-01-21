/* This code is (c) Ian Molton. It is GPL, however I *must be asked for permission to use it* should you desire to use it in your own project. */

#include <stdio.h>
#include <usb.h>
#include <sys/ioctl.h>

void send_ctrl(usb_dev_handle *handle, int rq, int rqt, int value, int index, unsigned char *str, int len){
	int p, q;
	q = usb_control_msg(handle, rq, rqt, value, index, str, len, 100);
        if(q != -1){
                printf("control: %02x\n", q);
                for(p = 0 ; p < q ; p++)
                        printf("%02x ", str[p]);fflush(0);
                printf("\n");
        }
}

usb_dev_handle *do_stuff(struct usb_device *dev){
	usb_dev_handle *handle;
	unsigned char str[64000];

	handle = usb_open(dev);
	printf("set_config: %d\n", usb_set_configuration(handle, 1));	
	printf("claim:      %d\n", usb_claim_interface(handle, 0));
	printf("set_alt:    %d\n", usb_set_altinterface(handle, 8));

	send_ctrl(handle, 0xc1, 0, 0, 0, str, 1);
	str[0] = 0x28; str[1] = 0x1e;
	send_ctrl(handle, 0x41, 0x08, 0x15, 0, str, 2);
	str[0] = 0x8e;
	send_ctrl(handle, 0x41, 0x08, 0x18, 0, str, 1);
	str[0] = 0x20;
	send_ctrl(handle, 0x41, 0x08, 0x17, 0, str, 1);
	str[0] = 0x32;
	send_ctrl(handle, 0x41, 0x08, 0x19, 0, str, 1);
	str[0] = 0x02; str[1] = 0x03; str[2] = 0x0f; str[3] = 0x0c;
	send_ctrl(handle, 0x41, 0x08, 0x1c, 0, str, 4);
	str[0] = 0x06;
	send_ctrl(handle, 0x41, 0x08, 0x01, 0, str, 1);
	str[0] = 0xb0; str[1] = 0x11; str[2] = 0x20; str[3] = 0x00;
	str[4] = 0x2a; str[5] = 0x00; str[6] = 0x00; str[7] = 0x10;
	send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0xb0; str[1] = 0x11; str[2] = 0x22; str[3] = 0x00;
        str[4] = 0x28; str[5] = 0x00; str[6] = 0x00; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0xd0; str[1] = 0x11; str[2] = 0x10; str[3] = 0x00;
        str[4] = 0x04; str[5] = 0x00; str[6] = 0x03; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0xd0; str[1] = 0x11; str[2] = 0x14; str[3] = 0x01;
        str[4] = 0xe0; str[5] = 0x02; str[6] = 0x80; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0xb0; str[1] = 0x11; str[2] = 0x20; str[3] = 0x00;
        str[4] = 0x2a; str[5] = 0x02; str[6] = 0x80; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0x06;
	send_ctrl(handle, 0x41, 0x08, 0x01, 0, str, 1);
	str[0] = 0x20;
	send_ctrl(handle, 0x41, 0x08, 0x17, 0, str, 1);
	str[0] = 0x90; str[1] = 0x11; str[2] = 0x00; str[3] = 0x00;
        str[4] = 0x00; str[5] = 0x00; str[6] = 0x00; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0x92; str[1] = 0x11; str[2] = 0x00; str[3] = 0x00;
        str[4] = 0x00; str[5] = 0x00; str[6] = 0x00; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	send_ctrl(handle, 0xc1, 0, 0x0a, 0, str, 5);
	str[0] = 0x16; str[1] = 0x12;
        send_ctrl(handle, 0x41, 0x08, 0x15, 0, str, 2);
	str[0] = 0x8e;
	send_ctrl(handle, 0x41, 0x08, 0x18, 0, str, 1);
	str[0] = 0x20;
        send_ctrl(handle, 0x41, 0x08, 0x17, 0, str, 1);
        str[0] = 0x22;
        send_ctrl(handle, 0x41, 0x08, 0x19, 0, str, 1);
	str[0] = 0x02; str[1] = 0x02; str[2] = 0x09; str[3] = 0x07;
        send_ctrl(handle, 0x41, 0x08, 0x1c, 0, str, 4);
	str[0] = 0x06;
	send_ctrl(handle, 0x41, 0x08, 0x01, 0, str, 1);
        str[0] = 0xb0; str[1] = 0x11; str[2] = 0x20; str[3] = 0x00;
        str[4] = 0x2d; str[5] = 0x00; str[6] = 0x00; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xb0; str[1] = 0x11; str[2] = 0x22; str[3] = 0x00;
        str[4] = 0x03; str[5] = 0x00; str[6] = 0x00; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xd0; str[1] = 0x11; str[2] = 0x10; str[3] = 0x00;
        str[4] = 0x64; str[5] = 0x00; str[6] = 0x91; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xd0; str[1] = 0x11; str[2] = 0x14; str[3] = 0x01;
        str[4] = 0x20; str[5] = 0x01; str[6] = 0x60; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xb0; str[1] = 0x11; str[2] = 0x20; str[3] = 0x00;
        str[4] = 0x2d; str[5] = 0x01; str[6] = 0x60; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0x06; str[1] = 0x03; str[2] = 0x00; str[3] = 0x00;
        str[4] = 0x00; str[5] = 0x00; str[6] = 0x00; str[7] = 0xb0;
	str[8] = 0x11; str[9] = 0x20; str[10] = 0x00; str[11] = 0x2d;
        str[12] = 0x01; str[13] = 0x60; str[14] = 0x10; str[15] = 0x00;
	str[16] = 0x00; str[17] = 0x01; str[18] = 0x01; str[19] = 0x00;
        str[20] = 0x16; str[21] = 0x12; str[22] = 0x20; str[23] = 0x8e;
        str[24] = 0x22; str[25] = 0x00; str[26] = 0x00; str[27] = 0x02;
        str[28] = 0x02; str[29] = 0x09; str[30] = 0x07;
	send_ctrl(handle, 0x41, 0x08, 0x01, 0, str, 31);
	
	str[0] = 0xb0; str[1] = 0x11; str[2] = 0x01; str[3] = 0x05;
        str[4] = 0x02; str[5] = 0x01; str[6] = 0x60; str[7] = 0x10;
	send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xd0; str[1] = 0x11; str[2] = 0x10; str[3] = 0x00;
        str[4] = 0x64; str[5] = 0x00; str[6] = 0x91; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xd0; str[1] = 0x11; str[2] = 0x14; str[3] = 0x01;
        str[4] = 0x20; str[5] = 0x01; str[6] = 0x60; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xd0; str[1] = 0x11; str[2] = 0x20; str[3] = 0x00;
        str[4] = 0x2d; str[5] = 0x00; str[6] = 0x03; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xc0; str[1] = 0x11; str[2] = 0x25; str[3] = 0x00;
        str[4] = 0x02; str[5] = 0x88; str[6] = 0x03; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xd0; str[1] = 0x11; str[2] = 0x30; str[3] = 0x20;
        str[4] = 0x19; str[5] = 0x19; str[6] = 0x19; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xa0; str[1] = 0x11; str[2] = 0x34; str[3] = 0x02;
        str[4] = 0x19; str[5] = 0x19; str[6] = 0x19; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xa0; str[1] = 0x11; str[2] = 0x5b; str[3] = 0x0a;
        str[4] = 0x19; str[5] = 0x19; str[6] = 0x19; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
        str[0] = 0xb0; str[1] = 0x11; str[2] = 0x20; str[3] = 0x00;
        str[4] = 0x2d; str[5] = 0x19; str[6] = 0x19; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);

	str[0] = 0x30;
        //send_ctrl(handle, 0x41, 0x08, 0x17, 0, str, 1);
	str[0] = 0x0e;
        //send_ctrl(handle, 0x41, 0x08, 0x18, 0, str, 1);

	usb_resetep(handle, 0x83);
	usb_resetep(handle, 0x81);
	usb_resetep(handle, 0x82);
	return handle;
}

struct usb_dev_handle *get_dev(void){
        struct usb_bus *busses;
        struct usb_bus *bus;

        usb_init();
        usb_find_busses();
        usb_find_devices();

        busses = usb_get_busses();

        for (bus = busses; bus; bus = bus->next) {
                struct usb_device *dev;

                for (dev = bus->devices; dev; dev = dev->next) {
                        if (dev->descriptor.idVendor == 0x0c45 && dev->descriptor.idProduct == 0x602e) {
//                              printf("%d\n", dev->config[0].bConfigurationValue);
                                return do_stuff(dev);
                        }
                }
        }

        return NULL;
}

