#ifndef CO_OPERATORS_H
#define CO_OPERATORS_H

#include "co_compile.h"
extern int add_function(cval *result, cval *op1, cval *op2);

extern int sub_function(cval *result, cval *op1, cval *op2);

extern int mul_function(cval *result, cval *op1, cval *op2);

extern int div_function(cval *result, cval *op1, cval *op2);

extern int mod_function(cval *result, cval *op1, cval *op2);

extern int boolean_xor_function(cval *result, cval *op1, cval *op2);

extern int boolean_not_function(cval *result, cval *op1, cval *op2);

extern int concat_function(cval *result, cval *op1, cval *op2);

#endif
