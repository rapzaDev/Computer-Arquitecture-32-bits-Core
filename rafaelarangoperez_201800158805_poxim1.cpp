#include <iostream>
#include <stdint.h>
#include <string>
#include <math.h>

using namespace std;

void NOP (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    std::fprintf(output, "0X%08X:\tmov r0,0\tR0=0X%08X", *PC, 0);
                
    //UPDATING PC & IR VALUES
    *PC = *PC + 4;
    *IR = MEM[*PC];
}

void MOV (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //TYPE U - Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t attributionValue = (*IR & 0x001FFFFF);
    REGISTER[Z] = attributionValue;

    std::fprintf(output, "0X%08X:\tmov r%i,%i\tR%i=0X%08X", *PC, Z, attributionValue, Z, attributionValue);

    //UPDATING PC & IR VALUES
    *PC = *PC + 4;
    *IR = MEM[*PC];
}

void MOVS (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
    uint32_t Z = 0;
    //Z MASK:
    Z = (*IR & 0x03E00000) >> 21;

    uint32_t attributionValue = (*IR & 0x001FFFFF);
            
    //MASK FOR X BIT 
    uint32_t bitX = (attributionValue & 0X00100000) >> 20;

    if (bitX == 1) attributionValue = (attributionValue | 0XFFE00000);

    REGISTER[Z] = attributionValue;

    std::fprintf(output, "0X%08X:\tmovs r%i,%i\tR%i=0X%08X", *PC, Z, attributionValue, Z, attributionValue);

    //UPDATING PC & IR VALUES
    *PC = *PC + 4;
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

            REGISTER[Z] = ( REGISTER[X] + REGISTER[Y] );

            //SN CHECK -> R[Z]31 = 1
            uint32_t checkSN = REGISTER[Z] >> 31;

            //CY CHECK = R[Z]32 = 1
            uint64_t result = ( REGISTER[X] + REGISTER[Y] ); 
            result = result >> 32;

            //OV CHECK
            uint32_t RX31 =  REGISTER[X] >> 31;
            uint32_t RY31 =  REGISTER[Y] >> 31;
            uint32_t RZ31 =  REGISTER[Z] >> 31;

            //ZN CHECK -> R[Z] = 0
            if (REGISTER[Z] == 0) {
                *SR = 0x00000040;

                std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
            } else if (checkSN == 1){
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
            } else if (result == 1) {
                *SR = 0X00000001;

                std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
            } else if ( (RX31 == RY31) && (RZ31 != RX31) ) {
                *SR = 0X00000008;

                std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];        
            } else {
                *SR = 0X00000000;
                std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
            }

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

            //SN CHECK -> R[Z]31 = 1
            uint32_t checkSN = REGISTER[Z] >> 31;

            //CY CHECK = R[Z]32 = 1
            uint64_t result = ( REGISTER[X] + REGISTER[Y] ); 
            result = result >> 32;

            //OV CHECK
            uint32_t RX31 =  REGISTER[X] >> 31;
            uint32_t RY31 =  REGISTER[Y] >> 31;
            uint32_t RZ31 =  REGISTER[Z] >> 31;

            //ZN CHECK -> R[Z] = 0
            if (REGISTER[Z] == 0) {
                *SR = 0x00000040;

                std::fprintf(output, "0X%08X:\tsub r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (checkSN == 1) {
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\tsub r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
            } else if (result == 1) {
                *SR = 0X00000001;

                std::fprintf(output, "0X%08X:\tsub r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
            } else if ( (RX31 != RY31) && (RZ31 != RX31) ) {
                *SR = 0X00000008;

                std::fprintf(output, "0X%08X:\tsub r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
            } else {
                *SR = 0X00000000;
                std::fprintf(output, "0X%08X:\tsub r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
            }

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

                REGISTER[Z] = (uint32_t)( result & 0X00000000FFFFFFFF );

                REGISTER[RI4] = (uint32_t)(result >> 32);

                //ZN CASE:
                if (result == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tmul r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else if (REGISTER[RI4] != 0) {
                    *SR = 0X00000001;

                    std::fprintf(output, "0X%08X:\tmul r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];
                } else {
                    *SR = 0X00000000;
                    std::fprintf(output, "0X%08X:\tmul r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);    

                    *PC = *PC + 4;
                    *IR = MEM[*PC];
                }

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

                uint32_t RI4 = ( *IR & 0X0000001F );

                //Adding RZ value first:
                uint64_t result = REGISTER[Z];
                result = result << 32;

                //Adding RY value:
                result = (result | REGISTER[Y]);

                //Applying offset:
                uint32_t powV = RI4 + 1; 
                uint32_t offset = pow(2, powV);
                result = result << offset;

                //Separating result: 
                REGISTER[X] = (uint32_t)(result & 0X00000000FFFFFFFF);
                REGISTER[Z] = (uint32_t)(result >> 32);

                //ZN CASE:
                if (result == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tsll r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else if (REGISTER[Z] != 0) {
                    *SR = 0X00000001;

                    std::fprintf(output, "0X%08X:\tsll r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];
                } else {
                    *SR = 0X00000000;
                    std::fprintf(output, "0X%08X:\tsll r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];
                }

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

                REGISTER[Z] = (uint32_t)( result & 0X00000000FFFFFFFF );

                REGISTER[RI4] = (uint32_t)(result >> 32);

                //ZN CASE:
                if (result == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tmuls r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else if (REGISTER[RI4] != 0) {
                    *SR = 0X00000008;

                    std::fprintf(output, "0X%08X:\tmuls r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];
                } else {
                    *SR = 0X00000000;
                    std::fprintf(output, "0X%08X:\tmuls r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);    

                    *PC = *PC + 4;
                    *IR = MEM[*PC];
                }

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

                uint32_t RI4 = ( *IR & 0X0000001F );

                //Adding RZ value first:
                int64_t result = (int32_t)REGISTER[Z];
                result = result << 32;

                //Adding RY value:
                result = (result | (int32_t)REGISTER[Y]);

                //Applying offset:
                uint32_t powV = RI4 + 1; 
                uint32_t offset = pow(2, powV);
                result = result << offset;

                //Separating result: 
                REGISTER[X] = (uint32_t)(result & 0X00000000FFFFFFFF);
                REGISTER[Z] = (uint32_t)result >> 32;

                //ZN CASE:
                if (result == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tsla r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else if (REGISTER[Z] != 0) {
                    *SR = 0X00000008;

                    std::fprintf(output, "0X%08X:\tsla r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];
                } else {
                    *SR = 0X00000000;
                    std::fprintf(output, "0X%08X:\tsla r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);    

                    *PC = *PC + 4;
                    *IR = MEM[*PC];
                }

}

void DIV (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
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

                REGISTER[RI4] = ( REGISTER[X] % REGISTER[Y] );

                REGISTER[Z] = ( REGISTER[X] / REGISTER[Y] );

                //ZN CASE:
                if (REGISTER[Z] == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tdiv r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else if (REGISTER[Y] == 0) {
                    *SR = 0x00000020;

                    std::fprintf(output, "0X%08X:\tdiv r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else if (REGISTER[RI4] != 0) {
                    *SR = 0X00000001;

                    std::fprintf(output, "0X%08X:\tdiv r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else {
                    *SR = 0X00000000;
                    std::fprintf(output, "0X%08X:\tdiv r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);    

                    *PC = *PC + 4;
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
                uint32_t offset = pow(2, powV);
                result = result >> offset;

                //Separating result: 
                REGISTER[X] = (uint32_t)(result & 0X00000000FFFFFFFF);
                REGISTER[Z] = (uint32_t)(result >> 32);

                //ZN CASE:
                if (result == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tsrl r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i>>%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else if (REGISTER[Z] != 0) {
                    *SR = 0X00000001;

                    std::fprintf(output, "0X%08X:\tsrl r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i>>%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else {
                    *SR = 0X00000000;
                    std::fprintf(output, "0X%08X:\tsrl r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i>>%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);    

                    *PC = *PC + 4;
                    *IR = MEM[*PC];
                }

}

void DIVS (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
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

                //ZN CASE:
                if (REGISTER[Z] == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tdivs r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else if (REGISTER[Y] == 0) {
                    *SR = 0x00000020;

                    std::fprintf(output, "0X%08X:\tdivs r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else if (REGISTER[RI4] != 0) {
                    *SR = 0X00000008;

                    std::fprintf(output, "0X%08X:\tdivs r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else {
                    *SR = 0X00000000;
                    std::fprintf(output, "0X%08X:\tdivs r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

                    *PC = *PC + 4;
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

                uint32_t RI4 = ( *IR & 0X0000001F );

                //Adding RZ value first:
                int64_t result = (int32_t)REGISTER[Z];
                result = result << 32;

                //Adding RY value:
                result = (result | (int32_t)REGISTER[Y]);

                //Applying offset:
                uint32_t powV = RI4 + 1; 
                uint32_t offset = pow(2, powV);
                result = result >> offset;

                //Separating result: 
                REGISTER[X] = (uint32_t)(result & 0X00000000FFFFFFFF);
                REGISTER[Z] = (uint32_t)(result >> 32);

                //ZN CASE:
                if (result == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tsra r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i>>%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, Y, Z, X, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else if (REGISTER[Z] != 0) {
                    *SR = 0X00000008;

                    std::fprintf(output, "0X%08X:\tsra r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i>>%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, Y, Z, X, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                } else {
                    *SR = 0X00000000;
                    std::fprintf(output, "0X%08X:\tsra r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i>>%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, Y, Z, X, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];
                }

}

void CMP (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
    uint32_t X = 0;
            //X MASK:
            X = (*IR & 0x001F0000) >> 16;

            uint32_t Y = 0;
            //Y MASK:
            Y = (*IR & 0x0000F800) >> 11;

            //CMP:
            uint64_t CMP = (uint64_t)REGISTER[X] - (uint64_t)REGISTER[Y];

            //SN CASE:
            uint32_t cmp31 = (uint32_t)(CMP & 0x00000000FFFFFFFF);
            cmp31 = cmp31 >> 31;

            //OV CASE:
            uint32_t RX31 = (REGISTER[X] >> 31);
            uint32_t RY31 = (REGISTER[Y] >> 31);

            //CY CASE:
            uint64_t CMPCY = ( CMP & 0X0000000100000000 );
            CMPCY = CMPCY >> 32;

            //ZN CASE:
            if (CMP == 0) {
                *SR = 0x00000040;

                std::fprintf(output, "0X%08X:\tcmp r%i,r%i\tSR=0X%08X", *PC, X, Y, *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (cmp31 == 1) {
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\tcmp r%i,r%i\tSR=0X%08X", *PC, X, Y, *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if ( (RX31 != RY31) && (cmp31 != RX31) ) {
                *SR = 0x00000008;

                std::fprintf(output, "0X%08X:\tcmp r%i,r%i\tSR=0X%08X", *PC, X, Y, *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (CMPCY == 1) {
                *SR = 0x00000001;

                std::fprintf(output, "0X%08X:\tcmp r%i,r%i\tSR=0X%08X", *PC, X, Y, *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else {
                *SR = 0X00000000;
                std::fprintf(output, "0X%08X:\tcmp r%i,r%i\tSR=0X%08X", *PC, X, Y, *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];    
            }
            
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

            REGISTER[Z] = ( REGISTER[X] & REGISTER[Y] );

            //SN CASE:
            uint32_t RZ31 = (REGISTER[Z] >> 31);

            //ZN CASE:
            if (REGISTER[Z] == 0){
                *SR = 0X00000040;

                std::fprintf(output, "0X%08X:\tand r%i,r%i,r%i\tR%i=R%i&R%i=0X%08X,SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (RZ31 == 1) {
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\tand r%i,r%i,r%i\tR%i=R%i&R%i=0X%08X,SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else {
                *SR = 0x00000000;

                std::fprintf(output, "0X%08X:\tand r%i,r%i,r%i\tR%i=R%i&R%i=0X%08X,SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];    

            }

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

            REGISTER[Z] = ( REGISTER[X] | REGISTER[Y] );

            //SN CASE:
            uint32_t RZ31 = (REGISTER[Z] >> 31);

            //ZN CASE:
            if (REGISTER[Z] == 0){
                *SR = 0X00000040;

                std::fprintf(output, "0X%08X:\tor r%i,r%i,r%i\tR%i=R%i|R%i=0X%08X,SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (RZ31 == 1) {
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\tor r%i,r%i,r%i\tR%i=R%i|R%i=0X%08X,SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else {
                *SR = 0x00000000;

                std::fprintf(output, "0X%08X:\tor r%i,r%i,r%i\tR%i=R%i|R%i=0X%08X,SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];    

            }

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

            //ZN CASE:
            if (REGISTER[Z] == 0){
                *SR = 0X00000040;

                std::fprintf(output, "0X%08X:\tnot r%i,r%i\tR%i=~R%i=0X%08X,SR=0X%08X", *PC, Z, X, Z, X, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (RZ31 == 1) {
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\tnot r%i,r%i\tR%i=~R%i=0X%08X,SR=0X%08X", *PC, Z, X, Z, X, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else {
                *SR = 0x00000000;

                std::fprintf(output, "0X%08X:\tnot r%i,r%i\tR%i=~R%i=0X%08X,SR=0X%08X", *PC, Z, X, Z, X, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
            }

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

            REGISTER[Z] = ( REGISTER[X] ^ REGISTER[Y] );

            //SN CASE:
            uint32_t RZ31 = (REGISTER[Z] >> 31);

            //ZN CASE:
            if (REGISTER[Z] == 0){
                *SR = 0X00000040;

                std::fprintf(output, "0X%08X:\txor r%i,r%i,r%i\tR%i=R%i^R%i=0X%08X,SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
                
            } else if (RZ31 == 1) {
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\txor r%i,r%i,r%i\tR%i=R%i^R%i=0X%08X,SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else {
                *SR = 0x00000000;

                std::fprintf(output, "0X%08X:\txor r%i,r%i,r%i\tR%i=R%i^R%i=0X%08X,SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];    

            }

}

void ADDI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
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

            REGISTER[Z] = REGISTER[X] + I15;

            //SN CASE:
            uint32_t RZ31 = (REGISTER[Z] >> 31);

            //OV CASE:
            uint32_t RX31 = (REGISTER[X] >> 31);

            //CY CASE:
            uint64_t result = (uint64_t)(REGISTER[X] + I15);
            result = ( result & 0X0000000100000000 );
            result = result >> 32;

            //ZN CASE:
            if (REGISTER[Z] == 0){
                *SR = 0X00000040;

                std::fprintf(output, "0X%08X:\taddi r%i,r%i,%i\tR%i=R%i+0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (RZ31 == 1) {
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\taddi r%i,r%i,%i\tR%i=R%i+0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if ( (RX31 != checkI15) && (RZ31 != RX31) ) {
                *SR = 0x00000008;

                std::fprintf(output, "0X%08X:\taddi r%i,r%i,%i\tR%i=R%i+0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (result == 1) {
                *SR = 0x00000001;

                std::fprintf(output, "0X%08X:\taddi r%i,r%i,%i\tR%i=R%i+0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else {
                *SR = 0x00000000;

                std::fprintf(output, "0X%08X:\taddi r%i,r%i,%i\tR%i=R%i+0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];    
            }

}

void SUBI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
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

            REGISTER[Z] = REGISTER[X] - I15;

            //SN CASE:
            uint32_t RZ31 = (REGISTER[Z] >> 31);

            //OV CASE:
            uint32_t RX31 = (REGISTER[X] >> 31);

            //CY CASE:
            uint64_t result = ((uint64_t)REGISTER[X] + (uint64_t)I15);
            result = ( result & 0X0000000100000000 );
            result = result >> 32;

            //ZN CASE:
            if (REGISTER[Z] == 0){
                *SR = 0X00000040;

                std::fprintf(output, "0X%08X:\tsubi r%i,r%i,%i\tR%i=R%i-0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (RZ31 == 1) {
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\tsubi r%i,r%i,%i\tR%i=R%i-0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if ( (RX31 != checkI15) && (RZ31 != RX31) ) {
                *SR = 0x00000008;

                std::fprintf(output, "0X%08X:\tsubi r%i,r%i,%i\tR%i=R%i-0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (result == 1) {
                *SR = 0x00000001;

                std::fprintf(output, "0X%08X:\tsubi r%i,r%i,%i\tR%i=R%i-0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else {
                *SR = 0x00000000;

                std::fprintf(output, "0X%08X:\tsubi r%i,r%i,%i\tR%i=R%i-0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];    
            }

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
            int64_t result = (int64_t)REGISTER[X] * (int64_t)I15;
            result = ( result & 0XFFFFFFFF00000000);
            result = result >> 32; 

            //ZN CASE:
            if (REGISTER[Z] == 0){
                *SR = 0X00000040;

                std::fprintf(output, "0X%08X:\tmuli r%i,r%i,%i\tR%i=R%i*0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (result != 0) {
                *SR = 0x00000008;

                std::fprintf(output, "0X%08X:\tmuli r%i,r%i,%i\tR%i=R%i*0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else {
                *SR = 0x00000000;

                std::fprintf(output, "0X%08X:\tmuli r%i,r%i,%i\tR%i=R%i*0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];    
            }

}

void DIVI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
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

            REGISTER[Z] = (uint32_t)((int32_t)REGISTER[X] / (int32_t)I15);

            //ZN CASE:
            if (REGISTER[Z] == 0){
                *SR = 0X00000040;

                std::fprintf(output, "0X%08X:\tdivi r%i,r%i,%i\tR%i=R%i/0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
                
            } else if (I15 == 0) {
                *SR = 0X00000020;

                std::fprintf(output, "0X%08X:\tdivi r%i,r%i,%i\tR%i=R%i/0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else {
                *SR = 0X00000000;

                std::fprintf(output, "0X%08X:\tdivi r%i,r%i,%i\tR%i=R%i/0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];    
            }

}

void MODI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *SR, uint32_t *REGISTER) {
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

            REGISTER[Z] = (uint32_t)((int32_t)REGISTER[X] % (int32_t)I15);

            //ZN CASE:
            if (REGISTER[Z] == 0){
                *SR = 0X00000040;

                std::fprintf(output, "0X%08X:\tmodi r%i,r%i,%i\tR%i=R%i%%0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
                
            } else if (I15 == 0) {
                *SR = 0X00000020;

                std::fprintf(output, "0X%08X:\tmodi r%i,r%i,%i\tR%i=R%i%%0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
            } else {
                *SR = 0X00000000;

                std::fprintf(output, "0X%08X:\tmodi r%i,r%i,%i\tR%i=R%i%%0X%08X=0X%08X,SR=0X%08X", *PC, Z, X, I15, Z, X, I15, REGISTER[Z], *SR);

                *PC = *PC + 4;
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

            //ZN CASE:
            if (CMPI == 0){
                *SR = 0X00000040;

                std::fprintf(output, "0X%08X:\tcmpi r%i,%i\tSR=0X%08X", *PC, X, I15, *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];
                
            } else if (CMPI31 == 1) {
                *SR = 0X00000010;

                std::fprintf(output, "0X%08X:\tcmpi r%i,%i\tSR=0X%08X", *PC, X, I15, *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if ((RX31 != checkI15) && (CMPI31 != RX31)) {
                *SR = 0X00000004;

                std::fprintf(output, "0X%08X:\tcmpi r%i,%i\tSR=0X%08X", *PC, X, I15, *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else if (CMP32 == 1) {
                *SR = 0X00000001;

                std::fprintf(output, "0X%08X:\tcmpi r%i,%i\tSR=0X%08X", *PC, X, I15, *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            } else {
                *SR = 0X00000000;
            
                std::fprintf(output, "0X%08X:\tcmpi r%i,%i\tSR=0X%08X", *PC, X, I15, *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

            }
            
}

void L8 (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM, uint32_t *REGISTER) {
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

            uint32_t L8 = REGISTER[X] + I15;
            
            REGISTER[Z] = MEM[L8];

            std::fprintf(output, "0X%08X:\tl8 r%i,[r%i+-%i]\tR%i=MEM[0X%08X]=0X%02X", *PC, Z, X, I15, Z, L8, REGISTER[Z]);

            *PC = *PC + 4;
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

            std::fprintf(output, "0X%08X:\tl16 r%i,[r%i+-%i]\tR%i=MEM[0X%08X]=0X%04X", *PC, Z, X, I15, Z, L16, REGISTER[Z]);

            *PC = *PC + 4;
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

            std::fprintf(output, "0X%08X:\tl32 r%i,[r%i+-%i]\tR%i=MEM[0X%08X]=0X%08X", *PC, Z, X, I15, Z, L32, REGISTER[Z]);

            *PC = *PC + 4;
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

            std::fprintf(output, "0X%08X:\ts8 [r%i+-%i],r%i\tMEM[0X%08X]=R%i=0X%02X", *PC, X, I15, Z, S8, Z, REGISTER[Z]);

            *PC = *PC + 4;
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

            std::fprintf(output, "0X%08X:\ts16 [r%i+-%i],r%i\tMEM[0X%08X]=R%i=0X%04X", *PC, X, I15, Z, S16, Z, REGISTER[Z]);

            *PC = *PC + 4;
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

            std::fprintf(output, "0X%08X:\ts32 [r%i+-%i],r%i\tMEM[0X%08X]=R%i=0X%08X", *PC, X, I15, Z, S32, Z, REGISTER[Z]);

            *PC = *PC + 4;
            *IR = MEM[*PC];
}

void BAE (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BAE = I25;
            BAE = BAE << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BAE; 

            std::fprintf(output, "0X%08X:\tbae %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BAT (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BAT = I25;
            BAT = BAT << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BAT; 

            std::fprintf(output, "0X%08X:\tbat %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BBE (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BBE = I25;
            BBE = BBE << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BBE; 

            std::fprintf(output, "0X%08X:\tbbe %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BBT (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BBT = I25;
            BBT = BBT << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BBT; 

            std::fprintf(output, "0X%08X:\tbbt %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BEQ (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BEQ = I25;
            BEQ = BEQ << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BEQ; 

            std::fprintf(output, "0X%08X:\tbeq %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BGE (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BGE = (int32_t)I25;
            BGE = BGE << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BGE; 

            std::fprintf(output, "0X%08X:\tbge %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BGT (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BGT = (int32_t)I25;
            BGT = BGT << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BGT; 

            std::fprintf(output, "0X%08X:\tbgt %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BIV (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BIV = I25;
            BIV = BIV << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BIV; 

            std::fprintf(output, "0X%08X:\tbiv %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BLE (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BLE = (int32_t)I25;
            BLE = BLE << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BLE; 

            std::fprintf(output, "0X%08X:\tble %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BLT (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BLT = (int32_t)I25;
            BLT = BLT << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BLT; 

            std::fprintf(output, "0X%08X:\tblt %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BNE (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BNE = I25;
            BNE = BNE << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BNE; 

            std::fprintf(output, "0X%08X:\tbne %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BNI (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BNI = I25;
            BNI = BNI << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BNI; 

            std::fprintf(output, "0X%08X:\tbni %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BNZ (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BNZ = I25;
            BNZ = BNZ << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BNZ; 

            std::fprintf(output, "0X%08X:\tbnz %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BUN (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BUN = I25;
            BUN = BUN << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BUN; 

            std::fprintf(output, "0X%08X:\tbun %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void BZD (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *MEM) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);
            uint32_t checkI25 = I25 >> 25;
            if (checkI25 == 1) I25 = ( I25 | 0xFC000000);

            uint32_t BZD = I25;
            BZD = BZD << 2;

            uint32_t initialPC = *PC;
            *PC = *PC + 4 + BZD; 

            std::fprintf(output, "0X%08X:\tbzd %i\tPC=0X%08X", initialPC, I25, *PC);

            *IR = MEM[*PC];
}

void INT (FILE *output, uint32_t *PC, uint32_t *IR, uint32_t *loopControl) {
    uint32_t I25 = 0;
            //I MASK:
            I25 = (*IR & 0x03FFFFFF);

            if (I25 == 0) std::fprintf(output, "0X%08X:\tint %i\tCR=0X%08X,PC=0X%08X", *PC, I25, 0, *PC);

            *loopControl = 0;
            // *PC = *PC + 4;
            // *IR = MEM[*PC];
}


int main(int argc, char const *argv[])
{

    //Opening files

        FILE* input = fopen(argv[1], "r");
	    FILE* output = fopen(argv[2], "w");

    //32KiB MEMORY  => (32768/4) = 8192
        uint32_t * MEM = (uint32_t*) calloc(8192, sizeof(uint32_t));

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
        

    // // ------------- START OF PROGRAM -------------
    
    std::fprintf(output, "[START OF SIMULATION]\n");

    uint32_t opcode = 0;
    uint32_t loopControl = 1;

    while(loopControl)
    {
        opcode = (*IR & 0xFC000000) >> 26;

        switch (opcode)
        {
        
        // ----------- TYPE U OPERATIONS -----------
        case 0X00:
            
            if (*IR == 0){
                NOP(output, PC, IR, MEM);
            }
            else {
                MOV(output, PC, IR, MEM, REGISTER);
            }

            break;
        
        //TYPE U - MOVS
        case 0X01:

            MOVS(output, PC, IR, MEM, REGISTER);

            break;

        //TYPE U - ADD
        case 0X02:

            ADD(output, PC, IR, MEM, SR, REGISTER);

            break;
        
        //TYPE U - SUB
        case 0X03:

            SUB(output, PC, IR, MEM, SR, REGISTER);

            break;

        case 0X04:
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
                 
                DIV(output, PC, IR, MEM, SR, REGISTER);
                break;
            }

            //If is SRL:
            if (checkIsOp == 0x00000005) {
                
                SRL(output, PC, IR, MEM, SR, REGISTER);
                break;
            }

            //If is DIVS:
            if (checkIsOp == 0x00000006) {
                
                DIVS(output, PC, IR, MEM, SR, REGISTER);
                break;
            }

            //If is SRA:
            if (checkIsOp == 0x00000007) {
                
                SRA(output, PC, IR, MEM, SR, REGISTER);
                break;
            }
            
            break;

        case 0X05: 
            
            CMP(output, PC, IR, MEM, SR, REGISTER);
            break;
        
        case 0x06: 

            AND(output, PC, IR, MEM, SR, REGISTER);
            break;

        case 0X07:

            OR(output, PC, IR, MEM, SR, REGISTER);
            break;

        case 0X08:
                
            NOT(output, PC, IR, MEM, SR, REGISTER);
            break;

        case 0X09:

            XOR(output, PC, IR, MEM, SR, REGISTER);
            break;


        // ----------- TYPE F OPERATIONS -----------
        case 0X12:

            ADDI(output, PC, IR, MEM, SR, REGISTER);
            break;

        case 0X13:

            SUBI(output, PC, IR, MEM, SR, REGISTER);
            break;

        case 0X14:

            MULI(output, PC, IR, MEM, SR, REGISTER);
            break;
        
        case 0X15:

            DIVI(output, PC, IR, MEM, SR, REGISTER);
            break;

        case 0X16:

            MODI(output, PC, IR, MEM, SR, REGISTER);
            break;

        case 0X17:

            CMPI(output, PC, IR, MEM, SR, REGISTER);
            break;

        case 0X18:

            L8(output, PC, IR, MEM, REGISTER);
            break;

        case 0X19:

            L16(output, PC, IR, MEM, REGISTER);
            break;

        case 0X1A:

            L32(output, PC, IR, MEM, REGISTER);
            break;

        case 0X1B:

            S8(output, PC, IR, MEM, REGISTER);
            break;

        case 0X1C:

            S16(output, PC, IR, MEM, REGISTER);
            break;

        case 0X1D:

            S32(output, PC, IR, MEM, REGISTER);
            break;

        case 0X2A:

            BAE(output, PC, IR, MEM);
            break;

        case 0X2B:

            BAT(output, PC, IR, MEM);
            break;

        case 0X2C:

            BBE(output, PC, IR, MEM);
            break;
        
        case 0X2D:

            BBT(output, PC, IR, MEM);
            break;

        case 0X2E:

            BEQ(output, PC, IR, MEM);
            break;
        
        case 0X2F:

            BGE(output, PC, IR, MEM);
            break;

        case 0X30:

            BGT(output, PC, IR, MEM);
            break;

        case 0X31:

            BIV(output, PC, IR, MEM);
            break;

        case 0X32:

            BLE(output, PC, IR, MEM);
            break;

        case 0X33:

            BLT(output, PC, IR, MEM);
            break;

        case 0X34:

            BNE(output, PC, IR, MEM);
            break;

        case 0X35:

            BNI(output, PC, IR, MEM);
            break;

        case 0X36:

            BNZ(output, PC, IR, MEM);
            break;

        case 0X37:

            BUN(output, PC, IR, MEM);
            break;

        case 0X38:

            BZD(output, PC, IR, MEM);
            break;

        case 0X3F:

            INT(output, PC, IR, &loopControl);
            break;

        default:

            std::fprintf(output, "[INVALID INSTRUCTION @ 0X%08X\n", *IR);  
                              
            break;
        }


    }

    std::fprintf(output, "[END OF SIMULATION]\n");        

    std::free(MEM);
    std::free(REGISTER);
    std::fclose(input);
    std::fclose(output);

    return 0;
}




