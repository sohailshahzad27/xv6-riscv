# Assignment 2

The assignment-2 branch contains both the utilities and system-call labs.
The default lab in conf/lab.mk is syscall.

Run these commands in the WSL checkout at
/home/qasim/os-assignment2/xv6-labs-2026 (or another path without spaces):

```sh
make grade
make LAB=util grade
make clean
make qemu
```

For individual system-call tests:

```sh
make GRADEFLAGS=sandbox grade
make GRADEFLAGS=attack grade
```

The GDB answers are in answers-syscall.txt. The intentionally vulnerable
allocator is the syscall lab starter implementation, enabled only when
LAB_SYSCALL is defined. secret.c is unchanged from that starter branch.

Reported time: utilities 10 hours, system calls 11 hours. Separate records
are in time-util.txt and time-syscall.txt; time.txt contains the active
system-call lab estimate.
