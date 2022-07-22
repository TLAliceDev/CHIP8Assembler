# CHIP8Assembler
Very simple Assembler for the CHIP-8 Fantasy Console

# Quick Reference:
SYSC NNN : Syscall (0x0NNN).

JUMP NNN : Jump to address NNN (0x1NNN) 

CALL NNN : Call subroutine NNN (0x2NNN) 

JNEM X NN: Skip if VX Not Equal Immediate NN (0x3XNN)

JEQM X NN: Skip if VX Equal Immediate NN (0x4XNN)

JEQU X Y : Skip if VX Equal VY (0x5XY0)

SETM X NN: Set VX to Immediate NN (0x6XNN)

ADDM X NN: Add Immediate NN to VX (0x7XNN)

RGOP X Y : Register Operations (See further down for more info, 0x8XYI)

JNEQ X Y : Skip if VX not equal VY (0x9XY0)

SETI NNN : Set I to Immediate NNN (0xANNN)

JMPV NNN : Jump to Address Immediate NNN + V0 (0xBNNN)

RAND X NN: Set VX to a Random Number & NN (0xCXNN)

DRAW X Y N: Draws a Sprite at Position (VX,VY) with the Height of N (0xDXYN)

KEYD X   : Skips if the Key Stored in VX is Not Pressed (0xEX9E)

KEYU X   : Skips if the Key Stored in VX is Pressed (0xEXA1)

DGET X   : Sets VX to the Delay Timer (0xFX07)

KGET X   : Blocks Until a Key Press, Set VX to that Key Press (0xFX0A)

DSET X   : Sets the Delay Timer to VX (0xFX15)

ASET X   : Sets the Sound Timer to VX (0xFX18)

ADDI X   : Adds VX to I (0xFX1E)

FONT X   : Sets I to the Address of Hex Character Font Data Stored in VX (0xFX29)

SBCD X   : Stores the Binary-Coded Decimal Representation of VX Starting at I (0xFX33)

DUMP X   : Stores V0-VX in Memory Starting at I (0xFX55)

LOAD X   : Loads X Bytes of Data from Memory Starting at I, Storing in V0-VX (0xFX65)


