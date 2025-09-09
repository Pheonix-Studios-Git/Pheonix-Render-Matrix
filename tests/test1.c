// test/test1.c
#include <prm.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h> // for sleep

int main() {
  prm_window_resolution_t res = {
    .width = 155,
    .height = 30,
    .scale_x = 1,
    .scale_y = 1
  };
    prm_window_config_t cfg = {
        .width = res.width,          // logical width
        .height = res.height,         // logical height
        .scale_x = res.scale_x,         // 8 GPU pixels per logical pixel
        .scale_y = res.scale_y,
        .title = "PRM CPU Test",
        .backend_type = PRM_BACKEND_OPENGL_ES,
        .usage_type = PRM_GPU,
    };

    if (prm_init(&cfg) != 0) {
        printf("Failed to initialize PRM!\n");
        return -1;
    }

    prm_set_resolution(&res);
    
    int height = cfg.height;
    int width = cfg.width;

    // clear framebuffer
    prm_clear();

    // animation
    int x = 20;
    int y = 5;
    int z = 0;

    for (;;) {
    	// draw something simple
    	for (int l1;l1 < 25;l1++) {
		prm_draw_pixel(x, y, 0xFFFFFFFF);
		y++;
	}
	y = 0;
	x = 30;
	for (int l2;l2 < 25;l2++) {
		prm_draw_pixel(x, y, 0xFFFFFFFF);
		y++;
	}
	prm_present();
    }

    prm_shutdown();
    return 0;
}
