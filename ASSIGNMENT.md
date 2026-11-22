# Task 3: Page tables

This branch implements all five exercises in the
[2026 MIT page-table lab](https://pdos.csail.mit.edu/6.828/2026/labs/pgtbl.html):
inspection, recursive printing, access-bit detection, shared-page PID lookup,
and kernel superpages. It starts from MIT's `pgtbl` starter commit
`d1eb62f6339ae811083a0c2f24fc496f8390fd9d` and is independent of `task-4`.

The written answers are in [answers-pgtbl.txt](answers-pgtbl.txt). They explain
every printed user mapping and its permissions, the shared-page alternatives,
and the measured 95 superpages saving 380 KiB of page-table memory.

## Build and test in WSL

Use `~/xv6-task-3` as the checkout directory in Ubuntu.
Keep the build on Linux's filesystem in a path without spaces.

```sh
cd ~/xv6-task-3
make grade
python3 grade-lab-extra
make qemu
```

Inside xv6, run `pgtbltest`, `pgtblcheck`, or `usertests -q`. Exit QEMU
with Ctrl-A followed by X. On a fresh machine, clone this branch first:

```sh
git clone --branch task-3 https://github.com/iHuzaifakhan/xv6-repository.git xv6-task-3
cd xv6-task-3
make grade
```

## Implementation details

- `vmprint` prints virtual addresses, complete PTEs, physical addresses, and
  leaf permissions; it identifies superpages without descending into them.
- `pgaccess` supports 1 to 4096 pages with a byte-packed bitmask. It checks
  overflow, unmapped/supervisor input, read-only output, and output page
  crossings. Errors leave the bitmask and access bits intact. Successful
  scans clear access bits and invalidate translations.
- Each process owns a zero-initialized PID page, mapped user-readable with
  no write/execute permission. Allocation failures, fork, exec, and process
  cleanup preserve ownership and release the page correctly.
- The kernel mapper uses 2 MiB pages only for complete aligned regions.
  Ordinary user mappings and kernel text/data permission boundaries remain
  4 KiB mappings.
- ELF load headers separate executable kernel text from writable data.
  The small `forktest` binary uses the standard user linker layout. These
  avoid the starter's RWX-segment linker warnings with current binutils.

## Validation

Tests use Ubuntu WSL, GCC 15.2.0, QEMU 10.2.1, the default three virtual CPUs,
and 128 MiB RAM. MIT's grading script and `user/pgtbltest.c` are unchanged.
`validation/grade-summary.txt` records the official grading results, and
`validation/extra-checks.txt` records the additional runtime checks.

`pgtblcheck` checks large/multi-byte bitmasks, clearing, partial final bytes,
invalid counts and ranges, page-crossing output, read-only/supervisor pages,
preservation of bits on error, and PID-page write protection. Its child
deliberately writes USYSCALL and is killed: the resulting user fault in the
log is the expected write-protection result. It also reports kernel mapping
counts. The extra grader runs the original `forktest` too.

`time.txt` records one hour, rounding up the automated implementation and
validation time for this lab; it is not a claim about prior student effort.
