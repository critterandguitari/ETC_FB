#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

uint8_t *fbp, *bbp, *vbp, *tmp;  // front and back and virtual buffers.  we draw on virtual, move it to back, fb offset gets swapped, repeat
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
    vinfo.grayscale=0;
    vinfo.bits_per_pixel=16;
    vinfo.yres_virtual = 1440; // shouldn't this be implied?
    ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
    ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

    ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);
    
    screensize = vinfo.yres * finfo.line_length;

   // fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);

    fbp = mmap(0, screensize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);
   bbp = fbp + screensize;
   vbp = (uint8_t *) malloc(screensize);
    
    // set buffer
    vinfo.yoffset=0;
    vinfo.xoffset=0;
    ioctl(fb_fd, FBIOPAN_DISPLAY, &vinfo); 

    printf("about to draw. size x = %d, size y = %d, bpp = %d, y virtual = %d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, vinfo.yres_virtual);
    printf("line len whatever that is %d\n", finfo.line_length);

    int x,y;
    int c;
/*    printf("trying\n");
    *((uint32_t*)(fbp)) = 0;
    printf("set\n");
    exit(0);*/


    printf("cleared bufs\n");
    int color = 0;
    int count = 0;
    unsigned int randox, randoy;
    unsigned int sx, sy;
    sx = vinfo.xres;
    sy = vinfo.yres; 

    for (c = 0; c <1000; c++){
 //   for (;;){

        color = rand();
         for (x=0;x<vinfo.xres;x++)
            for (y=0;y<vinfo.yres;y++)
            {
                long location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
                *((uint16_t*)(vbp + location)) = (uint16_t)y*x*(c+1);//pixel_color(0xFF,0x00,0xFF, &vinfo);
            }
      

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
        // fill one h line
    //    for (x=0;x<vinfo.xres;x++) {
     //           long location = x * (vinfo.bits_per_pixel/8);
      //          *((uint32_t*)(vbp + location)) = color;//pixel_color(0xFF,0x00,0xFF, &vinfo);
       // }
        // copy to all v lines
        //for (y=1;y<vinfo.yres;y++){
         //   memcpy(vbp + (finfo.line_length * y), vbp, finfo.line_length);
        //}



   //rando rects
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
	    
        // 30 frames a sec
//        usleep(20000);
    }

    return 0;
}
