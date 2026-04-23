# Simple Calculator

Terminal-based interactive calculator in C.

## Compilation

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 calculator.c -o calculator
```

## Usage

```bash
./calculator
```

## Supported Operations

- `1` Addition (+)
- `2` Subtraction (-)
- `3` Multiplication (*)
- `4` Division (/) with division-by-zero check
- `0` Quit

## Numeric Behavior

- Addition, subtraction, and multiplication use integer arithmetic.
- Division uses floating-point arithmetic and displays decimals when needed (e.g. 2.5).
- Division by zero prints `Error: division by zero` instead of crashing.

## Known Limitations

- Non-numeric input (e.g. letters) is not handled and may cause an infinite loop.
- Integer overflow is not checked.
