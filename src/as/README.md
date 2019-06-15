# Assembler Information
## Opcodes
The table below lists all opcodes and their accepted operand formats.
| Opcode Format         | Function                  | RTL                                           |
|-----------------------|---------------------------|-----------------------------------------------|
| `ADD reg,reg,reg`     | Signed addition           | `reg <- reg + reg; setcc`                     |
| `ADD reg,reg,imm`     | Signed addition           | `reg <- reg + imm; setcc`                     |
| `AND reg,reg,reg`     | Bitwise AND               | `reg <- reg & reg; setcc`                     |
| `AND reg,reg,imm`     | Bitwise AND               | `reg <- reg & imm; setcc`                     |
| `BR addr`             | Branch (unconditional)    | `PC <- PC^ + addr` (same as `BRnzp`)          |
| `BRn addr`            | Branch (negative)         | `PC <- PC^ + addr`                            |
| `BRz addr`            | Branch (zero)             | `PC <- PC^ + addr`                            |
| `BRp addr`            | Branch (positive)         | `PC <- PC^ + addr`                            |
| `BRnz addr`           | Branch (negative,zero)    | `PC <- PC^ + addr`                            |
| `BRnp addr`           | Branch (negative,positive)| `PC <- PC^ + addr`                            |
| `BRzp addr`           | Branch (zero,positive)    | `PC <- PC^ + addr`                            |
| `BRnzp addr`          | Branch (unconditional)    | `PC <- PC^ + addr`                            |
| `JMP reg`             | Jump                      | `PC <- reg`                                   |
| `JSR addr`            | Call subroutine           | `R7 <- PC^; PC <- PC^ + (addr << 1)`          |
| `JSRR reg`            | Call subroutine           | `R7 <- PC^; PC <- reg`                        |
| `LDB reg,reg,addr`    | Load byte                 | `reg <- mem[reg + addr]; setcc`               |
| `LDW reg,reg,addr`    | Load word                 | `reg <- mem[reg + (addr << 1)]; setcc`        |
| `LDI reg,reg,addr`    | Load word (indirect)      | `reg <- mem[mem[reg + (addr << 1)]]; setcc`   |
| `LEA reg,addr`        | Load effective address    | `reg <- PC^ + (addr << 1); setcc`             |
| `NOT reg,reg`         | Bitwise NOT               | `reg <- ~reg; setcc`                          |
| `RET`                 | Return from subroutine    | `PC <- R7`                                    |
| `RTI`                 | Return from interrupt     | TODO (it's quite complex)                     |
| `LSHF reg,reg,imm`    | Left shift                | `reg <- reg << imm; setcc`                    |
| `RSHFL reg,reg,imm`   | Right shift (logical)     | `reg <- reg >> imm; setcc`                    |
| `RSHFA reg,reg,imm`   | Right shift (arithmetic)  | `reg <- reg >> imm; setcc`                    |
| `STB reg,reg,addr`    | Store byte                | `mem[reg + addr] <- reg`                      |
| `STW reg,reg,addr`    | Store word                | `mem[reg + (addr << 1)] <- reg`               |
| `STI reg,reg,addr`    | Store word (indirect)     | `mem[mem[reg + (addr << 1)]] <- reg`          |
| `TRAP addr`           | Call trap service routine | `R7 <- PC; PC <- mem[addr << 1]`              |
| `XOR reg,reg,reg`     | Bitwise XOR               | `reg <- reg ^ reg; setcc`                     |
| `XOR reg,reg,imm`     | Bitwise XOR               | `reg <- reg ^ imm; setcc`                     |

Notes:
  * For the store instructions (`STB`, `STW`, `STI`) the first operand is the
    register to store while the second operand is the base address register.
  * `PC^` refers to the incremented `PC` value (`PC` is incremented by 2 during
    each fetch cycle).
  * `setcc` means the instruction sets the condition codes (`nzp`) used for
    conditional branches.

## Instruction Format
`LABEL: OPCODE OPERANDS ; COMMENTS`

### Format Details
`LABEL`:
  * Memory address identifier.
  * Alphanumeric, case-sensitive, underscore (_) allowed.
  * 20 characters maximum.
  * Label declaration must be terminated with a colon (:).
  * Label declaration can exist on a line of its own or preceding opcode.

`OPCODE`:
  * Instruction mnemonic.
  * Case-insensitive.
  * Includes LC-3 instructions and assembler pseudo-ops.
    * Pseudo-ops preceded by a period (.).

`OPERANDS`:
  * Instruction operands.
  * Zero or more required depending on opcode.
  * Must appear on same line as opcode.
  * Operands separated by a comma (,).
  * For multi-operand instructions, format is `dest,srcA,srcB`
  * Integer literals can be represented in decimal or hexadecimal.
    * Hexadecimal indicated by preceding 'x' or '0x' (case-insensitive).
  * Operand types: register, immediate, address.
    * Register:
      * References a general purpose register.
      * Specified by 'R' followed by a digit 0-7 (e.g. `R3`).
    * Immediate:
      * Constant value embedded into instruction.
      * Used for arithmetic operations.
      * Must be an integer literal.
    * Address:
      * References a memory location.
      * Can appear as either integer literal or label name (case-sensitive).
      * Register-relative, PC-relative, or absolute address embedded into
        instruction.
        * **Register-relative addressing** creates an absolute address by adding
          the embedded value to the value of a general purpose register.
        * **PC-relative addressing** creates an absolute address by adding the
          embedded value to the current `PC` value. Note that `PC` is
          incremented by 2 during each instruction fetch, so the `PC` value at
          the time of address creation will point to the start of the *next*
          instruction.
        * **Absolute addressing uses** the value embedded in the instruction as
          an absolute memory address. `TRAP` is the only instruction to use
          absolute addressing.

`COMMENT`:
  * Preceded by a semicolon (;).
  * Everything following semicolon until end-of-line ignored by the assembler.

### Format Examples
```
START:                      ; label by itself
BEGIN:  RET                 ; label w/ opcode, no operands

        ; operand types
        JMP R7              ; register
        BR START            ; address (label)
        BRnz -1             ; address (literal, dec)
        TRAP 0x25           ; address (literal, hex)
        NOT R0, R6          ; register & register
        LEA R0, DATA0       ; register & address (as label)
        LEA R1, 60          ; register & address (literal, dec)
        LEA R2, 0x3C        ; register & address (literal, hex)
        LDW R3, R5, DATA1   ; register & register & address (label)
        STW R3, R5, 2       ; register & register & address (literal)
        ADD R0, R1, R2      ; register & register & register
        ADD R3, R4, -2      ; register & register & immediate (dec)
        ADD R5, R6, x1E     ; register & register & immediate (hex)
        ADD R7, R0, START   ; NOT ALLOWED! ADD does not take an address

DATA0:  .FILL 0xBEEF        ; pseudo-op w/ hex operand
DATA1:  .FILL 1337          ; pseudo-op w/ dec operand
```
