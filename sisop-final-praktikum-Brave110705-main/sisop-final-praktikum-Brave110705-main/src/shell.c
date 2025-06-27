#include "shell.h"
#include "kernel.h"
#include "std_lib.h"
#include "filesystem.h"

void shell() {
  char cmd[64];
  char buf[64];
  char arg[2][64];

  byte cwd = FS_NODE_P_ROOT;

  printString("        ____ ___   \n");
  printString("  _____/ __ <  /  \n");
  printString(" / ___/ / / / /  \n");
  printString("/ /__/ /_/ / /   \n");
  printString("\\___/\\____/_/      \n");
  printString("\n");
  printString("Final Project SISOP 2025:\n\n");

  
  while (true) {


    printString("MengOS:");
    printCWD(cwd);
    printString("$ ");
    readString(buf);
    parseCommand(buf, cmd, arg);
    // printString(cmd);
    // printString("\n");

    // printString(arg[0]);
    // printString("\n");
    
    // printString(arg[1]);
    // printString("\n");

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
    struct node_fs node_fs_buf;
    char* names[FS_MAX_NODE];
    int depth = 0;
    int i;

  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);        
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

    while (cwd != FS_NODE_P_ROOT && depth < FS_MAX_NODE) {
        names[depth++] = node_fs_buf.nodes[cwd].node_name;
        cwd = node_fs_buf.nodes[cwd].parent_index;
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

    if (node->parent_index == *cwd && node->data_index == FS_NODE_D_DIR && strncmp(node->node_name, dirname,MAX_FILENAME) == true) {
      *cwd = i; 
      return;
    }
  }
  printString("cd: directory not found\n");
}


// // TODO: 7. Implement ls function
// void ls(byte * cwd, char* dirname) {
//   // kalau nutut mau tak tambahin warna weheheh
//   struct node_fs node_fs_buf;
//   struct node_item node_now;
//   int i;
//   readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);        
//   readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  

//   // if only "ls" without dirname
//   if (strcmp(dirname, "") == true) {
//       for (i = 0; i < FS_MAX_NODE;i++) {
//         node_now = node_fs_buf.nodes[i];

//         if (node_now.parent_index == cwd && !strcmp(node_now.node_name,"")) {
//           printString((node_now.data_index == FS_NODE_D_DIR) ? "d  " : "-  ");

//           printString(node_now.node_name);
//           printString("\n");

//         }
//       }
//     return;
//   }

//   if (strcmp(dirname, "/") == true) {
//       for (i = 0; i < FS_MAX_NODE;i++) {
//         node_now = node_fs_buf.nodes[i];

//         if (node_now.parent_index == 0xFF && !strcmp(node_now.node_name,"")) {
//           printString((node_now.data_index == FS_NODE_D_DIR) ? "d  " : "-  ");

//           printString(node_now.node_name);
//           printString("\n");

//         }
//       }
//     return;
//   }

//   //kalok syntax ".." dan bukan lagi di root = ke parent index
//   if (strcmp(dirname, "..") == true) {
//     if (cwd != FS_NODE_P_ROOT) {
//       for (i = 0; i < FS_MAX_NODE;i++) {
//         node_now = node_fs_buf.nodes[i];

//         if (node_now.parent_index == node_fs_buf.nodes[*cwd].parent_index && !strcmp(node_now.node_name,"")) {
//           printString((node_now.data_index == FS_NODE_D_DIR) ? "d  " : "-  ");

//           printString(node_now.node_name);
//           printString("\n");

//         }
//       }
//     }
//     return;
//   }


//   // if dirname is present
//   for (i = 0; i < FS_MAX_NODE; i++) {
//     struct node_item* node = &node_fs_buf.nodes[i];

//     if (strcmp(node->node_name,dirname)) {
//       for (i = 0; i < FS_MAX_NODE;i++) {

//         if (node_fs_buf.nodes[i].parent_index == node->parent_index) {
//           printString((node_now.data_index == FS_NODE_D_DIR) ? "d  " : "-  ");

//           printString(node_fs_buf.nodes[i].node_name);
//           printString("\n");
//         }
//       }
//       return;
//     }

    
//   }

//   return;
// }


// TODO: 7. Implement ls function
void ls(byte cwd, char* dirname) {
  struct node_fs node_fs_buf;
  struct node_item* node;
  struct node_item node_now;
  int i;
  byte dir_sekarang;

  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  
  // if only "ls" without dirname
  if (strcmp(dirname, "") == true) {
      for (i = 0; i < FS_MAX_NODE;i++) {
        node_now = node_fs_buf.nodes[i];

        if (node_now.parent_index == cwd && !strcmp(node_now.node_name,"")) {
          printString((node_now.data_index == FS_NODE_D_DIR) ? "d  " : "-  ");

          printString(node_now.node_name);
          if (node_now.data_index == FS_NODE_D_DIR) printString("/");

          printString("\n");

        }
      }
    return;
  }

  if (strcmp(dirname, "/") == true) {
      for (i = 0; i < FS_MAX_NODE;i++) {
        node_now = node_fs_buf.nodes[i];

        if (node_now.parent_index == 0xFF && !strcmp(node_now.node_name,"")) {
          printString((node_now.data_index == FS_NODE_D_DIR) ? "d  " : "-  ");

          printString(node_now.node_name);
          if (node_now.data_index == FS_NODE_D_DIR) printString("/");

          printString("\n");

        }
      }
    return;
  }

  //kalok syntax ".." dan bukan lagi di root = ke parent index
  if (strcmp(dirname, "..") == true) {
    if (cwd != FS_NODE_P_ROOT) {
      for (i = 0; i < FS_MAX_NODE;i++) {
        node_now = node_fs_buf.nodes[i];

        if (node_now.parent_index == node_fs_buf.nodes[cwd].parent_index && !strcmp(node_now.node_name,"")) {
          printString((node_now.data_index == FS_NODE_D_DIR) ? "d  " : "-  ");

          printString(node_now.node_name);
          if (node_now.data_index == FS_NODE_D_DIR) printString("/");
          printString("\n");

        }
      }
    }
    return;
  }

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
      printString((node_now.data_index == FS_NODE_D_DIR) ? "d  " : "-  ");
      printString(node->node_name);
      if (node->data_index == FS_NODE_D_DIR) printString("/");
      printString("\n");
    }
  }
}

// TODO: 8. Implement mv function
void mv(byte cwd, char* src, char* dst) {
  struct node_fs node_fs_buf;
  struct node_item now_node;
  char * dst_dir;
  char * dst_fname;
  int i;
  int dst_idx;
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);        
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  if (strcmp(dst,"")) {
    printString("mv: destination is not specified\n");
    return;
  } else if (strcmp(cwd,"")) {
    printString("mv: no source provided\n");
    return;
  }

  // finding dest
  if (strcmp(dst,"/")) {
    // printString("akaraka (noname)\n");
    dst_idx = FS_NODE_P_ROOT;
    dst_fname = src;

  } else if (strcmp(dst,"..")) {

    dst_idx = node_fs_buf.nodes[cwd].parent_index;
    dst_fname = src;

  } else if (strncmp(dst,"/",1)) {
    // /out_name
    // printString("akaraka\n");

    dst_idx = FS_NODE_P_ROOT;
    dst_fname = dst+1;

  } else if (strncmp(dst,"..",2)) {
    // ../out_name
    // printString(".........\n");
    dst_idx = node_fs_buf.nodes[cwd].parent_index;
    dst_fname = dst+3;
  } else {
    char * slash = strchchr(dst,'/');
    if (slash == 0) {
      // printString("same dir (rename)\n");
      dst_idx = cwd;
      dst_fname = dst;
    } else {
      // printString("a/b\n");
      // printString("\n");
      // printString(slash);

      // printString("\n");

      *slash = '\0';

      dst_dir = dst;
      dst_fname = slash+1;

      if (strcmp(dst_fname,"")) {
        dst_fname = src;
      }

      // printString("dest: \n");
      // printString(dst_dir);
      // printString("\n");
      // printString("fname: \n");
      // printString(dst_fname);
      // printString("\n");



      for (i = 0; i < 64; i++) {
        if (node_fs_buf.nodes[i].node_name != "") {
          if (strcmp(node_fs_buf.nodes[i].node_name, dst_dir)) {
            // printString("dir found \n\n");
            dst_idx = i;
            break;
          }
        }
      }

      printString("mv: target ");
      printString(dst_dir);
      printString(" not found\n");
      return;
    }
  }

  for (i = 0; i < 64; i++) {
    now_node = node_fs_buf.nodes[i];
    if (strcmp(now_node.node_name,src)) {

      strcpy(node_fs_buf.nodes[i].node_name, dst_fname);
      node_fs_buf.nodes[i].parent_index = dst_idx;

      writeSector(&(node_fs_buf.nodes[0]),FS_NODE_SECTOR_NUMBER);
      writeSector(&(node_fs_buf.nodes[32]),FS_NODE_SECTOR_NUMBER+1);

      return;
    }
  }

  printString("mv: ");
  printString(src);

  printString(" not found\n");

}
 
// TODO: 9. Implement cp function
void cp(byte cwd, char* src, char* dst) {
    struct map_fs map_fs_buf;
    struct node_fs node_fs_buf;
    struct data_fs data_fs_buf;
    struct node_item now_node;
    struct file_metadata src_md;
    int i;
    int j;
    byte dst_idx;
    char *dst_fname, *dst_dir;
    int empty_node, empty_data;
    int sectors, written;
    enum fs_return status;

    readSector(&map_fs_buf, FS_MAP_SECTOR_NUMBER);
    readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
    readSector(&data_fs_buf, FS_DATA_SECTOR_NUMBER);

    // untuk cari src
    for (i = 0; i < FS_MAX_NODE; i++) {
        if (strcmp(node_fs_buf.nodes[i].node_name, src) &&
            node_fs_buf.nodes[i].parent_index == cwd) {
            now_node = node_fs_buf.nodes[i];
            break;
        }
    }
    if (i == FS_MAX_NODE) {
        printString("cp: source not found\n");
        return;
    }
    if (now_node.data_index == FS_NODE_D_DIR) {
        printString("cp: cannot copy a directory\n");
        return;
    }

    src_md.parent_index = cwd;
    strcpy(src_md.node_name, src);
    fsRead(&src_md, &status);
    if (status != FS_SUCCESS) {
        printString("cp: read error\n");
        return;
    }

    dst_idx = cwd; 
    dst_fname = dst;

    if (dst[0] == '/' && dst[1] != '\0') {
        dst_idx   = FS_NODE_P_ROOT;
        dst_fname = dst + 1;
    }
    else if (dst[0] == '.' && dst[1] == '.' && dst[2] == '/') {
        dst_idx   = node_fs_buf.nodes[cwd].parent_index;
        dst_fname = dst + 3;
    }

    else {
        for (i = 0; dst[i] != '\0'; i++) {
            if (dst[i] == '/') {
                dst[i]   = '\0';  
                dst_dir   = dst;
                dst_fname = dst + i + 1;
                for (j = 0; j < FS_MAX_NODE; j++) {
                    if (strcmp(node_fs_buf.nodes[j].node_name, dst_dir) &&
                        node_fs_buf.nodes[j].parent_index == cwd &&
                        node_fs_buf.nodes[j].data_index == FS_NODE_D_DIR) {
                        dst_idx = j;
                        break;
                    }
                }
                if (dst_idx == cwd) {
                    printString("cp: target dir not found\n");
                    return;
                }
                break;
            }
        }
    }

    for (empty_node = 0; empty_node < FS_MAX_NODE; empty_node++) {
        if (node_fs_buf.nodes[empty_node].node_name[0] == '\0') break;
    }
    if (empty_node == FS_MAX_NODE) {
        printString("cp: no free node\n");
        return;
    }
    for (empty_data = 0; empty_data < FS_MAX_DATA; empty_data++) {
        if (data_fs_buf.datas[empty_data].sectors[0] == 0x00) break;
    }
    if (empty_data == FS_MAX_DATA) {
        printString("cp: no free data\n");
        return;
    }

    strcpy(node_fs_buf.nodes[empty_node].node_name, dst_fname);
    node_fs_buf.nodes[empty_node].parent_index = dst_idx;
    node_fs_buf.nodes[empty_node].data_index = (byte)empty_data;

    sectors = (src_md.filesize + SECTOR_SIZE - 1) / SECTOR_SIZE;
    written = 0;
    for (i = 16; i < SECTOR_SIZE && written < sectors; i++) {
        if (!map_fs_buf.is_used[i]) {
            map_fs_buf.is_used[i] = true;
            data_fs_buf.datas[empty_data].sectors[written] = (byte)i;
            writeSector(src_md.buffer + written * SECTOR_SIZE, i);
            written++;
        }
    }

    writeSector(&map_fs_buf,             FS_MAP_SECTOR_NUMBER);
    writeSector(node_fs_buf.nodes,       FS_NODE_SECTOR_NUMBER);
    writeSector(node_fs_buf.nodes + 32,  FS_NODE_SECTOR_NUMBER + 1);
    writeSector(&data_fs_buf,            FS_DATA_SECTOR_NUMBER);

  printString("cp: ");
  printString(src);
  printString("\n");
}


// TODO: 10. Implement cat functionW
void cat(byte cwd, char* filename) {

  // file found
  struct file_metadata fmd;
  enum fs_return retVal;      
  int it;

  strcpy(fmd.node_name,filename);
  fmd.parent_index = cwd;

  fsRead(&fmd,&retVal);

  if (retVal == FS_SUCCESS) {
    // printString("this is the file you wanted\n\n");
    printString(fmd.buffer);
    printString("\n");
    return;
  } else if (retVal == FS_R_TYPE_IS_DIRECTORY) {
    printString("cat: ");
    printString(filename);
    printString(" is a directory\n");
  } else {
    printString("cat: file not found\n");
  }




  // struct node_fs node_fs_buf;
  // struct node_item node_now;
  // int i;
  // readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);        
  // readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  




  // for (i = 0; i< 64; i++) {
  //   node_now = node_fs_buf.nodes[i];
  //   if (strcmp(node_now.node_name, filename) && node_now.data_index != 0xFF && node_now.parent_index == cwd) {
  //     // file found
  //     struct file_metadata fmd;
  //     enum fs_return retVal;      
  //     int it;

  //     strcpy(fmd.node_name,filename);
  //     fmd.parent_index = cwd;

  //     fsRead(&fmd,&retVal);

  //     printString("this is the file you wanted\n\n");
  //     printString(fmd.buffer);
  //     return;
  //   }

  // }

}

// TODO: 11. Implement mkdir function
void mkdir(byte cwd, char* dirname) {

  struct node_fs node_fs_buf;
  struct node_item new_node;
  int i;
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);        
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

  strcpy(new_node.node_name,dirname);
  new_node.parent_index = cwd;
  new_node.data_index = FS_NODE_D_DIR;
  for (i = 0; i < 64; i++) {
    if (strcmp(node_fs_buf.nodes[i].node_name,"")) {
      node_fs_buf.nodes[i] = new_node;
      writeSector(&(node_fs_buf.nodes[0]),FS_NODE_SECTOR_NUMBER);
      writeSector(&(node_fs_buf.nodes[32]),FS_NODE_SECTOR_NUMBER+1);

      break;
    }
  }

}



// void eexit() {
//   printString("credit:");
//   sleep(2);
//   printString("5025241xxx: Brave Juliada\n\n");
//   sleep(2);
//   printString("5025241xxx: Ananda Aryasatya Z A\n\n");
//   sleep(2);
//   printString("5025241xxx: Hosea Felix S\n\n");



  
  
// }

/* 0x100. Node akan disimpan sebanyak 2 sektor pada sektor 0x101 dan 0x102. 
Data akan disimpan sebanyak 1 sektor pada sektor 0x103.*/
