#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel.h"

// Test create/destroy process.
// Totally 25 points.
int case1(){
  KERNEL_SPACE_SIZE = 8192;
  VIRTUAL_SPACE_SIZE = 512;
  PAGE_SIZE = 32;
  MAX_PROCESS_NUM = 8;
  int score = 0;

  struct Kernel * kernel = init_kernel();
  int pid1 = proc_create_vm(kernel, VIRTUAL_SPACE_SIZE);
  if(pid1 == 0)
    score += 1;

  int pid = proc_create_vm(kernel, VIRTUAL_SPACE_SIZE + 1);
  if(pid == -1)
    score += 1;

  int pid2 = proc_create_vm(kernel, VIRTUAL_SPACE_SIZE);
  if(pid2 == 1)
    score += 1;

  int pid3 = proc_create_vm(kernel, VIRTUAL_SPACE_SIZE);
  if(pid3 == 2)
    score += 1;

  int pid4 = proc_create_vm(kernel, VIRTUAL_SPACE_SIZE);
  if(pid4 == 3)
    score += 1;

  int pid5 = proc_create_vm(kernel, VIRTUAL_SPACE_SIZE);
  if(pid5 == 4)
    score += 1;

  int pid6 = proc_create_vm(kernel, VIRTUAL_SPACE_SIZE);
  if(pid6 == 5)
    score += 1;

  int pid7 = proc_create_vm(kernel, VIRTUAL_SPACE_SIZE);
  if(pid7 == 6)
    score += 1;

  int pid8 = proc_create_vm(kernel, VIRTUAL_SPACE_SIZE);
  if(pid8 == 7)
    score += 1;

  pid = proc_create_vm(kernel, VIRTUAL_SPACE_SIZE);
  if(pid == -1)
    score += 1;

  // Check if present is clear.
  int set = 1;
  for(int i = 0; i < 8; i ++) {
    for(int j = 0; j < kernel->mm[i].size/PAGE_SIZE; j++) {
      if(kernel->mm[i].page_table->ptes[j].present != 0){
        set = 0;
        break;
      }
    }
    if(set == 0) break;
  }
  if(set == 1)
    score += 2;

  // Check if running is set.
  set = 1;
  for(int i = 0; i < 8; i ++){
    if(kernel->running[i] != 1){
      set = 0;
      break;
    }
  }
  if(set == 1)
    score += 2;

  if(proc_exit_vm(kernel, pid1) == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid2) == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid3) == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid4) == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid5) == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid6) == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid7) == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid8) == 0)
    score += 1;

  // Check if occupied_pages is reset.
  set = 1;
  for(int i = 0; i < VIRTUAL_SPACE_SIZE/PAGE_SIZE*8; i ++){
    if(kernel->occupied_pages[i] != 0){
      set = 0;
      break;
    }
  }
  if(set == 1)
    score += 2;

  // Check if running is reset.
  set = 1;
  for(int i = 0; i < 8; i ++){
    if(kernel->running[i] != 0){
      set = 0;
      break;
    }
  }
  if(set == 1)
    score += 1;

  destroy_kernel(kernel);
  return score;
}

// Verification on kernel free space.
// Totally 25 points.
int case2(){
  KERNEL_SPACE_SIZE = 8192;
  VIRTUAL_SPACE_SIZE = 512;
  PAGE_SIZE = 32;
  MAX_PROCESS_NUM = 8;

  int score = 0;
  char * buf = (char *)malloc(128);
  char * temp_buf = (char *)malloc(512);

  struct Kernel * kernel = init_kernel();

  int pid1 = proc_create_vm(kernel, 10);
  memset(buf, 0, 128);
  memset(temp_buf, 0, 512);
  vm_read(kernel, 0, (char *)(0), 10, temp_buf);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:32, size:8160)\n") == 0)
    score += 1;

  int pid2 = proc_create_vm(kernel, 1);
  memset(buf, 0, 128);
  memset(temp_buf, 0, 512);
  vm_write(kernel, 1, (char *)(0), 1, temp_buf);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:64, size:8128)\n") == 0)
    score += 1;

  int pid3 = proc_create_vm(kernel, 300);
  memset(buf, 0, 128);
  memset(temp_buf, 0, 512);
  vm_read(kernel, 2, (char *)(0), 300, temp_buf);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:384, size:7808)\n") == 0)
    score += 1;

  int pid4 = proc_create_vm(kernel, 500);
  memset(buf, 0, 128);
  memset(temp_buf, 0, 512);
  vm_write(kernel, 3, (char *)(0), 500, temp_buf);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:896, size:7296)\n") == 0)
    score += 1;

  int pid5 = proc_create_vm(kernel, 122);
  memset(buf, 0, 128);
  memset(temp_buf, 0, 512);
  vm_read(kernel, 4, (char *)(0), 122, temp_buf);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:1024, size:7168)\n") == 0)
    score += 1;

  int pid6 = proc_create_vm(kernel, 31);
  memset(buf, 0, 128);
  memset(temp_buf, 0, 512);
  vm_write(kernel, 5, (char *)(0), 31, temp_buf);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:1056, size:7136)\n") == 0)
    score += 1;

  int pid7 = proc_create_vm(kernel, 7);
  memset(buf, 0, 128);
  memset(temp_buf, 0, 512);
  vm_read(kernel, 6, (char *)(0), 7, temp_buf);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:1088, size:7104)\n") == 0)
    score += 1;

  int pid8 = proc_create_vm(kernel, 48);
  memset(buf, 0, 128);
  memset(temp_buf, 0, 512);
  vm_write(kernel, 7, (char *)(0), 48, temp_buf);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:1152, size:7040)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid1) == 0)
    score += 1;
  memset(buf, 0, 128);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:32) -> (addr:1152, size:7040)\n") == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid2) == 0)
    score += 1;
  memset(buf, 0, 128);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:64) -> (addr:1152, size:7040)\n") == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid3) == 0)
    score += 1;
  memset(buf, 0, 128);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:384) -> (addr:1152, size:7040)\n") == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid4) == 0)
    score += 1;
  memset(buf, 0, 128);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:896) -> (addr:1152, size:7040)\n") == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid5) == 0)
    score += 1;
  memset(buf, 0, 128);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:1024) -> (addr:1152, size:7040)\n") == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid6) == 0)
    score += 1;
  memset(buf, 0, 128);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:1056) -> (addr:1152, size:7040)\n") == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid7) == 0)
    score += 1;
  memset(buf, 0, 128);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:1088) -> (addr:1152, size:7040)\n") == 0)
    score += 1;
  if(proc_exit_vm(kernel, pid8) == 0)
    score += 1;
  memset(buf, 0, 128);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:8192)\n") == 0)
    score += 2;
  destroy_kernel(kernel);
  free(buf);
  free(temp_buf);
  return score;
}

// Verification on kernel free space.
// Totally 25 points.
int case3(){
  KERNEL_SPACE_SIZE = 8192;
  VIRTUAL_SPACE_SIZE = 512;
  PAGE_SIZE = 32;
  MAX_PROCESS_NUM = 8;

  int score = 0;
  char * buf = (char *)malloc(512);
  char * temp_buf = (char *)malloc(128);

  struct Kernel * kernel = init_kernel();

  int pid1 = proc_create_vm(kernel, 64);
  memset(temp_buf, 0, 128);
  vm_read(kernel, 0, (char *)(0), 64, temp_buf);

  int pid2 = proc_create_vm(kernel, 64);
  memset(temp_buf, 0, 128);
  vm_write(kernel, 1, (char *)(0), 64, temp_buf);

  int pid3 = proc_create_vm(kernel, 64);
  memset(temp_buf, 0, 128);
  vm_read(kernel, 2, (char *)(0), 64, temp_buf);

  int pid4 = proc_create_vm(kernel, 64);
  memset(temp_buf, 0, 128);
  vm_write(kernel, 3, (char *)(0), 64, temp_buf);

  int pid5 = proc_create_vm(kernel, 64);
  memset(temp_buf, 0, 128);
  vm_read(kernel, 4, (char *)(0), 64, temp_buf);

  int pid6 = proc_create_vm(kernel, 64);
  memset(temp_buf, 0, 128);
  vm_write(kernel, 5, (char *)(0), 64, temp_buf);

  int pid7 = proc_create_vm(kernel, 64);
  memset(temp_buf, 0, 128);
  vm_read(kernel, 6, (char *)(0), 64, temp_buf);

  int pid8 = proc_create_vm(kernel, 64);
  memset(temp_buf, 0, 128);
  vm_write(kernel, 7, (char *)(0), 64, temp_buf);

  if(proc_exit_vm(kernel, pid2) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:64, size:64) -> (addr:512, size:7680)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid4) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:64, size:64) -> (addr:192, size:64) -> (addr:512, size:7680)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid6) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:64, size:64) -> (addr:192, size:64) -> (addr:320, size:64) -> (addr:512, size:7680)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid8) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:64, size:64) -> (addr:192, size:64) -> (addr:320, size:64) -> (addr:448, size:7744)\n") == 0)
    score += 1;

  int pid9 = proc_create_vm(kernel, 32);
  memset(temp_buf, 0, 128);
  vm_write(kernel, 1, (char *)(0), 32, temp_buf);
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:96, size:32) -> (addr:192, size:64) -> (addr:320, size:64) -> (addr:448, size:7744)\n") == 0)
    score += 1;

  int pid10 = proc_create_vm(kernel, 65);
  memset(temp_buf, 0, 128);
  vm_write(kernel, 3, (char *)(0), 65, temp_buf);
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:320, size:64) -> (addr:448, size:7744)\n") == 0)
    score += 1;

  int pid11 = proc_create_vm(kernel, 119);
  memset(temp_buf, 0, 128);
  vm_write(kernel, 5, (char *)(0), 119, temp_buf);
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:512, size:7680)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid9) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:64, size:32) -> (addr:512, size:7680)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid10) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:64, size:64) -> (addr:192, size:64) -> (addr:512, size:7680)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid11) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:64, size:64) -> (addr:192, size:64) -> (addr:320, size:64) -> (addr:448, size:7744)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid1) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:128) -> (addr:192, size:64) -> (addr:320, size:64) -> (addr:448, size:7744)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid3) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:256) -> (addr:320, size:64) -> (addr:448, size:7744)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid5) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:384) -> (addr:448, size:7744)\n") == 0)
    score += 1;

  if(proc_exit_vm(kernel, pid7) == 0)
    score += 1;
  memset(buf, 0, 512);
  get_kernel_free_space_info(kernel, buf);
  if(strcmp(buf, "free space: (addr:0, size:8192)\n") == 0)
    score += 1;

  destroy_kernel(kernel);
  free(buf);
  free(temp_buf);
  return score;
}

// Test read/write.
// Totally 25 points.
int case4(){
  KERNEL_SPACE_SIZE = 8192;
  VIRTUAL_SPACE_SIZE = 512;
  PAGE_SIZE = 32;
  MAX_PROCESS_NUM = 8;

  int score = 0;
  char * rbuf = (char *)malloc(512);
  char * wbuf = (char *)malloc(512);

  struct Kernel * kernel = init_kernel();

  int pid1 = proc_create_vm(kernel, 32);
  int pid2 = proc_create_vm(kernel, 32);
  int pid3 = proc_create_vm(kernel, 32);
  int pid4 = proc_create_vm(kernel, 64);

  // Test out-of-bounds read/write.
  if(vm_write(kernel, pid1, (char *)(0), 310, wbuf) == -1)
    score += 1;
    printf("score %d, %d\n", score, __LINE__);
  if(vm_read(kernel, pid1, (char *)(7), 58, rbuf) == -1)
    score += 1;
    printf("score %d, %d\n", score, __LINE__);

  // Test 0 when allocated.
  memset(rbuf, 0, 512);
  memset(wbuf, 0, 512);
  if(vm_read(kernel, pid3, (char *)(0), 32, rbuf) == 0)
    score += 1;
    printf("score %d, %d\n", score, __LINE__);
  if(strncmp(wbuf, rbuf, 32) == 0)
    score += 2;
    printf("score %d, %d\n", score, __LINE__);

  // Test read/write on a single page.
  memset(wbuf, 0, 512);
  sprintf(wbuf, "CSCI3150 assignment 3");
  if(vm_write(kernel, pid3, (char *)(10), strlen(wbuf), wbuf) == 0)
    score += 2;
    printf("score %d, %d\n", score, __LINE__);
  memset(rbuf, 0, 512);
  if(vm_read(kernel, pid3, (char *)(14), strlen(wbuf)-4, rbuf) == 0)
    score += 2;
    printf("score %d, %d\n", score, __LINE__);
  if(strncmp("3150 assignment 3", rbuf, strlen(wbuf)-4) == 0)
    score += 2;
    printf("score %d, %d\n", score, __LINE__);

  // Test read/write on continuous pages.
  memset(wbuf, 0, 512);
  for(int i = 0; i < 55; i ++)
    wbuf[i] = (char)(i);
  if(vm_write(kernel, pid4, (char *)(9), 55, wbuf) == 0)
    score += 2;
    printf("score %d, %d\n", score, __LINE__);
  memset(rbuf, 0, 512);
  if(vm_read(kernel, pid4, (char *)(9), 55, rbuf) == 0)
    score += 2;
    printf("score %d, %d\n", score, __LINE__);
  if(strncmp(wbuf, rbuf, 55) == 0)
    score += 3;
    printf("score %d, %d\n", score, __LINE__);

  // Test read/write on not-continuos pages.
  proc_exit_vm(kernel, pid2);
  int pid5 = proc_create_vm(kernel, 64);
  memset(wbuf, 0, 512);
  for(int i = 0; i < 47; i ++)
    wbuf[i] = (char)(i);
  if(vm_write(kernel, pid5, (char *)(3), 47, wbuf) == 0)
    score += 2;
    printf("score %d, %d\n", score, __LINE__);
  memset(rbuf, 0, 512);
  if(vm_read(kernel, pid5, (char *)(3), 47, rbuf) == 0)
    score += 2;
    printf("score %d, %d\n", score, __LINE__);
  if(strncmp(wbuf, rbuf, 47) == 0)
    score += 3;
    printf("score %d, %d\n", score, __LINE__);

  proc_exit_vm(kernel, pid1);
  proc_exit_vm(kernel, pid3);
  proc_exit_vm(kernel, pid4);
  proc_exit_vm(kernel, pid5);
  destroy_kernel(kernel);
  free(rbuf);
  free(wbuf);
  return score;
}

int main(int argc, char **argv){
  if (argc == 1) {
    printf("Test case 1 score: %d\n", case1());
    printf("Test case 2 score: %d\n", case2());
    printf("Test case 3 score: %d\n", case3());
    printf("Test case 4 score: %d\n", case4());
  } else if (argc == 2) {
    int score1 = 0;
    int score2 = 0;
    int score3 = 0;
    int score4 = 0;
    score1 = case1();
    printf("case1 %d\n", score1);
    score2 = case2();
    printf("case2 %d\n", score2);
    score3 = case3();
    printf("case3 %d\n", score3);
    score4 = case4();
    printf("case4 %d\n", score4);
    FILE* f = fopen(argv[1], "w");
    char buf[32];
    int n = sprintf(buf, "%d,%d,%d,%d", score1, score2, score3, score4);
    fwrite(buf, 1, n, f);
    fclose(f);
  } else if (argc == 3 && argv[2][0] == '1') {
    FILE* f = fopen("1", "w");
    int score = case1();
    printf("case1 %d\n", score);
    char buf[12];
    int n = sprintf(buf, "%d", score);
    fwrite(buf, 1, n, f);
    fclose(f);
  } else if (argc == 3 && argv[2][0] == '2') {
    FILE* f = fopen("2", "w");
    int score = case2();
    printf("case2 %d\n", score);
    char buf[12];
    int n = sprintf(buf, "%d", score);
    fwrite(buf, 1, n, f);
    fclose(f);
  } else if (argc == 3 && argv[2][0] == '3') {
    FILE* f = fopen("3", "w");
    int score = case3();
    printf("case3 %d\n", score);
    char buf[12];
    int n = sprintf(buf, "%d", score);
    fwrite(buf, 1, n, f);
    fclose(f);
  } else if (argc == 3 && argv[2][0] == '4') {
    FILE* f = fopen("4", "w");
    int score = case4();
    printf("case4 %d\n", score);
    char buf[12];
    int n = sprintf(buf, "%d", score);
    fwrite(buf, 1, n, f);
    fclose(f);
  }
}