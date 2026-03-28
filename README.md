# HW2: Floating-Point Overflow Checker

CS3339, Spring 2026

## Description

`fp_overflow_checker` detects floating-point precision overflow — the condition
where a loop counter's increment is silently lost because the accumulator value
is too large to represent the change (insufficient mantissa bits).

## Build

```bash
make
```

Requires a C++17-capable compiler (g++ or clang++).

## Usage

```
./fp_overflow_checker <loop_bound> <loop_counter>
```

Both arguments are positive floating-point values (decimal or scientific notation).

## Examples

```
$ ./fp_overflow_checker 1e+08 1.0
Loop bound:   0 10011001 01111101011110000100000
Loop counter: 0 01111111 00000000000000000000000

Warning: Possible overflow!
Overflow threshold:
      1.67772e+07
      0 10010111 00000000000000000000000

$ ./fp_overflow_checker 1e+05 2.0
Loop bound:   0 10001111 10000110101000000000000
Loop counter: 0 10000000 00000000000000000000000

There is no overflow!
```

## How It Works

For an IEEE 754 single-precision float `v` with biased exponent `E`, the unit
in the last place is `ULP(v) = 2^(E-150)`. An increment `inc` is lost
(i.e., `v + inc == v` in round-to-nearest) when `inc ≤ ULP(v)/2`.

The threshold is the smallest `v` satisfying this condition. Using bit
manipulation on `inc`'s raw bits:

- Extract biased exponent `E_inc` and mantissa `M_inc` from `inc`
- If `M_inc == 0` (exact power of 2): `E_threshold = E_inc + 24`
- Otherwise: `E_threshold = E_inc + 25`
- Threshold = float with biased exponent `E_threshold` and mantissa 0

No brute-force loop, no `log2()` / `pow()` used.

## Known Bugs / Limitations

- Only handles `float` (32-bit IEEE 754 single precision), not `double`.
- Assumes positive, normal (non-subnormal, non-NaN, non-Inf) inputs.
- If the computed `E_threshold` exceeds 255, the result is undefined (would
  require an astronomically small increment; not a realistic test case).
