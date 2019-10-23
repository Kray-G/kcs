.POSIX:
.SUFFIXES:
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib
SRCDIR = $(CURDIR)$(.CURDIR)

CC = gcc
CPP = g++
CFLAGS = -O2 -Wno-missing-braces -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -std=gnu99
CPPFLAGS = -O2 -Wno-missing-braces

SOURCES = \
	src/kcc.c \
	src/kccutil.c \
	src/context.c \
	src/util/argparse.c \
	src/util/hash.c \
	src/util/string.c \
	src/backend/x86_64/instr.c \
	src/backend/x86_64/jit_util.c \
	src/backend/x86_64/jit.c \
	src/backend/x86_64/dwarf.c \
	src/backend/x86_64/elf.c \
	src/backend/x86_64/abi.c \
	src/backend/x86_64/assemble.c \
	src/backend/vm/vmdump.c \
	src/backend/vm/vminstr.c \
	src/backend/vm/vmrunlir.c \
	src/backend/vm/vmimplir.c \
	src/backend/vm/vmsevelir.c \
	src/backend/compile.c \
	src/backend/graphviz/dot.c \
	src/backend/linker.c \
	src/optimizer/transform.c \
	src/optimizer/liveness.c \
	src/optimizer/optimize.c \
	src/preprocessor/tokenize.c \
	src/preprocessor/strtab.c \
	src/preprocessor/input.c \
	src/preprocessor/directive.c \
	src/preprocessor/preprocess.c \
	src/preprocessor/macro.c \
	src/parser/typetree.c \
	src/parser/symtab.c \
	src/parser/parse.c \
	src/parser/statement.c \
	src/parser/initializer.c \
	src/parser/expression.c \
	src/parser/declaration.c \
	src/parser/eval.c

LIBDIR_SOURCE = $(SRCDIR)/include/stdlib
LIBDIR_TARGET = $(LIBDIR)/lacc/include
TARGET = kcc
OBJS=$(SOURCES:%.c=%.o)

BUILTIN = \
	src/backend/vm/builtin/vmbuiltin.c \
	src/backend/vm/builtin/vmacpconv.c \
	src/util/string.c \
	src/kccutil.c

JIT = \
	src/backend/x86_64/builtin/jitbuiltin.c \
	src/backend/vm/builtin/vmacpconv.c

EXTOBJ = \
	src/_extdll/ext.c \
	src/_extdll/ext/fileio.c \
	src/_extdll/ext/regex.c \
	src/_extdll/ext/timer.c \
	src/_extdll/ext/zip_unzip.c \
	src/_extdll/lib/fileio/fileio_.c

all: $(TARGET)

$(TARGET): bin/bootstrap/kcc bin/bootstrap/kccbltin.so bin/bootstrap/kccjit.so bin/bootstrap/kccext.so
	cp -f bin/bootstrap/kcc .
	cp -f bin/bootstrap/kccbltin.so .
	cp -f bin/bootstrap/kccjit.so .
	cp -f bin/bootstrap/kccext.so .

bin/bootstrap/kcc:
	@mkdir -p $(@D)
	for file in $(SOURCES) ; do \
		target=$(@D)/$$(basename $$file .c).o ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target -D'LACC_STDLIB_PATH="$(LIBDIR_SOURCE)"' ; \
	done
	$(CC) $(@D)/*.o -o $@ -Wl,-rpath,'$$ORIGIN' -ldl

bin/bootstrap/kccbltin.so:
	@mkdir -p $(@D)
	for file in $(BUILTIN) ; do \
		target=$(@D)/$$(basename $$file .c).o ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target -D'LACC_STDLIB_PATH="$(LIBDIR_SOURCE)"' ; \
	done
	$(CC) $(@D)/vmbuiltin.o $(@D)/vmacpconv.o $(@D)/kccutil.o $(@D)/string.o -shared -Wl,-rpath,'$$ORIGIN' -o $@ -lm

bin/bootstrap/kccjit.so:
	@mkdir -p $(@D)
	for file in $(JIT) ; do \
		target=$(@D)/$$(basename $$file .c).o ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target -D'LACC_STDLIB_PATH="$(LIBDIR_SOURCE)"' ; \
	done
	$(CC) $(@D)/jitbuiltin.o $(@D)/vmacpconv.o $(@D)/kccutil.o $(@D)/string.o -shared -Wl,-rpath,'$$ORIGIN' -o $@ -lm

bin/bootstrap/kccext.so: bin/bootstrap/libonig.a
	@mkdir -p $(@D)
	for file in $(EXTOBJ) ; do \
		target=$(@D)/$$(basename $$file .cpp).o ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target -D'LACC_STDLIB_PATH="$(LIBDIR_SOURCE)"' ; \
	done
	$(CC) $(CFLAGS) -fPIC -Iinclude -c src/_extdll/lib/zip/miniz.c -o $(@D)/miniz.o -D'LACC_STDLIB_PATH="$(LIBDIR_SOURCE)"' ; \
	$(CC) $(@D)/ext.o $(@D)/fileio.o $(@D)/regex.o $(@D)/timer.o $(@D)/zip_unzip.o $(@D)/miniz.o -shared -Wl,-rpath,'$$ORIGIN' -o $@ -lm -L$(@D) -lonig

bin/bootstrap/libonig.a:
	cd src/_extdll/lib/onig; \
	autoreconf -vfi; \
	./configure --with-pic; \
	make
	cp -f src/_extdll/lib/onig/src/.libs/libonig.a $(@D)/libonig.a

test-8cc: $(TARGET)
	sh ./test/test-8cc/test.sh

test-qcc: $(TARGET)
	sh ./test/test-qcc/test.sh

test-lacc: $(TARGET)
	sh ./test/test-lacc/test.sh

test-picoc: $(TARGET)
	sh ./test/test-picoc/test.sh
	sh ./test/test-picoc/csmith.sh
	sh ./test/test-picoc/csmith.sh -j

test: test-8cc test-qcc test-lacc test-picoc

install: bin/release/kcc
	mkdir -p $(LIBDIR_TARGET)
	cp $(LIBDIR_SOURCE)/*.h $(LIBDIR_TARGET)/
	cp $? $(BINDIR)/kcc

uninstall:
	rm -rf $(LIBDIR_TARGET)
	rm $(BINDIR)/kcc

clean:
	rm -rf bin
	rm -f test/*.out test/*.txt test/*.s
	cd src/_extdll/lib/onig; make clean

.PHONY: install uninstall clean test \
	test-c89 test-c99 test-c11 test-gnu test-sqlite
