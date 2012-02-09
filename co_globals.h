#ifndef CO_GLOBALS_H
#define CO_GLOBALS_H

#include "co.h"
#include "co_stack.h"
#include "co_hash.h"
#include "co_vm_execute.h"

#define CG(v)   compiler_globals.v
#define EG(v)   executor_globals.v
#define EX(v)   execute_data.v

typedef struct _co_compiler_globals co_compiler_globals;
typedef struct _co_executor_globals co_executor_globals;

struct _co_compiler_globals {
    co_stack function_call_stack;
    HashTable function_symboltable;
    HashTable variable_symboltable;
    co_op_array *active_op_array;
    int line;
};

struct _co_executor_globals {
    HashTable symbol_table;
    co_op_array *active_op_array;
    co_execute_data *current_execute_data;
    co_vm_stack *argument_stack;
};

extern co_executor_globals executor_globals;
extern co_compiler_globals compiler_globals;

#endif
