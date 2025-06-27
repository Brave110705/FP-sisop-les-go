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

  int i, j;
  int node_kosong;
  int data_kosong;
  int blok_kosong = 0;

  //1. Membaca filesystem dari disk ke memory.
  readSector(&map_fs_buf, FS_MAP_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  readSector(&data_fs_buf, FS_DATA_SECTOR_NUMBER);

  //2. Lakukan iterasi setiap item node untuk mencari node yang memiliki nama yang sama dengan 
  // metadata->node_name dan parent index yang sama dengan metadata->parent_index. Jika node 
  // yang dicari ditemukan, maka set status dengan FS_R_NODE_ALREADY_EXISTS dan keluar.
  for (i = 0; i < FS_MAX_NODE; i++) {
    if (node_fs_buf.nodes[i].parent_index == metadata->parent_index && strncmp(node_fs_buf.nodes[i].node_name, metadata->node_name, MAX_FILENAME) == 0) {
      *status = FS_W_NODE_ALREADY_EXISTS;
      return;
    }
  }               

  //3. Selanjutnya, cari node yang kosong (nama node adalah string kosong) dan simpan index-nya. 
  // Jika node yang kosong tidak ditemukan, maka set status dengan FS_W_NO_FREE_NODE dan keluar.
  for (node_kosong = 0; node_kosong < FS_MAX_NODE; node_kosong++) {
    if (node_fs_buf.nodes[node_kosong].node_name[0] == '\0') {
      // ketemu node kosong, langsung keluar loop
      break;
    }
  }
  // kalau sampai akhir loop tanpa break, brarti ga ada slot kosong
  if (node_kosong == FS_MAX_NODE) {
    *status = FS_W_NO_FREE_NODE;
    return;
  }


  //4. Iterasi setiap item data untuk mencari data yang kosong (alamat sektor data ke-0 adalah 0x00) 
  // dan simpan index-nya. Jika data yang kosong tidak ditemukan, maka set status dengan FS_W_NO_FREE_DATA 
  // dan keluar.
  for (data_kosong = 0; data_kosong < FS_MAX_DATA; data_kosong++) {
    if (data_fs_buf.datas[data_kosong].sectors[0] == 0x00) {
      break;
    }
  }

  if (data_kosong == FS_MAX_DATA) {
    *status = FS_W_NO_FREE_DATA;
    return;
  }

  //5. Iterasi setiap item map dan hitung blok yang kosong (status blok adalah 0x00 atau false). Jika blok
  // yang kosong kurang dari metadata->filesize / SECTOR_SIZE, maka set status dengan FS_W_NOT_ENOUGH_SPACE 
  // dan keluar.
  for (i = 16; i < 256; i++) {
    if (!map_fs_buf.is_used[i]) {
      blok_kosong++;
    }
  }

  if (blok_kosong < (metadata->filesize + SECTOR_SIZE - 1) / SECTOR_SIZE) {
    *status = FS_W_NOT_ENOUGH_SPACE;
    return;
  }

  //6. Set nama dari node yang ditemukan dengan metadata->node_name, parent index dengan metadata->parent_index,
  // dan data index dengan index data yang kosong.
  strcpy(node_fs_buf.nodes[node_kosong].node_name, metadata->node_name);
  node_fs_buf.nodes[node_kosong].parent_index = metadata->parent_index;

  if (metadata->filesize == 0)
    node_fs_buf.nodes[node_kosong].data_index = FS_NODE_D_DIR;
  else {
    node_fs_buf.nodes[node_kosong].data_index = data_kosong;

    //7. Lakukan penulisan data dengan cara sebagai berikut.
    j = 0;
    for (i = 0; i < SECTOR_SIZE && j < (metadata->filesize + SECTOR_SIZE - 1) / SECTOR_SIZE; i++) {
        if (!map_fs_buf.is_used[i]) {
            map_fs_buf.is_used[i] = true;
            data_fs_buf.datas[data_kosong].sectors[j] = (byte)i;
            writeSector(
                metadata->buffer + (j * SECTOR_SIZE),i
            );
            j++;
        }
    }
  }

  //8. Tulis kembali filesystem yang telah diubah ke dalam disk.
  writeSector(&map_fs_buf, FS_MAP_SECTOR_NUMBER);
  writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
  writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
  writeSector(&data_fs_buf, FS_DATA_SECTOR_NUMBER);

  //9. Set status dengan FS_W_SUCCESS.
  *status = FS_W_SUCCESS;
}
