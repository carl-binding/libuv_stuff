#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

uv_loop_t *loop;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  *buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
}



void on_close(uv_handle_t* handle)
{
  fprintf(stderr, "on_close...\n");
}

void on_write(uv_write_t* req, int status)
{

  // finished the write...
  
  if (status) {
    fprintf( stderr, "uv_write error \n");
    return;
  }

  fprintf(stderr, "on_write\n");
  free(req);
  // data was statically allocated
}

void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf)
{

  // callback on read completion
  
  fprintf(stderr, "on_read...\n");
  
  if(nread >= 0) {
    // we did include the null terminator when sending data to server...
    fprintf(stderr, "read: %ld %s\n", nread, buf->base);  // print it nicely
  } else {
    //we got an EOF or a failure
    fprintf( stderr, "EOF or failure\n");
    uv_close((uv_handle_t*)tcp, on_close);
  }

  // free buffer space
  free(buf->base);
}

void write_to_server( uv_stream_t *stream, const char *data, const int data_len) {
  
  uv_buf_t buffer;

  buffer.base = (char *) data;
  buffer.len = data_len;
  
  uv_write_t *req = malloc( sizeof( uv_write_t));
  uv_write( req, stream, &buffer, 1, on_write);
}

void on_connect(uv_connect_t* connection, int status)
{
    if (status < 0) {
      fprintf( stderr, "failed to connect\n"); return;
    }
    fprintf(stderr, "connected. %p %d\n",connection, status);

    uv_stream_t* stream = connection->handle;
    free(connection);

    // send message to server, incl null byte
    const char *msg = "hello server...\n";
    write_to_server( stream, msg, strlen(msg)+1); // the null byte...

    // and request a read for response from server
    uv_read_start(stream, alloc_buffer, on_read);
}

#define PORT 7000
#define HOST "127.0.0.1"

void start_conn(const char *host, const int port) {
  
    uv_tcp_t *pSock = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, pSock);
    uv_tcp_keepalive(pSock, 1, 60);

    struct sockaddr_in dest;
    uv_ip4_addr(host, port, &dest);

    uv_connect_t *pConn = malloc(sizeof(uv_connect_t));
    fprintf( stderr, "allocated %p\n", pConn);
    uv_tcp_connect(pConn, pSock, (const struct sockaddr*)&dest, on_connect);
}

int main(int argc, char **argv) {
    loop = uv_default_loop();
    int i;
    for (i=0; i<10; i++)
        start_conn(HOST, PORT);

    uv_run(loop, UV_RUN_DEFAULT);
}
