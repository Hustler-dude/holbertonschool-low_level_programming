# Peer Review Readiness Checklist

**Author:** Desmond (Hustler-dude)
**Date:** April 2026
**Project:** AI Memory Visualizer + Valgrind Tracer

---

## Self-Assessment

### 1. Memory Lifetimes — explicitly explained?

**[x] YES**

In `memory_maps.md`:

- Stack variables in `dump_frame` (`local_int`, `local_buf`, `p_local`) are
  explicitly described as valid only during the execution of `dump_frame`.
  The moment the function returns, the stack pointer moves back and those bytes
  are subject to overwrite by the next call.
- The exit-phase behavior of `walk_stack` is explained: when `dump_frame("exit")`
  is called, `walk_stack`'s frame is still active, so variables remain valid.
- Heap allocations in `heap_example` are tracked individually: 4 allocs, 3 frees,
  with the leaked block (`alice->name`, 6 bytes) identified by allocation site
  (`heap_example.c:23`).

**Key statement from analysis:**
> "The moment dump_frame returns, its entire stack frame is reclaimed by moving
> the stack pointer back. The bytes are not cleared. They remain readable until
> overwritten by the next function call, but accessing them through any stored
> pointer is undefined behavior."

---

### 2. Pointer Relationships and Aliasing — clear?

**[x] YES**

In `memory_maps.md` and `valgrind_analysis.md`:

- `b = a` is explicitly identified as aliasing: both pointers hold the same
  heap address (`0x1855EEF0`). Changing `a[1]` is identical to changing `b[1]`.
- After `free(a)`, `b` is labeled a **dangling pointer**: it still holds the
  address, but the block is no longer owned by the program.
- The ownership chain for `heap_example` is diagrammed:
  ```
  alice (stack) → alice struct (heap) → alice->name (heap)
  ```
  Each arrow represents a pointer. When the struct is freed, the pointer to
  `alice->name` becomes inaccessible.

**I can explain aliasing without running the program:**
Two pointer variables hold the same address when one is assigned from the other
without allocating new memory. Freeing through one invalidates all aliases.

---

### 3. Valgrind Findings — interpreted, not pasted?

**[x] YES**

In `valgrind_analysis.md`:

- Each Valgrind error category is defined and mapped to a code line:
  - "Invalid read of size 4" → `b[2]` at line 42, 8 bytes inside a 20-byte freed block
  - "Invalid write of size 4" → `b[3] = 1234` at line 44, 12 bytes inside freed block
  - "Definitely lost: 6 bytes" → `alice->name` allocated at line 23, lost when struct freed at line 38
- The reason `b[2]` still returned 22 is explained: the allocator does not zero freed
  memory; the bytes remain intact until reused by another allocation.
- The distinction between "definitely lost" and "indirectly lost" is explicitly
  addressed and an AI error on this point is corrected.

**If Valgrind output were removed**, the explanations still make sense because
they are derived from the code's memory state, not from Valgrind's messages.

---

### 4. Crash Explained Causally?

**[x] YES**

In `crash_report.md`:

The causal chain is written step by step:
```
n = 0 → allocate_numbers(0) → if (n <= 0) return NULL → nums = NULL
→ nums[0] = 42 → *(NULL + 0) → address 0x0 → page not mapped → SIGSEGV
```

- The crash is explicitly called **deterministic**: it happens on every run
  because it depends only on the constant `n = 0`.
- The crash is not described as "the program fails" or "it segfaults".
  It is described as: a write of 4 bytes to virtual address 0x00000000,
  which is unmapped by the kernel as a safety measure, triggering SIGSEGV.
- The location of the bug is precisely identified: `main()` line 31,
  not inside `allocate_numbers`.

**At a whiteboard, without tools:**
"`allocate_numbers(0)` has an explicit guard: if n is zero or negative, return NULL.
The caller at line 31 does not check the return value and immediately dereferences it.
Address 0 is always unmapped on Linux. The kernel sends SIGSEGV. This happens
every single time — it is not a race condition or intermittent failure."

---

### 5. Critical Use of AI — documented and critiqued?

**[x] YES**

Three AI errors were identified and corrected across the analysis files:

**memory_maps.md — AI error 1:**
AI claimed stack memory is "reclaimed by the operating system" on function return.
Corrected: the OS never reclaims individual stack frames. Only the stack pointer
register moves. Bytes remain in RAM until overwritten.

**memory_maps.md — AI error 2:**
AI claimed a dangling pointer dereference "always crashes".
Corrected: on Linux with standard allocators, freed pages remain mapped.
The old bytes persist until reuse. The observed output (`b[2]=22`) confirms this.

**valgrind_analysis.md — AI error 3:**
AI classified `alice->name` as "indirectly lost".
Corrected: "indirectly lost" requires a live pointer to the container.
Since `alice` (the struct) was also freed, both objects are unreachable.
Valgrind classifies it as "definitely lost".

**AI error 4 (crash_report.md):**
AI attributed the crash to a malloc failure inside `allocate_numbers`.
Corrected: malloc is never called. The early return at `if (n <= 0)` fires
before malloc is reached. These are two distinct code paths that both return NULL.

**AI was used as a hypothesis generator, not as authority.**
Every AI claim was checked against the source code and either confirmed or corrected.

---

## Final Confirmation

| Criterion | Status | Location |
|-----------|--------|----------|
| Memory lifetimes explicitly explained | PASS | memory_maps.md §Variable lifetimes |
| Pointer aliasing clearly described | PASS | memory_maps.md §2, valgrind_analysis.md §2 |
| Valgrind output interpreted (not pasted) | PASS | valgrind_analysis.md §Issue classification |
| Crash explained causally | PASS | crash_report.md §2 Root cause |
| AI usage documented and critiqued | PASS | All three analysis files, §AI Critique |

**Submission status: READY FOR PEER REVIEW**

A reviewer can verify understanding of stack vs heap, pointer ownership,
use-after-free, memory leaks, and NULL dereference without asking follow-up
questions, because each claim in the analysis files is supported by code line
references and causal reasoning.
