#include "uv.h"
#include "uv/unix.h"
#include <stdio.h>
#include <stdlib.h>

void write_cb(uv_write_t *req, int status) {
  if (status) {
    printf("write_cb status: %s\n", uv_strerror(status));
    return;
  }

  printf("written data\n");
}

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
    uv_write_t req = {0};
    uv_buf_t wr_buf = {.base = buf->base, .len = nread};
    int err;
    if ((err = uv_write(&req, stream, &wr_buf, 1, write_cb)) != 0) {
      printf("ERROR: uv_write = %s\n", uv_strerror(err));
      return;
    }
  } else if (nread == UV_EOF) {
    printf("read_cb received EOF\n");
    uv_tcp_close_reset((void *)stream, close_cb);
  }
  free(buf->base);
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

  if ((err = uv_read_start(client, alloc_cb, read_cb)) != 0) {
    printf("ERROR: uv_read_start = %s\n", uv_strerror(err));
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
