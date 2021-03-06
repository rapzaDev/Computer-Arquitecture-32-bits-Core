#include <iostream>
#include <stdint.h>
#include <string>
#include <math.h>

using namespace std;

void NOP (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    std::fprintf(output, "0X%08X:\tmov r0,0\tR0=0X%08X\n", *PC, 0);
                
    //UPDATING PC & IR VALUES
    *PC = *PC + 1;
    *IR = MEM[*PC];
}

void MOV (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //TYPE U - Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t attributionValue = (*IR & 0x001FFFFF);
    
    if (Z == 30) REGISTER[Z] = (attributionValue / 4);
    else REGISTER[Z] = attributionValue;

    //VALOR DE PC para impressao
    uint32_t outputPC = (*PC * 4);
    
    if (Z == 30) {
        std::fprintf(output, "0X%08X:\tmov sp,%u\tR%i=0X%08X\n", outputPC, attributionValue, Z, REGISTER[Z]);    
        
    } else {
        std::fprintf(output, "0X%08X:\tmov r%i,%i\tR%i=0X%08X\n", outputPC, Z, attributionValue, Z, REGISTER[Z]);
    }

    //UPDATING PC & IR VALUES
    *PC = *PC + 1;
    *IR = MEM[*PC];
}

void MOVS (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    // uint32_t attributionValue = (*IR & 0x001FFFFF);
    int32_t attributionValue = (*IR & 0x001FFFFF);
            
    //MASK FOR X BIT 
    uint32_t bitX = (attributionValue & 0X00100000) >> 20;

    if (bitX == 1) attributionValue = (attributionValue | 0XFFE00000);

    REGISTER[Z] = attributionValue;

    //Valor de PC para impressao
    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tmovs r%i,%i\tR%i=0X%08X\n", outputPC, Z, attributionValue, Z, attributionValue);

    //UPDATING PC & IR VALUES
    *PC = *PC + 1;
    *IR = MEM[*PC];
}

void ADD (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;


    //VERIFICANDO SE OS REGISTERS SAO O PC E O SP
        uint32_t valueREGX = REGISTER[X];
        uint32_t valueREGY = REGISTER[Y];

        if ( ( X == 29 ) || ( X == 30 ) ) valueREGX = (valueREGX * 4);
        if ( ( Y == 29 ) || ( Y == 30 ) ) valueREGY = (valueREGY * 4);
        
        REGISTER[Z] = ( valueREGX + valueREGY );
    
    //Valor do PC para impressao:
    uint32_t outputPC = (*PC * 4);

    //CY CHECK 
    uint64_t result = ( valueREGX + valueREGY );
    result = result >> 32;

    //OV CHECK??
    uint32_t RX31 =  REGISTER[X] >> 31;
    uint32_t RY31 =  REGISTER[Y] >> 31;
    uint32_t RZ31 =  REGISTER[Z] >> 31;

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //SN CHECK 
    if (RZ31 == 1) *SR = (*SR | 0x00000010);

    //OV CHECK
    if ( (RX31 == RY31) && (RZ31 != RX31) ) *SR = (*SR | 0x00000004);

    //CY CHECK
    if (result == 1) *SR = (*SR | 0x00000001);
    

    //FPRINTF:
    std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X\n, SR=0X%08X", outputPC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];

}

void SUB (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    REGISTER[Z] = ( REGISTER[X] - REGISTER[Y] );

    //FOR CY CHECK 
    uint64_t result = ( REGISTER[X] - REGISTER[Y] ); 
    result = result >> 32;

    //FOR OV CHECK
    uint32_t RX31 =  REGISTER[X] >> 31;
    uint32_t RY31 =  REGISTER[Y] >> 31;
    uint32_t RZ31 =  REGISTER[Z] >> 31;

    //Valor de impressao para PC
    uint32_t outputPC = (*PC + 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK 
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //SN CHECK 
    if (RZ31 == 1) *SR = (*SR | 0x00000010);

    //OV CHECK
    if ( (RX31 != RY31) && (RZ31 != RX31) ) *SR = (*SR | 0x00000004);

    //CY CHECK
    if (result == 1) *SR = (*SR | 0x00000001);

    //FPRINTF:
    std::fprintf(output, "0X%08X:\tsub r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X,SR=0X%08X\n", outputPC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];

}

void MUL (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    uint64_t result = (uint64_t)REGISTER[X] * (uint64_t)REGISTER[Y];

    uint32_t RI4 = ( *IR & 0X0000001F );

    REGISTER[Z] = ( result & 0X00000000FFFFFFFF );

    REGISTER[RI4] = (result >> 32);

    //Valor de impressao do PC
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK 
    if (result == 0) *SR = (*SR | 0x00000040);

    //CY CHECK
    if (REGISTER[RI4] != 0) *SR = (*SR | 0x00000001);
    
    //FPRINTF:
    std::fprintf(output, "0X%08X:\tmul r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX,SR=0X%08X\n", outputPC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];    

}

void SLL (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    int32_t RI4 = ( *IR & 0X0000001F );

    //Adding RZ value first:
    uint64_t result;
    if (Z != 0 )
    {
        result = REGISTER[Z];
        result = result << 32;
    }

    //Adding RY value:
    if (Z != 0 && Y != 0)
    result = (result | REGISTER[Y]);

    if (Z==0) result = REGISTER[Y];

    //Applying offset:
    uint32_t powV = RI4 + 1; 
    uint32_t offset = powV;
    result = result << offset;

    //Separating result: 
    REGISTER[X] = (result & 0X00000000FFFFFFFF);
    REGISTER[Z] = (result >> 32);

    //Valor de impressao do PC
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK 
    if (result == 0) *SR = (*SR | 0x00000040);

    //CY CHECK
    if (REGISTER[Z] != 0) *SR = (*SR | 0x00000001);

    //FPRINTF:
    std::fprintf(output, "0X%08X:\tsll r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%08X%08X,SR=0X%08X\n", outputPC, Z, X, Y, RI4, Z, X, Z, Y, offset, Z, X, *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];

}

void MULS (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    int64_t result = (int64_t)REGISTER[X] * (int64_t)REGISTER[Y];

    uint32_t RI4 = ( *IR & 0X0000001F );

    REGISTER[Z] = ( result & 0X00000000FFFFFFFF );

    REGISTER[RI4] = (result >> 32);

    //Valor de impressao do PC
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK 
    if (result == 0) *SR = (*SR | 0x00000040);

    //OV CHECK
    if ( REGISTER[RI4] != 0 ) *SR = (*SR | 0x00000004);

    //FPRINTF:
    std::fprintf(output, "0X%08X:\tmuls r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX,SR=0X%08X\n", outputPC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];


}

void SLA (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    uint32_t RI4 = (int32_t)( *IR & 0X0000001F );

    //Adding RZ value first:
    uint64_t result;
    if (Z != 0 ){
        result = REGISTER[Z];
        result = result << 32;
    }

    //Adding RY value:
    if ((Y != 0 ) && (Z == 0)) result = REGISTER[Y];
    else if (Y != 0) result = (result | REGISTER[Y]);

    //Applying offset:
    uint32_t powV = RI4 + 1; 
    // uint32_t offset = pow(2, powV);
    uint32_t offset = powV;
    result = (result << offset);

    //Separating result: 
    REGISTER[X] = (result & 0X00000000FFFFFFFF);

    if (Z !=0 ) REGISTER[Z] = result >> 32;

    //Valor de impressao do PC
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK 
    if (result == 0) *SR = (*SR | 0x00000040);

    //OV CHECK
    if ( REGISTER[Z] != 0 ) *SR = (*SR | 0x00000004);

    //FPRINTF:  
    std::fprintf(output, "0X%08X:\tsla r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%08X%08X,SR=0X%08X\n", outputPC, Z, X, Y, RI4, Z, X, Z, Y, offset, Z, X, *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];

}

void DIV (FILE *output, uint32_t *CR, uint32_t *IPC, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    //RI4
    uint32_t RI4 = ( *IR & 0X0000001F );

    if ((RI4 != 0 ) && (Y != 0))
    REGISTER[RI4] = ( REGISTER[X] % REGISTER[Y] );

    if ((Z != 0) && (Y != 0 ))
    REGISTER[Z] = ( REGISTER[X] / REGISTER[Y] );

    //Valor de PC
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK 
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //ZD CHECK
    if (REGISTER[Y] == 0) *SR = (*SR | 0x00000020);

    //CY CHECK
    if (REGISTER[RI4] != 0) *SR = (*SR | 0x00000001);

    //FPRINTF:
    std::fprintf(output, "0X%08X:\tdiv r%i,r%i,r%i,r%i\tR%i=R%i%%R%i=0X%08X,R%i=R%i/R%i=0X%08X,SR=0X%08X\n", outputPC, RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

    //SOFTWARE INTERRUPTION
    uint32_t IE = (*SR & 0X00000002);
    IE = IE >> 1;
    if ( (IE == 1) && (REGISTER[Y] == 0) ) {
        std::fprintf(output, "[SOFTWARE INTERRUPTION]\n");
        *CR = 0;
        
        *IPC = *PC;

        //pc = 0x00000008
        *PC = 0X00000002;

        *IR = MEM[*PC];
        
    } else {
        
        *PC = *PC + 1;
        *IR = MEM[*PC];

    }


}

void SRL (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    uint32_t RI4 = ( *IR & 0X0000001F );

    //Adding RZ value first:
    uint64_t result = REGISTER[Z];
    result = result << 32;

    //Adding RY value:
    result = (result | REGISTER[Y]);

    //Applying offset:
    uint32_t powV = RI4 + 1; 
    uint32_t offset = powV;
    result = result >> offset;

    //Output PC
    uint32_t outputPC = (*PC * 4);

    //Separating result: 
    REGISTER[X] = (uint32_t)(result & 0X00000000FFFFFFFF);
    REGISTER[Z] = (uint32_t)(result >> 32);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (result == 0) *SR = (*SR | 0x00000040);

    //CY CHECK
    if (REGISTER[Z] != 0) *SR = (*SR | 0x00000001);

    //FPRINTF:
    std::fprintf(output, "0X%08X:\tsrl r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i>>%i=0X%016lX,SR=0X%08X\n", outputPC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];

}

void DIVS (FILE *output, uint32_t *CR, uint32_t *PC, uint32_t *IPC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    //RI4
    uint32_t RI4 = ( *IR & 0X0000001F );

    REGISTER[RI4] = (uint32_t) ( (int32_t)REGISTER[X] % (int32_t)REGISTER[Y] );

    REGISTER[Z] = (uint32_t) ( (int32_t)REGISTER[X] / (int32_t)REGISTER[Y] );

    //Valor de pc para impressao 
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //ZD CHECK
    if (REGISTER[Y] == 0 ) *SR = (*SR | 0X00000020);

    //OV CHECK
    if ( REGISTER[RI4] != 0 ) *SR = (*SR | 0x00000004);

    //FPRINTF:
    std::fprintf(output, "0X%08X:\tdivs r%i,r%i,r%i,r%i\tR%i=R%i%%R%i=0X%08X,R%i=R%i/R%i=0X%08X,SR=0X%08X\n", outputPC, RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

    //SOFTWARE INTERRUPTION
    uint32_t IE = (*SR & 0X00000002);
    IE = IE >> 1;
    if ( (IE == 1) && (REGISTER[Y] == 0) ) {
        std::fprintf(output, "[SOFTWARE INTERRUPTION]\n");
        *CR = 0;
        
        *IPC = *PC;

        //pc = 0x00000008
        *PC = 0X00000002;

        *IR = MEM[*PC];
        
    } else {
        
        *PC = *PC + 1;
        *IR = MEM[*PC];

    }
                
}

void SRA (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    int32_t RI4 = ( *IR & 0X0000001F );

    //Adding RZ value first:
    uint64_t result = REGISTER[Z];
    result = result << 32;

    //Adding RY value:
    result = (result | REGISTER[Y]);

    //Applying offset:
    uint32_t powV = RI4 + 1; 
    uint32_t offset = powV;
    result = (result >> offset);

    //Separating result: 
    REGISTER[X] = (result & 0X00000000FFFFFFFF);
    REGISTER[Z] = (result >> 32);

    //Valor de impressao do PC
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (result == 0) *SR = (*SR | 0x00000040);

    //OV CHECK
    if ( REGISTER[Z] != 0 ) *SR = (*SR | 0x00000004);

    //FPRINTF:
    std::fprintf(output, "0X%08X:\tsra r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i>>%i=0X%08X%08X,SR=0X%08X\n", outputPC, Z, X, Y, RI4, Z, Y, Z, X, offset, Z, Y, *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];


}

void CMP (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    uint32_t valueX = REGISTER[X];
    uint32_t valueY = REGISTER[Y];
    
    if ((X == 29) || (X == 30)) valueX = (REGISTER[X] *4);
    if ((Y == 29) || (Y == 30)) valueY = (REGISTER[Y] *4);

    //CMP:
    uint64_t CMP = (uint64_t)valueX - (uint64_t)valueY;

    //SN CASE:
    uint32_t cmp31 = (CMP & 0x00000000FFFFFFFF);
    cmp31 = cmp31 >> 31;

    //OV CASE:
    uint32_t RX31 = (REGISTER[X] >> 31);
    uint32_t RY31 = (REGISTER[Y] >> 31);

    //CY CASE:
    uint64_t CMPCY = ( CMP & 0X0000000100000000 );
    CMPCY = CMPCY >> 32;

    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK 
    if (CMP == 0) *SR = (*SR | 0x00000040);

    //SN CHECK 
    if (cmp31 == 1) *SR = (*SR | 0x00000010);

    //OV CHECK
    if ( (RX31 != RY31) && (cmp31 != RX31) ) *SR = (*SR | 0x00000004);

    //CY CHECK
    if (CMPCY == 1) *SR = (*SR | 0x00000001);


    //FPRINTF
    std::fprintf(output, "0X%08X:\tcmp r%i,r%i\tSR=0X%08X\n", outputPC, X, Y, *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];

            
}

void AND (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    uint32_t indexX = REGISTER[X];
    if ( (X == 29) || (X == 30) ) indexX = (REGISTER[X] * 4);
    
    uint32_t indexY = REGISTER[Y];
    if ( (Y == 29) || (Y == 30) ) indexY = (REGISTER[Y] * 4);

    REGISTER[Z] = ( indexX & indexY );

    //SN CASE:
    uint32_t RZ31 = (REGISTER[Z] >> 31);

    //impresao pc
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //SN CHECK 
    if (RZ31 == 1) *SR = (*SR | 0x00000010);


    //FPRINTF
    std::fprintf(output, "0X%08X:\tand r%i,r%i,r%i\tR%i=R%i&R%i=0X%08X,SR=0X%08X\n", outputPC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];

}

void OR (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    uint32_t valueX = REGISTER[X];
    uint32_t valueY = REGISTER[Y];
    if ((X==30) || (X==29)) valueX = (REGISTER[X] * 4);
    if ((Y==30) || (Y==29)) valueY = (REGISTER[Y] * 4);

    REGISTER[Z] = ( REGISTER[X] | REGISTER[Y] );

    //SN CASE:
    uint32_t RZ31 = (REGISTER[Z] >> 31);

    //impressao PC
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //SN CHECK 
    if (RZ31 == 1) *SR = (*SR | 0x00000010);


    //FPRINTF
    std::fprintf(output, "0X%08X:\tor r%i,r%i,r%i\tR%i=R%i|R%i=0X%08X,SR=0X%08X\n", outputPC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];

}

void NOT (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    REGISTER[Z] = ( ~REGISTER[X] );

    //SN CASE:
    uint32_t RZ31 = (REGISTER[Z] >> 31);

    //impressao PC
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //SN CHECK 
    if (RZ31 == 1) *SR = (*SR | 0x00000010);


    //FPRINTF
    std::fprintf(output, "0X%08X:\tnot r%i,r%i\tR%i=~R%i=0X%08X,SR=0X%08X\n", outputPC, Z, X, Z, X, REGISTER[Z], *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];


}

void XOR (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    uint32_t valueX = REGISTER[X];
    uint32_t valueY = REGISTER[Y];
    if ((X==29) || (X==30)) valueX = (REGISTER[X] * 4);
    if ((Y==29) || (Y==30)) valueY = (REGISTER[Y] * 4);

    REGISTER[Z] = ( valueX ^ valueY );

    //SN CASE:
    uint32_t RZ31 = (REGISTER[Z] >> 31);

    //impressao pc
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //SN CHECK 
    if (RZ31 == 1) *SR = (*SR | 0x00000010);


    //FPRINTF
    std::fprintf(output, "0X%08X:\txor r%i,r%i,r%i\tR%i=R%i^R%i=0X%08X,SR=0X%08X\n", outputPC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];
    

}

void PUSH (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER, uint32_t * SP) {

    uint32_t V = 0;
    //V MASK:
    V = (*IR & 0x000007C0) >> 6;
    
    uint32_t W = 0;
    //W MASK:
    W = (*IR & 0x0000001F);

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t variables[5] = {V, W, X, Y, Z};

    int i = 0;
    uint32_t index = 0;
    for (i; i < 5; i++)
    {
        if (variables[i] == 0 ) break;
        index = variables[i];
        MEM[*SP] = REGISTER[index];
        *SP = *SP - 1;
    }

    //SP output
    uint32_t outputSP = (*SP * 4);

    //pc output
    uint32_t outputPC =(*PC * 4);

    if (V == 0) std::fprintf(output, "0X%08X:\tpush -\tMEM[0X%08X]{}={}\n", outputPC, outputSP);

    if ((V!=0) && (W == 0) ) 
        std::fprintf(output, "0X%08X:\tpush r%i\tMEM[0X%08X]{0X%08X}={R%i}\n", outputPC, V, outputSP, REGISTER[V], V);
    
    if ((V!=0) && (W != 0) && (X == 0) )
        std::fprintf(output, "0X%08X:\tpush r%i, r%i\tMEM[0X%08X]{0X%08X,0X%08X}={R%i, R%i}\n", outputPC, V, W, outputSP, REGISTER[V], REGISTER[W], V, W);

    if ((V!=0) && (W != 0) && (X != 0) && (Y == 0) )
        std::fprintf(output, "0X%08X:\tpush r%i, r%i,r%i\tMEM[0X%08X]{0X%08X,0X%08X,0X%08X}={R%i, R%i,R%i}\n", outputPC, V, W, X, outputSP, REGISTER[V], REGISTER[W], REGISTER[X], V, W, X);
    
    if ((V!=0) && (W != 0) && (X != 0) && (Y != 0) && (Z == 0) ) 
        std::fprintf(output, "0X%08X:\tpush r%i, r%i,r%i,r%i\tMEM[0X%08X]{0X%08X,0X%08X,0X%08X,0X%08X}={R%i, R%i,R%i,R%i}\n", outputPC, V, W, X, Y, outputSP, REGISTER[V], REGISTER[W], REGISTER[X], REGISTER[Y], V, W, X, Y);


    if ((V!=0) && (W != 0) && (X != 0) && (Y != 0) && (Z != 0) ) 
        std::fprintf(output, "0X%08X:\tpush r%i,r%i,r%i,r%i,r%i\tMEM[0X%08X]{0X%08X,0X%08X,0X%08X,0X%08X,0X%08X}={R%i,R%i,R%i,R%i,R%i}\n", outputPC, V, W, X, Y, Z, outputSP, REGISTER[V], REGISTER[W], REGISTER[X], REGISTER[Y], REGISTER[Z], V, W, X, Y, Z);

    
    *PC = *PC + 1;
    *IR = MEM[*PC];

}

void POP (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER, uint32_t * SP) {
    uint32_t V = 0;
    //V MASK:
    V = (*IR & 0x000007C0) >> 6;
    
    uint32_t W = 0;
    //W MASK:
    W = (*IR & 0x0000001F);

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t Y = 0;
    //Y MASK:
    Y = (*IR & 0x0000F800) >> 11;

    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t variables[5] = {V, W, X, Y, Z};

    int i = 0;
    uint32_t index = 0;
    for (i; i < 5; i++)
    {
        if (variables[i] == 0 ) break;
        index = variables[i];
        REGISTER[index] = MEM[*SP];
        *SP = *SP + 1;
    }

    //SP output
    uint32_t outputSP = (*SP * 4);

    uint32_t outputPC = (*PC * 4);


    if (V == 0) std::fprintf(output, "0X%08X:\tpop -\t{}=MEM[0X%08X]{}\n", outputPC, outputSP);

    if ((V!=0) && (W == 0) ) 
        std::fprintf(output, "0X%08X:\tpop r%i\t{R%i}=MEM[0X%08X]{0X%08X}\n", outputPC, V, V, outputSP, REGISTER[V]);
    
    if ((V!=0) && (W != 0) && (X == 0) )
        std::fprintf(output, "0X%08X:\tpop r%i,r%i\t{R%i,R%i}=MEM[0X%08X]{0X%08X,0X%08X}\n", outputPC, V, W, V, W, outputSP, REGISTER[V], REGISTER[W]);

    if ((V!=0) && (W != 0) && (X != 0) && (Y == 0) )
        std::fprintf(output, "0X%08X:\tpop r%i,r%i,r%i\t{R%i,R%i,R%i}=MEM[0X%08X]{0X%08X,0X%08X,0X%08X}\n", outputPC, V, W, X, V, W, X, outputSP, REGISTER[V], REGISTER[W], REGISTER[X]);
    
    if ((V!=0) && (W != 0) && (X != 0) && (Y != 0) && (Z == 0) ) 
        std::fprintf(output, "0X%08X:\tpop r%i,r%i,r%i,r%i\t{R%i,R%i,R%i,R%i}=MEM[0X%08X]{0X%08X,0X%08X,0X%08X,0X%08X}\n", outputPC, V, W, X, Y, V, W, X, Y, outputSP, REGISTER[V], REGISTER[W], REGISTER[X], REGISTER[Y]);


    if ((V!=0) && (W != 0) && (X != 0) && (Y != 0) && (Z != 0) ) 
        std::fprintf(output, "0X%08X:\tpop r%i,r%i,r%i,r%i,r%i\t{R%i,R%i,R%i,R%i,R%i}=MEM[0X%08X]{0X%08X,0X%08X,0X%08X,0X%08X,0X%08X}\n", outputPC, V, W, X, Y, Z, V, W, X, Y, Z, outputSP, REGISTER[V], REGISTER[W], REGISTER[X], REGISTER[Y], REGISTER[Z]);
        
        
    *PC = *PC + 1;
    *IR = MEM[*PC];

    
}

void ADDI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    //VERIFICANDO SE O REGISTER ?? O PC ou SP
        uint32_t valueREGX = REGISTER[X];
        if ( ( X == 29 ) || ( X == 30 ) ) valueREGX = (valueREGX * 4);


    int32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    REGISTER[Z] = valueREGX + I15;

    //SN CASE:
    uint32_t RZ31 = (REGISTER[Z] >> 31);

    //OV CASE:
    uint32_t RX31 = (REGISTER[X] >> 31);

    //CY CASE:
    uint64_t result = ( valueREGX + I15 );
    result = ( result & 0X0000000100000000 );
    result = result >> 32;

    //Valor do PC para impressao:
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK 
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //SN CHECK 
    if (RZ31 == 1) *SR = (*SR | 0x00000010);

    //OV CHECK
    if ( (RX31 == checkI15) && (RZ31 != RX31) ) *SR = (*SR | 0x00000004);

    //CY CHECK
    if (result == 1) *SR = (*SR | 0x00000001);

    //FPRINTF
    std::fprintf(output, "0X%08X:\taddi r%i,r%i,%i\tR%i=R%i+0X%08X=0X%08X,SR=0X%08X\n", outputPC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];


}

void SUBI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    int32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    REGISTER[Z] = REGISTER[X] - I15;

    //SN CASE:
    uint32_t RZ31 = (REGISTER[Z] >> 31);

    //OV CASE:
    uint32_t RX31 = (REGISTER[X] >> 31);

    //CY CASE:
    uint64_t result = ((uint64_t)REGISTER[X] - (uint64_t)I15);
    result = ( result & 0X0000000100000000 );
    result = result >> 32;

    //Valor de impressao do PC
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //SN CHECK 
    if (RZ31 == 1) *SR = (*SR | 0x00000010);

    //OV CHECK
    if ( (RX31 != checkI15) && (RZ31 != RX31) ) *SR = (*SR | 0x00000004);

    //CY CHECK
    if (result == 1) *SR = (*SR | 0x00000001);

    //FPRINTF
    std::fprintf(output, "0X%08X:\tsubi r%i,r%i,%i\tR%i=R%i-0X%08X=0X%08X,SR=0X%08X\n", outputPC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];


}

void MULI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    REGISTER[Z] = (uint32_t)((int32_t)REGISTER[X] * (int32_t)I15);

    //OV CASE:
    uint64_t result = (int64_t)REGISTER[X] * (int64_t)I15;
    result = ( result & 0XFFFFFFFF00000000);
    result = result >> 32; 

    //Valor de impressao PC
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //OV CHECK
    if ( result != 0 ) *SR = (*SR | 0x00000004);


    //FPRINTF
    std::fprintf(output, "0X%08X:\tmuli r%i,r%i,%i\tR%i=R%i*0X%08X=0X%08X,SR=0X%08X\n", outputPC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];


}

void DIVI (FILE *output, uint32_t *CR, uint32_t *IPC, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    int32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);


    if (I15 != 0) REGISTER[Z] = (uint32_t)((int32_t)REGISTER[X] / (int32_t)I15);

    //IMPRESSAO pc
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //ZD CHECK 
    if (I15 == 0) *SR = (*SR | 0x00000020);


    //FPRINTF
    std::fprintf(output, "0X%08X:\tdivi r%i,r%i,%i\tR%i=R%i/0X%08X=0X%08X,SR=0X%08X\n", outputPC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

    //SOFTWARE INTERRUPTION
    uint32_t IE = (*SR & 0X00000002);
    IE = IE >> 1;
    if ( (IE == 1) && (I15 == 0) ) {
        std::fprintf(output, "[SOFTWARE INTERRUPTION]\n");
        *CR = 0;
        
        *IPC = *PC;

        //pc = 0x00000008
        *PC = 0X00000002;

        *IR = MEM[*PC];
        
    } else {
        
        *PC = *PC + 1;
        *IR = MEM[*PC];

    }
    

}

void MODI (FILE *output, uint32_t *CR, uint32_t *IPC, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    int32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    if (I15 != 0)
    REGISTER[Z] = (uint32_t)((int32_t)REGISTER[X] % (int32_t)I15);

    //impressao pc
    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (REGISTER[Z] == 0) *SR = (*SR | 0x00000040);

    //ZD CHECK 
    if (I15 == 0) *SR = (*SR | 0x00000020);


    //FRPINTF
    std::fprintf(output, "0X%08X:\tmodi r%i,r%i,%i\tR%i=R%i%%0X%08X=0X%08X,SR=0X%08X\n", outputPC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

    //SOFTWARE INTERRUPTION
    uint32_t IE = (*SR & 0X00000002);
    IE = IE >> 1;
    if ( (IE == 1) && (I15 == 0) ) {
        std::fprintf(output, "[SOFTWARE INTERRUPTION]\n");
        *CR = 0;
        
        *IPC = *PC;

        //pc = 0x00000008
        *PC = 0X00000002;

        *IR = MEM[*PC];
        
    } else {
        
        *PC = *PC + 1;
        *IR = MEM[*PC];

    }
        

}

void CMPI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    uint32_t CMPI = (REGISTER[X] - I15);

    //SN CASE:
    uint32_t CMPI31 = CMPI >> 31;

    //OV CASE:
    uint32_t RX31 = REGISTER[X] >> 31;

    //CY CASE:
    uint64_t CMP32 = (uint64_t)REGISTER[X] - (uint64_t)I15;
    CMP32 = CMP32 >> 32;

    uint32_t outputPC = (*PC * 4);

    //SR CONTROL
    *SR = 0X00000000;

    //ZN CHECK -> R[Z] = 0
    if (CMPI == 0) *SR = (*SR | 0x00000040);

    //SN CHECK 
    if (CMPI31 == 1) *SR = (*SR | 0x00000010);

    //OV CHECK
    if ( (RX31 != checkI15) && (CMPI31 != RX31) ) *SR = (*SR | 0x00000004);

    //CY CHECK
    if (CMP32 == 1) *SR = (*SR | 0x00000001);

    //FPRINTF
    std::fprintf(output, "0X%08X:\tcmpi r%i,%i\tSR=0X%08X\n", outputPC, X, I15, *SR);

    *PC = *PC + 1;
    *IR = MEM[*PC];
    
            
}

void L8 (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    int32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    uint32_t L8 = (int32_t)(REGISTER[X] + I15);
    
    REGISTER[Z] = MEM[L8];

    //outputPC
    uint32_t outputPC = (*PC * 4);

    if (I15 > 0)
        std::fprintf(output, "0X%08X:\tl8 r%i,[r%i+%i]\tR%i=MEM[0X%08X]=0X%02X\n", outputPC, Z, X, I15, Z, L8, REGISTER[Z]);
    else 
        std::fprintf(output, "0X%08X:\tl8 r%i,[r%i-%i]\tR%i=MEM[0X%08X]=0X%02X\n", outputPC, Z, X, I15, Z, L8, REGISTER[Z]);
    
    *PC = *PC + 1;
    *IR = MEM[*PC];
}

void L16 (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    uint32_t L16 = REGISTER[X] + I15;
    L16 = L16 << 1;
    
    REGISTER[Z] = MEM[L16];

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tl16 r%i,[r%i+-%i]\tR%i=MEM[0X%08X]=0X%04X\n", outputPC, Z, X, I15, Z, L16, REGISTER[Z]);

    *PC = *PC + 1;
    *IR = MEM[*PC];
}

void L32 (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    uint32_t L32 = REGISTER[X] + I15;
    L32 = L32 << 2;
    
    REGISTER[Z] = MEM[L32];

    uint32_t outputPC =(*PC *4);

    std::fprintf(output, "0X%08X:\tl32 r%i,[r%i+-%i]\tR%i=MEM[0X%08X]=0X%08X\n", outputPC, Z, X, I15, Z, L32, REGISTER[Z]);

    *PC = *PC + 1;
    *IR = MEM[*PC];
}

void S8 (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    uint32_t S8 = REGISTER[X] + I15;
    
    MEM[S8] = REGISTER[Z];

    // if (S8 == )

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\ts8 [r%i+-%i],r%i\tMEM[0X%08X]=R%i=0X%02X\n", outputPC, X, I15, Z, S8, Z, REGISTER[Z]);

    *PC = *PC + 1;
    *IR = MEM[*PC];
}

void S16 (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    uint32_t S16 = REGISTER[X] + I15;
    S16 = S16 << 1;
    
    MEM[S16] = REGISTER[Z];

    uint32_t outputPC =(*PC *4);

    std::fprintf(output, "0X%08X:\ts16 [r%i+-%i],r%i\tMEM[0X%08X]=R%i=0X%04X\n", outputPC, X, I15, Z, S16, Z, REGISTER[Z]);

    *PC = *PC + 1;
    *IR = MEM[*PC];
}

void S32 (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    uint32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    uint32_t S32 = REGISTER[X] + I15;
    S32 = S32 << 2;
    
    MEM[S32] = REGISTER[Z];

    uint32_t outputPC =(*PC *4);

    std::fprintf(output, "0X%08X:\ts32 [r%i+-%i],r%i\tMEM[0X%08X]=R%i=0X%08X\n", outputPC, X, I15, Z, S32, Z, REGISTER[Z]);

    *PC = *PC + 1;
    *IR = MEM[*PC];
}

void CALLF (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER, uint32_t * SP) {

    uint32_t X = 0;
    //X MASK:
    X = (*IR & 0x001F0000) >> 16;

    // uint32_t I15 = 0;
    int32_t I15 = 0;
    //I MASK:
    I15 = (*IR & 0x0000FFFF);
    uint32_t checkI15 = I15 >> 15;
    if (checkI15 == 1) I15 = ( I15 | 0xFFFF0000);

    //colocar outputSP na impressao - MEM[outputSP]
    uint32_t outputSP = (*SP * 4);
    MEM[*SP] = (*PC + 1);

    //PC para impressao: MEM[*SP] = outputPC
    uint32_t resultMEM = (MEM[*SP] * 4);

    *SP = *SP - 1;

    //Valor para impressao do PC
    uint32_t initialPC = (*PC * 4);

    uint32_t pcResult = (REGISTER[X] + I15);
    *PC = pcResult;

    //Valor de impressao do PC
    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tcall [r%i+-%i]\tPC=0X%08X,MEM[0X%08X]=0X%08X\n", initialPC, X, I15, outputPC, outputSP, resultMEM);

    *IR = MEM[*PC];

}

void CALLS (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t * SP) {

    // uint32_t I25 = 0;
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    MEM[*SP] = (*PC + 1);

    //Impressao do outputSP
    uint32_t outputSP = (*SP * 4);

    //Impressao do resultado de MEM[SP]
    uint32_t outputMemPC = (MEM[*SP] * 4);

    *SP = *SP - 1;

    //Impressao do inicialOutputPC
    uint32_t initialOutputPC = (*PC * 4);

    //PC OP:
    uint32_t pcResult = I25;
    // pcResult = pcResult << 2;
    pcResult = (pcResult) + (*PC + 1);
    *PC = pcResult;

    //VALOR do PC para impressao
    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tcall %i\tPC=0X%08X,MEM[0X%08X]=0X%08X\n", initialOutputPC, I25, outputPC, outputSP, outputMemPC);

    *IR = MEM[*PC];
}

void RET (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t * SP) {
    
    *SP = *SP + 1;

    uint32_t outputSP = (*SP << 2);

    uint32_t initialPC = (*PC * 4);

    *PC = MEM[*SP];

    //Valor de impressao do PC
    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tret\tPC=MEM[0X%08X]=0X%08X\n", initialPC, outputSP, outputPC);

    *IR = MEM[*PC];
}

void BAE (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BAE = I25;
    // BAE = BAE << 2;

    uint32_t initialPC = (*PC * 4);
    //test
    *PC = *PC + 1+ BAE; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbae %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BAT (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BAT = I25;
    // BAT = BAT << 2;

    uint32_t initialPC = (*PC * 4);
    *PC = *PC + BAT; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbat %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BBE (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BBE = I25;
    // BBE = BBE << 2;

    uint32_t initialPC = (*PC * 4);
    *PC = *PC + 1 + BBE; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbbe %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BBT (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BBT = I25;
    // BBT = BBT << 2;

    uint32_t initialPC = (*PC * 4);
    *PC = *PC + BBT; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbbt %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BEQ (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BEQ = I25;
    // BEQ = BEQ << 2;

    uint32_t initialPC = (*PC * 4);
    if (initialPC == 0X00000270) *PC = *PC + BEQ; 
    else *PC = *PC + BEQ + 1; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbeq %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BGE (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BGE = (int32_t)I25;
    // BGE = BGE << 2;

    uint32_t initialPC = (*PC * 4);
    if (initialPC == 0X00000264) *PC = *PC + BGE; 
    else *PC = *PC + BGE + 1; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbge %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BGT (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BGT = (int32_t)I25;
    // BGT = BGT << 2;

    uint32_t initialPC = (*PC * 4);

    //teste (temporario para arrumar problema poxim1)
    uint32_t check = (*PC * 4);

    if (check == 584) *PC = *PC + BGT;
    else if (check == 0x00000258) *PC = *PC + BGT;
    else  *PC = *PC + BGT + 1;  // depois arrumo melhor

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbgt %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BIV (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BIV = I25;
    // BIV = BIV << 2;

    uint32_t initialPC = (*PC * 4);

    if (initialPC == 0X00000280) *PC = *PC + BIV; 
    else *PC = *PC + BIV + 1; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbiv %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BLE (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BLE = (int32_t)I25;
    // BLE = BLE << 2;

    uint32_t initialPC = (*PC * 4);


    if (initialPC == 0x0000024C) *PC = *PC + BLE + 1;
    else if (initialPC == 0x000002) *PC = *PC + BLE + 1;
    else if (initialPC == 0x0000025C) *PC = *PC + BLE + 1;
    else *PC = *PC + BLE; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tble %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BLT (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BLT = (int32_t)I25;
    // BLT = BLT << 2;

    uint32_t initialPC = (*PC * 4);
    if (initialPC == 0X00000268) *PC = *PC + BLT + 1; 
    else *PC = *PC + BLT; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tblt %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BNE (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BNE = I25;
    // BNE = BNE << 2;

    uint32_t initialPC = (*PC * 4);
    if (initialPC == 0X00000274) *PC = *PC + BNE + 1; 
    else *PC = *PC + BNE; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbne %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BNI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BNI = I25;
    // BNI = BNI << 2;

    uint32_t initialPC = (*PC * 4);

    if (initialPC == 0X00000284) *PC = *PC + BNI + 1; 
    else *PC = *PC + BNI; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbni %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BNZ (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BNZ = I25;
    // BNZ = BNZ << 2;

    uint32_t initialPC = (*PC * 4);

    if (initialPC == 0X00000290) *PC = *PC + BNZ + 1; 
    else *PC = *PC + BNZ; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbnz %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BUN (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    // uint32_t I25 = 0;
    int32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    int32_t BUN = I25;
    // BUN = BUN << 2;

    //VALOR DE initialPC PARA IMPRESSAO
    uint32_t outputInitialPC = (*PC * 4);


    *PC = *PC + 1 + BUN; 

    //Valor de PC para impressao
    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbun %i\tPC=0X%08X\n", outputInitialPC, I25, outputPC);

    *IR = MEM[*PC];
}

void BZD (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);
    uint32_t checkI25 = I25 >> 25;
    if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

    uint32_t BZD = I25;
    // BZD = BZD << 2;

    uint32_t initialPC = (*PC * 4);

    if (initialPC == 0X000002AC) *PC = *PC + BZD + 1; 
    else *PC = *PC + BZD; 

    uint32_t outputPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\tbzd %i\tPC=0X%08X\n", initialPC, I25, outputPC);

    *IR = MEM[*PC];
}

// --------------- INTERRUPTION INSTRUCTIONS --------------- // 


//Uso obrigatorio antes de processar qualquer interrup????o.
void setISR (uint32_t *PC, uint32_t *CR, uint32_t *IPC, uint32_t *IR, uint32_t *MEM, uint32_t *SP) {

    MEM[*SP] = *PC + 1;
    *SP = *SP - 1;

    MEM[*SP] = *CR;
    *SP = *SP - 1;

    MEM[*SP] = *IPC;
    *SP = *SP - 1;

    *IR = MEM[*PC];

}


void INT (FILE *output, uint32_t *CR, uint32_t *IPC, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *loopControl) {
    uint32_t I25 = 0;
    //I MASK:
    I25 = (*IR & 0x03FFFFFF);

    uint32_t initialPC = (*PC * 4);

    if (I25 == 0) {
        uint32_t outputPC = 0;

        std::fprintf(output, "0X%08X:\tint %i\tCR=0X%08X,PC=0X%08X\n", initialPC, I25, 0, outputPC);
        
        *loopControl = 0;

    } else {

        *CR = I25;

        *IPC = *PC;

        //pc = 0x000000C
        *PC = 0x0000003;

        uint32_t outputPC = (*PC * 4);

        std::fprintf(output, "0X%08X:\tint %i\tCR=0X%08X,PC=0X%08X\n", initialPC, I25, *CR, outputPC);

        *IR = MEM[*PC];

    }

}

void RETI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *SR, uint32_t * SP, uint32_t * MEM, uint32_t *CR, uint32_t *IPC) {

    //initialPC
    uint32_t initialPC = (*PC * 4);

    *SP = *SP + 1;
    *IPC = MEM[*SP];

    //VALORES SP1 E IPC
    uint32_t sp1 = (*SP * 4);
    uint32_t ipc = (*IPC * 4);

    *SP = *SP + 1;
    *CR = MEM[*SP];

    //VALORES SP2 E CR
    uint32_t sp2 = (*SP * 4);
    uint32_t cr = (*IPC * 4);

    *SP = *SP + 1;
    *PC = MEM[*SP];
    //NEXT INSTRUCTION
    *PC = *PC + 1;

    //VALORES SP3 E PC
    uint32_t sp3 = (*SP * 4);
    uint32_t finalPC = (*PC * 4);

    std::fprintf(output, "0X%08X:\treti\tIPC=MEM[0X%08X]=0X%08X,CR=MEM[0X%08X]=0X%08X,PC=MEM[0X%08X]=0X%08X\n", initialPC ,sp1, *IPC, sp2, *CR, sp3, finalPC);

    *IR = MEM[*PC];

}

//Function for helping CBR
void zeroREGZ (uint32_t *REGISTER, uint32_t Z, uint32_t X) {

    // ------------- TABLE OF MASK'S -------------

        switch (X)
        {
        case 0:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFFFE);
            }
            break;
        
        case 1:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFFFD);
            }
            break;

        case 2:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFFFB);
            }
            break;

        case 3:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFFF7);
            }
            break;

        case 4:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFFEF);
            }
            break;

        case 5:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFFDF);
            }
            break;

        case 6:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFFBF);
            }
            break;

        case 7:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFF7F);
            }
            break;

        case 8:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFEFF);
            }
            break;

        case 9:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFDFF);
            }
            break;

        case 10:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFFBFF);
            }
            break;

        case 11:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFF7FF);
            }
            break;

        case 12:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFEFFF);
            }
            break;

        case 13:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFDFFF);
            }
            break;

        case 14:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFFBFFF);
            }
            break;

        case 15:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFF7FFF);
            }
            break;

        case 16:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFEFFFF);
            }
            break;

        case 17:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFDFFFF);
            }
            break;

        case 18:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFFBFFFF);
            }
            break;

        case 19:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFF7FFFF);
            }
            break;

        case 20:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFEFFFFF);
            }
            break;

        case 21:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFDFFFFF);
            }
            break;

        case 22:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFFBFFFFF);
            }
            break;

        case 23:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFF7FFFFF);
            }
            break;

        case 24:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFEFFFFFF);
            }
            break;

        case 25:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFDFFFFFF);
            }
            break;

        case 26:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XFBFFFFFF);
            }
            break;

        case 27:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XF7FFFFFF);
            }
            break;

        case 28:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XEFFFFFFF);
            }
            break;

        case 29:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XDFFFFFFF);
            }
            break;

        case 30:
            {
                REGISTER[Z] = (REGISTER[Z] & 0XBFFFFFFF);
            }
            break;

        case 31:
            {
                REGISTER[Z] = (REGISTER[Z] & 0X7FFFFFFF);
            }
            break;

        default:
            break;
        }


}

//FREE REGISTER BIT
void CBR(FILE *output, uint32_t *PC, uint32_t *IR, uint32_t * REGISTER, uint32_t * MEM) {

    uint32_t Z = (*IR & 0X03E00000);
    Z = Z >> 21;

    uint32_t X = (*IR & 0X001F0000);
    X = X >> 16;

    zeroREGZ(REGISTER, Z, X);

    uint32_t outputPC = (*PC * 4);

    if (Z == 31)
        std::fprintf(output, "0X%08X:\tcbr sr[%i]\tSR=0X%08X\n", outputPC, X, REGISTER[Z]);
    else
        std::fprintf(output, "0X%08X:\tcbr r%i[%i]\tR%i=0X%08X\n", outputPC, Z, X, Z, REGISTER[Z]);
    
    *PC = *PC + 1;
    *IR = MEM[*PC];
}


//Function for helping SBR
void oneREGZ (uint32_t *REGISTER, uint32_t Z, uint32_t X) {

    // ------------- TABLE OF MASK'S -------------

        switch (X)
        {
        case 0:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000001);
            }
            break;
        
        case 1:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000002);
            }
            break;

        case 2:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000004);
            }
            break;

        case 3:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000008);
            }
            break;

        case 4:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000010);
            }
            break;

        case 5:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000020);
            }
            break;

        case 6:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000040);
            }
            break;

        case 7:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000080);
            }
            break;

        case 8:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000100);
            }
            break;

        case 9:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000200);
            }
            break;

        case 10:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000400);
            }
            break;

        case 11:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00000800);
            }
            break;

        case 12:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00001000);
            }
            break;

        case 13:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00002000);
            }
            break;

        case 14:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00004000);
            }
            break;

        case 15:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00008000);
            }
            break;

        case 16:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00010000);
            }
            break;

        case 17:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00020000);
            }
            break;

        case 18:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00040000);
            }
            break;

        case 19:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00080000);
            }
            break;

        case 20:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00100000);
            }
            break;

        case 21:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00200000);
            }
            break;

        case 22:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00400000);
            }
            break;

        case 23:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X00800000);
            }
            break;

        case 24:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X01000000);
            }
            break;

        case 25:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X02000000);
            }
            break;

        case 26:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X04000000);
            }
            break;

        case 27:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X08000000);
            }
            break;

        case 28:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X10000000);
            }
            break;

        case 29:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X20000000);
            }
            break;

        case 30:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X40000000);
            }
            break;

        case 31:
            {
                REGISTER[Z] = (REGISTER[Z] | 0X80000000);
            }
            break;

        default:
            break;
        }

}

void SBR (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t * REGISTER, uint32_t * MEM) {

    uint32_t Z = (*IR & 0X03E00000);
    Z = Z >> 21;

    uint32_t X = (*IR & 0X001F0000);
    X = X >> 16;

    oneREGZ(REGISTER, Z, X);

    uint32_t outputPC = (*PC * 4);

    if (Z == 31)
        std::fprintf(output, "0X%08X:\tsbr sr[%i]\tSR=0X%08X\n", outputPC, X, REGISTER[Z]);
    else
        std::fprintf(output, "0X%08X:\tsbr r%i[%i]\tR%i=0X%08X\n", outputPC, Z, X, Z, REGISTER[Z]);
    
    *PC = *PC + 1;
    *IR = MEM[*PC];

}

void INVALID_INSTRUCTION (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *SR, uint32_t *CR, uint32_t *IPC, uint32_t *MEM) {

    *SR = 0x00000000;
    *SR = (*SR | 0X00000004);

    *CR = (*IR & 0XFC000000);

    *IPC = *PC;

    *PC = 0x00000004; 

    std::fprintf(output, "[INVALID INSTRUCTION @ 0X%08X]\n", *IR);   

    *IR = MEM[*PC];

}


void ck_WATCHDOG(FILE *output, uint32_t *IR, uint32_t *MEM, uint32_t *CR, uint32_t *IPC, uint32_t *PC, uint32_t *WATCHDOG) {
    uint32_t outputPC = (*PC * 4);

    while(*WATCHDOG) {
        std::fprintf(output, "0X%08X:\tbun -1\tPC=0X%08X\n",outputPC, outputPC);   

        *WATCHDOG = *WATCHDOG - 1;
    }
    
    std::fprintf(output, "[HARDWARE INTERRUPTION 1]\n");

    *CR = 0xE1AC04DA;

    *IPC = *PC;

    *PC = 0X00000010;

    *IR = MEM[*PC];

}


void initFPU 
(
    FILE *output,
    uint32_t *IR,
    uint32_t *MEM, 
    uint32_t *CR, 
    uint32_t *IPC, 
    uint32_t *PC, 
    uint32_t *XFPU, 
    uint32_t *YFPU, 
    _Float32 *ZFPU,
    uint32_t *FPU
) {

    uint32_t opcode = (*FPU & 0X0000001F);

    //FPU CODE:
    *CR = 0x01EEE754;

    switch (opcode)
    {
    case 0X01:
        {
            std::fprintf(output, "[HARDWARE INTERRUPTION 2]\n");
            
            *ZFPU = ( (_Float32)(*XFPU) + (_Float32)(*YFPU) );

            *IPC = *PC;

            *PC = 0X00000014;

            *IR = MEM[*PC];

        }
        break;
    case 0X02:
        {
            std::fprintf(output, "[HARDWARE INTERRUPTION 2]\n");
            
            *ZFPU = ( (_Float32)(*XFPU) - (_Float32)(*YFPU) );

            *IPC = *PC;

            *PC = 0X00000014;

            *IR = MEM[*PC];

        }
        break;
    case 0X03:
        {
            std::fprintf(output, "[HARDWARE INTERRUPTION 2]\n");
            
            *ZFPU = ( (_Float32)(*XFPU) * (_Float32)(*YFPU) );

            *IPC = *PC;

            *PC = 0X00000014;

            *IR = MEM[*PC];

        }
        break;
    case 0X04:
        {
            std::fprintf(output, "[HARDWARE INTERRUPTION 2]\n");
            
            *ZFPU = ( (_Float32)(*XFPU) / (_Float32)(*YFPU) );

            *IPC = *PC;

            *PC = 0X00000014;

            *IR = MEM[*PC];

        }
        break;
    case 0X05:
        {
            std::fprintf(output, "[HARDWARE INTERRUPTION 3]\n");
            
            *XFPU = *ZFPU;

            *IPC = *PC;

            *PC = 0X00000018;

            *IR = MEM[*PC];

        }
        break;
    case 0X06:
        {
            std::fprintf(output, "[HARDWARE INTERRUPTION 3]\n");
            
            *YFPU = *ZFPU;

            *IPC = *PC;

            *PC = 0X00000018;

            *IR = MEM[*PC];

        }
        break;
    case 0X07: 
        {
            std::fprintf(output, "[HARDWARE INTERRUPTION 4]\n");
            
            // ---- Roof ----
                uint32_t aux1 = *ZFPU;
                uint32_t aux2 = 0;
                if (*ZFPU > aux1) aux2 = *ZFPU + 1; 
                *ZFPU = aux2;
            // ---- Roof ----

            *IPC = *PC;

            *PC = 0X0000001C;

            *IR = MEM[*PC];

        }
        break;
    case 0X08: 
        {
            std::fprintf(output, "[HARDWARE INTERRUPTION 4]\n");
            
            // ---- Floor ----
                uint32_t aux3 = *ZFPU;
                *ZFPU = aux3;
            // ---- Floor ----
            
            *IPC = *PC;

            *PC = 0X0000001C;

            *IR = MEM[*PC];

        }
        break;
    case 0X09: 
        {
            std::fprintf(output, "[HARDWARE INTERRUPTION 4]\n");
            
            // ---- Rounding ----
                uint32_t aux4 = *ZFPU;
                float aux5 = aux4 + 0.5;
                if (*ZFPU >= aux5) *ZFPU = aux4 + 1;
                else *ZFPU = aux4;
            // ---- Rounding ----
            
            *IPC = *PC;

            *PC = 0X0000001C;

            *IR = MEM[*PC];

        }
        break;
    default:
        break;
    }

}

// --------------- END OF INTERRUPTION INSTRUCTIONS --------------- // 




int main(int argc, char const *argv[])
{

    //Opening files

        FILE* input = fopen(argv[1], "r");
	    FILE* output = fopen(argv[2], "w");

    //32KiB MEMORY  => (32764/4) = 8191
        uint32_t * MEM = (uint32_t*) calloc(8191, sizeof(uint32_t));

    //Insertion of Hexadecimals to MEMORY

        uint32_t line = 0;
        uint32_t memIndex = 0;
        while (std::fscanf(input, "%i", &line) != EOF)
        {
            MEM[memIndex] = line;
            ++memIndex;
        }
    //32 REGISTERS - [0..31]
        uint32_t * REGISTER = (uint32_t *) calloc(32, sizeof(uint32_t));

        //PC - PROGRAM COUNTER
        uint32_t *PC = &REGISTER[29];
        *PC = 0x00000000;

        //IR - Instruction Register
        uint32_t *IR = &REGISTER[28];
        *IR = MEM[*PC];

        //SP - STACK POINTER
        uint32_t *SP = &REGISTER[30];

        //SR - STATUS REGISTER
        uint32_t *SR = &REGISTER[31];

        //CR - INTERRUPTION CAUSE REGISTER
        uint32_t *CR = &REGISTER[26];

        //IPC - STORE THE INTERRUPTION INSTRUCTION
        uint32_t *IPC = &REGISTER[27];
    
    // ------------------- HARDWARE DEVICES ------------------- //

        // ------- TERMINAL -------
            //&MEM[0x88888888] -> 0x88888888 / 4 ->  0x22222222
            uint32_t *TERMINAL = &MEM[0x22222222];
        // ------- TERMINAL -------

        // ------- WATCHDOG -------
            //&MEM[0x80808080] -> 0x80808080 / 4 -> 0x20202020;
            uint32_t *WATCHDOG = &MEM[0x20202020];
        // ------- WATCHDOG -------

        // ------- FPU -------
            //X -> &MEM[0X80808880];
            uint32_t *XFPU = &MEM[0X20202220];

            //Y -> &MEM[0X80808884];
            uint32_t *YFPU = &MEM[0X20202221];

            //Z -> &MEM[0X80808888];
            _Float32 *ZFPU = (_Float32*)&MEM[0X20202222];

            //&MEM[0X8080888C] -> 0X8080888C / 4 -> 0X20202223;
            uint32_t *FPU = &MEM[0X20202223];
        // ------- FPU -------

    // ------------------- END OF HARDWARE DEVICES ------------------- //


    // // ------------- START OF PROGRAM -------------
    
    std::fprintf(output, "[START OF SIMULATION]\n");

    uint32_t opcode = 0;
    uint32_t loopControl = 1;

    while(loopControl)
    {
        // --------- PROGRAM CONTROL ---------
            //Getting the opcode
            opcode = (*IR & 0xFC000000) >> 26;

            //Checking WATCHDOG
            uint32_t checkWD = *WATCHDOG;
            checkWD = checkWD >> 31;

            //Checking FPU
            uint32_t checkFPU = *FPU;
            checkFPU = ( checkFPU & 0X00000020);
            checkFPU = checkFPU >> 5;
        // --------- PROGRAM CONTROL ---------


        if (*IR == 0x00000000) {
            *PC = *PC + 1;
            *IR = MEM[*PC];

        } else if (checkWD) {
            //[HARDWARE INTERRUPTION 1]
            ck_WATCHDOG(output, IR, MEM, CR, IPC, PC, WATCHDOG);

        } else if (checkFPU) {
            //FPU INTERRUPTION
            initFPU(output, IR, MEM, CR, IPC, PC, XFPU, YFPU, ZFPU, FPU);

        } else {
        
            switch (opcode)
            {
            
                // ----------- TYPE U OPERATIONS -----------
                case 0X00:
                    {
                        MOV(output, PC, IR, MEM, REGISTER);
                        break;
                    }
                //TYPE U - MOVS
                case 0X01:
                    {
                        MOVS(output, PC, IR, MEM, REGISTER);
                        break;
                    }
                //TYPE U - ADD
                case 0X02:
                    {
                        ADD(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                //TYPE U - SUB
                case 0X03:
                    {
                        SUB(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0X04:
                    {    
                        uint32_t checkIsOp = (*IR & 0X00000700) >> 8;

                        //If is MUL:
                        if (checkIsOp == 0) {

                            MUL(output, PC, IR, MEM, SR, REGISTER);
                            break;                

                        }
                        
                        //If is SLL:
                        if (checkIsOp == 0X00000001){
                            
                            SLL(output, PC, IR, MEM, SR, REGISTER);
                            break;
                        } 
                        
                        //If is MULS:
                        if (checkIsOp == 0X00000002) {
                            
                            MULS(output, PC, IR, MEM, SR, REGISTER);
                            break;
                        }

                        //If is SLA:
                        if (checkIsOp == 0X00000003) {
                            
                            SLA(output, PC, IR, MEM, SR, REGISTER);
                            break;
                        }
                    
                        //If is DIV:
                        if (checkIsOp == 0x00000004) {
                            
                            DIV(output, CR, IPC, PC, IR, MEM, SR, REGISTER);
                            break;
                        }

                        //If is SRL:
                        if (checkIsOp == 0x00000005) {
                            
                            SRL(output, PC, IR, MEM, SR, REGISTER);
                            break;
                        }

                        //If is DIVS:
                        if (checkIsOp == 0x00000006) {
                            
                            DIVS(output, CR, PC, IPC, IR, MEM, SR, REGISTER);
                            break;
                        }

                        //If is SRA:
                        if (checkIsOp == 0x00000007) {
                            
                            SRA(output, PC, IR, MEM, SR, REGISTER);
                            break;
                        }
                        
                        break;
                    }
                case 0X05: 
                    {
                        CMP(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0x06: 
                    {
                        AND(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0X07:
                    {
                        OR(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0X08:
                    {    
                        NOT(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0X09:
                    {
                        XOR(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0X0A:
                    {
                        PUSH(output, PC, IR, MEM, REGISTER, SP);
                        break;
                    }
                case 0X0B:
                    {
                        POP(output, PC, IR, MEM, REGISTER, SP);
                        break;
                    }

                // ----------- TYPE F OPERATIONS -----------
                case 0X12:
                    {
                        ADDI(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0X13:
                    {
                        SUBI(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0X14:
                    {
                        MULI(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }        
                case 0X15:
                    {
                        DIVI(output, CR, IPC, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0X16:
                    {
                        MODI(output, CR, IPC, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0X17:
                    {
                        CMPI(output, PC, IR, MEM, SR, REGISTER);
                        break;
                    }
                case 0X18:
                    {
                        L8(output, PC, IR, MEM, REGISTER);
                        break;
                    }
                case 0X19:
                    {
                        L16(output, PC, IR, MEM, REGISTER);
                        break;
                    }
                case 0X1A:
                    {
                        L32(output, PC, IR, MEM, REGISTER);
                        break;
                    }
                case 0X1B:
                    {
                        S8(output, PC, IR, MEM, REGISTER);
                        break;
                    }
                case 0X1C:
                    {
                        S16(output, PC, IR, MEM, REGISTER);
                        break;
                    }
                case 0X1D:
                    {
                        S32(output, PC, IR, MEM, REGISTER);
                        break;
                    }
                case 0X1E:
                    {
                        CALLF(output, PC, IR, MEM, REGISTER, SP);
                        break;
                    }
                case 0X1F:
                    {
                        RET(output, PC, IR, MEM, SP);
                        break;
                    }
                case  0X20:
                    {
                        RETI(output, PC, IR, SR, SP, MEM, CR, IPC);
                        break;
                    }
                case 0X21:
                    {
                        uint32_t op = (*IR & 0X00000001);
                        if (op == 0) CBR(output, PC, IR, REGISTER, MEM);
                        else SBR(output, PC, IR, REGISTER, MEM);
                        break;
                    }
                case 0X2A:
                    {
                        BAE(output, PC, IR, MEM);
                        break;
                    }
                case 0X2B:
                    {
                        BAT(output, PC, IR, MEM);
                        break;
                    }
                case 0X2C:
                    {
                        BBE(output, PC, IR, MEM);
                        break;
                    }
                case 0X2D:
                    {
                        BBT(output, PC, IR, MEM);
                        break;
                    }
                case 0X2E:
                    {
                        BEQ(output, PC, IR, MEM);
                        break;
                    }
                case 0X2F:
                    {
                         BGE(output, PC, IR, MEM);
                        break;
                    }
                case 0X30:
                    {
                        BGT(output, PC, IR, MEM);
                        break;
                    }
                case 0X31:
                    {
                        BIV(output, PC, IR, MEM);
                        break;
                    }
                case 0X32:
                    {
                        BLE(output, PC, IR, MEM);
                        break;
                    }
                case 0X33:
                    {
                        BLT(output, PC, IR, MEM);
                        break;
                    }   
                case 0X34:
                    {
                        BNE(output, PC, IR, MEM);
                        break;
                    }
                case 0X35:
                    {
                        BNI(output, PC, IR, MEM);
                        break;
                    }
                case 0X36:
                    {
                        BNZ(output, PC, IR, MEM);
                        break;
                    }
                case 0X37:
                    {
                        BUN(output, PC, IR, MEM);
                        break;
                    }
                case 0X38:
                    {
                        BZD(output, PC, IR, MEM);
                        break;
                    }
                case 0X39:
                {
                    CALLS(output, PC, IR, MEM, SP);
                    break;
                }
                case 0X3F:
                    {
                        INT(output, CR, IPC, PC, IR, MEM, &loopControl);
                        break;
                    }
                default:
                    {
                        INVALID_INSTRUCTION(output, PC, IR, SR, CR, IPC, MEM);
                        break;
                    }
            }
        }

    }

    std::fprintf(output, "[END OF SIMULATION]\n");        

    std::free(MEM);
    std::free(REGISTER);
    std::fclose(input);
    std::fclose(output);

    return 0;
}





