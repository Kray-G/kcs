#ifndef _EXT_H
#define _EXT_H

/* ---------------------------------------------------------------------------------------------
    KCC Extended Library - to use external dll module
--------------------------------------------------------------------------------------------- */

void *kcc_extlib(void);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/kcc/ext.c>
#endif
#endif

#endif /* _EXT_H */
