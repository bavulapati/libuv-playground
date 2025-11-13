#include "uv.h"
#include <stdio.h>
#include <stdlib.h>

int repetetions = 0;
void timer_cb(uv_timer_t *handle) {
  printf("------ timeout ------\n");
  if (++repetetions == 4) {
    int err;
    if ((err = uv_timer_stop(handle)) != 0) {
      printf("ERROR: uv_timer_stop = %s\n", uv_strerror(err));
    }
  }
}

int main() {
  uv_loop_t *loop = uv_default_loop();
  int err;

  uv_timer_t timer = {0};
  if ((err = uv_timer_init(loop, &timer)) != 0) {
    printf("ERROR: uv_timer_init = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  if ((err = uv_timer_start(&timer, timer_cb, 1000, 1000)) != 0) {
    printf("ERROR: uv_timer_start = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  if ((err = uv_run(loop, UV_RUN_DEFAULT)) != 0) {
    printf("ERROR: uv_run = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  uv_loop_close(loop);

  return EXIT_SUCCESS;
}
