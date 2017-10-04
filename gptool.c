#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GPT.h"

int main(int argc, char *argv[]) 
{
  FILE *inputFile = NULL;
  GPTHeader header;

  if(argc != 2) {
    printf("%s %s %s", "Usage:", argv[0], "<binary>");
    return 1;
  }

  inputFile = fopen(argv[1], "rb");

  if(inputFile == NULL) {
    printf("%s %s", "Unable to open input file:", argv[1]);
    return 1;
  }

  header = readGPTHeader(inputFile);

  printGPTHeader(header, stdout);
}

GPTHeader readGPTHeader(FILE* f)
{
  /* READ GPT HEADER INTO STRUCT */
  GPTHeader out;
  fread(&out, sizeof(GPTHeader), 1, f);
  return out;
}

void printGPTHeader(const GPTHeader h, FILE* f)
{
  fprintf(f, "GPT HEADER\n----------\n%-30s", "Signature");

  printHex(f, h.signature, 8);

  fprintf(f, "\n%-30s%" PRIu32 "\n"
      "%-30s%" PRIu32 "\n"
      "%-30s", "Revision", h.revision, "Header Size", h.hsize, 
      "CRC32 of Header");

  printHex(f, &h.crc, sizeof(uint32_t));

  /* We never inspect h.reserved */

  fprintf(f, "\n%-30s%" PRIu64 "\n"
      "%-30s%" PRIu64 "\n"
      "%-30s%" PRIu64 "\n"
      "%-30s%" PRIu64 "\n"
      "%-30s", "Current Header LBA", h.current_lba, "Backup Header LBA", 
      h.backup_lba, "First Usable LBA", h.first_lba, "Last Usable LBA",
      h.last_lba, "Disk GUID");

  printHex(f, h.disk_guid, 16);

  fprintf(f, "\n%-30s%" PRIu64 "\n"
      "%-30s%" PRIu32 "\n"
      "%-30s%" PRIu32 "\n"
      "%-30s", "Start of Partition Entries", h.pent_lba, 
      "Number of Partition Entries", h.pent_num,
      "Size of Partition Entries", h.pent_size,
      "CRC32 of Partition Array");

  printHex(f, &h.crc_part, sizeof(uint32_t));

  /* DO STUFF WITH TRAILING BYTES IF THIS WERKS */
}



/*PRINT HEX STRING FROM DATA*/
void printHex(FILE* f, const void* data, const unsigned int len)
{
  int i = 1;
  fprintf(f, "%02X", ((unsigned char*) data)[0]);

  for(; i < len; i++)
    fprintf(f, " %02X", ((unsigned char*) data)[i]);
}















