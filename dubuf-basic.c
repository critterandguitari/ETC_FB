#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>


uint8_t *fbp, *bbp;  // front and back buffers
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

    screensize = vinfo.yres_virtual * finfo.line_length;

    fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);
    //bbp = fbp + screensize;

    int x,y;
    int c;
        for (x=0;x<vinfo.xres;x++)
            for (y=0;y<vinfo.yres;y++)
            {
                long location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
                *((uint32_t*)(fbp + location)) = y*x*(c+1 + x + y + c);//pixel_color(0xFF,0x00,0xFF, &vinfo);
            }

    for (c = 0; c <2000; c++){
       	if (vinfo.yoffset==0)
			vinfo.yoffset = 500;//screensize;
		else
			vinfo.yoffset=0;

		//"Pan" to the back buffer	
		ioctl(fb_fd, FBIOPAN_DISPLAY, &vinfo); 
        
//        usleep(30000);
    }

    return 0;
}
