#ifndef VM_H
#define VM_H

#include <lacc/ir.h>

#include <stdio.h>

INTERNAL void add_import_module(const char *name);

INTERNAL void add_ref_module(const char *name);

/* Set up virtual machine. */
INTERNAL void vm_init(FILE *stream, const char *file);

/* Generate low level IR for VM. */
INTERNAL void vm_gen_lir(struct definition *def);

INTERNAL int vm_run_lir(void);

INTERNAL int vm_print_lir(void);

INTERNAL int vm_finalize_lir(void);

INTERNAL int vm_get_return_value(void);

INTERNAL int vm_save_lir(void);

#endif
