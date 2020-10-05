#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Format
   scroll status (8) | bitstream | parity bit (1)
   divider (2)       | bitstream | divider (1)
   current byte (8)  | bitstream | clock (16)
*/

#define WIDTH 21
#define HEIGHT 24

char frameBuffer[HEIGHT][WIDTH] = {{0}};
char bitstringTable[256][9] = {{0}};
int byteParityTable[256] = {0};

void initTables();
void writeFrame(char (*output)[HEIGHT][WIDTH], char *buffer, int tick, int totalLen);
void writeFile(char *fnameBase, char (*buffer)[HEIGHT][WIDTH], int tick);
void writeBuffer(char (*buffer)[16], char *message, int offset, int messageLen);

void initTables() {
  for(int i = 0; i < 256; i++) {
    for(int b = 0; b < 8; b++) {
      if((i >> b) & 1) {
        bitstringTable[i][7 - b] = '1';
        byteParityTable[i] ^= 1;
      } else {
        bitstringTable[i][7 - b] = '0';
      }
    }
  }
}

void writeFrame(char (*output)[HEIGHT][WIDTH], char *buffer, int tick, int totalLen) {
  /* blank out */
  for(int i = 0; i < HEIGHT; i++) {
    for(int j = 0; j < WIDTH; j++) {
      (*output)[i][j] = '0';
    }
  }

  /* tob and bottom border */
  for(int j = 4; j < WIDTH - 4; j++) {
    (*output)[2][j] = (*output)[HEIGHT - 3][j] = '1';
  }

  /* left and right border */
  for(int i = 2; i < HEIGHT - 2; i++) {
    (*output)[i][4] = (*output)[i][WIDTH - 5] = '1';
  }

  /* border separators */
  /* 11-12, 2-3 */
  for(int i = 11; i < 13; i++) {
    for(int j = 2; j < 4; j++) {
      (*output)[i][j] = '1';
    }
  }

  /* 4, 18-19 */
  for(int i = 4; i < 5; i++) {
    for(int j = 17; j < 19; j++) {
      (*output)[i][j] = '1';
    }
  }

  /* render bitstring */
  for(int i = 0; i < 16; i++) {
    char *bitstring = bitstringTable[buffer[i]];
    for(int j = 0; j < 4; j++) {
      (*output)[i + 4][j + 6] = bitstring[j];
      (*output)[i + 4][j + 11] = bitstring[j + 4];
    }
  }

  char frontier = buffer[15];
  char *frontierBitstring = bitstringTable[frontier];
  char progress = (8 * tick / totalLen);
  char progressBitstring[] = "11111111";
  progressBitstring[progress] = '0' + !(tick % 8);
  char tickBitstring[16];
  for(int i = 0; i < 8; i++) {
    tickBitstring[i] = bitstringTable[(tick >> 8) & 0xFF][i];
    tickBitstring[i + 8] = bitstringTable[tick & 0xFF][i];
  }

  /* render parity bit */
  (*output)[2][18] = '0' + byteParityTable[frontier];

  /* render frontier */
  for(int i = 0; i < 8; i++) {
    (*output)[i + 14][2] = frontierBitstring[i];
  }

  /* render progress */
  for(int i = 0; i < 8; i++) {
    (*output)[i + 2][2] = progressBitstring[i];
  }

  /* render tick bitstring */
  for(int i = 0; i < 16; i++) {
    (*output)[21 - i][18] = tickBitstring[15 - i];
  }
  /* invert every 8 cycles */
  /* if(!(tick % 8)) {
    for(int i = 0; i < 16; i++) {
      (*output)[21 - i][18] ^= 1;
    }
  } */
}

void writeFile(char *fnameBase, char (*buffer)[HEIGHT][WIDTH], int tick) {
  char fname[256];
  sprintf(fname, "%s_%06d.pbm", fnameBase, tick);
  FILE *file = fopen(fname, "w");

  /* Header */
  fprintf(file, "P1\n# Frame %d\n%d %d\n", tick, WIDTH, HEIGHT);

  /* body */
  for(int i = 0; i < HEIGHT; i++) {
    for(int j = 0; j < WIDTH; j++) {
      fprintf(file, "%c ", frameBuffer[i][j]);
    }
    fprintf(file, "\n");
  }

  fclose(file);
}

void writeBuffer(char (*buffer)[16], char *message, int offset, int messageLen) {
  for(int i = 0; i < 16; i++) {
    (*buffer)[i] = message[(i + offset) % messageLen];
  }
}

int main(int argc, char **argv) {
  char *message;
  char buffer[16];
  size_t allocLen = 0;
  size_t messageLen = 0;

  initTables();
  
  /* If no argv, prompt for input */
  if(argc < 2) {
    printf("Message: ");
    getline(&message, &allocLen, stdin);
    messageLen = strlen(message);
  } else {
    messageLen = strlen(argv[1]);
    if(messageLen > 4096) {
      messageLen = 4096;
    }
    message = (char *) malloc(messageLen);
    strncpy(message, argv[1], 4096);
  }

  /* for(int i = 0; i < messageLen; i++) {
    printf("%c\t%d\n", message[i], message[i]);
  } */

  for(int frame = 0; frame < messageLen; frame++) {
    writeBuffer(&buffer, message, frame, messageLen);
    writeFrame(&frameBuffer, buffer, frame, messageLen);
    writeFile("frame", &frameBuffer, frame);
  }

  if(message) {
    free(message);
  }

  return 0;
}
