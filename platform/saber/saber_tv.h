
#ifndef _SABER_TV_H_
#define _SABER_TV_H_

#define TV_ADDR_VCR_OFFSET 0x0
#define TV_ADDR_VDR_OFFSET 0x4

#define TV_SCREEN_W 80
#define TV_SCREEN_H 60

#define TV_COMMAND_SET_CHAR 0b00000001
#define TV_COMMAND_ADVANCE  0b00000010
#define TV_COMMAND_PUT_CHAR 0b00000011
#define TV_COMMAND_SET_X    0b00000100
#define TV_COMMAND_SET_Y    0b00000101
#define TV_COMMAND_NEW_LINE 0b00001000

#define TV_COMMAND_GET_X    0b00010100
#define TV_COMMAND_GET_Y    0b00010101


struct saber_tv_device {
	unsigned long addr;
  int input_index;
  char* input_buffer;
};


static int _saber_tv_next_input_char(struct saber_tv_device *device) {
	char next;
	next = device->input_buffer[device->input_index];
	if(next != '\0'){
		device->input_index++;
	}
	return next;
}

static void _saber_tv_set_data(struct saber_tv_device *device, u8 data) {
    *((u8*)(device->addr + TV_ADDR_VDR_OFFSET)) = data;
}

static u8 _saber_tv_get_data(struct saber_tv_device *device) {
    return *((u8*)(device->addr + TV_ADDR_VDR_OFFSET));
}

static void _saber_tv_run_command(struct saber_tv_device *device, u8 command) {
    *((u8*)(device->addr + TV_ADDR_VCR_OFFSET)) = command;
}

static void _saber_tv_new_line(struct saber_tv_device *device) {
	int x;
	int y;
	_saber_tv_run_command(device, TV_COMMAND_NEW_LINE);

	_saber_tv_run_command(device, TV_COMMAND_GET_Y);
	y = _saber_tv_get_data(device);

	_saber_tv_set_data(device, ' ');
	for(x = 0; x < TV_SCREEN_W; x++){
		_saber_tv_run_command(device, TV_COMMAND_PUT_CHAR);
	}
	
	_saber_tv_set_data(device, y);
	_saber_tv_run_command(device, TV_COMMAND_SET_Y);
}


static int _saber_tv_get_char(struct saber_tv_device *device) {
  return _saber_tv_next_input_char(device);
}


static void _saber_tv_put_char(struct saber_tv_device *device, char ch) {
	switch(ch){
		case '\n': 
			_saber_tv_new_line(device);
			break;
		
		case '\r':
			break;

		default:
			if(ch >= ' ' && ch <= '~'){
				_saber_tv_set_data(device, ch);
				_saber_tv_run_command(device, TV_COMMAND_PUT_CHAR);
			}
			break;
	}
}


#endif