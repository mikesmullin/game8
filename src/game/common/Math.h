#pragma once

typedef double f64;

f64 Math__map(f64 n, f64 input_start, f64 input_end, f64 output_start, f64 output_end);

#define Math__min(a, b) (((a) < (b)) ? (a) : (b))
#define Math__max(a, b) (((a) > (b)) ? (a) : (b))
