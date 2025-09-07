// test/test1.c
#include <prm.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h> // for sleep

int main() {
    prm_window_resolution_t res = PRM_PRESET_RES_RETRO;
    prm_window_config_t cfg = {
        .width = res.width,          // logical width
        .height = res.height,         // logical height
        .scale_x = res.scale_x,         // 8 GPU pixels per logical pixel
        .scale_y = res.scale_y,
        .title = "PRM CPU Test",
        .backend_type = PRM_BACKEND_OPENGL_ES,
        .usage_type = PRM_CPU,
    };

    if (prm_init(&cfg) != 0) {
        printf("Failed to initialize PRM!\n");
        return -1;
    }
    
    int height = cfg.height;
    int width = cfg.width;

    // clear framebuffer
    prm_clear();

    // draw a simple triangle
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // simple triangle math
            if (y > height/4 && y < 3*height/4 && 
                x > width/4 && x < 3*width/4 && 
                y < (2*x) - width/2 && 
                y < (-2*x) + 3*width) {

                prm_draw_pixel(x, y, 0xFF0000FF);
            }
        }
    }
    prm_draw_pixel(1, 1, 0x00FF00FF);
    prm_present();

    prm_shutdown();
    return 0;
}