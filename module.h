#ifndef MODULE_H
#define MODULE_H
/**
 * Implementaion of module system.
 */

#define IMPORT_DEFAULT   0
#define IMPORT_ALL       1

struct module_entry {
    char *name;
    COObject *(*initfunc) (void);
    int flag;
};

COObject *module_base_init(void);
COObject *module_gc_init(void);
COObject *module_io_init(void);
COObject *module_init(void);

#endif
