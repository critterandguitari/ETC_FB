#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

uint8_t *fbp, *bbp, *vbp;  // front and back and virtual buffers.  we draw on virtual, move it to back, fb offset gets swapped, repeat
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;
int fb_fd = 0;
long screensize = 0;

inline uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo)
{
    return (r<<vinfo->red.offset) | (g<<vinfo->green.offset) | (b<<vinfo->blue.offset);
}

inline void swap_buffers() {


}

int main()
{
    fb_fd = open("/dev/fb0",O_RDWR);
    //Get variable screen information
    ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
   // vinfo.grayscale=0;
   //  vinfo.bits_per_pixel=32;
    ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
    ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

    ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

    ////screensize = vinfo.yres_virtual * finfo.line_length;
    screensize = vinfo.yres * finfo.line_length;

    fbp = mmap(0, screensize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);
    bbp = fbp + screensize;
    vbp = (uint8_t *) malloc(screensize);
    
    // set buffer
    vinfo.yoffset=0;
    vinfo.xoffset=0;
    ioctl(fb_fd, FBIOPAN_DISPLAY, &vinfo); 

    printf("about to draw. size x = %d, size y = %d, bpp = %d \n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    printf("line len whatever that is %d\n", finfo.line_length);

    int x,y;
    int c;

    // clearem
    for (x=0;x<vinfo.xres;x++)
        for (y=0;y<vinfo.yres;y++)
        {
            long location = x * (vinfo.bits_per_pixel/8) + y * finfo.line_length;
            *((uint32_t*)(fbp + location)) = y * x;//pixel_color(0xFF,0x00,0xFF, &vinfo);
            *((uint32_t*)(bbp + location)) = rand();//pixel_color(0xFF,0x00,0xFF, &vinfo);
        }

   
    int color = 0;
    int count = 0;
    unsigned int randox, randoy;
    unsigned int sx, sy;
    sx = vinfo.xres;
    sy = vinfo.yres; 

    for (c = 0; c <60; c++){

        color = rand();
       

      //  for (x=0;x<vinfo.xres;x++)
       //     for (y=0;y<vinfo.yres;y++)
  
        // rando bg
        // really fast way 
        //memset(vbp, color, screensize);
        // slow way
        /*for (x=0;x<vinfo.xres;x++)
            for (y=0;y<vinfo.yres;y++)
            {
                long location = x * (vinfo.bits_per_pixel/8) + y * finfo.line_length;
                *((uint32_t*)(vbp + location)) = color;//pixel_color(0xFF,0x00,0xFF, &vinfo);
        }*/


   //rando rects
        for (count = 0; count < 100; count++){
        color = rand();
        randox = rand() % sx;
        randoy = rand() % sy;
        for (x=randox;x<10+randox;x++){
            for (y=randoy;y<100+randoy;y++){
                long location = ((x + count) % sx) * (vinfo.bits_per_pixel/8) + (y % sy) * finfo.line_length;
                *((uint32_t*)(vbp + location)) = color;//pixel_color(0xFF,0x00,0xFF, &vinfo);
            }
        }
        }
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
		long tmp;
		tmp=fbp;
		fbp=bbp;
		bbp=tmp;
	    
        // 30 frames a sec
 //       usleep(20000);
    }

    return 0;
}
