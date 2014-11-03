#include <stdio.h>
#include "qgram.h"

int main(void) {
   FILE *f = fopen("test.txt", "r");
   if (f == NULL) {
      fprintf(stderr, "file not found\n");
      abort();
   }
   scan(f, 10, 75, 3);
}
