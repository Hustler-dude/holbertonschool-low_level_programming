# Valgrind Analysis — AI Memory Tracer

**Author:** Desmond (Hustler-dude)
**Date:** April 2026
**Tool:** Valgrind Memcheck (expected output on Ubuntu 20.04 / GCC 9)
**Command template:** `valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes ./program`

> **Note on environment:** Analysis performed on Windows (GCC 15.2 / MinGW).
> Valgrind is Linux-only. The output below is reconstructed from precise code analysis
> and confirmed against runtime behavior observed on Windows. All bug classifications
> are grounded in the source code, not guessed.

---

## Table of Contents

1. [heap_example — Memory Leak Analysis](#1-heap_example--memory-leak-analysis)
2. [aliasing_example — Use-After-Free Analysis](#2-aliasing_example--use-after-free-analysis)
3. [AI Critique — Where the AI Was Wrong](#3-ai-critique--where-the-ai-was-wrong)
4. [Summary Table](#4-summary-table)

---

## 1. heap_example — Memory Leak Analysis

### Valgrind command

```bash
valgrind --leak-check=full --show-reachable=yes ./heap_example
```

### Expected Valgrind output

```
==PID== Memcheck, a memory error detector
==PID== Command: ./heap_example
==PID==
heap_example: allocations and a deliberate leak
  alice=0x5204040 name=0x5204080 age=30
  bob=0x52040b0 name=0x52040d0 age=41
==PID==
==PID== HEAP SUMMARY:
==PID==     in use at exit: 6 bytes in 1 blocks
==PID==   total heap usage: 4 allocs, 3 frees, 1,038 bytes allocated
==PID==
==PID== 6 bytes in 1 blocks are definitely lost in loss record 1 of 1
==PID==    at 0x...: malloc (vg_replace_malloc.c)
==PID==    by 0x...: person_new (heap_example.c:23)
==PID==    by 0x...: main (heap_example.c:46)
==PID==
==PID== LEAK SUMMARY:
==PID==    definitely lost: 6 bytes in 1 blocks
==PID==    indirectly lost: 0 bytes in 0 blocks
==PID==      possibly lost: 0 bytes in 0 blocks
==PID==    still reachable: 0 bytes in 0 blocks
==PID==         suppressed: 0 bytes in 0 blocks
==PID==
==PID== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```

### Allocation accounting

| Alloc # | Location | Size | Freed by |
|---------|----------|------|----------|
| 1 | `person_new("Alice")` → `malloc(sizeof(Person))` | 12 bytes | `person_free_partial(alice)` → `free(alice)` ✓ |
| 2 | `person_new("Alice")` → `malloc(6)` for `"Alice\0"` | 6 bytes | **NEVER FREED** ✗ |
| 3 | `person_new("Bob")` → `malloc(sizeof(Person))` | 12 bytes | `free(bob)` ✓ |
| 4 | `person_new("Bob")` → `malloc(4)` for `"Bob\0"` | 4 bytes | `free(bob->name)` ✓ |

**Total: 4 allocs, 3 frees → 1 block lost.**

### Root cause analysis

```c
static void person_free_partial(Person *p)
{
    if (!p)
        return;
    free(p);       /* heap_example.c:38 — frees the struct body */
    /* p->name is NEVER freed */
}
```

`person_free_partial` releases the `Person` struct (the container), but `p->name`
is a **separate heap allocation** pointed to by a field inside that struct.
After `free(p)`, the pointer `p->name` is inaccessible because `p` no longer exists.

**Ownership chain:**

```
alice (stack, main)
  └── alice struct (heap, 12 bytes) ← freed by person_free_partial
        └── alice->name (heap, 6 bytes) ← NO OWNER after alice struct is freed
                                           → DEFINITELY LOST
```

**Why "definitely lost" and not "still reachable":**

"Definitely lost" means: at program exit, no pointer in any live memory region
points to the lost block. Since `alice` (the only pointer to the struct) was freed,
and the struct held the only copy of `alice->name`, that address is completely
unreachable. The allocator cannot recover it.

**Correct fix (not required by this task):**

```c
static void person_free_correct(Person *p)
{
    if (!p)
        return;
    free(p->name);   /* free the name first */
    free(p);         /* then free the struct */
}
```

---

## 2. aliasing_example — Use-After-Free Analysis

### Valgrind command

```bash
valgrind --leak-check=full --track-origins=yes ./aliasing_example
```

### Expected Valgrind output

```
==PID== Memcheck, a memory error detector
==PID== Command: ./aliasing_example
==PID==
aliasing_example: aliasing and use-after-free (Valgrind should report it)
  a=0x5204040 b=0x5204040 a[2]=22 b[2]=22
  after free(a): b=0x5204040 (dangling)
==PID== Invalid read of size 4
==PID==    at 0x...: main (aliasing_example.c:42)
==PID==  Address 0x5204040 is 8 bytes inside a block of size 20 free'd
==PID==    at 0x...: free (vg_replace_malloc.c)
==PID==    by 0x...: main (aliasing_example.c:38)
==PID==  Block was alloc'd at
==PID==    at 0x...: malloc (vg_replace_malloc.c)
==PID==    by 0x...: make_numbers (aliasing_example.c:12)
==PID==    by 0x...: main (aliasing_example.c:30)
==PID==
  reading b[2]=22
==PID== Invalid write of size 4
==PID==    at 0x...: main (aliasing_example.c:44)
==PID==  Address 0x5204040 is 12 bytes inside a block of size 20 free'd
==PID==    at 0x...: free (vg_replace_malloc.c)
==PID==    by 0x...: main (aliasing_example.c:38)
==PID==
==PID== Invalid read of size 4
==PID==    at 0x...: main (aliasing_example.c:45)
==PID==  Address 0x5204040 is 12 bytes inside a block of size 20 free'd
==PID==
  wrote b[3]=1234
==PID==
==PID== HEAP SUMMARY:
==PID==     in use at exit: 0 bytes in 0 blocks
==PID==   total heap usage: 1 allocs, 1 frees, 1,044 bytes allocated
==PID==
==PID== All heap blocks were freed -- no leaks are possible
==PID==
==PID== ERROR SUMMARY: 3 errors from 3 contexts (suppressed: 0 from 0)
```

### Issue classification

#### Issue 1 — Invalid read at line 42

```c
printf("  reading b[2]=%d\n", b[2]);   /* aliasing_example.c:42 */
```

**Category:** Use-after-free (invalid read, size 4)

**Explanation:**
`b[2]` dereferences the pointer `b` at offset `2 * sizeof(int) = 8` bytes.
The block at that address was freed at line 38 via `free(a)`.
Since `b == a`, `b` is now a dangling pointer.

Valgrind reports: `"8 bytes inside a block of size 20 free'd"`.

- Block size: `5 * sizeof(int) = 20 bytes`
- Offset of `b[2]`: `2 * 4 = 8 bytes` into the block
- Block state: freed at line 38

**Why it still returned 22:** The allocator does not zero freed memory.
The bytes `{0, 11, 22, 33, 44}` remained physically intact in RAM because no
subsequent allocation reused the block before this read. This is expected behavior
in most allocators — it is **not** a sign that the code is correct.

#### Issue 2 — Invalid write at line 44

```c
b[3] = 1234;   /* aliasing_example.c:44 */
```

**Category:** Use-after-free (invalid write, size 4)

**Explanation:**
Writing to a freed block is undefined behavior. `b[3]` is at offset
`3 * 4 = 12 bytes` inside the 20-byte block. The allocator owns this memory.
Writing here can corrupt allocator metadata or future allocations.

Valgrind reports: `"12 bytes inside a block of size 20 free'd"`.

In this run, the write succeeded without crashing because the page is still mapped.
Under heap hardening, address sanitization, or different allocator states, this
write could trigger an immediate crash or silent data corruption in another object.

#### Issue 3 — Invalid read at line 45

```c
printf("  wrote b[3]=%d\n", b[3]);   /* aliasing_example.c:45 */
```

**Category:** Use-after-free (invalid read, size 4)

**Explanation:**
Same as Issue 1. Reads `b[3]` (offset 12) from freed memory.
Returns 1234 only because Issue 2 wrote that value to the same freed block,
and no other operation modified it between the write and this read.
Both operations are undefined behavior.

### No memory leak in aliasing_example

```
All heap blocks were freed -- no leaks are possible
```

The single block (20 bytes) allocated in `make_numbers` was correctly freed
via `free(a)`. There is no ownership problem — the problem is that `b` continued
to be used **after** ownership was released.

### Root cause: aliasing without ownership tracking

```c
a = make_numbers(n);   /* a owns the block */
b = a;                 /* b is an alias — no ownership */
free(a);               /* a releases ownership */
                       /* b is now dangling — b[x] is illegal */
```

The C language provides no built-in mechanism to track that `b` no longer has
a valid target after `free(a)`. The programmer must track ownership manually.

---

## 3. AI Critique — Where the AI Was Wrong

### Incorrect claim 1: leak category for heap_example

**AI initial explanation:**
> "Valgrind will report `alice->name` as 'indirectly lost' because it was lost
> as a consequence of losing the pointer to alice."

**Why this is wrong:**

"Indirectly lost" applies when a live pointer exists to a container, but objects
reachable through that container are lost. In this case:

- `alice` (the struct pointer) is itself freed by `person_free_partial`.
- After that call, there is NO live pointer to either `alice` or `alice->name`.
- Both are unreachable at program exit.

However, Valgrind tracks the **last known direct allocation call**. The name block
was allocated in `person_new` via `malloc(len + 1)`. At program exit, no pointer
in any live memory refers to this block. Valgrind classifies it as
**"definitely lost"**, not "indirectly lost".

"Indirectly lost" would apply if, for example, a pointer to `alice` existed in
another heap structure that was itself leaked — creating a chain of unreachable
pointers. That is not the case here.

**Correct classification:** `definitely lost: 6 bytes in 1 blocks`

---

### Incorrect claim 2: invalid read size in aliasing_example

**AI initial explanation:**
> "Valgrind will report an invalid read of size 1 when accessing b[2]."

**Why this is wrong:**

The array element type is `int`, which is 4 bytes on all standard 32-bit and 64-bit
platforms. `b[2]` reads one `int` — that is a read of **size 4**, not size 1.

Valgrind reports read/write sizes in bytes corresponding to the actual memory
access width. `size 1` would indicate a `char` access.

**Correct Valgrind line:** `Invalid read of size 4`

---

### Incorrect claim 3: aliasing_example has a memory leak

**AI initial explanation:**
> "Since `b` is never freed, Valgrind will report a memory leak for the
> block allocated in make_numbers."

**Why this is wrong:**

`b` is not a separately allocated block — it is an alias (a copy of the pointer
value) for the same block allocated in `make_numbers`. There is only one heap
allocation, and it is freed via `free(a)`.

`b` is a local variable stored on the stack. Stack variables are not heap
allocations and are never reported as leaks by Valgrind. The heap block was freed;
only the pointer value in `b` was not cleared, which is irrelevant to leak tracking.

**Correct Valgrind line:** `All heap blocks were freed -- no leaks are possible`

---

## 4. Summary Table

| Program | Valgrind category | Count | Line(s) | Object involved | Lifetime violation |
|---------|------------------|-------|---------|-----------------|-------------------|
| heap_example | Definitely lost | 1 | 23 (alloc), 38 (last owner freed) | `alice->name` (6 bytes) | Ownership lost when struct freed without freeing name |
| aliasing_example | Invalid read (size 4) | 2 | 42, 45 | 20-byte int array | Accessed after free(a) via dangling pointer b |
| aliasing_example | Invalid write (size 4) | 1 | 44 | 20-byte int array | Written after free(a) via dangling pointer b |
| stack_example | None | 0 | — | — | No heap involved, no dangling pointers |
| crash_example | Segfault (not Valgrind) | — | 31 | NULL (page 0) | NULL dereference before Valgrind can report |
