#include <iostream>
#include <stdint.h>
#include <string>
#include <math.h>

using namespace std;

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

    while(*IR == 0xFC000000)
    {
        opcode = (*IR & 0xFC000000) >> 26;

        switch (opcode)
        {
        
        // TYPE U - NOP/MOV
        case 0X00:
            
            if (*IR == 0){
                std::fprintf(output, "0X%08X:\tmov r0,0\tR0=0X%08X", *PC, 0);
                
                //UPDATING PC & IR VALUES
                *PC = *PC + 4;
                *IR = MEM[*PC];
            }
            else {

                uint32_t Z = 0;
                //TYPE U - Z MASK:
                Z = (*IR & 0x03E00000) >> 21;

                uint32_t attributionValue = (*IR & 0x001FFFFF);
                REGISTER[Z] = attributionValue;

                std::fprintf(output, "0X%08X:\tmov r%i, %i\tR%i=0X%08X", *PC, Z, attributionValue, Z, attributionValue);

                //UPDATING PC & IR VALUES
                *PC = *PC + 4;
                *IR = *PC;

            }

            break;
        
        //TYPE U - MOVS
        case 0X01:

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
            *IR = *PC;

            break;

        //TYPE U - ADD
        case 0X02:

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

            //ZN CHECK -> R[Z] = 0
            if (REGISTER[Z] == 0) {
                *SR = 0x00000040;

                std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

                break;
            }

            //SN CHECK -> R[Z]31 = 1

            uint32_t checkSN = REGISTER[Z] >> 31;

            if (checkSN == 1) {
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

                break;
            }

            //CY CHECK = R[Z]32 = 1
            uint64_t result = ( REGISTER[X] + REGISTER[Y] ); 
            result = result >> 32;

            if (result == 1) {
                *SR = 0X00000001;

                std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

                break;
            }

            //OV CHECK
            uint32_t RX31 =  REGISTER[X] >> 31;
            uint32_t RY31 =  REGISTER[Y] >> 31;
            uint32_t RZ31 =  REGISTER[Z] >> 31;

            if ( (RX31 == RY31) && (RZ31 != RX31) ) {
                *SR = 0X00000008;

                std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

                break;
            }


            *SR = 0X00000002;
            std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i+R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);


            break;
        
        //TYPE U - SUB
        case 0X03:

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

            //ZN CHECK -> R[Z] = 0
            if (REGISTER[Z] == 0) {
                *SR = 0x00000040;

                std::fprintf(output, "0X%08X:\tsub r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

                break;
            }

            //SN CHECK -> R[Z]31 = 1

            uint32_t checkSN = REGISTER[Z] >> 31;

            if (checkSN == 1) {
                *SR = 0x00000010;

                std::fprintf(output, "0X%08X:\tsub r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

                break;
            }

            //CY CHECK = R[Z]32 = 1
            uint64_t result = ( REGISTER[X] + REGISTER[Y] ); 
            result = result >> 32;

            if (result == 1) {
                *SR = 0X00000001;

                std::fprintf(output, "0X%08X:\tsub r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

                break;
            }

            //OV CHECK
            uint32_t RX31 =  REGISTER[X] >> 31;
            uint32_t RY31 =  REGISTER[Y] >> 31;
            uint32_t RZ31 =  REGISTER[Z] >> 31;

            if ( (RX31 != RY31) && (RZ31 != RX31) ) {
                *SR = 0X00000008;

                std::fprintf(output, "0X%08X:\tsub r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

                *PC = *PC + 4;
                *IR = MEM[*PC];

                break;
            }


            *SR = 0X00000000;
            std::fprintf(output, "0X%08X:\tadd r%i,r%i,r%i\tR%i=R%i-R%i=0X%08X, SR=0X%08X", *PC, Z, X, Y, Z, X, Y, REGISTER[Z], *SR);

            break;

        //MUL / SLL
        case 0X04:
            uint32_t checkIsOp = (*IR & 0X00000700) >> 8;

            //If is MUL:
            if (checkIsOp == 0) {
                
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

                REGISTER[RI4] = result >> 32;

                //ZN CASE:
                if (result == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tmul r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                    break;
                }

                //CY CASE:
                if (REGISTER[RI4] != 0) {
                    *SR = 0X00000001;

                    std::fprintf(output, "0X%08X:\tmul r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                    break;
                }

                *SR = 0X00000000;
                std::fprintf(output, "0X%08X:\tmul r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

                break;

            }
            
            //If is SLL:
            if (checkIsOp == 0X00000001){
                
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
                REGISTER[X] = (result & 0X00000000FFFFFFFF);
                REGISTER[Z] = result >> 32;

                //ZN CASE:
                if (result == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tsll r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                    break;
                }

                //CY CASE:
                if (REGISTER[Z] != 0) {
                    *SR = 0X00000001;

                    std::fprintf(output, "0X%08X:\tsll r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                    break;
                }

                *SR = 0X00000000;
                std::fprintf(output, "0X%08X:\tsll r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                break;
            } 
            
            //If is MULS:
            if (checkIsOp == 0X00000002) {
                
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

                REGISTER[RI4] = result >> 32;

                //ZN CASE:
                if (result == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tmuls r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                    break;
                }

                //OV CASE:
                if (REGISTER[RI4] != 0) {
                    *SR = 0X00000008;

                    std::fprintf(output, "0X%08X:\tmuls r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                    break;
                }

                *SR = 0X00000000;
                std::fprintf(output, "0X%08X:\tmuls r%i,r%i,r%i,r%i\tR%i:R%i=R%i*R%i=0X%016lX, SR=0X%08X", *PC, RI4, Z, X, Y, RI4, Z, X, Y, result, *SR);

                break;
            }

            //If is SLA:
            if (checkIsOp == 0X00000003) {
                
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
                REGISTER[X] = (result & 0X00000000FFFFFFFF);
                REGISTER[Z] = result >> 32;

                //ZN CASE:
                if (result == 0) {
                    *SR = 0x00000040;

                    std::fprintf(output, "0X%08X:\tsla r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                    break;
                }

                //OV CASE:
                if (REGISTER[Z] != 0) {
                    *SR = 0X00000001;

                    std::fprintf(output, "0X%08X:\tsla r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                    break;
                }

                *SR = 0X00000000;
                std::fprintf(output, "0X%08X:\tsla r%i,r%i,r%i,%i\tR%i:R%i=R%i:R%i<<%i=0X%016X, SR=0X%08X", *PC, Z, X, Y, offset, Z, X, Z, Y, offset, result, *SR);

                break;
            }
        
            //If is DIV:
            if (checkIsOp == 0x00000004) {
                 
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

                    break;
                }

                //ZD CASE:
                if (REGISTER[Y] == 0) {
                    *SR = 0x00000020;

                    std::fprintf(output, "0X%08X:\tdiv r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                    break;
                }

                //CY CASE:
                if (REGISTER[RI4] != 0) {
                    *SR = 0X00000001;

                    std::fprintf(output, "0X%08X:\tdiv r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

                    *PC = *PC + 4;
                    *IR = MEM[*PC];

                    break;
                }

                *SR = 0X00000000;
                std::fprintf(output, "0X%08X:\tdiv r%i,r%i,r%i,r%i\tR%i=R%i%R%i=0X%08X,R%i=R%i/R%i=0X%08X, 0X%08X", RI4, Z, X, Y, RI4, X, Y, REGISTER[RI4], Z, X, Y, REGISTER[Z], *SR);

                break;
            }

        default:
            break;
        }


    }
        

    std::free(MEM);
    std::free(REGISTER);
    std::fclose(input);
    std::fclose(output);

    return 0;
}




