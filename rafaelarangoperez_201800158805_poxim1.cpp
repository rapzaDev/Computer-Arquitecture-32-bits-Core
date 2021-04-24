#include <iostream>
#include <stdint.h>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{

    //Opening files

        FILE* input = fopen(argv[1], "r");
	    FILE* output = fopen(argv[2], "w");

    //32KiB MEMORY
        u_char * MEM = (u_char*) calloc(32768, sizeof(u_char));

    //32 REGISTERS - [0..31]
        uint32_t * REGISTER = (uint32_t *) calloc(32, sizeof(uint32_t));

        //IR - Instruction Register
        uint32_t *IR = &REGISTER[28];
        *IR = 0x00000000;

        //PC - PROGRAM COUNTER
        uint32_t *PC = &REGISTER[29];
        *PC = 0x00000000;

        //SP - STACK POINTER
        uint32_t *SP = &REGISTER[30];

        //SR - STATUS REGISTER
        uint32_t *SR = &REGISTER[31];


    //Array of Hexadecimal Commands
        int * HEXADECIMAL;

        uint32_t line;
        int totalHexaFile = 0;

        while (fscanf(input, "%i", &line) != EOF)
            ++totalHexaFile;

        HEXADECIMAL = (int*) malloc(sizeof(int) * totalHexaFile);

        rewind(input);

        int hexaIndex = 0;
        while (fscanf(input, "%i", &line) != EOF)
            HEXADECIMAL[hexaIndex++] = line;

    // ------------- START OF PROGRAM -------------
    
    fprintf(output, "[START OF SIMULATION]\n");

    uint32_t opcode = 0;

    for (int i = 0; i < totalHexaFile; i++)
    {
        opcode = (HEXADECIMAL[i] & 0xFC000000) >> 26;

        switch (opcode)
        {
        
        // TYPE U - NOP/MOV
        case 0X00:
            
            if (HEXADECIMAL[i] == 0) fprintf(output, "0X%08X: mov r0,0\t R0=0X%08X", *IR, 0);
            else {

                uint32_t Z = 0;
                //TYPE U - Z MASK:
                Z = (HEXADECIMAL[i] & 0x03E00000) >> 21;

                uint32_t attributionValue = (HEXADECIMAL[i] & 0x001FFFFF);
                REGISTER[Z] = attributionValue;

                fprintf(output, "0X%08X: mov r%i, %i\t R%i=0X%08X", *IR, Z, attributionValue, attributionValue);

                //UPDATING PC & IR VALUES
                *PC = *PC + 4;
                *IR = *PC;

            }

            break;
        
        //TYPE U - MOVS
        case 0X01:

            uint32_t Z = 0;
            //Z MASK:
            Z = (HEXADECIMAL[i] & 0x03E00000) >> 21;

            uint32_t attributionValue = (HEXADECIMAL[i] & 0x001FFFFF);
            
            //MASK FOR X BIT 
            uint32_t bitX = (attributionValue & 0X00100000) >> 20;

            if (bitX == 1) attributionValue = (attributionValue | 0XFFE00000);

            REGISTER[Z] = attributionValue;

            fprintf(output, "0X%08X: movs r%i, %i\t R%i=0X%08X", *IR, Z, attributionValue, Z, attributionValue);

            //UPDATING PC & IR VALUES
            *PC = *PC + 4;
            *IR = *PC;

            break;

        //TYPE U - ADD
        case 0X02:

            uint32_t Z = 0;
            //Z MASK:
            Z = (HEXADECIMAL[i] & 0x03E00000) >> 21;

            uint32_t X = 0;
            //X MASK:
            X = (HEXADECIMAL[i] & 0x001F0000) >> 16;

            uint32_t Y = 0;
            //Y MASK:
            Y = (HEXADECIMAL[i] & 0x03E00000) >> 21;

            break;
        
        default:
            break;
        }


    }
        

    // free(MEM);
    // free(REGISTER);
    // free(HEXADECIMAL);
    fclose(input);
    fclose(output);

    return 0;
}




