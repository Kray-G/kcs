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

MAINSRC = \
	src/kcc.c

SOURCES = \
	src/kccmain.c \
	src/kccutil.c \
	src/context.c \
	src/util/argparse.c \
	src/util/fmemopen.c \
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

LIBDIR_TARGET = $(LIBDIR)
TARGETDIR = bin/release
TARGET = $(TARGETDIR)/kcc
OBJS=$(SOURCES:%.c=%.o)

BUILTIN = \
	src/backend/vm/builtin/vmbuiltin.c \
	src/backend/vm/builtin/vmacpconv.c \

JIT = \
	src/backend/x86_64/builtin/jitbuiltin.c \
	src/backend/vm/builtin/vmacpconv.c

EXTSRC = \
	src/_extdll/ext.c \
	src/_extdll/ext/fileio.c \
	src/_extdll/ext/regex.c \
	src/_extdll/ext/timer.c \
	src/_extdll/ext/zip_unzip.c \
	src/_extdll/ext/sqlite3x.c \
	src/_extdll/lib/fileio/_fileio.c \
	src/_extdll/lib/sqlite3/sqlite3.c \
	src/_extdll/lib/zip/miniz.c

all: $(TARGET)

$(TARGET): bin/bootstrap/kcc bin/bootstrap/kccbltin.so bin/bootstrap/kccjit.so bin/bootstrap/kccext.so
	mkdir -p $(TARGETDIR)
	cp -f bin/bootstrap/kcc         $(TARGETDIR)/
	cp -f bin/bootstrap/libkcc.so   $(TARGETDIR)/
	cp -f bin/bootstrap/kccbltin.so $(TARGETDIR)/
	cp -f bin/bootstrap/kccjit.so   $(TARGETDIR)/
	cp -f bin/bootstrap/kccext.so   $(TARGETDIR)/
	cp -f bin/bootstrap/kcc         .
	cp -f bin/bootstrap/libkcc.so   .
	cp -f bin/bootstrap/kccbltin.so .
	cp -f bin/bootstrap/kccjit.so   .
	cp -f bin/bootstrap/kccext.so   .

bin/bootstrap/kcc: bin/bootstrap/libkcc.so
	@mkdir -p $(@D)
	$(CC) $(MAINSRC) -o $@ -Wl,-rpath,'$$ORIGIN' -L$(@D) -lkcc

bin/bootstrap/libkcc.so:
	@mkdir -p $(@D)
	for file in $(SOURCES) ; do \
		target=$(@D)/$$(basename $$file .c).o ; \
		echo $$target ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target ; \
	done
	$(CC) $(@D)/*.o -o $@ -shared -Wl,-rpath,'$$ORIGIN' -ldl

bin/bootstrap/kccbltin.so:
	@mkdir -p $(@D)/bltin
	for file in $(BUILTIN) ; do \
		target=$(@D)/bltin/$$(basename $$file .c).o ; \
		echo $$target ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target ; \
	done
	$(CC) $(@D)/bltin/*.o $(@D)/kccutil.o $(@D)/string.o -shared -Wl,-rpath,'$$ORIGIN' -o $@ -lm

bin/bootstrap/kccjit.so:
	@mkdir -p $(@D)/jit
	for file in $(JIT) ; do \
		target=$(@D)/jit/$$(basename $$file .c).o ; \
		echo $$target ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target ; \
	done
	$(CC) $(@D)/jit/*.o $(@D)/kccutil.o $(@D)/string.o -shared -Wl,-rpath,'$$ORIGIN' -o $@ -lm

bin/bootstrap/kccext.so: bin/bootstrap/libonig.a
	@mkdir -p $(@D)/ext
	for file in $(EXTSRC) ; do \
		target=$(@D)/ext/$$(basename $$file .c).o ; \
		echo $$target ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target ; \
	done
	$(CC) $(@D)/ext/*.o -shared -Wl,-rpath,'$$ORIGIN' -o $@ -pthread -lm -L$(@D) -lonig

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
	cp -r kccrt/                $(LIBDIR_TARGET)
	cp $(TARGETDIR)/kcc         $(BINDIR)/kcc
	cp $(TARGETDIR)/libkcc.so   $(BINDIR)/libkcc.so
	cp $(TARGETDIR)/kccbltin.so $(BINDIR)/kccbltin.so
	cp $(TARGETDIR)/kccjit.so   $(BINDIR)/kccjit.so
	cp $(TARGETDIR)/kccext.so   $(BINDIR)/kccext.so

uninstall:
	rm -rf $(LIBDIR_TARGET)/kccrt
	rm -f $(BINDIR)/kcc
	rm -f $(BINDIR)/libkcc.so
	rm -f $(BINDIR)/kccbltin.so
	rm -f $(BINDIR)/kccjit.so
	rm -f $(BINDIR)/kccext.so

clean:
	rm -rf bin
	rm -f test/*.out test/*.txt test/*.s
	cd src/_extdll/lib/onig; make clean

.PHONY: install uninstall clean test \
	test-8cc test-qcc test-lacc test-picoc

