#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GPT.h"

int main(int argc, char *argv[]) 
{

  if(argc < 2) {
    /* Display all usage here eventually */
    printf("%s\n", "Usage: blah blah blah");
    return 1;
  }

  if(strcmp(argv[1], "--print-header") == 0) {

    if(argc != 3) {
      printf("%s %s %s %s\n", "Usage:", argv[0], 
          "--print-header", "<GPT_HEADER>");
      return 1;
    }

    FILE *inputFile = NULL;
    GPTHeader header;

    inputFile = fopen(argv[2], "rb");

    if(inputFile == NULL) {
      printf("%s %s\n", "Unable to open input file:", argv[2]);
      return 1;
    }

    header = readGPTHeader(inputFile);
    printGPTHeader(header, stdout);
  }

  if(strcmp(argv[1], "--print-table") == 0) {

    if(argc != 4) {
      printf("%s %s %s %s %s\n", "Usage:", argv[0],
          "--print-table","<GPT_HEADER>", "<GPT_TABLE>");
      return 1;
    }

    FILE *inputFileHeader = NULL;
    FILE *inputFileTable = NULL;
    GPTHeader header;
    GPTPartitionEntry *partitions;
    GPTPartitionEntry partition;

    inputFileHeader = fopen(argv[2], "rb");
    inputFileTable = fopen(argv[3], "rb");

    if(inputFileHeader == NULL) {
      printf("%s %s\n", "Unable to open input file:", argv[2]);
      return 1;
    }

    if(inputFileTable == NULL) {
      printf("%s %s\n", "Unable to open input file:", argv[3]);
      return 1;
    }

    header = readGPTHeader(inputFileHeader);
    partitions = calloc(header.pent_num, sizeof(GPTPartitionEntry));
    readGPTPartitionArray(inputFileTable, partitions, header.pent_num);

    for(int i = 0; i < header.pent_num; i++) {
      partition = convertPartitionNameToUTF8(partitions[i]);
      printGPTPartitionEntry(partition, stdout);
    }

    free(partitions);
  }

  if(strcmp(argv[1], "--print-all") == 0) {

    if(argc != 4) {
      printf("%s %s %s %s %s\n", "Usage:", argv[0],
          "--print-all","<GPT_HEADER>", "<GPT_TABLE>");
      return 1;
    }

    FILE *inputFileHeader = NULL;
    FILE *inputFileTable = NULL;
    GPTHeader header;
    GPTPartitionEntry *partitions;
    GPTPartitionEntry partition;

    inputFileHeader = fopen(argv[2], "rb");
    inputFileTable = fopen(argv[3], "rb");

    if(inputFileHeader == NULL) {
      printf("%s %s\n", "Unable to open input file:", argv[2]);
      return 1;
    }

    if(inputFileTable == NULL) {
      printf("%s %s\n", "Unable to open input file:", argv[3]);
      return 1;
    }

    header = readGPTHeader(inputFileHeader);
    partitions = calloc(header.pent_num, sizeof(GPTPartitionEntry));
    readGPTPartitionArray(inputFileTable, partitions, header.pent_num);
    printGPTHeader(header, stdout);
    printf("\n");

    for(int i = 0; i < header.pent_num; i++) {
      partition = convertPartitionNameToUTF8(partitions[i]);
      printGPTPartitionEntry(partition, stdout);
      printf("\n");
    }

    free(partitions);
  }

  return 0;
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

  fprintf(f, "\n%-30s%" PRIu32, "Reserved", h.reserved);

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
  printf("\n");

  /* DO STUFF WITH TRAILING BYTES IF THIS WERKS */
}

GPTPartitionEntry convertPartitionNameToUTF8(GPTPartitionEntry pe)
{
  /* UTF-16 to UTF-8 */
  int i=1, j=2;

  for(; i < 32; i++, j += 2) {
    pe.part_name[i] = pe.part_name[j];
  }

  pe.part_name[32] = '\0';

  return pe;
}

void readGPTPartitionArray(FILE* f, GPTPartitionEntry* out, int num)
{
  fread(out, sizeof(GPTPartitionEntry), num, f);
}

void printGPTPartitionEntry(const GPTPartitionEntry pe, FILE* f)
{
  fprintf(f, "PARTITION ENTRY\n---------------\n  %-26s",
      "Partition Type GUID");

  printHex(f, pe.ptype_guid, 16);

  fprintf(f, "\n  %-26s", "Unique Partition GUID");

  printHex(f, pe.upart_guid, 16);

  fprintf(f, "\n  %-26s%" PRIu64 "\n"
      "  %-26s%" PRIu64 "\n"
      "  %-26s%" PRIu64 "\n"
      "  %-26s%s\n", "First LBA", pe.first_lba, "Last LBA", pe.last_lba,
      "Attributes", pe.attr_flags, "Partition Name", pe.part_name);
}

/*PRINT HEX STRING FROM DATA*/
void printHex(FILE* f, const void* data, const unsigned int len)
{
  int i = 1;
  fprintf(f, "%02X", ((unsigned char*) data)[0]);

  for(; i < len; i++)
    fprintf(f, " %02X", ((unsigned char*) data)[i]);
}
