
#include <sbi/sbi_console.h>

#include <saber/satori.h>

char register_names[32][5] = {
    "zero",
    "ra",
    "sp",
    "gp",
    "tp",
    "t0",
    "t1",
    "t2",
    "s0",
    "s1",
    "a0",
    "a1",
    "a2",
    "a3",
    "a4",
    "a5",
    "a6",
    "a7",
    "s2",
    "s3",
    "s4",
    "s5",
    "s6",
    "s7",
    "s8",
    "s9",
    "s10",
    "s11",
    "t3",
    "t4",
    "t5",
    "t6"
};

static void print_instruction_info(u32 instr) {
    int opcode = instr & 0x7F;
    int rd = (instr >> 7) & 0x1F;
    int func3 = (instr >> 12) & 0x7;
    int rs1 = (instr >> 15) & 0x1F;
    int rs2 = (instr >> 20) & 0x1F;
    int func7 = (instr >> 25) & 0x7F;

    int shamt = (instr >> 20) & 0x1F;
    int imm_u = instr & 0xFFFFF000;
    int imm_j = ((instr >> 20) & 0x100000) | ((instr >> 10) & 0x7FE) | ((instr >> 9) & 0x800) | (instr & 0xFF000);
    int imm_i = ((instr >> 20) & 0xFFF);
    int imm_b = ((instr >> 18) & 0x1000) |((instr >> 20) & 0x7E0) | ((instr >> 7) & 0x17) | ((instr << 4) & 0x800);
    int imm_s = ((instr >> 19) & 0xFE) | ((instr >> 7) & 0x1F);

    int flipper = (instr & 0x40000000);

    switch (opcode) {
        case 0b0110111: sbi_printf("lui %s, 0x%x", register_names[rd], imm_u); return;
        case 0b0010111: sbi_printf("auipc %s, 0x%x", register_names[rd], imm_u); return;
        case 0b1101111: sbi_printf("jal %s, 0x%x", register_names[rd], imm_j); return;
        case 0b1100111: {
            switch (func3) {
                case 0b000: sbi_printf("jalr %s, 0x%x(%s)", register_names[rd], imm_i, register_names[rs1]); return;
                default: return;
            }
            break;
        }
        case 0b1100011: {
            switch (func3) {
                case 0b000: sbi_printf("beq %s, %s, 0x%x", register_names[rs1], register_names[rs2], imm_b); return;
                case 0b001: sbi_printf("bne %s, %s, 0x%x", register_names[rs1], register_names[rs2], imm_b); return;
                case 0b100: sbi_printf("blt %s, %s, 0x%x", register_names[rs1], register_names[rs2], imm_b); return;
                case 0b101: sbi_printf("bge %s, %s, 0x%x", register_names[rs1], register_names[rs2], imm_b); return;
                case 0b110: sbi_printf("bltu %s, %s, 0x%x", register_names[rs1], register_names[rs2], imm_b); return;
                case 0b111: sbi_printf("bgeu %s, %s, 0x%x", register_names[rs1], register_names[rs2], imm_b); return;
                default: return;
            }
            break;
        }
        case 0b0000011: {
            switch (func3) {
                case 0b000: sbi_printf("lb %s, 0x%x(%s)", register_names[rd], imm_i, register_names[rs1]); return;
                case 0b001: sbi_printf("lh %s, 0x%x(%s)", register_names[rd], imm_i, register_names[rs1]); return;
                case 0b010: sbi_printf("lw %s, 0x%x(%s)", register_names[rd], imm_i, register_names[rs1]); return;
                case 0b100: sbi_printf("lbu %s, 0x%x(%s)", register_names[rd], imm_i, register_names[rs1]); return;
                case 0b101: sbi_printf("lhu %s, 0x%x(%s)", register_names[rd], imm_i, register_names[rs1]); return;
                default: return;
            }
            break;
        }
        case 0b0100011: {
            switch (func3) {
                case 0b000: sbi_printf("sb %s, 0x%x(%s)", register_names[rs2], imm_s, register_names[rs1]); return;
                case 0b001: sbi_printf("sh %s, 0x%x(%s)", register_names[rs2], imm_s, register_names[rs1]); return;
                case 0b010: sbi_printf("sw %s, 0x%x(%s)", register_names[rs2], imm_s, register_names[rs1]); return;
                default: return;
            }
            break;
        }
        case 0b0010011: {
            switch (func3) {
                case 0b000: sbi_printf("addi %s, %s, 0x%x", register_names[rd], register_names[rs1], imm_i); return;
                case 0b010: sbi_printf("slti %s, %s, 0x%x", register_names[rd], register_names[rs1], imm_i); return;
                case 0b011: sbi_printf("sltiu %s, %s, 0x%x", register_names[rd], register_names[rs1], imm_i); return;
                case 0b100: sbi_printf("xori %s, %s, 0x%x", register_names[rd], register_names[rs1], imm_i); return;
                case 0b110: sbi_printf("ori %s, %s, 0x%x", register_names[rd], register_names[rs1], imm_i); return;
                case 0b111: sbi_printf("andi %s, %s, 0x%x", register_names[rd], register_names[rs1], imm_i); return;
                case 0b001: sbi_printf("slli %s, %s, 0x%x", register_names[rd], register_names[rs1], shamt); return;
                case 0b101: sbi_printf("%s %s, %s, 0x%x", flipper?"slai":"slli", register_names[rd], register_names[rs1], shamt); return;
                default: return;
            }
            break;
        }
        case 0b0110011: {
            switch (func3) {
                case 0b000: sbi_printf("%s %s, %s, %s", flipper?"sub":"add", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b001: sbi_printf("sll %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b010: sbi_printf("slt %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b011: sbi_printf("sltu %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b100: sbi_printf("xor %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b101: sbi_printf("%s %s, %s, %s", flipper?"sla":"sll", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b110: sbi_printf("or %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b111: sbi_printf("and %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                default: return;
            }
            break;
        }
        case 0b0001111: {
            switch (func3) {
                case 0b000: sbi_printf("fence"); return;
                default: return;
            }
            break;
        }
        case 0b1110011: {
            switch (func3) {
                case 0b001: sbi_printf("csrrw %s, %s, 0x%x", register_names[rd], register_names[rs1], imm_i); return;
                case 0b010: sbi_printf("csrrs %s, %s, 0x%x", register_names[rd], register_names[rs1], imm_i); return;
                case 0b011: sbi_printf("csrrc %s, %s, 0x%x", register_names[rd], register_names[rs1], imm_i); return;
                case 0b101: sbi_printf("csrrwi %s, 0x%x, 0x%x", register_names[rd], rs1, imm_i); return;
                case 0b110: sbi_printf("csrrsi %s, 0x%x, 0x%x", register_names[rd], rs1, imm_i); return;
                case 0b111: sbi_printf("csrrci %s, 0x%x, 0x%x", register_names[rd], rs1, imm_i); return;
                default: {
                    switch (instr) {
                        case 0x00000073: sbi_printf("ecall"); return;
                        case 0x00100073: sbi_printf("ebreak"); return;
                        case 0x00200073: sbi_printf("uret"); return;
                        case 0x10200073: sbi_printf("sret"); return;
                        case 0x30200073: sbi_printf("mret"); return;
                        default: return;
                    }
                }
            }
            break;
        }
        case 0b0101111: {
            switch (func7>>2) {
                case 0b00001: sbi_printf("amuswap %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b00000: sbi_printf("amuadd %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b00100: sbi_printf("amuxor %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b01100: sbi_printf("amuand %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b01000: sbi_printf("amuor %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b10000: sbi_printf("amumin %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b10100: sbi_printf("amumax %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b11000: sbi_printf("amuminu %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                case 0b11100: sbi_printf("amumaxu %s, %s, %s", register_names[rd], register_names[rs1], register_names[rs2]); return;
                default: return;
            }
            break;
        }
        default: return;
    }
}

void saber_satori(u32 start_address) {
    int building_position = 0;

    bool exit = 0;

    sbi_printf("\033[2J");

    while(!exit){
        sbi_printf("\033[0;0H");
        sbi_printf("Saber Satori Interface\n");
        sbi_printf("+---------------------+\n");
        sbi_printf("| \033[32m");
        for(int i = 0; i < 8; i++){
            sbi_printf("%s%01x\033[0m", (building_position == i)?"\033[32m":"\033[0m", (start_address >> ((7-i)*4)) & 0xF);
        }
        sbi_printf(" <- Address |\n");
        sbi_printf("+---------------------+\n");

        for(int i = 0; i < 32; i++){
            u32 address = start_address + i*4;
            u32 read_data = *((u32*)address);
            sbi_printf("| %08x:  %08x | ", address, read_data);
            print_instruction_info(read_data);
            sbi_printf("\033[0K\n");
        }
        sbi_printf("+---------------------+\n\n");
        sbi_printf("Controls\n");
        sbi_printf(" 0-f: Input address\n");
        sbi_printf(" -/+: Previous/Next 32 words\n");
        sbi_printf(" ctrl-d: Exit\n");

        int c = 0;
        do{
            c = sbi_getc();
        }while(c <= 0);

        int num = -1;
        switch(c){
            case '0': num = 0; break;
            case '1': num = 1; break;
            case '2': num = 2; break;
            case '3': num = 3; break;
            case '4': num = 4; break;
            case '5': num = 5; break;
            case '6': num = 6; break;
            case '7': num = 7; break;
            case '8': num = 8; break;
            case '9': num = 9; break;
            case 'A': case 'a': num = 10; break;
            case 'B': case 'b': num = 11; break;
            case 'C': case 'c': num = 12; break;
            case 'D': case 'd': num = 13; break;
            case 'E': case 'e': num = 14; break;
            case 'F': case 'f': num = 15; break;
            case '+': case '=': start_address += 4*32; break;
            case '_': case '-': start_address -= 4*32; break;
            case '\r': case '\n': building_position = 0; break;
            case '\b': if(building_position > 0) building_position--; break;
            case '\033': {
                c = sbi_getc();
                switch(c){
                    case '[': {
                        c = sbi_getc();
                        switch(c){
                            case 'A': building_position = 0; break;
                            case 'B': building_position = 7; break;
                            case 'C': if(building_position < 7) building_position++; break;
                            case 'D': if(building_position > 0) building_position--; break;
                            case 'S': start_address -= 4*32; break;
                            case 'T': start_address += 4*32; break;
                            default: break;
                        }
                        break;
                    }
                }
                break;
            }
            case 0x03: case 0x04: exit = true; break; // control c, or d
            default: break;
        }

        if(num != -1){
            int i = (7-building_position) * 4;
            start_address = (start_address & ~(0xF << i)) | (num << i);

            building_position++;

            if(building_position == 8){
                building_position = 0;
            }
        }
    }
    sbi_printf("Goodbye\n");
}