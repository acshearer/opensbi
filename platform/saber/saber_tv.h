
#ifndef _SABER_TV_H_
#define _SABER_TV_H_

#define TV_ADDR_VCR_OFFSET 0x0
#define TV_ADDR_VDR_OFFSET 0x4

#define TV_COMMAND_SET_CHAR 0b00000001
#define TV_COMMAND_ADVANCE  0b00000010
#define TV_COMMAND_PUT_CHAR 0b00000011
#define TV_COMMAND_SET_X    0b00000100
#define TV_COMMAND_SET_Y    0b00000101
#define TV_COMMAND_NEW_LINE 0b00001000

#define TV_COMMAND_GET_X    0b00010100
#define TV_COMMAND_GET_Y    0b00010101

#define TV_VCR ((u8*)(SABER_TV_ADDR + TV_ADDR_VCR_OFFSET))
#define TV_VDR ((u8*)(SABER_TV_ADDR + TV_ADDR_VDR_OFFSET))

static void saber_TV_putChar(char c)
{
  switch(c){
    case '\n': {
      *TV_VCR = TV_COMMAND_NEW_LINE;
      break;
    }

    case '\r': break;

    default: {
      *TV_VDR = (u8)c;
      *TV_VCR = TV_COMMAND_PUT_CHAR;
      break;
    }
  }
}

#endif