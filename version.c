#include "uv.h"
#include <stdio.h>
#include <stdlib.h>

void version_print() {
  unsigned int version = uv_version();
  printf("version patch: %u\n", version & 0xFF);
  printf("version minor: %u\n", (version >> 8) & 0xFF);
  printf("version major: %u\n", (version >> 16) & 0xFF);
  printf("version string: %s\n", uv_version_string());
}

int main() {
  version_print();

  return EXIT_SUCCESS;
}
