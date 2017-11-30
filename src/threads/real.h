#ifndef THREADS_REAL_H
#define THREADS_REAL_H

#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include <debug.h>

struct real{
    int value;
};


struct real * int_to_real (int number);
int real_to_int_rounded_to_zero (struct real *number);
int real_to_int_rounded_to_nearest_integer (struct real *number);


struct real * real_add (struct real *number1, struct real *number2);
struct real * real_subtract (struct real *number1, struct real *number2);
struct real * real_multiply (struct real *number1, struct real *number2);
struct real * real_divide (struct real *number1, struct real *number2);

struct real * real_int_add (struct real *number1, int number2);
struct real * real_int_subtract (struct real *number1, int number2);
struct real * real_int_multiply (struct real *number1, int number2);
struct real * real_int_divide (struct real *number1, int number2);

#endif
