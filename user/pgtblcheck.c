#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/riscv.h"
#include "kernel/memlayout.h"
#include "user/user.h"

static void
check(int ok, char *why)
{
  if (!ok) {
    fprintf(2, "pgtblcheck: %s\n", why);
    exit(1);
  }
}

int
main(void)
{
  enum { NPAGES = 4096, ALLOC = (NPAGES + 2) * PGSIZE };
  unsigned char bits[NPAGES / 8 + 2];
  volatile char *buf = sbrk(ALLOC);
  check((char *)buf != SBRK_ERROR, "allocate test pages");
  char *end = sbrk(0);

  check(pgaccess((void *)buf, NPAGES, bits) == 0, "maximum page count");
  int touched[] = {0, 7, 8, 255, NPAGES - 1};
  for (int i = 0; i < sizeof(touched) / sizeof(touched[0]); i++)
    buf[touched[i] * PGSIZE] += 1;
  check(pgaccess((void *)buf, NPAGES, bits) == 0, "read large bitmask");
  for (int i = 0; i < NPAGES; i++) {
    int expected = i == 0 || i == 7 || i == 8 || i == 255 || i == NPAGES - 1;
    check(((bits[i / 8] >> (i % 8)) & 1) == expected, "large bitmask contents");
  }
  check(pgaccess((void *)buf, NPAGES, bits) == 0, "second access scan");
  for (int i = 0; i < NPAGES / 8; i++)
    check(bits[i] == 0, "access bits were not cleared");

  memset(bits, 0xa5, sizeof(bits));
  buf[0] += 1;
  buf[8 * PGSIZE] += 1;
  check(pgaccess((char *)buf + 3, 9, bits + 1) == 0,
        "unaligned start and partial final byte");
  check(bits[0] == 0xa5 && bits[1] == 1 && bits[2] == 1 && bits[3] == 0xa5,
        "bitmask boundaries or unused high bits");

  check(pgaccess((void *)buf, 0, bits) == -1, "reject zero count");
  check(pgaccess((void *)buf, -1, bits) == -1, "reject negative count");
  check(pgaccess((void *)buf, NPAGES + 1, bits) == -1, "reject excessive count");
  check(pgaccess(end, 1, bits) == -1, "reject unmapped input");
  check(pgaccess((void *)MAXVA, 1, bits) == -1, "reject non-user address");
  check(pgaccess((void *)(MAXVA - PGSIZE), 2, bits) == -1,
        "reject input range overflow");
  check(pgaccess((void *)TRAPFRAME, 1, bits) == -1,
        "reject supervisor-only input");

  check(pgaccess((void *)buf, 32, bits) == 0, "clear before failure tests");
  buf[7 * PGSIZE] += 1;
  check(pgaccess((void *)buf, 32, (void *)USYSCALL) == -1,
        "reject read-only output");
  check(pgaccess((void *)buf, 32, (void *)TRAPFRAME) == -1,
        "reject supervisor-only output");
  check(pgaccess((void *)buf, 32, (void *)MAXVA) == -1,
        "reject out-of-range output");
  check(pgaccess((void *)buf, 32, (void *)(MAXVA - 1)) == -1,
        "reject output range overflow");
  end[-2] = 0x5a;
  end[-1] = 0x6b;
  check(pgaccess((void *)buf, 32, end - 2) == -1,
        "reject output crossing into an unmapped page");
  check(end[-2] == 0x5a && end[-1] == 0x6b, "failed copy partially wrote output");
  check(pgaccess((void *)buf, 32, bits) == 0 && bits[0] == (1 << 7),
        "failed calls consumed access bits");

  check(ugetpid() == getpid(), "shared PID mismatch");
  int pid = fork();
  check(pid >= 0, "fork for write-protection test");
  if (pid == 0) {
    *(volatile int *)USYSCALL = 12345;
    exit(1);
  }
  int status;
  check(wait(&status) == pid && status == -1, "shared page allowed a user write");
  check(ugetpid() == getpid(), "child changed parent's PID page");

  uint64 super, regular;
  check(ksupernpte(&super, &regular) == 0, "read kernel mapping counts");
  printf("kernel mappings: %lu superpage PTEs, %lu ordinary leaf PTEs\n",
         super, regular);
  check(sbrk(-ALLOC) == end, "release test memory");
  printf("pgtblcheck: all checks passed\n");
  exit(0);
}
