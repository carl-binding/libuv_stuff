#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

uv_loop_t *loop;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  *buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

typedef struct {
  uv_write_t req;
  uv_buf_t buf;  // data payload
} write_req_t;

void free_write_req(uv_write_t *req) {
  // coerce to write_req_t
  write_req_t *wr = (write_req_t *) req;

  // free the buffer space, allocated by libuv
  free( wr->buf.base);
  // and the request
  free( wr);
  
}

void on_write(uv_write_t *req, int status) {
  // callback on write completion to client...

  fprintf( stderr, "on_write..\n");

  if (status == -1) {
    fprintf(stderr, "Write error!\n");
  }

  free_write_req( req);
  
}

void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
  // callback on read completion from client....

  fprintf( stderr, "on_read... %ld, %ld\n", nread, buf->len);
  
  if (nread == -1) {
    fprintf(stderr, "Read error!\n");
    uv_close((uv_handle_t*)client, NULL);
    return;
  }

  // we echo the data which was just read.

  // allocate a write request incl buffer , to be freed...
  write_req_t *req = (write_req_t *) malloc( sizeof( write_req_t));
  // initialize buffer, data is shared by reference and thus not yet freed here...
  // allocation done by libuv, to be freed when write is done
  req->buf = uv_buf_init( buf->base, nread);

  uv_write( (uv_write_t *) req, client, &(req->buf), 1, on_write);
  
}

void on_new_connection(uv_stream_t *server, int status) {
  // callback on listen() to new connection.
  
  fprintf( stderr, "on_new_connection...\n");
  if (status == -1) {
    return;
  }

  uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);

  // this seems to be synchronous, one wonders...
  if (uv_accept(server, (uv_stream_t*) client) == 0) {
    uv_read_start((uv_stream_t*) client, alloc_buffer, on_read);
  }
  else {
    fprintf( stderr, "failure on uv_accept...\n");
    uv_close((uv_handle_t*) client, NULL);
  }
}

#define PORT 7000
// #define HOST "127.0.0.1"
#define HOST "0.0.0.0"

int main() {
  loop = uv_default_loop();

  uv_tcp_t server;
  uv_tcp_init(loop, &server);

  struct sockaddr_in bind_addr;
  uv_ip4_addr(HOST, PORT, &bind_addr);
  
  uv_tcp_bind(&server, (const struct sockaddr *)  &bind_addr, 0);
  
  int r = uv_listen((uv_stream_t*) &server, 128, on_new_connection);
  if (r) {
    fprintf(stderr, "Listen error!\n");
    return 1;
  } else {
    fprintf( stderr, "server listening...\n");
  }
  
  return uv_run(loop, UV_RUN_DEFAULT);
}
