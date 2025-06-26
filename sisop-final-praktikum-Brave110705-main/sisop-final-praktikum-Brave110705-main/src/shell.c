#include "shell.h"
#include "kernel.h"
#include "std_lib.h"
#include "filesystem.h"

void shell() {
  char buf[64];
  char cmd[64];
  char arg[2][64];

  byte cwd = FS_NODE_P_ROOT;

  while (true) {
    printString("MengOS:");
    printCWD(cwd);
    printString("$ ");
    readString(buf);
    parseCommand(buf, cmd, arg);

    if (strcmp(cmd, "cd")) cd(&cwd, arg[0]);
    else if (strcmp(cmd, "ls")) ls(cwd, arg[0]);
    else if (strcmp(cmd, "mv")) mv(cwd, arg[0], arg[1]);
    else if (strcmp(cmd, "cp")) cp(cwd, arg[0], arg[1]);
    else if (strcmp(cmd, "cat")) cat(cwd, arg[0]);
    else if (strcmp(cmd, "mkdir")) mkdir(cwd, arg[0]);
    else if (strcmp(cmd, "clear")) clearScreen();
    else printString("Invalid command\n");
  }
}

// TODO: 4. Implement printCWD function
void printCWD(byte cwd) {}

// TODO: 5. Implement parseCommand function
void parseCommand(char* buf, char* cmd, char arg[2][64]) {}

// TODO: 6. Implement cd function
void cd(byte* cwd, char* dirname) {
  struct node_fs node_fs_buf;
  int i;
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);        
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  

  //kalok syntax "/" = ke root 
  if (strcmp(dirname, "/") == true) {
    *cwd = FS_NODE_P_ROOT;
    return;
  }

  //kalok syntax ".." dan bukan lagi di root = ke parent index
  if (strcmp(dirname, "..") == true) {
    if (*cwd != FS_NODE_P_ROOT) {
      *cwd = node_fs_buf.nodes[*cwd].parent_index;
    }
    return;
  }
  
  //buat cd (directory), nge-search secara iteratif ke directorynya, terus pindah directory ke situ
  for (i = 0; i < FS_MAX_NODE; i++) {
    struct node_item* node = &node_fs_buf.nodes[i];

    if (node->parent_index == *cwd && node->data_index == FS_NODE_D_DIR && strncmp(node->node_name, dirname, MAX_FILENAME) == true) {
      *cwd = i; 
      return;
    }
  }
}

// TODO: 7. Implement ls function
void ls(byte cwd, char* dirname) {}

// TODO: 8. Implement mv function
void mv(byte cwd, char* src, char* dst) {}

// TODO: 9. Implement cp function
void cp(byte cwd, char* src, char* dst) {}

// TODO: 10. Implement cat function
void cat(byte cwd, char* filename) {
  struct node_fs node_fs_buf;
  int i;
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);        
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
}

// TODO: 11. Implement mkdir function
void mkdir(byte cwd, char* dirname) {}

