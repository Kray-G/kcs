# KCC (Kray-G C Compiler)

Small but Useful C Execution Environment by JIT(x64)/VM.

## Overview

KCC is a compact C Compiler and Interpreter.
This compiler has a JIT routine to run the code on the fly.
And this has also a VM execution routine to run the code on the VM.
It is basically same except the performance.

The main feature is below.

*   The language is C, so it is easy to use for almost all programmers.
*   You can execute the code on the fly with JIT or VM like a scripting.
*   KCC provides a standard C library.
    *   Not completed so far, though. 
    *   See [doc/cstdlib.md](doc/cstdlib.md) for the current status.
*   KCC also provides useful libraries by default like regex, zip, and so on.
    *   Some libraries will be coming soon.
    *   Please let me know if you have library which you want to use.

## Getting Started

### Prerequisites

Now KCC was tested only on a platform below.

*   Windows (x64) with Visual Studio 2017 (Express Edition).
*   Linux (x64) with gcc/g++.

I am waiting for pull requests to support any other platforms & compilers.

### Building & Testing

#### Windows

After installing Visual Studio and prepareing an environment, do make & test as below.

```
C:> make.cmd all test
```

#### Linux

Do make & test KCC on Linux x64 as below.

```
$ make all test
```

### Architecture

Here is the basic block diagram of KCC.

```
+-----------------------------------------------------------+
|                                                           |
|                        Your C Code                        |
|                                                           |
+-----------------------------+-----------------------------+
                             ---
+-----------------------------+-----------------------------+
| KCC - Kray-G C Compiler & Interpreter                     |
|                                                           |   +---------------------------------------+
|   +---------------------------------------------------+   |   |  stdio, stdlib, string...             |
|   |                   Library Code                    +-------+    bigint, regex, xml, json...        |
|   +---------------------------------------------------+   |   |      zip/unzip, sqlite3, libcurl...   |
|                                                           |   +-------------------+-------------------+
|   +-----------------------+   +-----------------------+   |                       |
|   |     JIT builtin       |   |     VM builtin        |   |   +-------------------+-------------------+
|   |        kccjit.dll     |   |       kccbltin.dll    |   |   |             Extension DLL             |
|   +-----------------------+   +-----------------------+   |   |                  kccext.dll           |
|   +-----------------------+   +-----------------------+   |   +---+-----------------------------------+
|   |     JIT Executor      |   |    VM Interpreter     |   |       |       |       |       |       |
|   +-----------+-----------+   +-----------+-----------+   |   +-------+   :       :       :       :
|               |                           |               |   | Some  |   .       .       .       .
|   +===========+=============+=============+===========+   |   |   OSS |   . . . . . . . . . . . . . . .
|   |     x86_64 Compiler     |       VM Compiler       |   |   +-------+
|   |                      Backend                      |   |
|   +-------------------------+-------------------------+   |
|   |                                                   |<------ Based on lacc
|   |                        [ Modified *lacc* core ]   |   |       with additional VM backends.
|   +===================================================+   |
|                                                           |
+-----------------------------------------------------------+

```

## Usage

### Run on the x64 JIT

For JIT use the option `-j`.
Use the option `-J` if you want to see the x64 assembly code.

```c
// Run on the x64 JIT.
$ kcc -j program.c
// Show the x64 assembly code.
$ kcc -J program.c
```

### Run on the VM

No options or use the option `-x` to run on the VM.
Use the option `-X` if you want to see the VM instructions.

```c
// Run on the VM.
$ kcc program.c
$ kcc -x program.c
// Show the VM instruction code.
$ kcc -X program.c
```

## Examples

### Fibonacci

#### Source Code

```c
#include <stdio.h>

int fib(int n)
{
    if (n < 3) return n;
    return fib(n-2) + fib(n-1);
}

int main()
{
    return printf("%d\n", fib(34));
}
```

#### Execution Sample

The results are below.

```c
$ kcc -j fib.c
9227465

$ kcc fib.c
9227465
```

#### Benchmark

Maybe it will depend on the environment.
Here is one of samples on Windows.
For the reference, it shows a result of Ruby and Python.

|           | KCC VM(64bit) | KCC JIT(x64) | Ruby 2.4.0 | Ruby 2.6.3 | Python 2.7.13 |              |
| --------- | :-----------: | :----------: | :--------: | :--------: | :-----------: | ------------ |
| `fib(34)` |     0.718     |  **0.062**   |   1.171    |   0.734    |     1.578     | (in seconds) |

Ruby 2.6.3 is very fast against my expectations.

#### Compiled Code

Here is a compiled x64 code and a VM instructions.
Those are very long and it includes also lots of library code,
so it shows the fib function only.

##### x64 assembly code

```asm
$ kcc -J fib.c
...(omitted)...
                                  fib
0000BCE6: 55                            pushq   %rbp
0000BCE7: 48 89 E5                      movq    %rsp, %rbp
0000BCEA: 53                            pushq   %rbx
0000BCEB: 41 54                         pushq   %r12
0000BCED: 41 55                         pushq   %r13
0000BCEF: 41 56                         pushq   %r14
0000BCF1: 48 83 EC 10                   subq    $16, %rsp
0000BCF5: 89 7D D8                      movl    %edi, -40(%rbp)
                                  .L2343
0000BCF8: 83 7D D8 03                   cmpl    $3, -40(%rbp)
0000BCFC: 0F 8D 10 00 00 00             jge     .L2345
                                  .L2344
0000BD02: 8B 45 D8                      movl    -40(%rbp), %eax
0000BD05: 48 8D 65 E0                   leaq    -32(%rbp), %rsp
0000BD09: 41 5E                         popq    %r14
0000BD0B: 41 5D                         popq    %r13
0000BD0D: 41 5C                         popq    %r12
0000BD0F: 5B                            popq    %rbx
0000BD10: C9                            leave
0000BD11: C3                            ret
                                  .L2345
0000BD12: 8B 45 D8                      movl    -40(%rbp), %eax
0000BD15: B9 02 00 00 00                movl    $2, %ecx
0000BD1A: 29 C8                         subl    %ecx, %eax
0000BD1C: 89 C3                         movl    %eax, %ebx
0000BD1E: 89 DF                         movl    %ebx, %edi
0000BD20: E8 C1 FF FF FF                call    fib
0000BD25: 41 89 C4                      movl    %eax, %r12d
0000BD28: 8B 45 D8                      movl    -40(%rbp), %eax
0000BD2B: B9 01 00 00 00                movl    $1, %ecx
0000BD30: 29 C8                         subl    %ecx, %eax
0000BD32: 41 89 C5                      movl    %eax, %r13d
0000BD35: 44 89 EF                      movl    %r13d, %edi
0000BD38: E8 A9 FF FF FF                call    fib
0000BD3D: 41 89 C6                      movl    %eax, %r14d
0000BD40: 44 89 F0                      movl    %r14d, %eax
0000BD43: 44 01 E0                      addl    %r12d, %eax
0000BD46: 48 8D 65 E0                   leaq    -32(%rbp), %rsp
0000BD4A: 41 5E                         popq    %r14
0000BD4C: 41 5D                         popq    %r13
0000BD4E: 41 5C                         popq    %r12
0000BD50: 5B                            popq    %rbx
0000BD51: C9                            leave
0000BD52: C3                            ret
...(omitted)...
```

##### VM instructions

```asm
$ kcc -X fib.c
...(omitted)...
----------------------------------------------------------------
fib
      7856:     enter                   32
  .L2295
      7857:     push(32)                3 (0x3)
      7858:     push                    [BP-24] : n(i32)
      7859:     gt                      (i32)
      7860:     jnz                     * +15 <.L2296>
  .L2297
      7861:     push                    [BP-24] : n(i32)
      7862:     push(32)                2 (0x2)
      7863:     sub                     (i32)
      7864:     call                    * 7856 <fib>
      7865:     cleanup                 (8)
      7866:     pop                     [BP+8] : .t849(i32)
      7867:     push                    [BP-24] : n(i32)
      7868:     push(32)                1 (0x1)
      7869:     sub                     (i32)
      7870:     call                    * 7856 <fib>
      7871:     cleanup                 (8)
      7872:     push                    [BP+8] : .t849(i32)
      7873:     add                     (i32)
      7874:     ret                     (4)
  .L2296
      7875:     push                    [BP-24] : n(i32)
      7876:     ret                     (4)
...(omitted)...
```

###

## Todo List

I have a plan to do the followings when I have a time.

*   [ ] Providing all of Standard C Library.
*   [ ] Adding a library of SQLite3.
*   [ ] Adding a library of XML Parser.
*   [ ] Adding a library of JSON Parser.
*   [ ] Adding a library with libCurl.
*   [ ] Supporting encryption of Zip/Unzip.
*   [ ] Supporting jump table for switch-case.
*   [ ] One instruction for increment and decrement.
*   [ ] Combining VM instructions to improve the performance.
*   [ ] Building libkcc to use it as a shared library.

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.
For the licenses of libraries used internally, see [doc/licenses](doc/licenses) folder.

## Acknowledgments

I am very appreciative of many Open Source projects.

In particular, [lacc][], which is a very compact C compiler,
has a frontend and a backend separated well.
That was why I could add a VM very easily.
In fact I wrote my own C parser at first,
but I replaced it by [lacc][] because it was very easy to add a new backend.

Note that a lot of modifications were done for the old code base of [lacc][],
so it can not apply the latest one.
The main modifications are:

*   For Windows JIT, adapting calling convention between Microsoft x64 and System V.
*   Linking the compiled function for JIT on the fly.
*   Replacing types to build it with Visual Studio.
*   Some bug fixes.

And special thanks to the following products for useful test codes.

*   Test Code
    *   [lacc][] (MIT License)
    *   [8cc][] (MIT License)
    *   [qcc][] (MIT License)
    *   [picoc][] (BSD License)

[lacc]: https://github.com/larmel/lacc/
[8cc]: https://github.com/rui314/8cc/
[qcc]: https://github.com/maekawatoshiki/qcc/
[picoc]: https://github.com/jpoirier/picoc/

About library, thanks to the following amazing products.

*   Library
    *   [Miniz][] (MIT License)
    *   [Oniguruma][] (BSD License)
    *   [bigint] (Public Domain)

[Oniguruma]: https://github.com/kkos/oniguruma/
[Miniz]: https://github.com/richgel999/miniz
[bigint]: https://github.com/983/bigint

Lastly, thank you very much for all programmers in the world.
