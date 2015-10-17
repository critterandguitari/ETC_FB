#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "etcfb.h"

uint8_t *fbp, *bbp, *vbp, *tmp;  // front and back and virtual buffers.  we draw on virtual, move it to back, fb offset gets swapped, repeat
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;
int fb_fd = 0;
long screensize = 0;


void fb_setup(void) {
    fb_fd = open("/dev/fb0",O_RDWR);
    //Get variable screen information
    ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
    vinfo.grayscale=0;
    vinfo.bits_per_pixel=32;
    vinfo.yres_virtual = 1440; // shouldn't this be implied?
    ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
    ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

    ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);
    
    screensize = vinfo.yres * finfo.line_length;

    fbp = mmap(0, screensize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);
    bbp = fbp + screensize;
    vbp = (uint8_t *) malloc(screensize);
    
    // set buffer
    vinfo.yoffset=0;
    vinfo.xoffset=0;
    ioctl(fb_fd, FBIOPAN_DISPLAY, &vinfo); 

    printf("setup screen size x = %d, size y = %d, bpp = %d, y virtual = %d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, vinfo.yres_virtual);
    printf("red offset = %d, blue offset = %d, green offset = %d \n", vinfo.red.offset, vinfo.blue.offset, vinfo.green.offset);
    printf("alpha offset = %d, alpha length = %d \n", vinfo.transp.offset, vinfo.transp.length);
    printf("line len whatever that is %d\n", finfo.line_length);
}

void fb_fill(int color) {

    int x, y;

    // fill one h line
    for (x=0;x<vinfo.xres;x++) {
            long location = x * (vinfo.bits_per_pixel/8);
            *((uint32_t*)(vbp + location)) = color;//pixel_color(0xFF,0x00,0xFF, &vinfo);
    }
    // copy to all v lines
    for (y=1;y<vinfo.yres;y++){
        memcpy(vbp + (finfo.line_length * y), vbp, finfo.line_length);
    }
}

void fb_flip(void) {
    // copy to back  
    memcpy(bbp, vbp, screensize);

    // flip
    if (vinfo.yoffset==0)
        vinfo.yoffset = vinfo.yres;//screensize;
    else
        vinfo.yoffset=0;

    //"Pan" to the back buffer	
    ioctl(fb_fd, FBIOPAN_DISPLAY, &vinfo); 
 
    //Update the pointer to the back buffer so we don't draw on the front buffer
    //long tmp;
    tmp=fbp;
    fbp=bbp;
    bbp=tmp;
}

void fb_rect(void) {
   //rando rects
   int sx = 1280;
   int sy = 720;
   int count;
   int x, y, randox, randoy, color;
        color = rand();
        randox = rand() % (sx - 101);
        randoy = rand() % (sy - 101);
        int sourceloc;
        for (x=randox;x<100+randox;x++){
            long location = x * (vinfo.bits_per_pixel/8) + randoy * finfo.line_length;
            *((uint32_t*)(vbp + location)) = color & 0x00FFFFFF;//pixel_color(0xFF,0x00,0xFF, &vinfo);      
        }
        sourceloc =  randox * (vinfo.bits_per_pixel/8) + randoy * finfo.line_length;
        for (y=1+randoy;y<100+randoy;y++){
            memcpy(vbp + (y * finfo.line_length) +  randox * (vinfo.bits_per_pixel/8), vbp + sourceloc, 400);
        }

}

