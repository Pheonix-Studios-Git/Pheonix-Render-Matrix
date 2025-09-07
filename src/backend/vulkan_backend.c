#include <stdio.h>
#include <stdlib.h>

#include "prm.h"

int prm_vulkan_init(prm_window_config_t *cfg) {
    // TODO: setup VkInstance, VkSurfaceKHR (via GLFW/SDL2), swapchain, pipeline
    return 0;
}

void prm_vulkan_set_resolution(prm_window_resolution_t* resolution) {
  // TODO: add resolution stuff
}

void prm_vulkan_draw_pixel(int x, int y, uint32_t color) {
    // TODO: push pixel into a staging buffer or draw via shader
}

void prm_vulkan_clear() {
  // TODO: add this stuff
}

void prm_vulkan_present() {
    // TODO: submit command buffer + present swapchain
}

void prm_vulkan_shutdown() {
    // TODO: cleanup
}