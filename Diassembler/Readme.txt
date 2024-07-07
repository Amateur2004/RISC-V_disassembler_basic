README:

INPUT: 
Run the code as gcc <filename.c> 
To execute it use ./a.out <inputfile.txt>
Here input.txt is the input file given through terminal.
Also, give the input with positive offset for branch and jump instructions.

OUTPUT:
It is as given in the instructions document.

Example:

Input:
01818463
0100026f
00110093
002a63b7
00f8d263
01a74133

Output:
beq x3, x24 ,L1
jal, x4 ,L2
L1 :addi x1, x2, 1
lui x7, 0x2a6
bge x17, x15 ,L2
L2 :xor x2, x14, x26

