#include <stdio.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

int fb_width;
int fb_height;
char* fbdata;


// Writes RGB pixel data to framebuffer.
void fb_write(unsigned char r, unsigned char g, unsigned char b) {
    for (size_t x = 0; x < fb_width; ++x) {
        for (size_t y = 0; y < fb_height; ++y) {
            int offset = (y * fb_width + x) * 4;
            fbdata[offset + 0] = r;
            fbdata[offset + 1] = g;
            fbdata[offset + 2] = b;
            fbdata[offset + 3] = 0;
        }
    }
}


int main(void) {
    // Open /dev/fb0.
    int fbfd = open("/dev/fb0", O_RDWR);

    // Get the screen information.
    struct fb_var_screeninfo fbinfo;
    ioctl(fbfd, FBIOGET_VSCREENINFO, &fbinfo);

    // Set width height etc variables.
    fb_width = fbinfo.xres;
    fb_height = fbinfo.yres;
    int fb_bpp = fbinfo.bits_per_pixel;
    int fb_bytes = fb_bpp / 8;
    int fb_data_size = fb_width * fb_height * fb_bytes;

    // Map the memory.
    fbdata = mmap(0, fb_data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0);


    // Fork the process so we can both make the screen change colors 
    // and rickroll you.
    int child = fork();

    if (child != 0) {
        while (1) { 
            fb_write(0xFF, 0, 0);
            usleep(5000);
            fb_write(0, 0xFF, 0);
            usleep(5000);
            fb_write(0, 0, 0xFF);
            usleep(5000);
        }
    } else {
        // Play the rickroll.
        system("cat lol.wav | aplay");
    }

    munmap(fbdata, fb_data_size);
    close(fbfd);
    return 0;
}
