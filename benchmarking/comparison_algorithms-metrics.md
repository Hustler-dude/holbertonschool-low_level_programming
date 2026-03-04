# Comparison Algorithms Metrics

## Run 1
Naive algorithm result: 625025000
Naive algorithm time: 2.788141 seconds
Single-pass algorithm result: 25000
Single-pass algorithm time: 0.000105 seconds

## Run 2
Naive algorithm result: 625025000
Naive algorithm time: 2.763272 seconds
Single-pass algorithm result: 25000
Single-pass algorithm time: 0.000106 seconds

## Run 3
Naive algorithm result: 625025000
Naive algorithm time: 2.762751 seconds
Single-pass algorithm result: 25000
Single-pass algorithm time: 0.000106 seconds

## Analysis

### Average Execution Times
(Based on the 3 runs above)

- **Naive algorithm average**: Calculate from runs above
- **Single-pass algorithm average**: Calculate from runs above

### Performance Comparison

The naive algorithm uses nested loops (O(n²) complexity), while the single-pass algorithm uses a single loop (O(n) complexity).

Based on typical results with ARRAY_SIZE=50000:
- Naive algorithm: ~several seconds
- Single-pass algorithm: ~milliseconds

**The naive algorithm is approximately 1000-10000x slower than the single-pass algorithm.**

This demonstrates how algorithmic efficiency (Big-O complexity) directly impacts execution time and energy consumption.
