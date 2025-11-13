#include "uv.h"
#include "uv/unix.h"
#include <stdio.h>
#include <stdlib.h>

void alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  printf("TRACE: alloc_cb allocating %zu Bytes\n", suggested_size);
  buf->base = malloc(suggested_size);
  buf->len = suggested_size;
}

void close_cb(uv_handle_t *handle) {
  printf("Closed connection\n");
  free(handle);
}

void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  if (nread > 0) {
    printf("read_cb read %zu Bytes\n", nread);
    buf->base[nread] = '\0';
    printf("read %s\n", buf->base);
  } else if (nread == UV_EOF) {
    printf("read_cb received EOF\n");
    uv_tcp_close_reset((void *)stream, close_cb);
  }
  free(buf->base);
}

void connect_cb(uv_connect_t *req, int status) {
  if (status) {
    printf("connect_cb status: %s\n", uv_strerror(status));
    return;
  }
  int err;

  if ((err = uv_read_start((void *)req->handle, alloc_cb, read_cb)) != 0) {
    printf("ERROR: uv_read_start = %s\n", uv_strerror(err));
    return;
  }

  printf("----------Connected-----------\n");
}

int main() {

  uv_loop_t *loop = uv_default_loop();
  int err;

  uv_tcp_t *socket;
  socket = malloc(sizeof(*socket));
  if ((err = uv_tcp_init(loop, socket)) != 0) {
    printf("ERROR: uv_tcp_init = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  uv_connect_t *connect;
  connect = malloc(sizeof(*connect));

  struct sockaddr_in dest;
  if ((err = uv_ip4_addr("127.0.0.1", 4242, &dest)) != 0) {
    printf("ERROR: uv_ip4_addr = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }
  if ((err = uv_tcp_connect(connect, socket, (void *)&dest, connect_cb)) != 0) {
    printf("ERROR: uv_tcp_connect = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  if ((err = uv_run(loop, UV_RUN_DEFAULT)) != 0) {
    printf("ERROR: uv_run = %s\n", uv_strerror(err));
    return EXIT_FAILURE;
  }

  uv_loop_close(loop);
  free(connect);

  return EXIT_SUCCESS;
}
