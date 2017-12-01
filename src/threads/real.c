#include "real.h"
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include <debug.h>

#define P 17;
#define Q 14;
static int f = 1<<Q;


struct real * int_to_real (int number){
    struct real *result;
    result->value = number * f;
    return result;
}

int real_to_int_rounded_to_zero (struct real *number){
    return (number->value) / f;
}

int real_to_int_rounded_to_nearest_integer (struct real *number){
    if(number->value >= 0){
      return (number->value + f / 2) / f;
    }
    else{
      return (number->value - f / 2) / f;
    }
}

struct real * real_add (struct real *number1, struct real *number2){
    struct real *result;
    result->value = number1->value + number2->value;
    return result;
}

struct real * real_subtract (struct real *number1, struct real *number2){
    struct real *result;
    result->value = number1->value - number2->value;
    return result;
}

struct real * real_multiply (struct real *number1, struct real *number2){
    struct real *result;
    result->value = ((int64_t)number1->value) * (number2->value) / f;
    return result;
}

struct real * real_divide (struct real *number1, struct real *number2){
    struct real *result;
    result->value = 0;
    printf("%d\n",result->value);
    result->value = (number1->value) * f / (number2->value);
    if(result == NULL)printf("null error\n");
    int x = result->value;
    printf("%d\n", x);
    return result;
}



struct real * real_int_add (struct real *number1, int number2){
    struct real *real_number2 = int_to_real(number2);

    struct real *result;
    result->value = number1->value + real_number2->value;
    return result;
}

struct real * real_int_subtract (struct real *number1, int number2){
    struct real *real_number2 = int_to_real(number2);

    struct real *result;
    result->value = number1->value - real_number2->value;
    return result;
}

struct real * real_int_multiply (struct real *number1, int number2){
    struct real *result;
    result->value = number1->value * number2;
    return result;
}

struct real * real_int_divide (struct real *number1, int number2){
    struct real *result;
    result->value = number1->value  / number2;
    return result;
}
