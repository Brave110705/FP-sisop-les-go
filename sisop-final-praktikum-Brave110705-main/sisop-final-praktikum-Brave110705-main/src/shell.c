#include "shell.h"
#include "kernel.h"
#include "std_lib.h"
#include "filesystem.h"

void shell() {
  char cmd[64];
  char buf[64];
  char arg[2][64];

  byte cwd = FS_NODE_P_ROOT;

  
  while (true) {
    printString("MengOS:");
    printCWD(cwd);
    printString("$ ");
    readString(buf);
    parseCommand(buf, cmd, arg);
    printString(cmd);
    printString("\n");

    printString(arg[0]);
    printString("\n");
    
    printString(arg[1]);
    printString("\n");

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
void parseCommand(char buf[], char* cmd, char arg[2][64]) {
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
void ls(byte cwd, char* dirname) {}

// TODO: 8. Implement mv function
void mv(byte cwd, char* src, char* dst) {}

// TODO: 9. Implement cp function
void cp(byte cwd, char* src, char* dst) {}

// TODO: 10. Implement cat function
void cat(byte cwd, char* filename) {
  // byte RecLine = 0x00;
  // byte nss; // Node Sector Start
  // byte Sector[SECTOR_SIZE];
  

  // int i;
  // byte i_addr;

  // nss = 0x101*0x200;
  // i_addr = nss;
  // // go trough entire node
  // for (i = 0; i<64;i++) {
  //   printString(i_addr);
  //   printString("\n\n");

  //   printString(i_addr+2);
  //   printString("\n\n");
  //   i_addr+=16;
  // }

  struct file_metadata all_files[FS_MAX_NODE];
  enum fs_return retVal;
  int i = 0;


  fsRead(all_files,retVal);

  for (i = 0; i < FS_MAX_NODE;i++) {
    printString(all_files[i].node_name);
    printString('\n');
  }
  printString("hakioo\n");
  // readSector(Sector,nss);
  // printString(Sector);
}

// TODO: 11. Implement mkdir function
void mkdir(byte cwd, char* dirname) {

}

/* 0x100. Node akan disimpan sebanyak 2 sektor pada sektor 0x101 dan 0x102. 
Data akan disimpan sebanyak 1 sektor pada sektor 0x103.*/