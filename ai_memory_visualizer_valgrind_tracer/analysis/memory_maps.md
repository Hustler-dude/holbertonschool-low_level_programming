# Memory Maps — AI Memory Visualizer + Valgrind Tracer

**Author:** Desmond (Hustler-dude)
**Date:** April 2026
**Tools used:** GCC 15.2, manual analysis, AI-assisted generation (Claude)

---

## Table of Contents

1. [stack_example.c — Recursion and Stack Frames](#1-stack_examplec--recursion-and-stack-frames)
2. [aliasing_example.c — Pointer Aliasing and Use-After-Free](#2-aliasing_examplec--pointer-aliasing-and-use-after-free)
3. [heap_example.c — Heap Allocations and Memory Leak](#3-heap_examplec--heap-allocations-and-memory-leak)
4. [AI Critique — Where the AI Was Wrong](#4-ai-critique--where-the-ai-was-wrong)

---

## 1. stack_example.c — Recursion and Stack Frames

### What the program does

`main()` calls `walk_stack(0, 3)`, which recurses until `depth == max_depth` (3).
At each depth, `dump_frame()` is called on entry and exit, printing the addresses of
local variables.

### Key execution points

| Event | depth | Action |
|-------|-------|--------|
| walk_stack called | 0 | New stack frame created |
| dump_frame called | 0 | Another frame on top |
| walk_stack called recursively | 1 | New frame, lower address |
| ... | 2 | ... |
| walk_stack called recursively | 3 | Deepest frame |
| depth == max_depth, no more recursion | 3 | Frames start unwinding |
| walk_stack returns | 3→2→1→0 | Frames destroyed in reverse order |

### Actual addresses observed (from execution)

```
Stack grows DOWNWARD (higher address → lower address as depth increases)

depth=0:  &local_int = 0x9F873FF964    &marker = 0x9F873FF9AC
depth=1:  &local_int = 0x9F873FF924    &marker = 0x9F873FF96C
depth=2:  &local_int = 0x9F873FF8E4    &marker = 0x9F873FF92C
depth=3:  &local_int = 0x9F873FF8A4    &marker = 0x9F873FF8EC
```

Each recursive call shifts addresses down by approximately 0x40 bytes (64 bytes),
which is the size of one `walk_stack` + `dump_frame` combined stack frame.

### Memory map — Stack at maximum depth (depth=3)

```
HIGH ADDRESS
┌─────────────────────────────────────────────────────┐
│  main() stack frame                                  │
│    (no locals relevant to analysis)                  │
├─────────────────────────────────────────────────────┤
│  walk_stack(depth=0, max_depth=3)                    │
│    depth     = 0        @ 0x9F873FF9AC+offset        │
│    max_depth = 3                                     │
│    marker    = 0        @ 0x9F873FF9AC               │
│    [dump_frame frame on top when active]             │
├─────────────────────────────────────────────────────┤
│  walk_stack(depth=1, max_depth=3)                    │
│    depth     = 1                                     │
│    max_depth = 3                                     │
│    marker    = 10       @ 0x9F873FF96C               │
├─────────────────────────────────────────────────────┤
│  walk_stack(depth=2, max_depth=3)                    │
│    depth     = 2                                     │
│    max_depth = 3                                     │
│    marker    = 20       @ 0x9F873FF92C               │
├─────────────────────────────────────────────────────┤
│  walk_stack(depth=3, max_depth=3)  ← DEEPEST        │
│    depth     = 3                                     │
│    max_depth = 3                                     │
│    marker    = 30       @ 0x9F873FF8EC               │
├─────────────────────────────────────────────────────┤
│  dump_frame("enter", 3)  ← TOP OF STACK             │
│    label     = ptr to "enter" (in .rodata)           │
│    depth     = 3                                     │
│    local_int = 103      @ 0x9F873FF8A4               │
│    local_buf = [D,\0,...] @ 0x9F873FF890 (16 bytes)  │
│    p_local   = 0x9F873FF8A4 (points to local_int)    │
└─────────────────────────────────────────────────────┘
LOW ADDRESS (stack pointer here)
```

### Key observation: p_local is a pointer to a local variable

```c
int *p_local = &local_int;
```

`p_local` stores the address of `local_int`, which lives in the same frame.
As long as `dump_frame` is executing, this is valid.

**Critical invariant:** The moment `dump_frame` returns, its entire stack frame is
reclaimed (stack pointer moves back up). Any stored copy of `p_local` outside the
function would become a **dangling pointer** pointing to memory that may be reused
by the next function call.

In this program, `p_local` is not returned or stored externally, so no bug occurs.

### Variable lifetimes

| Variable | Lives in | Lifetime |
|----------|----------|----------|
| `marker` (walk_stack) | Stack | From call to return of walk_stack |
| `local_int` (dump_frame) | Stack | From call to return of dump_frame |
| `local_buf[16]` (dump_frame) | Stack | From call to return of dump_frame |
| `p_local` (dump_frame) | Stack | From call to return of dump_frame |
| The value *p_local points to | Stack | Same as local_int (same frame) |

### On "exit" calls: why values are still valid

When `dump_frame("exit", 3)` is called, `walk_stack(depth=3)` has NOT yet returned.
Its stack frame is still active. Therefore `marker` and all variables in the
`walk_stack(3)` frame remain valid and readable.

---

## 2. aliasing_example.c — Pointer Aliasing and Use-After-Free

### What the program does

```
make_numbers(5) → malloc(5 * sizeof(int)) → {0, 11, 22, 33, 44}
a = returned pointer
b = a         ← aliasing: both point to same block
free(a)       ← block released
b[2] read     ← use-after-free (invalid read)
b[3] = 1234   ← use-after-free (invalid write)
```

### Heap state — BEFORE free(a)

```
HEAP
┌──────────────────────────────────────────────────────┐
│  Block @ 0x1855EEF0  (size: 20 bytes = 5 * int)     │
│  ┌────────┬────────┬────────┬────────┬────────┐      │
│  │   0    │   11   │   22   │   33   │   44   │      │
│  │[0]     │[1]     │[2]     │[3]     │[4]     │      │
│  └────────┴────────┴────────┴────────┴────────┘      │
└──────────────────────────────────────────────────────┘

STACK (main)
  a = 0x1855EEF0  ──────────────────────────┐
  b = 0x1855EEF0  ──────────────────────────┘ (same target)
  n = 5
```

Both `a` and `b` hold the same address. Changing `a[1]` is identical to
changing `b[1]`. This is aliasing: **two names for the same memory**.

### Heap state — AFTER free(a)

```
HEAP
┌──────────────────────────────────────────────────────┐
│  Block @ 0x1855EEF0  ← FREED (owned by allocator)   │
│  ┌────────┬────────┬────────┬────────┬────────┐      │
│  │  ???   │  ???   │  ???   │  ???   │  ???   │      │
│  │ (may   │ still  │ appear │ intact │ or not)│      │
│  └────────┴────────┴────────┴────────┴────────┘      │
└──────────────────────────────────────────────────────┘

STACK (main)
  a = 0x1855EEF0  (still holds old address — but block is freed)
  b = 0x1855EEF0  ← DANGLING POINTER
```

`b` still contains `0x1855EEF0`. The address is still a valid memory location,
but ownership has been transferred to the allocator. Reading or writing through
`b` is undefined behavior.

### Why b[2] still printed 22 (observed behavior)

The output showed `b[2]=22` after `free(a)`. This is **not** proof of correctness.
The allocator released the block but did not zero the bytes (it never does).
The bytes remained physically intact because no other allocation reused the block
between the `free` and the `printf`.

This is the most dangerous property of use-after-free: it often appears to work,
making the bug invisible until conditions change (different OS, different load,
different compiler optimizations).

### Valgrind classification

| Operation | Valgrind category | Line |
|-----------|-------------------|------|
| `printf("...b=%p...", b)` | Use-after-free (invalid read, size 0) | 40 |
| `b[2]` read | Invalid read of size 4 | 42 |
| `b[3] = 1234` | Invalid write of size 4 | 44 |
| `b[3]` read | Invalid read of size 4 | 45 |

### GCC detection

GCC 15 with `-Wuse-after-free` detected all four violations at **compile time**,
before the program ran. On Ubuntu 20.04 (GCC 9, used by Holberton checker),
this warning does not exist, and the program compiles and runs silently.

This illustrates why Valgrind is necessary: compiler warnings are version-dependent,
and runtime behavior can mask the bug.

---

## 3. heap_example.c — Heap Allocations and Memory Leak

### What the program does

Two `Person` structs are allocated. Each allocation consists of two parts:
1. The `Person` struct itself (`sizeof(Person)` bytes)
2. The `name` string (a separate heap block)

`bob` is fully freed. `alice` is freed via `person_free_partial`, which only
frees the `Person` struct — **not** `alice->name`.

### Heap layout after all allocations

```
HEAP
┌─────────────────────────────────────────────────────────┐
│  alice (Person struct) @ 0x227E6FCF020                  │
│  ┌──────────────────────┬──────────┐                    │
│  │ name = 0x227E6FC47E0 │ age = 30 │                    │
│  └──────────────────────┴──────────┘                    │
│         │                                               │
│         ▼                                               │
│  alice->name @ 0x227E6FC47E0  (6 bytes)                 │
│  ┌────┬────┬────┬────┬────┬────┐                        │
│  │'A' │'l' │'i' │'c' │'e' │'\0'│                       │
│  └────┴────┴────┴────┴────┴────┘                        │
│                                                         │
│  bob (Person struct) @ 0x227E6FCF120                    │
│  ┌──────────────────────┬──────────┐                    │
│  │ name = 0x227E6FC4800 │ age = 41 │                    │
│  └──────────────────────┴──────────┘                    │
│         │                                               │
│         ▼                                               │
│  bob->name @ 0x227E6FC4800  (4 bytes)                   │
│  ┌────┬────┬────┬────┐                                  │
│  │'B' │'o' │'b' │'\0'│                                  │
│  └────┴────┴────┴────┘                                  │
└─────────────────────────────────────────────────────────┘
```

### Deallocation sequence

| Step | Operation | Effect |
|------|-----------|--------|
| 1 | `free(bob->name)` | 4-byte block freed |
| 2 | `free(bob)` | Person struct freed |
| 3 | `person_free_partial(alice)` | `free(alice)` — struct freed |
| — | `alice->name` | **NEVER FREED — memory leak** |

### The bug in person_free_partial

```c
static void person_free_partial(Person *p)
{
    if (!p)
        return;
    free(p);       /* frees the struct */
                   /* p->name is NOT freed — leaked */
}
```

The function frees the `Person` container but ignores `p->name`.
After `free(p)`, the pointer `p->name` is inaccessible because `p` itself is freed.
The 6 bytes of `"Alice\0"` are permanently leaked.

### Valgrind output (expected on Linux)

```
==PID== LEAK SUMMARY:
==PID==    definitely lost: 6 bytes in 1 blocks
==PID==    indirectly lost: 0 bytes in 0 blocks
==PID==      possibly lost: 0 bytes in 0 blocks
==PID==    still reachable: 0 bytes in 0 blocks
```

The 6 bytes are "definitely lost" because no pointer to that block exists at
program exit — `alice` (the only pointer to the struct that held `alice->name`)
was freed.

---

## 4. AI Critique — Where the AI Was Wrong

### Example 1: stack_example — incorrect description of frame cleanup

**AI initial explanation (verbatim):**
> "When dump_frame returns, its local variables are freed and the memory is
> reclaimed by the operating system."

**Why this is wrong:**

Stack memory is **never** reclaimed by the operating system on a per-function
basis. The OS allocates the entire stack region upfront (typically 8 MB on Linux).
When `dump_frame` returns, the CPU simply moves the **stack pointer** register
(RSP on x86-64) back to where it was before the call. The bytes that held
`local_int`, `local_buf`, and `p_local` are not zeroed or released to the OS —
they remain in the same physical memory page. They will be silently overwritten
by the next function call that uses that portion of the stack.

**Correct statement:**
> "When dump_frame returns, its stack frame is invalidated by moving the stack
> pointer back. The bytes are not cleared. They remain readable until overwritten
> by the next function call, but accessing them through any stored pointer is
> undefined behavior."

---

### Example 2: aliasing_example — incorrect claim about dangling pointer reads

**AI initial explanation:**
> "After free(a), reading b[2] will always crash the program."

**Why this is wrong:**

A dangling pointer read does **not** always crash. On Linux (and Windows), the
allocator does not unmap the page after a `free`. The freed block's page remains
mapped in the process's virtual address space. The bytes retain their previous
values until the allocator reuses the block. As confirmed by execution:

```
reading b[2]=22
wrote b[3]=1234
```

The program ran to completion without crashing. The crash only happens if:
- The OS unmaps the page (unlikely for small blocks)
- Another allocation reuses and partially overwrites the block before the read
- Hardware memory tagging (e.g., ARM MTE) is active

**Correct statement:**
> "After free(a), reading b[2] is undefined behavior. It may return the old
> value, return garbage, or crash — depending on allocator state and timing.
> The undefined behavior is real regardless of whether a crash occurs."

---

### Example 3: heap_example — AI missed which pointer is leaked

**AI initial explanation:**
> "The memory leak is in `person_free_partial` because it does not free all
> the memory of the Person struct."

**Why this is imprecise:**

The `Person` struct itself IS freed by `free(p)`. What is leaked is not part
of the struct body — it is a **separately allocated heap block** pointed to by
`p->name`. The struct and the name string are two independent allocations.
`sizeof(Person)` bytes are properly freed; the issue is that `p->name` is a
pointer stored inside the struct that points to a different allocation, and
that second allocation has no other reference.

**Correct statement:**
> "`person_free_partial` frees the `Person` struct (sizeof(Person) bytes) but
> does not free the heap block pointed to by `p->name`. Since `p->name` is the
> only pointer to that block, and `p` is then freed, `alice->name` becomes
> permanently unreachable — a definite memory leak of 6 bytes."

---

## Summary Table

| Program | Memory region | Bug type | Valgrind category |
|---------|--------------|----------|-------------------|
| stack_example | Stack | None (safe) | No error |
| aliasing_example | Heap | Use-after-free (read + write) | Invalid read/write |
| heap_example | Heap | Memory leak | Definitely lost |
| crash_example | — | NULL dereference | Segfault (not Valgrind) |
