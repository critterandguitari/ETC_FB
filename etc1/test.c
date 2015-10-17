#include "etcfb.h"


int main()
{

    int color = 0;

    fb_setup();

    int i,j;
   for (i=0; i<100; i++){

        color = rand();
        fb_fill(color);
        for(j=0;j<100;j++) fb_rect();
        fb_flip();
        // 30 frames a sec
        usleep(20000);
    }

    return 0;
}
