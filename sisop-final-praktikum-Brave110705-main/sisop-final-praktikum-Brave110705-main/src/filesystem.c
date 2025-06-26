#include "kernel.h"
#include "std_lib.h"
#include "filesystem.h"

void fsInit() {
  struct map_fs map_fs_buf;
  int i = 0;

  readSector(&map_fs_buf, FS_MAP_SECTOR_NUMBER);
  for (i = 0; i < 16; i++) map_fs_buf.is_used[i] = true;
  for (i = 256; i < 512; i++) map_fs_buf.is_used[i] = true;
  writeSector(&map_fs_buf, FS_MAP_SECTOR_NUMBER);
}

// TODO: 2. Implement fsRead function
void fsRead(struct file_metadata* metadata, enum fs_return* status) {
  struct node_fs node_fs_buf;
  struct data_fs data_fs_buf;
  

  int i, j; //buat loop
  int ada = 0; //sesuai nama, kalok ada = 1
  int node_index = -1; //buat nyari index, mulai dari -1 sesuai ketentuanny

  byte data_index;


  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);        
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER);   
  readSector(&data_fs_buf, FS_DATA_SECTOR_NUMBER);                   

  //sesuai soal, cari secara iteratif "mencari node yang memiliki nama yang sesuai dengan metadata->node_name dan parent index sesuai dengan metadata->parent_index"
  for (i = 0; i < FS_MAX_NODE; i++) {
    struct node_item* node = &node_fs_buf.nodes[i];
    if (node->parent_index == metadata->parent_index &&
        strncmp(node->node_name, metadata->node_name, MAX_FILENAME) == 0) {
      ada = 1;
      node_index = i;
      break;
    }
  }

  //sesuai soal, kalok ga nemu = set status dengan FS_R_NODE_NOT_FOUND
  if (!ada) {
    *status = FS_R_NODE_NOT_FOUND;
    return;
  }

  //sesuai soal, cek dulu direktori/file
  if (node_fs_buf.nodes[node_index].data_index == FS_NODE_D_DIR) {
    *status = FS_R_TYPE_IS_DIRECTORY;
    return;
  }

  // ngikut soal
  metadata->filesize = 0;
  data_index = node_fs_buf.nodes[node_index].data_index;

  //ngikut soal, secara iteratif pake readSector buat baca data dari sektor yang ditunjuk oleh data pada data index dengan sectors ke-i 
  for (i = 0; i < FS_MAX_SECTOR; i++) {
    byte sector_number = data_fs_buf.datas[data_index].sectors[i];
    if (sector_number == 0x00) break; //di-break sesuai soal

  //sesuai  soal, disimpan ke dalam metadata->buffer + i * SECTOR_SIZE
    readSector(metadata->buffer + i * SECTOR_SIZE, sector_number);
    metadata->filesize += SECTOR_SIZE;
  }

  *status = FS_SUCCESS;
}


// TODO: 3. Implement fsWrite function
void fsWrite(struct file_metadata* metadata, enum fs_return* status) {
  struct map_fs map_fs_buf;
  struct node_fs node_fs_buf;
  struct data_fs data_fs_buf;
}
