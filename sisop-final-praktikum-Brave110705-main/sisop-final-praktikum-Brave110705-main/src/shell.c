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
void printCWD(byte cwd) {
    struct node_fs fs;
    char* names[FS_MAX_NODE];
    int depth = 0;
    int i;

    readSector(fs.nodes, FS_NODE_SECTOR_NUMBER);
    readSector(fs.nodes + 32, FS_NODE_SECTOR_NUMBER + 1);

    while (cwd != FS_NODE_P_ROOT && depth < FS_MAX_NODE) {
        names[depth++] = fs.nodes[cwd].node_name;
        cwd = fs.nodes[cwd].parent_index;
    }

    if (depth == 0) {
        printString("/");
        return;
    }

    for (i = depth - 1; i >= 0; i--) {
        printString("/");
        printString(names[i]);
    }
    printString("/");
}

// TODO: 5. Implement parseCommand function
void parseCommand(char* buf, char* cmd, char arg[2][64]) {
  // this command will do string inside "" or '' the same, it will store the same
  int buf_i = 0;
  int copier_i = 0; // trachk the index of copy destination
  int part = 0;

  memset(cmd,(byte)'\0',64);
  memset(arg[0],(byte)'\0',64);
  memset(arg[1],(byte)'\0',64);

  while (buf[buf_i] != '\0') {
    if (33 <= (int)buf[buf_i] && (int)buf[buf_i] <= 127 ) {
      // non space
      if (part == 0) {
        cmd[copier_i] = buf[buf_i];
      } else if (part > 0) {
        arg[part-1][copier_i] = buf[buf_i]; 
      }
      copier_i++;
    } else if ((int)buf[buf_i] == (int)' ') {
      // printString("ada spasi\n");
      // space
      if (part == 0) {
        cmd[copier_i] ='\0';
      }
      copier_i = 0; // reset to make new
      part+=1;
    }
    buf_i++;
  }

}

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
void ls(byte cwd, char* dirname) {
  struct node_fs node_fs_buf;
  struct node_item* node;
  int i;
  byte dir_sekarang;

  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  //masukkan direktori sekarang (cwd) ke variabel dir_sekarang
  dir_sekarang = cwd;

  if (dirname != 0 && dirname[0] != '\0' && strcmp(dirname, ".") != true) {
    //Secara iteratif, cari kalok ada direktori yang namanya sesuai dirname (di argument/syntax ls)
    for (i = 0; i < FS_MAX_NODE; i++) {
      node = &node_fs_buf.nodes[i];
      if (node->parent_index == cwd && strncmp(node->node_name, dirname, MAX_FILENAME) == true) {
        dir_sekarang = i;
        break;
      }
    }

    //kalok udah sampe iterasi terakhir (FS_MAX_NODE, soalnya loopnya cuma sampe FS_MAX_NODE - 1) jadi ga ada yg sama
    if (i == FS_MAX_NODE) {
      printString("ls: direktoriny hoax, g ada bang\n");
      return;
    }
  }

  //secara iteratif (lagi) cari node yang parentnya itu "dir_sekarang" trus print isi file/direktori yg ada
  for (i = 0; i < FS_MAX_NODE; i++) {
    node = &node_fs_buf.nodes[i];

    if (node->parent_index == dir_sekarang && node->node_name[0] != '\0') {
      printString(node->node_name);
      if (node->data_index == FS_NODE_D_DIR) printString("/");
      printString("\n");
    }
  }
}

// TODO: 8. Implement mv function
void mv(byte cwd, char* src, char* dst) {}

// TODO: 9. Implement cp function
void cp(byte cwd, char* src, char* dst) {}

// TODO: 10. Implement cat function
void cat(byte cwd, char* filename) {}

// TODO: 11. Implement mkdir function
void mkdir(byte cwd, char* dirname) {}

