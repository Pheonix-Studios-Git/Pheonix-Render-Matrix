#include <stdio.h>
#include <stdlib.h>
#include <prm.h>

int prm_opengl_es_init(prm_window_config_t* config);
int prm_vulkan_init(prm_window_config_t* config);
void prm_opengl_es_set_resolution(prm_window_resolution_t* resolution);
void prm_vulkan_set_resolution(prm_window_resolution_t* resolution);
void prm_opengl_es_draw_pixel(int x, int y, uint32_t color);
void prm_vulkan_draw_pixel(int x, int y, uint32_t color);
void prm_opengl_es_clear();
void prm_vulkan_clear();
void prm_opengl_es_present();
void prm_vulkan_present();
void prm_opengl_es_shutdown();
void prm_vulkan_shutdown();

static prm_backend_type active_backend = PRM_BACKEND_OPENGL_ES;

int prm_init(prm_window_config_t* config) {
  active_backend = config->backend_type;
  if (active_backend == PRM_BACKEND_OPENGL_ES) {
    return prm_opengl_es_init(config);
  } else if (active_backend == PRM_BACKEND_VULKAN) {
    return prm_vulkan_init(config);
  } else {
    printf("Unknown Backend Type: [%d]\n", active_backend);
    return -1;
  }
  
  return -1;
}

void prm_set_resolution(prm_window_resolution_t* resolution) {
  if (active_backend == PRM_BACKEND_OPENGL_ES) {
    prm_opengl_es_set_resolution(resolution);
  } else if (active_backend == PRM_BACKEND_VULKAN) {
    return prm_vulkan_set_resolution(resolution);
  } else {
    printf("Unknown Backend - [%d]\n", active_backend);
  }
}

void prm_draw_pixel(int x, int y, uint32_t color) {
  if (active_backend == PRM_BACKEND_OPENGL_ES) {
    return prm_opengl_es_draw_pixel(x, y, color);
  } else if (active_backend == PRM_BACKEND_VULKAN) {
    return prm_vulkan_draw_pixel(x, y, color);
  } else {
    printf("Unknown Backend Type: [%d]\n", active_backend);
  }
}

void prm_clear() {
  if (active_backend == PRM_BACKEND_OPENGL_ES) {
    return prm_opengl_es_clear();
  } else if (active_backend == PRM_BACKEND_VULKAN) {
    return prm_vulkan_clear();
  } else {
    printf("Unknown Backend Type: [%d]\n", active_backend);
  }
}

void prm_present() {
  if (active_backend == PRM_BACKEND_OPENGL_ES) {
    return prm_opengl_es_present();
  } else if (active_backend == PRM_BACKEND_VULKAN) {
    return prm_vulkan_present();
  } else {
    printf("Unknown Backend Type: [%d]\n", active_backend);
  }
}

void prm_shutdown() {
  if (active_backend == PRM_BACKEND_OPENGL_ES) {
    return prm_opengl_es_shutdown();
  } else if (active_backend == PRM_BACKEND_VULKAN) {
    return prm_vulkan_shutdown();
  } else {
    printf("Unknown Backend Type: [%d]\n", active_backend);
  }
}