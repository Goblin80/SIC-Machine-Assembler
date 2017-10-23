typedef struct
{
    char mnemonic[sSIZE];
    short opcode;
}instruction;

typedef struct
{
    short loc;
    instruction operator;
    char label[sSIZE],
         operand[sSIZE],
         objectcode[sSIZE];
}operation;

typedef struct
{
	short len;
	operation op[pSIZE];
}program;