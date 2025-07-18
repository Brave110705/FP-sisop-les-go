#include "std_lib.h"

int div(int a, int b) {
  int res = 0;
  while (a >= b) {
    a -= b;
    res++;
  }
  return res;
}

int mod(int a, int b) {
  while (a >= b) a -= b;
  return a;
}

void memcpy(byte* dst, byte* src, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++) dst[i] = src[i];
}


unsigned int strlen(char* str) {
  unsigned int len = 0;
  while (str[len] != '\0') len++;
  return len;
}

bool strcmp(char* str1, char* str2) {
  unsigned int i = 0;
  while (str1[i] != '\0' && str2[i] != '\0') {
    if (str1[i] != str2[i]) return false;
    i++;
  }
  return str1[i] == str2[i];
}

void strcpy(char* dst, char* src) {
  unsigned int i = 0;
  while (src[i] != '\0') {
    dst[i] = src[i];
    i++;
  }
  dst[i] = '\0';
}

void clear(byte* buf, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++) buf[i] = 0;
}

bool strncmp(char* str1, char* str2, int n) {
  unsigned int i;

  i = 0;
  while (str1[i] != '\0' && str2[i] != '\0' && i < n) {
    if (str1[i] != str2[i]) return false;
    i++;
  }
  return true;
}

char * strcmpuch(char * str1, char * str2, char delimiters) {
  unsigned int i;

  i = 0;
  while (str1[i] != '\0' && str2[i] != '\0') {
    if (str1[i] != str2[i]) return false;
    i++;
  }
  return true;

}

char * strchchr(char * str1, char delim) {
  unsigned int i;

  i = 0;
  while (str1[i] != '\0') {
    if (str1[i] == delim) return &(str1[i]);
    i++;
  }
  return 0;

}

void memset(byte* dst, byte tgt, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++) dst[i] = tgt;
}

void sleep(unsigned int time) {
  int i,j;
  int adjusted_count = 2* time * 100 * 10000;
  for (i = 0; i < adjusted_count ; i++) {
    asm("nop");
  }
  return ;
}
