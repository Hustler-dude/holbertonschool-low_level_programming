# Secure Input & Memory Lab (Student Starter)

This repository contains intentionally vulnerable C code for analysis and patching.

You will find the source files under `src/`.

---

## Task 1 – Memory Error Detection (Valgrind Analysis)

### Compilation

```bash
gcc -std=gnu89 -Wall -Wextra -Werror -pedantic -g -o lab main.c user_input.c session.c
```

### Valgrind Command

```bash
valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes ./lab
```

---

### Bug 1 — Stack Buffer Overflow (`user_input.c:11`)

**Location:** `read_username()` in `user_input.c`

```c
char buffer[32];
scanf("%s", buffer);  /* no bounds check */
```

**Category:** Invalid write (stack buffer overflow)
**Input-dependent:** YES — triggered only when input > 31 characters

**Valgrind output (boundary input — 42 'A' chars):**
```
==PID== Invalid write of size 1
==PID==    at 0x... (scanf internals)
==PID==    by 0x... read_username (user_input.c:11)
==PID==    by 0x... main (main.c:12)
==PID==  Address 0x... is 0 bytes after a block of size 32 alloc'd on the stack
```

---

### Bug 2 — Use-After-Free + Double Free (`main.c` + `session.c`)

**Root cause:** `session_create()` stores the pointer directly without copying.

```c
session->user = username;   /* session.c:13 — same pointer, no copy */
```

**Execution trace:**

| Step | Location | Action |
|------|----------|--------|
| 1 | `main.c:20` | `session_create(username)` stores pointer directly |
| 2 | `session.c:33` | `free(session->user)` — frees `username` (1st free) |
| 3 | `main.c:31` | `printf("Goodbye %s\n", username)` — USE-AFTER-FREE |
| 4 | `main.c:33` | `free(username)` — DOUBLE FREE |

**Category:** Use-after-free (UAF) + Invalid free
**Input-dependent:** NO — triggered on every normal execution

**Valgrind output (normal input):**
```
==PID== Invalid read of size 1
==PID==    at 0x... printf
==PID==    by 0x... main (main.c:31)
==PID==  Address 0x... is 0 bytes inside a block of size 6 free'd
==PID==    at 0x... free
==PID==    by 0x... session_destroy (session.c:33)
==PID==    by 0x... main (main.c:29)

==PID== Invalid free() / delete / delete[] / realloc()
==PID==    at 0x... free
==PID==    by 0x... main (main.c:33)
==PID==  Address 0x... is 0 bytes inside a block of size 6 free'd
==PID==    at 0x... free
==PID==    by 0x... session_destroy (session.c:33)
```

---

### Memory Leak Summary

```
==PID== LEAK SUMMARY:
==PID==    definitely lost: 0 bytes in 0 blocks
==PID==    still reachable: 0 bytes in 0 blocks
```

No leak — the problem is double-free, not a leak.

---

### Summary Table

| # | Category | File | Line | Input-dependent |
|---|----------|------|------|----------------|
| 1 | Stack buffer overflow | `user_input.c` | 11 | YES (> 31 chars) |
| 2 | Use-after-free | `main.c` | 31 | NO |
| 3 | Double free | `main.c` | 33 | NO |
