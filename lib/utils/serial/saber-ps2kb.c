#include <sbi/sbi_string.h>
#include <sbi/sbi_console.h>
#include <sbi_utils/serial/saber-ps2kb.h>
#include <saber/ps2kb.h>

struct saber_tv_device {
	unsigned long addr_data;
    bool expectingFromE0;
    bool expectingFromE0F0;
    bool expectingFromF0;
    bool shift_l;
    bool shift_r;
    bool control_l;
    bool control_r;
    bool capslock;
};

static struct saber_tv_device device = {
    .addr_data = 0,
    .expectingFromE0 = 0,
    .expectingFromE0F0 = 0,
    .expectingFromF0 = 0,
    .shift_l = 0,
    .shift_r = 0,
    .control_l = 0,
    .control_r = 0,
    .capslock = 0
};


static int _saber_ps2kb_process_code(struct saber_tv_device *device, u8 prePre, u8 pre, u8 code) {
    if(prePre == 0xE0){
        if(pre == 0xF0){
            // handle codes like E0,F0,__
            switch (code){
                case 0x14: device->control_r = false; return 0;
                default: return 0;
            }
            return 0;
        }else{
            // handle codes like E0,__,__
            // these don't exist
            return 0;
        }
    }else if(prePre == 0){
        if(pre == 0xF0){
            // handle codes like F0,__
            // these are releases
            switch (code){
                case 0x12: device->shift_l = false; return 0;
                case 0x59: device->shift_r = false; return 0;
                case 0x14: device->control_l = false; return 0;
                case 0x58: device->control_r = false; return 0;
                default: return 0;
            }
            return 0;
        }else if(pre == 0xE0){
            // handle codes like E0,__
            switch (code){
                case 0x14: device->control_r = true; return 0;
                default: return 0;
            }
            return 0;
        }else if(pre == 0){
            // handle basic single byte codes, this is where the action happens
            bool shft = device->capslock ^ (device->shift_l || device->shift_r);
            bool ctrl = device->control_l || device->control_r;
            switch (code) {

                case 0x15: return ctrl?0x11:(shft?'Q':'q');
                case 0x16: return shft?'!':'1';
                case 0x1A: return ctrl?0x1A:(shft?'Z':'z');
                case 0x1B: return ctrl?0x13:(shft?'S':'s');
                case 0x1C: return ctrl?0x01:(shft?'A':'a');
                case 0x1D: return ctrl?0x17:(shft?'W':'w');
                case 0x1E: return ctrl?0x00:(shft?'@':'2');
                case 0x21: return ctrl?0x03:(shft?'C':'c');
                case 0x22: return ctrl?0x18:(shft?'X':'x');
                case 0x23: return ctrl?0x04:(shft?'D':'d');
                case 0x24: return ctrl?0x05:(shft?'E':'e');
                case 0x25: return shft?'$':'4';
                case 0x26: return shft?'#':'3';
                case 0x29: return ' ';
                case 0x2A: return ctrl?0x16:(shft?'V':'v');
                case 0x2B: return ctrl?0x06:(shft?'F':'f');
                case 0x2C: return ctrl?0x14:(shft?'T':'t');
                case 0x2D: return ctrl?0x12:(shft?'R':'r');
                case 0x2E: return shft?'%':'5';
                case 0x31: return ctrl?0x0E:(shft?'N':'n');
                case 0x32: return ctrl?0x02:(shft?'B':'b');
                case 0x33: return ctrl?0x08:(shft?'H':'h');
                case 0x34: return ctrl?0x07:(shft?'G':'g');
                case 0x35: return ctrl?0x19:(shft?'Y':'y');
                case 0x36: return ctrl?0x1E:(shft?'^':'6');
                case 0x3A: return ctrl?0x0D:(shft?'M':'m');
                case 0x3B: return ctrl?0x0A:(shft?'J':'j');
                case 0x3C: return ctrl?0x15:(shft?'U':'u');
                case 0x3D: return shft?'&':'7';
                case 0x3E: return shft?'*':'8';
                case 0x41: return shft?'<':',';
                case 0x42: return ctrl?0x0B:(shft?'K':'k');
                case 0x43: return ctrl?0x09:(shft?'I':'i');
                case 0x44: return ctrl?0x0F:(shft?'O':'o');
                case 0x45: return shft?')':'0';
                case 0x46: return shft?'(':'9';
                case 0x49: return shft?'>':'.';
                case 0x4A: return shft?'?':'/';
                case 0x4B: return ctrl?0x0C:(shft?'L':'l');
                case 0x4C: return shft?':':';';
                case 0x4D: return ctrl?0x19:(shft?'P':'p');
                case 0x4E: return ctrl?0x1F:(shft?'_':'-');
                case 0x52: return shft?0x1C:'\'';
                case 0x54: return ctrl?0x1B:(shft?'{':'[');
                case 0x55: return shft?'+':'=';
                case 0x5A: return '\r';
                case 0x5B: return ctrl?0x1D:(shft?'}':']');
                case 0x66: return 8;

                case 0x12: device->shift_l = true; return 0;
                case 0x59: device->shift_r = true; return 0;
                case 0x14: device->control_l = true; return 0;
                case 0x58: device->capslock = !device->capslock; return 0;

                default: return 0;
            }
        }else{
            // handle codes like __,__
            // these don't exist
            return 0;
        }
    }else{
        // handle codes like __,__,__
        // these don't exist
        return 0;
    }
}


static int _saber_ps2kb_get_char(struct saber_tv_device *device) {
    u32 buffer = *((u32*)(device->addr_data));
    *((u32*)(device->addr_data)) = 0x00;

    u8 codes[4];
    codes[0] = (buffer >> 24) & 0xff;
    codes[1] = (buffer >> 16) & 0xff;
    codes[2] = (buffer >>  8) & 0xff;
    codes[3] = (buffer >>  0) & 0xff;

    for(int i = 0; i < 4; i++){
        u8 code = codes[i];
        if(code != 0){
            if(device->expectingFromE0){
                device->expectingFromE0 = false;
                if(code == 0xF0){
                    device->expectingFromE0F0 = true;
                }else{
                    return _saber_ps2kb_process_code(device, 0, 0xE0, code);
                }
            }else if(device->expectingFromE0F0){
                device->expectingFromE0F0 = false;
                return _saber_ps2kb_process_code(device, 0xE0, 0xF0, code);
            }else if(device->expectingFromF0){
                device->expectingFromF0 = false;
                return _saber_ps2kb_process_code(device, 0, 0xF0, code);
            }else{
                if(code == 0xE0){
                    device->expectingFromE0 = true;
                }else if(code == 0xF0){
                    device->expectingFromF0 = true;
                }else{
                    return _saber_ps2kb_process_code(device, 0, 0, code);
                }
            }
        }
    }

    return 0;
}

int saber_ps2kb_getc(void){
    return _saber_ps2kb_get_char(&device);
}

void saber_ps2kb_putc(char ch){
    // ignore
}

void saber_ps2kb_init(unsigned long addr_data){
    device.addr_data = addr_data;

    device.expectingFromE0 = false;
    device.expectingFromF0 = false;
}