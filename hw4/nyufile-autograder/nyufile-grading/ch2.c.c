void setFAT(void *fs, BootEntry *boot, DirEntry *dirEntry) {
  int fatOffset = boot->BPB_RsvdSecCnt * boot->BPB_BytsPerSec;
  unsigned int *fat = (unsigned int *)((char *)fs + fatOffset);

  unsigned int offsetToBlock = (dirEntry->DIR_FstClusHI << 16) | dirEntry->DIR_FstClusLO;

  int leftFileSize = dirEntry->DIR_FileSize;
  while (leftFileSize > 0){
    fat[offsetToBlock] = (leftFileSize < boot -> BPB_BytsPerSec) 
          ? EOF: offsetToBlock + 1;
    offsetToBlock++;
    leftFileSize -= boot -> BPB_BytsPerSec;
  }
}