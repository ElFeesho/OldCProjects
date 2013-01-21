/* This code is (c) Ian Molton. It is GPL, however I *must be asked for permission to use it* should you desire to use it in your own project. */

#include <stdio.h>
#include <usb.h>
#include <sys/ioctl.h>

struct usb_dev_handle {
  int fd;

  struct usb_bus *bus;
  struct usb_device *device;

  int config;
  int interface;
  int altsetting;

  /* Added by RMT so implementations can store other per-open-device data */
  void *impl_info;
};


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

struct usbdevfs_iso_packet_desc {
        unsigned int length;
        unsigned int actual_length;
        unsigned int status;
};

struct usbdevfs_urb {
        unsigned char type;
        unsigned char endpoint;
        int status;
        unsigned int flags;
        void *buffer;
        int buffer_length;
        int actual_length;
        int start_frame;
        int number_of_packets;
        int error_count;
        unsigned int signr;  /* signal to be sent on error, -1 if none should be sent */
        void *usercontext;
//        struct usbdevfs_iso_packet_desc iso_frame_desc[0];
};

struct usbthing {
	struct usbdevfs_urb urb;
	struct usbdevfs_iso_packet_desc iso[32];
};

#define USBDEVFS_URB_TYPE_ISO              0
#define USBDEVFS_SUBMITURB         _IOR('U', 10, struct usbdevfs_urb)

int submit_iso(usb_dev_handle *handle, int ep){
	struct usbthing thing;
	struct usbdevfs_urb *urb = &thing.urb;
	struct usbdevfs_iso_packet_desc *iso = &thing.iso;
	char buf[640000];
	int ret;
	int i;

	urb->type = USBDEVFS_URB_TYPE_ISO;
	urb->endpoint = ep;
	urb->flags = 0x0; //0x82??
	urb->buffer = buf;
	urb->buffer_length = 640000;
	urb->start_frame = 0;
	urb->number_of_packets = 32;
	urb->signr = 0; // -1
	//urb->iso_frame_desc = iso;

	for(i = 0 ; i < 32 ; i++){
		iso[i].length=1023;
		iso[i].status = 0;	
	}


	ret = ioctl(handle->fd, USBDEVFS_SUBMITURB, &thing);
	if(ret >= 0)
		printf("submit_iso: %d\n", ret);
}

void do_stuff(struct usb_device *dev){
	usb_dev_handle *handle;
	unsigned char str[64000];
	int q, p, j, i;
	FILE *file;

	handle = usb_open(dev);
	printf("%d\n", handle);
	printf("set_config: %d\n", usb_set_configuration(handle, 1));	
	printf("claim:      %d\n", usb_claim_interface(handle, 0));
	printf("set_alt:    %d\n", usb_set_altinterface(handle, 8));

	send_ctrl(handle, 0xc1, 0, 0, 0, str, 1);
	str[0] = 0x28; str[1] = 0x1e;
	send_ctrl(handle, 0x41, 0x08, 0x15, 0, str, 2);
	str[0] = 0x9e;
	send_ctrl(handle, 0x41, 0x08, 0x18, 0, str, 1);
	str[0] = 0x20;
	send_ctrl(handle, 0x41, 0x08, 0x17, 0, str, 1);
	str[0] = 0x22;
	send_ctrl(handle, 0x41, 0x08, 0x19, 0, str, 1);
	str[0] = 0x02; str[1] = 0x03; str[2] = 0x0f; str[3] = 0x0c;
	send_ctrl(handle, 0x41, 0x08, 0x1c, 0, str, 4);
	str[0] = 0x46;
	send_ctrl(handle, 0x41, 0x08, 0x01, 0, str, 1);
	str[0] = 0xb0; str[1] = 0x11; str[2] = 0x20; str[3] = 0x00;
	str[4] = 0xc1; str[5] = 0x00; str[6] = 0x00; str[7] = 0x10;
	send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0xb0; str[1] = 0x11; str[2] = 0x22; str[3] = 0x00;
        str[4] = 0x10; str[5] = 0x00; str[6] = 0x00; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0xd0; str[1] = 0x11; str[2] = 0x10; str[3] = 0x00;
        str[4] = 0x04; str[5] = 0x00; str[6] = 0x03; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0xd0; str[1] = 0x11; str[2] = 0x14; str[3] = 0x01;
        str[4] = 0xe0; str[5] = 0x02; str[6] = 0x80; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0xb0; str[1] = 0x11; str[2] = 0x20; str[3] = 0x00;
        str[4] = 0xc1; str[5] = 0x02; str[6] = 0x80; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);
	str[0] = 0x46;
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
        str[0] = 0xb0; str[1] = 0x11; str[2] = 0x20; str[3] = 0x00;
        str[4] = 0x2d; str[5] = 0x19; str[6] = 0x19; str[7] = 0x10;
        send_ctrl(handle, 0x41, 0x08, 0x08, 0, str, 8);

	str[0] = 0x30;
        //send_ctrl(handle, 0x41, 0x08, 0x17, 0, str, 1);
	str[0] = 0x0e;
        send_ctrl(handle, 0x41, 0x08, 0x18, 0, str, 1);

	usb_resetep(handle, 0x83);
	usb_resetep(handle, 0x81);
	usb_resetep(handle, 0x82);
sleep(1);
	//while(1)
//		submit_iso(handle, 0x81);
//	return;
	file = fopen("/home/ian/cam_in.raw", "wb");
	while(1){
		memset(str, 0, 1023);
		send_ctrl(handle, 0xc1, 0, 0x0a, 0, str, 5);
		q = usb_bulk_read(handle, 0x82, str, 64000, 1000);
		j = 0;
		if(q >= 0){
                       	fwrite(str, q, 1, file);
			for(i = 0 ; i < q ; i++)
				j += str[i];
			printf("%d  : %08x\n",q, j);
		}
		/*else{
			usb_resetep(handle, 0x83);
			usb_resetep(handle, 0x81);
		}*/
	}

	printf("\n");
}


int main(void){
	printf("Yay\n");
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
				printf("%d\n", dev->config[0].bConfigurationValue);
				do_stuff(dev);
				exit(0);
    			}
    		}
    	}
        
	return 0;
}
