# Emulator Information
## Instruction Encoding
LC-3 instructions are 16 bits wide.
The upper four bits are the opcode. The remaining 12 bits provide the operands.
The operand format varies per instruction.

### Terminology
| Term          | Meaning
|---------------|--------
| SR, SR1, SR2  | Source register; one of R0..R7 which specifies the register from which to obtain the source operand.
| DR            | Destination register; one of R0..R7 which specifies the register the result should be written to.
| BASER         | Base register; one of R0..R7 whose contents are added to a six-bit offset to obtain an absolute address.
| off6          | A six-bit signed value; added to the contents of a base register to obtain an absolute address.
| pcoff9        | A nine-bit signed value; added to the contents of PC to obtain an absolute address.
| pcoff11       | An 11-bit signed value; added to the contents of PC to obtain an absolute address.
| trapvect8     | An eight-bit unsigned value; used as an index into the Trap Vector Table to obtain the address of a trap service routine.
| imm4          | A four-bit unsigned value.
| imm5          | A five-bit signed value.
| A             | Argument bit; specifies the operand format for `ADD`, `AND`, `XOR`; specifies right-shift type for `SHF`.
| D             | Direction bit; specifies the direction of a shift operation.

### General Instruction Format
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              ADD+      │0 0 0 1│ DR  │ SR1 │A│ OP.SPEC │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              AND+      │0 1 0 1│ DR  │ SR1 │A│ OP.SPEC │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              BR        │0 0 0 0│n z p│     pcoff9      │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              JMP       │1 1 0 0│0 0 0│BASER│0 0 0 0 0 0│
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              JSR(R)    │0 1 0 0│A│  OPERAND.SPECIFIER  │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              LDB+      │0 0 1 0│ DR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              LDW+      │0 1 1 0│ DR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              LDI+      │1 0 1 0│ DR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              LEA+      │1 1 1 0│ DR  │     pcoff9      │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              RTI       │1 0 0 0│0 0 0 0 0 0 0 0 0 0 0 0│
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              SHF+      │1 1 0 1│ DR  │ SR  │A│D| imm4  │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              STB       │0 0 1 1│ SR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              STW       │0 1 1 1│ SR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              STI       │1 0 1 1│ SR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              TRAP      │1 1 1 1│0 0 0 0│   trapvect8   │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
              XOR+      │1 0 0 1│ DR  │ SR1 │A│ OP.SPEC │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
+Instruction modifies condition codes.

### Full Instruction Set
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    ADD DR,SR1,SR2      │0 0 0 1│ DR  │ SR1 │0│0 0│ SR2 │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    ADD DR,SR1,imm5     │0 0 0 1│ DR  │ SR1 │1│  imm5   │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    AND DR,SR1,SR2      │0 1 0 1│ DR  │ SR1 │0│0 0│ SR2 │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    AND DR,SR1,imm5     │0 1 0 1│ DR  │ SR1 │1│  imm5   │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    BR pcoff9           │0 0 0 0│n z p│     pcoff9      │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    JMP BASER           │1 1 0 0│0 0 0│BASER│0 0 0 0 0 0│
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    JSR pcoff11         │0 1 0 0│1│       pcoff11       │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    JSRR BASER          │0 1 0 0│0│0 0│BASER│0 0 0 0 0 0│
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    LDB DR,BASER,off6   │0 0 1 0│ DR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    LDW DR,BASER,off6   │0 1 1 0│ DR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    LDI DR,BASER,off6   │1 0 1 0│ DR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    LEA DR,pcoff9       │1 1 1 0│ DR  │     pcoff9      │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    NOT DR,SR           │1 0 0 1│ DR  │ SR  │1│1 1 1 1 1│
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    RET                 │1 1 0 0│0 0 0│1 1 1│0 0 0 0 0 0│
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    RTI                 │1 0 0 0│0 0 0 0 0 0 0 0 0 0 0 0│
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    LSHF DR,SR,imm4     │1 1 0 1│ DR  │ SR  │0│0| imm4  │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    RSHFL DR,SR,imm4    │1 1 0 1│ DR  │ SR  │0│1| imm4  │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    RSHFA DR,SR,imm4    │1 1 0 1│ DR  │ SR  │1│1| imm4  │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    STB SR,BASER,off6   │0 0 1 1│ SR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    STW SR,BASER,off6   │0 1 1 1│ SR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    STI SR,BASER,off6   │1 0 1 1│ SR  │BASER│   off6    │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    TRAP trapvect8      │1 1 1 1│0 0 0 0│   trapvect8   │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    XOR DR,SR1,SR2      │1 0 0 1│ DR  │ SR1 │0│0 0│ SR2 │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
                        ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
    XOR DR,SR1,imm5     │1 0 0 1│ DR  │ SR1 │1│  imm5   │
                        └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘

## Memory Map
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

### Memory-Mapped I/O
| Address   | Direction | Register Name | Function  |
| --------- | --------- | ------------- | --------- |
| `0xFE00`  | R/W       | KBSR          | Keyboard status register<ul><li>bit [15] - status bit, a character has been typed</li><li>bit [14] - interrupt enable, raise an interrupt when a character is typed</li><li> bits [13:0] - (not used)</ul>
| `0xFE02`  | R         | KBDR          | Keyboard data register<ul><li>bits [15:8] - (not used)</li><li>bits [7:0] - the typed character (ASCII)</li></ul>
| `0xFE04`  | R/W       | DSR           | Display status register<ul><li>bit [15] - status bit, ready to receive another character to print</li><li>bit [14] - interrupt enable, raise an interrupt when a character has finished printing</li><li> bits [13:0] - (not used)</ul>
| `0xFE06`  | W         | DDR           | Display data register<ul><li>bits [15:8] - (not used)</li><li>bits [7:0] - the character to print (ASCII)</li></ul>
| `0xFE10`  | W         | ICCR          | Interrupt controller command register<ul><li>bits [15:8] - (not used)</li><li>bits [7:0] - interrupt controller command</li></ul>
| `0xFE12`  | R/W       | ICDR          | Interrupt controller data register<ul><li>bits [15:8] - (not used)</li><li>bits [7:0] - data from/to interrupt controller</li></ul>
| `0xFFFE`  | R/W       | MCR           | Machine control register<ul><li>bit [15] - clock enable bit, instruction processing stops when cleared</li><li>bits [14:0] - (not used)</li></ul>

## The Interrupt Controller
The interrupt controller was added to help implement the LC-3's interrupt
handling behavior; it is not a part of the original LC-3 specification. The
interrupt controller is loosely based on the Intel 8259 PIC. There are eight
interrupt lines, prioritized such that IR7 is the highest and IR0 is the lowest
(this is the opposite of the 8259). The interrupt controller has three internal
registers that contain the current state of interrupts and two additional I/O
registers for reading and writing the internal registers.

### Interrupt Controller Registers
| Register  | Type      | Name                                                |
| --------- | ----------| --------------------------------------------------- |
| IRR       | Internal  | Interrupt Request Register                          |
| ISR       | Internal  | In-service Register                                 |
| IMR       | Internal  | Interrupt Mask Register                             |
| ICCR      | I/O       | Interrupt Controller Command Register               |
| ICDR      | I/O       | Interrupt Controller Data Register                  |

### Interrupt Controller Commands
| Command   | Direction | Function        | Data                              |
| --------- | ----------|---------------- | --------------------------------- |
| `0x01`    | Read      | Get IRR value   | bitmask of requested interrupts   |
| `0x02`    | Read      | Get ISR value   | bitmask of in-service interrupts  |
| `0x03`    | Read      | Get IMR value   | bitmask of disabled interrupts    |
| `0x04`    | Write     | Set IMR value   | bitmask of disabled interrupts    |

Commands are issued to the interrupt controller by writing to ICCR.
- For *read* commands, the argument is supplied by writing ICDR.
- For *write* commands, the result accessed by reading ICDR.
