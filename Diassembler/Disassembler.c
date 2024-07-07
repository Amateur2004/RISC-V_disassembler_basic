#include<stdio.h>
#include<string.h>

struct labelInfo{
    int label_number;
    int label_immediate;
};

void label_num(struct labelInfo *a,int index){
    int count = 0;
    static int label_number = 1;
    for(int i=0;i<index;i++){
        int a1 = (index -i)*4;
        int a2 = a[i].label_immediate - a[index].label_immediate;
        
        if(a[i].label_immediate == a[index].label_immediate){
            a[index].label_number = a[i].label_number;
            
            count++;
            break;
        }
    }
    if(count == 0){
        a[index].label_number = label_number;
        label_number++;
    }

}

void decLabelImm(struct labelInfo *a){
    for(int i=0;i<20;i++){
        if(a[i].label_immediate !=-1){
            a[i].label_immediate = a[i].label_immediate-4;
        }
    }
}

int labelReached(struct labelInfo *a){
    for(int i=0;i<20;i++){
        if(a[i].label_immediate ==0){
            return a[i].label_number ;
        }
    }
    return -1;
}

int checkCorrectInstruction(char *arr){
    int i=0;
    while(arr[i]!='\0'){
        if(! ((arr[i]>=48 && arr[i]<=57) || (arr[i]>=65 && arr[i]<=70) || (arr[i]>=97 && arr[i]<=102)) ){
            
            printf("not a valid hexadecimal instruction\n");
            return 0;
        }
        i++;
    }

    if(i!=8){ //if size is >8
        printf("not a 32 bit instruction\n");
        return 0;
    }
    
    else{
        return  1;
    }

}

unsigned int getBlockInCode(int end,int start,unsigned int instruction){
    unsigned int block= instruction << 31 - end;  //left shift 31 - end
    block = block >> (31 - (end-start));
    return block;
}

int signExtensionForBlock(int end,int start,unsigned int instruction){
    int block= instruction << 31 - end;  //left shift 31 - end
    block = block >> (31 - (end-start));
    return block;
}

int main(int argc,char *argv[]){
    char *file = argv[1];
    FILE *filePtr = fopen(file,"r");
    if (filePtr == NULL) {
        printf("error: couldn't open the file");
        return 0;
    }
    unsigned int instruction;
    char hex_code[9];

    struct labelInfo labelInfoArray[20];
//initialising the array
    for(int i=0;i<20;i++){
        labelInfoArray[i].label_immediate = -1;
        labelInfoArray[i].label_number = -1;
    }
    int instruction_count = 0;
    while ( fgets(hex_code,sizeof(hex_code),filePtr) != NULL ){
       if( hex_code[0] == ' ' || hex_code[0] == '\n' ){ //to ignore trailing spaces and new line char in the file
            continue;
       }
       sscanf(hex_code, "%x", &instruction);
       
        if(checkCorrectInstruction(hex_code)){
            
            unsigned int opcode = instruction & 127; //(2^7-1 i.e  7 1s in binary)
            int correct = 1;
            decLabelImm(labelInfoArray);
            if(labelReached(labelInfoArray) !=-1){
                printf("L%d: ",labelReached(labelInfoArray));
            }
//R format
            if(opcode == 51){
                
                int func7 = getBlockInCode(31,25,instruction);
                int rs2 = getBlockInCode(24,20,instruction);
                int rs1 = getBlockInCode(19,15,instruction);
                int func3 = getBlockInCode(14,12,instruction);
                int rd = getBlockInCode(11,7,instruction);
                

                if(func7 == 0){
                    switch (func3){
                        case 0:
                            printf("add ");
                            break;
                        case 4:
                            printf("xor ");
                            break;
                        case 6:
                            printf("or ");
                            break;
                        case 7:
                            printf("and ");
                            break;
                        case 1:
                            printf("sll ");
                            break;
                        case 5:
                            printf("srl ");
                            break;
                        case 2:
                            printf("slt ");
                            break;
                        case 3:
                            printf("sltu ");
                            break;
                        default:
                            printf("not a valid instruction\n");
                            break;        
                    }
                }

                else if(func7 == 32){
                    switch (func3){
                        case 0:
                            printf("sub ");
                            break;
                        case 5:
                            printf("sra ");
                            break;
                        default:
                            correct = 0;
                            printf("not a valid instruction\n");
                            break;    
                    }       
                }
                if(correct == 1){
                    printf("x%d, x%d, x%d\n",rd,rs1,rs2);
                }
                
                
            }
        
//I format            
            if(opcode == 19 || opcode == 3 || opcode == 103 || opcode == 115 ){
                int imm = signExtensionForBlock(31,20,instruction);
                int rs1 = getBlockInCode(19,15,instruction);
                int func3 = getBlockInCode(14,12,instruction);
                int rd = getBlockInCode(11,7,instruction);
                if(opcode == 19){
                    switch(func3){
                        case 0:
                            printf("addi ");
                            break;
                        case 4:
                            printf("xori ");
                            break;
                        case 6:
                            printf("ori ");
                            break;
                        case 7:
                            printf("andi ");
                            break;
                        case 3:
                            imm = getBlockInCode(31,20,instruction);
                            printf("sltiu ");
                            break; 
                        
                        case 1:
                            if(getBlockInCode(31,26,instruction) == 0){
                                imm = (unsigned)getBlockInCode(25,20,instruction) ;
                                printf("slli ");
                            }
                            else{
                                correct = 0;
                            }   
                            break;
                        case 5:
                            if(getBlockInCode(31,26,instruction) == 0){
                                imm = (unsigned)getBlockInCode(25,20,instruction) ;
                                printf("srli ");
                            }
                            else if((unsigned)getBlockInCode(31,26,instruction) == 16){
                                imm = (unsigned)getBlockInCode(25,20,instruction) ;
                                printf("srai ");
                            }
                            else{
                                correct = 0;
                            }   
                            break;
                        case 2:
                            printf("slti ");
                            break;
                        default:
                            correct = 0;
                            printf("not a valid instruction\n");
                            break;
                           
                    }
                    if(correct == 1){
                        printf("x%d, x%d, %d\n",rd,rs1,imm);                        
                    }
                    else{
                        printf("not a valid instruction\n");
                    }
                }

                if(opcode == 3){
                    switch(func3){
                        case 0:
                            printf("lb ");
                            break;
                        case 1:
                            printf("lh ");
                            break;
                        case 2:
                            printf("lw ");
                            break;
                        case 3:
                            printf("ld ");
                            break;
                        case 4:
                            printf("lbu ");
                            break;
                        case 5:
                            printf("lhu ");
                            break;
                        case 6:
                            printf("lwu ");
                            break;
                        default:
                            correct = 0;
                            printf("not a valid instruction\n");
                            break;
                    }
                    if(correct !=0){
                        printf("x%d, %d(x%d)\n",rd,imm,rs1);
                    }
                    
                }

                if(opcode == 103){
                    int correct = 1;
                    switch(func3){
                        case 0:
                            printf("jalr ");
                            break;
                        default:
                            correct = 0;
                            printf("not a valid instruction\n");
                            break;
                    }
                    if(correct == 1){
                        printf("x%d, x%d, %d\n",rd,rs1,imm);
                    }
                    
                }
                if(opcode == 115){
                    switch(func3){
                        case 0:
                            if(imm == 0){
                                printf("ecall\n");
                            }
                            else if(imm ==1){
                                printf("ebreak\n");
                            }
                            else{
                                correct = 0;
                                printf("not a valid instruction\n");
                            }
                            break;
                        default:
                            correct = 0;
                            printf("not a valid instruction\n");
                            break;
                    }
                }
                
            }
//S format
            if(opcode == 35){
                int imm = (signExtensionForBlock(31,25,instruction) << 5) + (getBlockInCode(11,7,instruction));
                int rs2 = getBlockInCode(24,20,instruction);
                int rs1 = getBlockInCode(19,15,instruction);
                int func3 = getBlockInCode(14,12,instruction);

                switch(func3){
                    case 0:
                        printf("sb ");
                        break;
                    case 1:
                        printf("sh ");
                        break;
                    case 2:
                        printf("sw ");
                        break;
                    case 3:
                        printf("sd ");
                        break;
                    default:
                        printf("not a valid instruction\n");
                        break;
                }
                printf("x%d, %d(x%d)\n",rs2,imm,rs1);
            }
        
//B format
            if(opcode == 99){
                
                int imm = (signExtensionForBlock(31,31,instruction) << 12) + (getBlockInCode(7,7,instruction)<<11)+(getBlockInCode(30,25,instruction)<<5)+(getBlockInCode(11,8,instruction)<<1);
                int rs2 = getBlockInCode(24,20,instruction);
                int rs1 = getBlockInCode(19,15,instruction);
                int func3 = getBlockInCode(14,12,instruction);
                int correct = 1;
                switch(func3){
                    case 0:
                        printf("beq ");
                        break;
                    case 1:
                        printf("bne ");
                        break;
                    case 4:
                        printf("blt ");
                        break;
                    case 5:
                        printf("bge ");
                        break;
                    case 6:
                        printf("bltu ");
                        break;
                    case 7:
                        printf("bgeu ");
                        break;
                        
                    default:
                        printf("not a valid instruction\n");
                        correct =0;
                        break;
                }
                if(correct == 1){
                    labelInfoArray[instruction_count].label_immediate = imm;
                    label_num(labelInfoArray,instruction_count);
                    printf("x%d, x%d ,L%d\n",rs1,rs2,labelInfoArray[instruction_count].label_number);

                }    

            }

//J-format
            if(opcode == 111){
                int rd = getBlockInCode(11,7,instruction);
            //problem here
                int imm = ( (signExtensionForBlock(31,31,instruction)<<20)+(getBlockInCode(19,12,instruction)<<12)+(getBlockInCode(20,20,instruction)<<11)+(getBlockInCode(30,21,instruction)<<1) ) ;
                labelInfoArray[instruction_count].label_immediate = imm;
                label_num(labelInfoArray,instruction_count);
                printf("jal, x%d ,L%d\n",rd,labelInfoArray[instruction_count].label_number);
            }

//U-format
            if(opcode == 23 || opcode == 55){
                int rd = getBlockInCode(11,7,instruction);
                int imm = (getBlockInCode(31,12,instruction)) ;
                if(opcode == 23){
                    printf("auipc x%d, 0x%x\n",rd,imm);
                }
                if(opcode == 55){
                    printf("lui x%d, 0x%x\n",rd,imm);
                }
                
            }
            
            instruction_count++;
        }
    }   
