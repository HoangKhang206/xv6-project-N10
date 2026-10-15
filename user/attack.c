#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define DATASIZE (64 * 4096)
#define MSG "Here it is: "
#define SECRET_LEN 8

int
main(int argc, char *argv[])
{
  char *data = sbrk(DATASIZE);
  uint msglen = strlen(MSG);

  if (data == SBRK_ERROR)
    exit(1);

  for (uint i = 0; i + msglen + SECRET_LEN < DATASIZE; i++) {
    if (memcmp(data + i, MSG, msglen) == 0) {
      char secret[SECRET_LEN + 1];
      memmove(secret, data + i + msglen, SECRET_LEN);
      secret[SECRET_LEN] = 0;
      printf("%s\n", secret);
      exit(0);
    }
  }

  exit(1);
}
