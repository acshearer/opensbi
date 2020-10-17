
#include <sbi/sbi_string.h>
#include <sbi_utils/serial/saber-tv.h>
#include <saber/tv.h>

#define MAX_ESCAPE_PARAMS 32

#define MOD_WRAP(n, d) ((((n)%(d))+(d))%(d))

struct saber_tv_device {
	unsigned long addr_command;
	unsigned long addr_data;
	int escaping;
	int escape_params[MAX_ESCAPE_PARAMS];
	int saved_x;
	int saved_y;
	int fg_color;
	int bg_color;
};

static struct saber_tv_device device = {
    .addr_command = 0,
    .addr_data = 0,
	.escaping = 0,
	.saved_x = 0,
	.saved_y = 0,
	.fg_color = 0,
	.bg_color = 0
};

static void _saber_tv_set_data(struct saber_tv_device *device, u32 data) {
    *((u32*)(device->addr_data)) = data;
}

static u32 _saber_tv_get_data(struct saber_tv_device *device) {
    return *((u32*)(device->addr_data));
}

static void _saber_tv_run_command(struct saber_tv_device *device, u32 command) {
    *((u32*)(device->addr_command)) = command;
}

static void _saber_tv_put_char_with_attributes(struct saber_tv_device *device, char ch){
	int code = TV_COLORIZE(ch, device->fg_color, device->bg_color);
	_saber_tv_set_data(device, code);
	_saber_tv_run_command(device, TV_COMMAND_PUT_CHAR);
}

static void _saber_tv_new_line(struct saber_tv_device *device) {
	int x;
	int y;
	_saber_tv_run_command(device, TV_COMMAND_NEW_LINE);

	_saber_tv_run_command(device, TV_COMMAND_GET_Y);
	y = _saber_tv_get_data(device);

	for(x = 0; x < TV_SCREEN_W; x++){
		_saber_tv_put_char_with_attributes(device, ' ');
	}
	
	_saber_tv_set_data(device, y);
	_saber_tv_run_command(device, TV_COMMAND_SET_Y);
}


static int _saber_tv_get_char(struct saber_tv_device *device) {
	return -1;
}


// returns the number of params it consumes
static int _saber_tv_set_attribute(struct saber_tv_device *device, int paramIndex){
	int code = device->escape_params[paramIndex];
	switch (code)
	{
		case 0:
			device->fg_color = 0;
			device->bg_color = 0;
			return 1;

		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
			device->fg_color = code - 30;
			return 1;

		case 38:
			if(device->escape_params[paramIndex+1] == 5){
				device->fg_color = device->escape_params[paramIndex+2];
				return 4;
			}else{
				return 1;
			}

		case 39: { // non standard
			int index = device->escape_params[paramIndex+1] & 0xf;
			int red   = device->escape_params[paramIndex+2] & 0xf;
			int green = device->escape_params[paramIndex+3] & 0xf;
			int blue  = device->escape_params[paramIndex+4] & 0xf;
			int color = (red << 8) | (green << 4) | blue;
			_saber_tv_set_data(device, color);
			_saber_tv_run_command(device, TV_COMMAND_SET_COLOR_INDEX(index));
			return 4;
		}

		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			device->bg_color = code - 40;
			return 1;

		case 50: { // non standard
			int red   = device->escape_params[paramIndex+1] & 0xf;
			int green = device->escape_params[paramIndex+2] & 0xf;
			int blue  = device->escape_params[paramIndex+3] & 0xf;
			int color = (red << 8) | (green << 4) | blue;
			_saber_tv_set_data(device, color);
			_saber_tv_run_command(device, TV_COMMAND_SET_HL_COLOR);
			return 4;
		}

		case 90:
		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97:
			device->fg_color = code - 90 + 8;
			return 1;
			
		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:
			device->bg_color = code - 100 + 8;
			return 1;
		
		default:
			return 1;
	}
}


static void _saber_tv_put_char(struct saber_tv_device *device, char ch) {
	switch(device->escaping){
		default:{
			int escape_index = device->escaping;
			if(escape_index % 2 == 0){
				int param_index = (escape_index-2) / 2;
				switch(ch){
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						device->escape_params[param_index] *= 10;
						device->escape_params[param_index] += (int)(ch - '0');
						break;
					
					default:
						device->escaping++;
						_saber_tv_put_char(device, ch);
						break;

				}
			}else{
				int paramQty = (escape_index-1) / 2;
				int param0 = device->escape_params[0];
				int param1 = device->escape_params[1];
				switch(ch){
					case 'A':{ // move cursor up
						if(paramQty == 1){
							_saber_tv_run_command(device, TV_COMMAND_GET_Y);
							int y = _saber_tv_get_data(device);
							int newY = MOD_WRAP(y - param0, TV_SCREEN_H);
							_saber_tv_set_data(device, newY);
							_saber_tv_run_command(device, TV_COMMAND_SET_Y);
							device->escaping = 0;
						}else{
                            device->escaping = 0;
                            _saber_tv_put_char(device, ch);
                        }
						break;
					}
					case 'B':{ // move cursor down
						if(paramQty == 1){
							_saber_tv_run_command(device, TV_COMMAND_GET_Y);
							int y = _saber_tv_get_data(device);
							int newY = MOD_WRAP(y + param0, TV_SCREEN_H);
							_saber_tv_set_data(device, newY);
							_saber_tv_run_command(device, TV_COMMAND_SET_Y);
							device->escaping = 0;
						}else{
                            device->escaping = 0;
                            _saber_tv_put_char(device, ch);
                        }
						break;
					}
					case 'C':{ // move cursor right
						if(paramQty == 1){
							_saber_tv_run_command(device, TV_COMMAND_GET_X);
							int x = _saber_tv_get_data(device);
							int newX = MOD_WRAP(x + param0, TV_SCREEN_W);
							_saber_tv_set_data(device, newX);
							_saber_tv_run_command(device, TV_COMMAND_SET_X);
							device->escaping = 0;
						}else{
                            device->escaping = 0;
                            _saber_tv_put_char(device, ch);
                        }
						break;
					}
					case 'D':{ // move cursor left
						if(paramQty == 1){
							_saber_tv_run_command(device, TV_COMMAND_GET_X);
							int x = _saber_tv_get_data(device);
							int newX = MOD_WRAP(x - param0, TV_SCREEN_W);
							_saber_tv_set_data(device, newX);
							_saber_tv_run_command(device, TV_COMMAND_SET_X);
							device->escaping = 0;
						}else{
                            device->escaping = 0;
                            _saber_tv_put_char(device, ch);
                        }
						break;
					}
					case 'H':{ // set cursor position
						if(paramQty == 2){
							int newY = MOD_WRAP(param0, TV_SCREEN_H);
							int newX = MOD_WRAP(param1, TV_SCREEN_W);
							_saber_tv_set_data(device, newY);
							_saber_tv_run_command(device, TV_COMMAND_SET_Y);
							_saber_tv_set_data(device, newX);
							_saber_tv_run_command(device, TV_COMMAND_SET_X);
							device->escaping = 0;
						}else{
                            device->escaping = 0;
                            _saber_tv_put_char(device, ch);
                        }
						break;
					}
					case ';':{ // there is another param
						device->escaping++;
						int next_param_index = (escape_index+1-2) / 2;
						device->escape_params[next_param_index] = 0;
						break;
					}
					case 'm':{ // set attributes
						for(int i = 0; i < paramQty;){
                            // _saber_tv_put_char_with_attributes(device, '0'+i);
							i += _saber_tv_set_attribute(device, i);
                            // _saber_tv_put_char_with_attributes(device, '0'+i);
						}
						device->escaping = 0;
						break;
					}
					default:{
                        device->escaping = 0;
						_saber_tv_put_char(device, ch);
						break;
					}
				}
			}
			break;
		}
		case 1:{
			switch(ch){
				case '[':{
					device->escape_params[0] = 0;
					device->escaping = 2;
					break;
				}
				case '7':{
					_saber_tv_run_command(device, TV_COMMAND_GET_X);
					device->saved_x = _saber_tv_get_data(device);
					_saber_tv_run_command(device, TV_COMMAND_GET_Y);
					device->saved_y = _saber_tv_get_data(device);
					device->escaping = 0;
					break;
				}
				case '8':{
					_saber_tv_set_data(device, device->saved_x);
					_saber_tv_run_command(device, TV_COMMAND_SET_X);
					_saber_tv_set_data(device, device->saved_y);
					_saber_tv_run_command(device, TV_COMMAND_SET_Y);
					device->escaping = 0;
					break;
				}
				case 'E':{
					_saber_tv_run_command(device, TV_COMMAND_NEW_LINE);
					device->escaping = 0;
					break;
				}
				default:
                    if(ch >= ' ' && ch <= '~'){
						_saber_tv_put_char_with_attributes(device, ch);
					}
					device->escaping = 0;
					break;
			}
			break;
		}
		case 0:{
			switch(ch){
				case '\n': 
					_saber_tv_new_line(device);
					break;
				
				case '\r':
					break;

				case 0x1b: // escape
					device->escaping = 1;
					break;

				default:
					if(ch >= ' ' && ch <= '~'){
						_saber_tv_put_char_with_attributes(device, ch);
					}
					break;
			}
		}
	}
}


int saber_tv_getc(void){
    return _saber_tv_get_char(&device);
}

void saber_tv_putc(char ch){
    _saber_tv_put_char(&device, ch);
}

void saber_tv_init(unsigned long addr_command, unsigned long addr_data){
    device.addr_command = addr_command;
    device.addr_data = addr_data;
}
