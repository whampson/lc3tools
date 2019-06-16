# lc3tools
An implementation of the LC-3 instruction set architecture and assorted tools.

## Tools
| Name        |  Status       | Notes                     |
| ----------- | ------------- | ------------------------- |
| `lc3emu`    | In-progress   | Emulator/Debugger         |
| `lc3as`     | Planned       | Assembler                 |
| `lc3disas`  | Planned       | Disassembler              |
| `lc3cc`     | Planned       | C Compiler                |

## The LC-3 Instruction Set Architecture
LC-3 is a simple 16-bit instruction set architecture designed for teaching
purposes. It was developed by Yale Patt of UT-Austin and Sanjay Patel of UIUC
and described in their textbook
*Introduction to Computing Systems: From Bits and Gates to C and Beyond*
(ISBN 0-07-246750-9). The LC-3 is a load-store architecture and features 16
instructions, 8 general purpose registers, conditional and unconditional jumps,
and can manipulate integers through addition, bitwise operations, and shifting.
Floating-point operations are not supported by the ISA.

### The LC-3b
LC-3b is a minor variation of the LC-3 ISA by Yale Patt not described in the
aforementioned text. However, partial description of the ISA can be found on
Yale Patt's university webpage:
    http://users.ece.utexas.edu/~patt/18f.460N/handouts/appA.pdf.

### The LC-3c
LC-3c is an unofficial extension of the LC-3 ISA, created by me, that is a cross
between the original LC-3 found in *From Bits to Gates...* and the LC-3b. The
aim of the LC-3c is to make a practical ISA based around the original LC-3 with
the added enhancements of the LC-3b.

From this point on, all mentions of "the LC-3" will refer to the LC-3c, unless
otherwise noted.

#### Summary of Changes
Below is a list of changes made to the LC-3 to create the LC-3c.
  - Memory is now byte-addressable.
  - `LD`, `ST` instructions (data storage using PC-relative addressing) have
    been removed.
  - `LDI`, `STI` instructions no longer use PC-relative addressing and instead
     use a base register.
     - Note: these instructions have been removed in Patt's description of the
     LC-3b, but I've decided to keep them because I find them useful.
  - `LDB`, `STB` instructions have been added to manipulate individual bytes of
    memory.
  - `LDR`, `STR` instructions have been renamed to `LDW`, `STW` to indicate that
    the instructions manipulate words of memory.
  - `SHF` instruction has been added to allow for arithmetic and logical shifts.
  - `XOR` instruction has been added and bitwise NOT ALU operation has been
    replaced with bitwise XOR. The instruction encoding for `NOT` remains
    unchanged.
