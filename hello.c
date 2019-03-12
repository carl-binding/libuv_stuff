#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <uv.h>

int main(int argc, char **argv) {
  
  printf("Hello World!\n");

  uv_loop_t *loop = malloc( sizeof *loop);
  uv_loop_init( loop);

  printf("Now quitting.\n");
  uv_run(loop, UV_RUN_DEFAULT);

  uv_loop_close( loop);
  free( loop); loop = NULL;
  

  return 0;
}
