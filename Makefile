.POSIX:
.SUFFIXES:
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib
SRCDIR = $(CURDIR)$(.CURDIR)

CC = gcc
CFLAGS = -O2 -Wno-missing-braces -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -std=gnu99 -DAES256=1

MAINSRC = \
	src/kcs.c

SOURCES = \
	src/kcsmain.c \
	src/kcsutil.c \
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
TARGET = $(TARGETDIR)/kcs
OBJS=$(SOURCES:%.c=%.o)

BUILTIN = \
	src/backend/vm/builtin/vmbuiltin.c \
	src/backend/vm/builtin/vmacpconv.c \

JIT = \
	src/backend/x86_64/builtin/jitbuiltin.c \
	src/backend/vm/builtin/vmacpconv.c

EXTSRC = \
	src/_extdll/ext.c \
	src/_extdll/ext/aesx.c \
	src/_extdll/ext/fileio.c \
	src/_extdll/ext/regex.c \
	src/_extdll/ext/timer.c \
	src/_extdll/ext/zip_unzip.c \
	src/_extdll/ext/sqlite3x.c \
	src/_extdll/lib/aes/aes.c \
	src/_extdll/lib/fileio/_fileio.c \
	src/_extdll/lib/sqlite3/sqlite3.c \
	src/_extdll/lib/zip/miniz.c

all: ext_json.c $(TARGET)

$(TARGET): bin/bootstrap/kcs bin/bootstrap/kcsbltin.so bin/bootstrap/kcsjit.so bin/bootstrap/kcsext.so
	mkdir -p $(TARGETDIR)
	cp -f bin/bootstrap/kcs         $(TARGETDIR)/
	cp -f bin/bootstrap/libkcs.so   $(TARGETDIR)/
	cp -f bin/bootstrap/kcsbltin.so $(TARGETDIR)/
	cp -f bin/bootstrap/kcsjit.so   $(TARGETDIR)/
	cp -f bin/bootstrap/kcsext.so   $(TARGETDIR)/
	cp -f bin/bootstrap/kcs         .
	cp -f bin/bootstrap/libkcs.so   .
	cp -f bin/bootstrap/kcsbltin.so .
	cp -f bin/bootstrap/kcsjit.so   .
	cp -f bin/bootstrap/kcsext.so   .

ext_json.c: myacc kcsrt/libsrc/kcs/json.y
	./myacc -y __json_yy -Y JSON_YY kcsrt/libsrc/kcs/json.y
	mv -f y.tab.c kcsrt/libsrc/kcs/ext_json.c

myacc:
	$(CC) $(CFLAGS) -o myacc utility/myacc.c

bin/bootstrap/kcs: bin/bootstrap/libkcs.so
	@mkdir -p $(@D)
	$(CC) $(MAINSRC) -o $@ -Wl,-rpath,'$$ORIGIN' -L$(@D) -lkcs

bin/bootstrap/libkcs.so:
	@mkdir -p $(@D)
	for file in $(SOURCES) ; do \
		target=$(@D)/$$(basename $$file .c).o ; \
		echo $$target ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target ; \
	done
	$(CC) $(@D)/*.o -o $@ -shared -Wl,-rpath,'$$ORIGIN' -ldl

bin/bootstrap/kcsbltin.so:
	@mkdir -p $(@D)/bltin
	for file in $(BUILTIN) ; do \
		target=$(@D)/bltin/$$(basename $$file .c).o ; \
		echo $$target ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target ; \
	done
	$(CC) $(@D)/bltin/*.o $(@D)/kcsutil.o $(@D)/string.o -shared -Wl,-rpath,'$$ORIGIN' -o $@ -lm

bin/bootstrap/kcsjit.so:
	@mkdir -p $(@D)/jit
	for file in $(JIT) ; do \
		target=$(@D)/jit/$$(basename $$file .c).o ; \
		echo $$target ; \
		$(CC) $(CFLAGS) -fPIC -Iinclude -c $$file -o $$target ; \
	done
	$(CC) $(@D)/jit/*.o $(@D)/kcsutil.o $(@D)/string.o -shared -Wl,-rpath,'$$ORIGIN' -o $@ -lm

bin/bootstrap/kcsext.so: bin/bootstrap/libonig.a
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

install: bin/release/kcs
	mkdir -p $(LIBDIR_TARGET)
	cp -r kcsrt/                $(LIBDIR_TARGET)
	cp $(TARGETDIR)/kcs         $(BINDIR)/kcs
	cp $(TARGETDIR)/libkcs.so   $(BINDIR)/libkcs.so
	cp $(TARGETDIR)/kcsbltin.so $(BINDIR)/kcsbltin.so
	cp $(TARGETDIR)/kcsjit.so   $(BINDIR)/kcsjit.so
	cp $(TARGETDIR)/kcsext.so   $(BINDIR)/kcsext.so

uninstall:
	rm -rf $(LIBDIR_TARGET)/kcsrt
	rm -f $(BINDIR)/kcs
	rm -f $(BINDIR)/libkcs.so
	rm -f $(BINDIR)/kcsbltin.so
	rm -f $(BINDIR)/kcsjit.so
	rm -f $(BINDIR)/kcsext.so

clean:
	rm -rf bin
	rm -f test/*.out test/*.txt test/*.s
	cd src/_extdll/lib/onig; make clean

.PHONY: install uninstall clean test \
	test-8cc test-qcc test-lacc test-picoc

