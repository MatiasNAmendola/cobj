#ifndef MODULE_H
#define MODULE_H
/**
 * Implementaion of module system.
 */

struct module_entry {
    char *name;
    COObject *(*initfunc) (void);
};

COObject *module_base_init(void);
COObject *module_gc_init(void);
COObject *module_init_builtins(void);

#endif
