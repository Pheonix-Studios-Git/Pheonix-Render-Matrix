#pragma once
#define PRM_H

#include <stdint.h>

typedef enum {
  PRM_BACKEND_OPENGL,
  PRM_BACKEND_OPENGL_ES,
  PRM_BACKEND_VULKAN
} prm_backend_type;

typedef enum {
  PRM_CPU,
  PRM_GPU
} prm_usage_type;

typedef struct {
  int width;
  int height;
  int scale_x;
  int scale_y;
} prm_window_resolution_t;
// Resolution presets
#define PRM_PRESET_RES_RETRO ((prm_window_resolution_t){ 64, 36, 8, 8 })
#define PRM_PRESET_RES_HD ((prm_window_resolution_t){128, 72, 8, 8 })
#define PRM_PRESET_RES_FHD ((prm_window_resolution_t){192,108, 6, 6 })
#define PRM_PRESET_RES_QHD ((prm_window_resolution_t){256,144, 5, 5 })
#define PRM_PRESET_RES_2K ((prm_window_resolution_t){512,288, 3, 3 })

typedef struct {
  int width;
  int height;
  const char* title;
  prm_backend_type backend_type;
  prm_usage_type usage_type;
  int scale_x;
  int scale_y;
} prm_window_config_t;

int prm_init(prm_window_config_t* config);
void prm_set_resolution(prm_window_resolution_t* resolution);
void prm_draw_pixel(int x, int y, uint32_t color);
void prm_clear();
void prm_present();
void prm_shutdown();