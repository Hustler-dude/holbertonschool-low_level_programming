# Crash Report — crash_example.c

**Author:** Desmond (Hustler-dude)
**Date:** April 2026
**Program:** `programs/crash_example.c`
**Observed behavior:** Segmentation fault (exit code 139)

---

## 1. Crash Reproduction

### Compile and run

```bash
gcc -Wall -Wextra -Werror -pedantic -std=gnu89 -g -o crash_example crash_example.c
./crash_example
```

### Observed output

```
crash_example: deterministic NULL dereference (segmentation fault)
  requesting n=0
Segmentation fault (core dumped)
```

The crash occurs before any output past the second `printf`. It is **deterministic**:
it happens on every run, with every input, because the bug depends only on the
constant value `n = 0`.

---

## 2. Root Cause Analysis

### Full causal chain

```
n = 0                               (main, line 24)
  │
  ▼
allocate_numbers(0)                 (main, line 29)
  │
  ▼
if (n <= 0) return NULL;           (allocate_numbers, line 9)
  │
  ▼
nums = NULL                         (main, line 29 — return value)
  │
  ▼
nums[0] = 42;                       (main, line 31)
  │
  = *(NULL + 0) = *(0x00000000)
  │
  ▼
SIGSEGV — page 0 is not mapped
```

### The invalid access

```c
int main(void)
{
    int *nums = NULL;
    int n = 0;                          /* n is 0 */

    nums = allocate_numbers(n);         /* returns NULL because n <= 0 */

    nums[0] = 42;                       /* NULL[0] = *(NULL + 0) = *0x0 → CRASH */
```

`nums[0]` is syntactic sugar for `*(nums + 0)`. When `nums == NULL` (address 0x0),
this dereferences address `0x00000000`. On all modern Linux systems, the first page
of virtual memory (addresses 0x0 to 0xFFF) is intentionally unmapped by the kernel
as a safety measure. Any access to this region raises **SIGSEGV** (signal 11).

### Why allocate_numbers returns NULL

```c
static int *allocate_numbers(int n)
{
    int *arr = NULL;
    int i = 0;

    if (n <= 0)          /* n=0 satisfies this condition */
        return NULL;     /* returns here — no malloc */
    ...
}
```

The guard `if (n <= 0) return NULL` is correct defensive programming.
The bug is not inside `allocate_numbers` — it is in `main`, which does not
check the return value before using it.

---

## 3. Memory State at Crash

```
STACK (main)
  nums = 0x00000000  (NULL)
  n    = 0

HEAP
  (empty — no malloc was called)

VIRTUAL MEMORY
  Page 0x00000000–0x00000FFF: NOT MAPPED (intentionally)
  ↑ Access here → kernel raises SIGSEGV
```

No heap memory was allocated. The crash does not involve a freed block,
an uninitialized pointer, or aliasing. It is a direct dereference of the
null pointer — the simplest possible memory misuse.

---

## 4. Valgrind Output

Valgrind can catch this crash and provide the exact faulting line:

```
==PID== Memcheck, a memory error detector
==PID== Command: ./crash_example
==PID==
crash_example: deterministic NULL dereference (segmentation fault)
  requesting n=0
==PID== Invalid write of size 4
==PID==    at 0x...: main (crash_example.c:31)
==PID==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==PID==
==PID== Process terminating with default action of signal 11 (SIGSEGV)
==PID==  Access not within mapped region at address 0x0
==PID==    at 0x...: main (crash_example.c:31)
==PID==
==PID== HEAP SUMMARY:
==PID==     in use at exit: 0 bytes in 0 blocks
==PID==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
==PID==
==PID== All heap blocks were freed -- no leaks are possible
==PID==
==PID== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```

**Key Valgrind line:**
`Address 0x0 is not stack'd, malloc'd or (recently) free'd`

This message means the address is not part of any known memory region.
It is not a stack variable, not a heap allocation, and not a recently freed block.
It is simply an invalid address.

---

## 5. AI Critique — Incorrect Explanation

### AI initial claim

> "The crash occurs because `allocate_numbers` returns NULL when malloc fails,
> and the caller does not handle the failure. The fix is to add a NULL check
> after the malloc call inside allocate_numbers."

### Why this is wrong

The `malloc` inside `allocate_numbers` **is never called**. The function returns
early at `if (n <= 0) return NULL` — before reaching the `malloc` line.
This is not a malloc failure. It is an **intentional early return** for invalid input.

The AI confused two distinct code paths:
1. `n <= 0` → early return NULL (what actually happens here)
2. `malloc` returns NULL due to out-of-memory (does not happen here)

Both paths return NULL, but for different reasons. The fix is not inside
`allocate_numbers` — it is in `main`, which must check the return value:

```c
nums = allocate_numbers(n);
if (nums == NULL)          /* missing check */
{
    fprintf(stderr, "Error: allocation failed or n=0\n");
    return 1;
}
nums[0] = 42;              /* only reached if nums is valid */
```

### Correct explanation

The crash is a **deterministic NULL dereference** caused by calling
`allocate_numbers(0)`, which returns NULL by design, followed by
an unchecked dereference of that NULL pointer. The root cause is
missing return value validation in `main`, not a malloc failure.

---

## 6. Classification

| Property | Value |
|----------|-------|
| Crash type | Segmentation fault (SIGSEGV, signal 11) |
| Faulting address | 0x00000000 (NULL) |
| Faulting line | `crash_example.c:31` — `nums[0] = 42` |
| Valgrind category | Invalid write of size 4 |
| Root cause | Unchecked NULL return value before dereference |
| Deterministic? | Yes — always crashes with n=0 |
| Heap involved? | No — zero allocations made |
| Fix location | `main()`, add NULL check after `allocate_numbers()` call |
