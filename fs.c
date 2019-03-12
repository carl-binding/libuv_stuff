#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

void on_open(uv_fs_t *req);
void on_read(uv_fs_t *req);
void on_write(uv_fs_t *req);
  
uv_fs_t open_req; 
uv_fs_t read_req;   
uv_fs_t write_req;
uv_fs_t close_req;

uv_buf_t buffer;

void on_open(uv_fs_t *req) {
  if (req->result != -1) {
    int s = 0;
    
    fprintf( stderr, "on_open: enq read request\n");
    s = uv_fs_read(uv_default_loop(), &read_req, req->result,
        &buffer, 1, -1, on_read);
    if ( s < 0) {
      fprintf( stderr, "failure on uv_fs_read\n");
    }
  }
  else {
    fprintf(stderr, "Error opening file!\n");
  }
}

#define STDOUT 1

void on_read(uv_fs_t *req) {
  uv_fs_req_cleanup(req);
  if (req->result < 0) {
    fprintf(stderr, "Read error!\n");
  }
  else if (req->result == 0) {
    // done reading, enqueue close request
    // uv_fs_t close_req;
    fprintf( stderr, "on_read: enq close request\n");
    uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
  }
  else {
    int s = 0;
    fprintf( stderr, "on_read: enq write request\n");
    s = uv_fs_write(uv_default_loop(), &write_req, STDOUT, 
        &buffer, 1, -1, on_write);
    if ( s < 0) {
      fprintf( stderr, "failure on uv_fs_write\n");
    }

  }
}


void on_write(uv_fs_t *req) {
  uv_fs_req_cleanup(req);
  if (req->result < 0) {
    fprintf(stderr, "Write error!");
  }
  else {
    int s = 0;
    fprintf( stderr, "on_write: enq read request\n");
    s = uv_fs_read(uv_default_loop(), &read_req, open_req.result, 
	       &buffer, 1, -1, on_read);
    if ( s < 0) {
      fprintf( stderr, "failure on uv_fs_read\n");
    }
  }
}

int main(int argc, char **argv) {

  unsigned int len = 1000*sizeof(char);
  
  char *base = (char *)malloc(len);
  int s = 0;

  buffer = uv_buf_init(base, len);

  s = uv_fs_open(uv_default_loop(), &open_req, argv[1], O_RDONLY, 0, on_open);
  if ( s < 0) {
    fprintf( stderr, "failure on uv_fs_open\n");
  }
  s = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  if ( s < 0) {
    fprintf( stderr, "failure on uv_run\n");
  }

  free(buffer.base); buffer.base=NULL;

  uv_fs_req_cleanup(&open_req);
  uv_fs_req_cleanup(&read_req);
  uv_fs_req_cleanup(&write_req);
  uv_fs_req_cleanup(&close_req);  
  

  return 0;
}
