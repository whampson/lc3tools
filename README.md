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

### Instruction Table
A table of all valid instructions and their encodings.
All offsets are word offsets, except for `boffset6`, which is a byte offset.

            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      ADD+  │0 0 0 1│ DR  │ SR1 │A│ op.spec │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      AND+  │0 1 0 1│ DR  │ SR1 │A│ op.spec │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      BR    │0 0 0 0│n z p│    pcoffset9    │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      JMP   │1 1 0 0│0 0 0│BaseR│0 0 0 0 0 0│
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      JSR(R)│0 1 0 0│A│  operand.specifier  │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      LDB+  │0 0 1 0│ DR  │BaseR│ boffset6  │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      LDW+  │0 1 1 0│ DR  │BaseR│  offset6  │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      LDI+  │1 0 1 0│ DR  │BaseR│  offset6  │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      LEA+  │1 1 1 0│ DR  │    pcoffset9    │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      RTI   │1 0 0 0│0 0 0 0 0 0 0 0 0 0 0 0│
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      SHF+  │1 1 0 1│ DR  │ SR  │A│D| imm4  │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      STB   │0 0 1 1│ SR  │BaseR│ boffset6  │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      STW   │0 1 1 1│ SR  │BaseR│  offset6  │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      STI   │1 0 1 1│ SR  │BaseR│1│  imm5   │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      TRAP  │1 1 1 1│0 0 0 0│   trapvect8   │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
            ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
      XOR+  │1 0 0 1│ DR  │ SR1 │A│ op.spec │
            └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
+Instruction modifies condition codes.

### Memory Map
The LC-3 uses a flat memory model with no protection. The stack grows towards
lower addresses (i.e. towards `0000`).

      0000  +-------------------------+
            |    Trap Vector Table    |
      0200  +-------------------------+
            | Interrupt Vector Table  |
      0400  +-------------------------+
            |                         |
            |    Operating System     |
            |                         |
      3000  +-------------------------+ <-- Supervisor Stack Pointer
            |                         |
            |                         |
            |                         |
            |   User Program Space    |
            |                         |
            |                         |
            |                         |
      FE00  +-------------------------+ <-- User Stack Pointer
            |    Memory-Mapped I/O    |
      FFFF  +-------------------------+

#### Memory-Mapped I/O
| Address   | Direction | Register Name | Function  |
| --------- | --------- | ------------- | --------- |
| `0xFE00`  | R/W       | KBSR          | Keyboard status register<ul><li>bit [15] - status bit, a character has been typed</li><li>bit [14] - interrupt enable, raise an interrupt when a character is typed</li><li> bits [13:0] - (not used)</ul>
| `0xFE02`  | R         | KBDR          | Keyboard data register<ul><li>bits [15:8] - (not used)</li><li>bits [7:0] - the typed character (ASCII)</li></ul>
| `0xFE04`  | R/W       | DSR           | Display status register<ul><li>bit [15] - status bit, ready to receive another character to print</li><li>bit [14] - interrupt enable, raise an interrupt when a character has finished printing</li><li> bits [13:0] - (not used)</ul>
| `0xFE06`  | W         | DDR           | Display data register<ul><li>bits [15:8] - (not used)</li><li>bits [7:0] - the character to print (ASCII)</li></ul>
| `0xFE10`  | W         | ICCR*         | Interrupt controller command register<ul><li>bits [15:8] - (not used)</li><li>bits [7:0] - interrupt controller command</li></ul>
| `0xFE12`  | R/W       | ICDR*         | Interrupt controller data register<ul><li>bits [15:8] - (not used)</li><li>bits [7:0] - data from/to interrupt controller</li></ul>
| `0xFFFE`  | R/W       | MCR           | Machine control register<ul><li>bit [15] - clock enable bit, instruction processing stops when cleared</li><li>bits [14:0] - (not used)</li></ul>

*Not included in the original LC-3 spec.

#### The Interrupt Controller
The interrupt controller was added to help implement the LC-3's interrupt
handling behavior; it is not a part of the original LC-3 specification. The
interrupt controller is loosely based on the Intel 8259 PIC. There are eight
interrupt lines, prioritized such that IR7 is the highest and IR0 is the lowest
(this is the opposite of the 8259). The interrupt controller has three internal
registers that contain the current state of interrupts and two additional
registers for configuration. The internal registers (IRR, ISR, IMR) are not
directly accessible to the CPU, while the configuration registers are accessible
(see table above).

| Register  | Description                                                     |
| --------- | --------------------------------------------------------------- |
| IRR       | Interrupt request register                                      |
| ISR       | In-service register                                             |
| IMR       | Interrupt mask register                                         |
| ICCR      | Command register                                                |
| ICDR      | Data register                                                   |

Below is a table of commands that can be issued to the interrupt controller via
ICCR. The argument or result of a command is supplied by ICDR.

| Command   | Name                        | Data                              |
| --------- | --------------------------- | --------------------------------- |
| `0x00`    | Read IMR                    | bitmask of disabled interrupts    |
| `0x01`    | Write IMR                   | bitmask of disabled interrupts    |
