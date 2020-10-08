

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

static u8* saber_tv_cmd;
static u8* saber_tv_data;

static void saber_TV_init(u32 address)
{
  saber_tv_cmd  = (u8*)(address + TV_ADDR_VCR_OFFSET);
  saber_tv_data = (u8*)(address + TV_ADDR_VDR_OFFSET);
}

static void saber_TV_putChar(char c)
{
  switch(c){
    case '\n': {
      *saber_tv_cmd = TV_COMMAND_NEW_LINE;
      break;
    }

    case '\r': break;

    default: {
      *saber_tv_data = (u8)c;
      *saber_tv_cmd = TV_COMMAND_PUT_CHAR;
      break;
    }
  }
}