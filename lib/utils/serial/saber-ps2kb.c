#include <sbi/sbi_string.h>
#include <sbi_utils/serial/saber-ps2kb.h>
#include <saber/ps2kb.h>

struct saber_tv_device {
	unsigned long addr_data;
    bool expectingFromE0;
    bool expectingFromE0F0;
    bool expectingFromF0;
};

static struct saber_tv_device device = {
    .addr_data = 0,
    .expectingFromE0 = 0,
    .expectingFromE0F0 = 0,
    .expectingFromF0 = 0
};


static int _saber_ps2kb_process_code(struct saber_tv_device *device, u8 prePre, u8 pre, u8 code) {
    if(prePre == 0xE0){
        if(pre == 0xF0){
            // handle codes like E0,F0,__
            // these are mostly control code releases, ignore for now
            return 0;
        }else{
            // handle codes like E0,__,__
            // these don't exist
            return 0;
        }
    }else if(prePre == 0){
        if(pre == 0xF0){
            // handle codes like F0,__
            // these are all releases I think, ignore for now
            return 0;
        }else if(pre == 0xE0){
            // handle codes like E0,__
            // these are mostly control codes, ignore for now
            return 0;
        }else if(pre == 0){
            // handle basic single byte codes, this is where the action happens
            switch (code) {

                case 0x15:
                    return 'q';
                   
                    
                case 0x1D:
                    return 'w';
                 
                    
                case 0x24:
                    return 'e';
                  
                    
                case 0x2D:
                    return 'r';
                   
                    
                case 0x2C:
                    return 't';
                  
                    
                case 0x35:
                    return 'y';
                   
                    
                case 0x3C:
                    return 'u';
                 
                    
                case 0x43:
                    return 'i';
                   
                    
                case 0x44:
                    return 'o';
                   
                    
                case 0x4D:
                    return 'p';
                   
                    
                case 0x54:
                    return '[';
                   
                    
                case 0x5B:
                    return ']';
                   
                    
                case 0x5A:
                    return 13;
                   
                    
                case 0x1C:
                    return 'a';
                    
                    
                case 0x1B:  
                    return 's';
                    
                    
                case 0x23:
                    return 'd';
                    
                    
                case 0x2B:
                    return 'f';
                   
                    
                case 0x34:
                    return 'g';
                    
                    
                case 0x33:
                    return 'h';
                    
                    
                case 0x3B:
                    return 'j';
                    
                    
                case 0x42:
                    return 'k';
                    
                
                case 0x4B:
                    return 'l';
                            
                
                case 0x4C:
                    return ';';
                    
                
                case 0x52:
                    return '\'';
                    
                    
                case 0x1A:
                    return 'z';
                    
                    
                case 0x22:
                    return 'x';
                    
                    
                case 0x21:
                    return 'c';
                    
                    
                case 0x2A:
                    return 'v';
                    
                    
                case 0x32:
                    return 'b';
                    
                    
                case 0x31:
                    return 'n';
                    
                    
                case 0x3A:
                    return 'm';
                    
                    
                case 0x41:
                    return ',';
                    
                    
                case 0x49:
                    return '.';
                    
                    
                case 0x4A:
                    return '/';
                    
                    
                case 0x16:
                    return '1';
                    
                    
                case 0x1E:
                   return '2';
                   
                
                case 0x26:
                   return '3';
                               
                
                case 0x25:
                   return '4';
                   
                
                case 0x2E:
                   return '5';
                          
                
                case 0x36:
                   return '6';
                   
                
                case 0x3D:
                   return '7';
                              
                
                case 0x3E:
                   return '8';
                   
                
                case 0x46:
                   return '9';
                       

                case 0x45:
                   return '0';
                   
                
                case 0x4E:
                   return'-';
                              
              
                case 0x55:
                   return '=';
                   

                case 0x66:
                   return 8;
                   
                
                case 0x29:
                    return ' ';
                        
                default:
                    return 0;
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