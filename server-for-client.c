#include "uv.h"
#include "uv/unix.h"
#include <stdio.h>
#include <stdlib.h>

void close_cb(uv_handle_t *handle) {
  printf("Closed connection\n");
  free(handle);
}

void write_cb(uv_write_t *req, int status) {
  if (status) {
    printf("write_cb status: %s\n", uv_strerror(status));
    return;
  }

  printf("written data\n");
  int err;
  if ((err = uv_tcp_close_reset((void *)req->handle, close_cb)) != 0) {
    printf("ERROR: uv_tcp_close_reset = %s\n", uv_strerror(err));
    return;
  }
}

void connection_cb(uv_stream_t *server, int status) {
  if (status) {
    printf("connection_cb status: %s\n", uv_strerror(status));
    return;
  }
  int err;
  uv_stream_t *client;
  client = malloc(sizeof(*client));

  uv_tcp_init(server->loop, (void *)client);

  if ((err = uv_accept(server, client)) != 0) {
    printf("ERROR: uv_accept = %s\n", uv_strerror(err));
    return;
  }

  uv_write_t req = {0};
  uv_buf_t write_buf = {.base = "Hello World!\n", .len = 12};
  if ((err = uv_write(&req, client, &write_buf, 1, write_cb)) != 0) {
    printf("ERROR: uv_write = %s\n", uv_strerror(err));
    return;
  }

  printf("----------Connected-----------\n");
}

int main() {

  uv_loop_t *loop = uv_default_loop();
  int err;

  uv_tcp_t tcp = {0};
  if ((err = uv_tcp_init(loop, &tcp)) != 0) {
    printf("ERROR: uv_tcp_init = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  struct sockaddr_in addr = {0};
  if ((err = uv_ip4_addr("127.0.0.1", 4242, &addr)) != 0) {
    printf("ERROR: uv_ip4_addr = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  if ((err = uv_tcp_bind(&tcp, (void *)&addr, 0)) != 0) {
    printf("ERROR: uv_tcp_bind = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  if ((err = uv_listen((void *)&tcp, 42, connection_cb)) != 0) {
    printf("ERROR: uv_listen = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  if ((err = uv_run(loop, UV_RUN_DEFAULT)) != 0) {
    printf("ERROR: uv_run = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  uv_loop_close(loop);

  return EXIT_SUCCESS;
}
