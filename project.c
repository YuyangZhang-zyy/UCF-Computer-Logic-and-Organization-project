#include "spimcore.h"
#include <string.h>
#define MEMSIZE 65536

//Group Member: Yuyang Zhang

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    if(ALUControl == 0)//Z = A + B
        *ALUresult = A + B;
    if(ALUControl == 1)//Z = A - B
        *ALUresult = A - B;
    if(ALUControl == 2)//if A < B, Z = 1; otherwise, Z = 0
    {
        int a = (int) A;
        int b = (int) B;
        if(a < b)
            *ALUresult = 1;
        else
            *ALUresult = 0;
    }
    if(ALUControl == 3)//if A < B, Z = 1; otherwise, Z = 0 (A and B are unsigned integers)
    {
        if( A < B)
            *ALUresult = 1;
        else
            *ALUresult = 0;
    }
    if(ALUControl == 4)//Z = A AND B
        *ALUresult = A & B;
    if(ALUControl == 5)//Z = A OR B
        *ALUresult = A | B;
    if(ALUControl == 6)//Z = Shift B left by 16 bits
        *ALUresult = B << 16;
    if(ALUControl == 7)//Z = NOT A
        *ALUresult = ~ A;
    if(*ALUresult == 0)//Assign Zero to 1 if the result is zero; otherwise, assign 0
       *Zero = 1;
}


/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if(PC > MEMSIZE || !(PC % 4 == 0))//Return 1 if a halt condition occurs; otherwise, return 0
        return 1;
    else
        *instruction = Mem [PC >> 2];//Fetch the instruction addressed by PC from Mem and write it to instruction
    return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    unsigned optemp = instruction & 0xfc000000;
    *op = optemp >> 26;//partition op, instruction[31-26]
    unsigned r1temp = instruction & 0x03e00000;
    *r1 = r1temp >> 21;//partition r1, instruction[25-21]
    unsigned r2temp = instruction & 0x001f0000;
    *r2 = r2temp >> 16;//partition r2, instruction[20-16]
    unsigned r3temp = instruction & 0x0000f800;
    *r3 = r3temp >> 11;//partition r3, instruction[15-11]
    unsigned functtemp = instruction & 0x0000003f;
    *funct = functtemp;//partition funct, instruction[5-0]
    unsigned offsettemp = instruction & 0x0000ffff;
    *offset = offsettemp;//partition offset, instruction[15-0]
    unsigned jsectemp = instruction & 0x03ffffff;
    *jsec = jsectemp;//partition jsec, instruction[25-0]
}


/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    if(op == 0){ //R-format
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 7;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 1;
        return 0;

    }
    else if(op == 2){ //jump
        controls->RegDst = 2;
        controls->Jump = 1;
        controls->Branch = 0;
        controls->MemRead = 2;
        controls->MemtoReg = 2;
        controls->ALUOp = 2;
        controls->MemWrite = 2;
        controls->ALUSrc = 2;
        controls->RegWrite = 2;
        return 0;
    }
    else if(op == 4){ //beq
        controls->RegDst = 2;
        controls->Jump = 0;
        controls->Branch = 1;
        controls->MemRead = 2;
        controls->MemtoReg = 0;
        controls->ALUOp = 1;
        controls->MemWrite = 2;
        controls->ALUSrc = 0;
        controls->RegWrite = 0;
        return 0;
    }
    else if(op == 8){ //addi
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        return 0;
    }
    else if(op == 10){ //slti
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 2;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        return 0;
    }
    else if(op == 11){ //sltiu
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 3;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        return 0;
    }
    else if(op == 15){ //lui
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 6;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        return 0;
    }
    else if(op == 35){ //lw
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        return 0;
    }
    else if(op == 43){ //sw
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0;
        controls->MemWrite = 1;
        controls->ALUSrc = 1;
        controls->RegWrite = 0;
        return 0;
    }

    else
        return 1;//illegal instruction
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    unsigned temp = offset;
    if((temp & 0x8000) == 0)//positive operation
        *extended_value = offset & 0x0000ffff;
    else//negative operation
        *extended_value = offset | 0xffff0000;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    if(ALUOp == 7)//R-format
    {
        if(funct == 32)//add
            ALUOp = 0;
        else if(funct == 34) //sub
            ALUOp = 1;
        else if(funct == 36) //and
            ALUOp = 4;
        else if(funct == 37) //or
            ALUOp = 5;
        else if(funct == 42) //slt
            ALUOp = 2;
        else if(funct == 43) //sltu
            ALUOp = 3;
        else
            return 1;
    }

    //Apply ALU
    if(ALUSrc == 0)
        ALU(data1, data2, ALUOp, ALUresult, Zero);
    if(ALUSrc == 1)
        ALU(data1, extended_value, ALUOp, ALUresult, Zero);

    return 0;
}


/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if(MemRead == 1)//reading from memory
    {
        if(ALUresult > (MEMSIZE << 2) || !(ALUresult % 4 == 0))//beyond the memory or not word-aligned
            return 1;
        else
            *memdata = Mem[ALUresult >> 2];
    }

    if(MemWrite == 1)//writing to memory
    {
        if(ALUresult > (MEMSIZE << 2) || !(ALUresult % 4 == 0))//beyond the memory or not word-aligned
            return 1;
        else
            Mem[ALUresult >> 2] = data2;
    }
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if(RegWrite == 1)//
    {
        if(RegDst == 0 && MemtoReg == 0)//Write ALUresult to r2
            Reg[r2] = ALUresult;
        if(RegDst == 0 && MemtoReg == 1)//Write memdata to r2
            Reg[r2] = memdata;
        if(RegDst == 1 && MemtoReg == 0)//Write ALUresult to r3
            Reg[r3] = ALUresult;
        if(RegDst == 1 && MemtoReg == 1)//Write memdata to r3
            Reg[r3] = memdata;
     }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //offset of address
    int temp = extended_value;
    if((temp & 0x80000000) == 0)
        temp = ~extended_value + 1;
    else
        temp = extended_value;

    if(Jump == 1)//break
        *PC = jsec << 2;
    else if(Branch == 1)//break with condition
    {
        if(Zero == 1)
            *PC = *PC + 4 + (temp * 4);
        if(Zero == 0)
            *PC = *PC + 4;
    }
    else
        *PC = *PC + 4;
}

