Smaller C is a simple and small single-pass C compiler,
currently supporting most of the C language common between C89/ANSI C
and C99 (minus some C89 and plus some C99 features).

Currently it generates 16-bit and 32-bit 80386+ assembly code for NASM
that can then be assembled and linked into DOS, Windows and Linux programs.

Code generation for MIPS CPUs is also supported (primarily for RetroBSD).

Code generation for the TR3200 CPU and VASM is supported (see Trillek).

The compiler is capable of compiling itself.

The core compiler comes with a linker and a compiler driver (the driver
invokes the core compiler, the assembler, and the linker and supports
options similar to those of gcc).

There's no decent preprocessor in Smaller C as of now, but the compiler
driver can invoke gcc (or gcc.exe) for preprocessing if instructed.

The standard C library is work-in-progress and it's close to completion.

See the Wiki for more up-to-date details:
http://github.com/alexfru/SmallerC/wiki

For the lack of a better place, you can discuss Smaller C here:
https://hackaday.io/project/5569-smaller-c

Links:
NASM: http://nasm.us/
YASM: http://yasm.tortall.net/
CWSDPMI: http://homer.rice.edu/~sandmann/cwsdpmi/
HX DOS Extender: https://web.archive.org/web/20141003032346/http://www.japheth.de/
RetroBSD: http://retrobsd.org/
VASM: http://sun.hasenbraten.de/vasm/
Trillek: http://trillek.org/

Normative and other useful documents on C:
C99 + TC1 + TC2 + TC3, WG14 N1256:
  http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf
Rationale for C99:
  http://www.open-std.org/jtc1/sc22/wg14/www/docs/C99RationaleV5.10.pdf
The New C Standard: An Economic and Cultural Commentary:
  http://www.knosof.co.uk/cbook/cbook.html

