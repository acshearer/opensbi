
#include <libfdt.h>
#include <sbi_utils/serial/fdt_serial.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/serial/multi-serial.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_console.h>

static char multi_input_buffer[256];
static int multi_input_buffer_index = -1;

static int serial_driver_count = 0;

struct serial_driver_info {
    const char* name;
    struct fdt_serial* driver;
    bool enabled;
};

static struct serial_driver_info multi_serial_drivers[16];

static const struct fdt_match multi_match[] = {
	{ .compatible = "saber,serial-multi" },
	{ },
};

static void multi_input_buffer_push(char c) {
    if(c > 0){
        multi_input_buffer_index++;
        multi_input_buffer[multi_input_buffer_index] = c;
    }
}

static int multi_input_buffer_pop(void) {
    int c = -1;
    if(multi_input_buffer_index >= 0){
        c = multi_input_buffer[multi_input_buffer_index];
        multi_input_buffer_index--;
    }
    return c;
}

void multi_input_buffer_push_str(const char* str) {
    int len = sbi_strlen(str);
    for(int i = len-1; i>=0; i--){
        multi_input_buffer_push(str[i]);
    }
}

void multi_serial_putc(char c) {
    for(int i = 0; i < serial_driver_count; i++){
        if(multi_serial_drivers[i].enabled){
            multi_serial_drivers[i].driver->putc(c);
        }
    }
}

int multi_serial_getc(void) {
    for(int i = 0; i < serial_driver_count; i++){
        if(multi_serial_drivers[i].enabled){
            multi_input_buffer_push(multi_serial_drivers[i].driver->getc());
        }
    }
    return multi_input_buffer_pop();
}

int multi_serial_init(void *fdt, int nodeoff, const struct fdt_match *my_match) {

    int offset = -1;

    int max_count = get_ftd_serial_driver_count();

    serial_driver_count = 0;

    
    for(int i = 0; i < max_count; i++){

        const struct fdt_match *match = NULL;

        struct fdt_serial* driver = get_ftd_serial_driver(i);

        int match_table_length = 0;
        while(driver->match_table[match_table_length].compatible){
            match_table_length++;
        }

        int prev_offset = offset;

        int match_offset = fdt_find_match(fdt, offset, driver->match_table, &match);

        offset = fdt_next_node(fdt, offset, NULL);

        if(match_offset < 0 || match_offset == nodeoff){
            continue;
        }

        int new_match_offset;
        do{
            new_match_offset = fdt_find_match(fdt, prev_offset, multi_match, NULL);
            prev_offset = fdt_next_node(fdt, prev_offset, NULL);
        }while(new_match_offset < match_offset && new_match_offset >= 0 && prev_offset >= 0);

        if(new_match_offset < 0 || match_offset != new_match_offset){
            continue;
        }

        int index = serial_driver_count;

        multi_serial_drivers[index].name = driver->match_table->compatible;
        multi_serial_drivers[index].driver = driver;
        multi_serial_drivers[index].enabled = false;

        if(!driver->init(fdt, match_offset, match)){
            multi_serial_drivers[index].enabled = true;
        }

        serial_driver_count++;
    }

    return 0;
}

void multi_serial_print_info(void){
    sbi_printf("\nDetected %d multi-compatable serial driver%s:\n", serial_driver_count, serial_driver_count == 1 ? "" : "s");
    for(int i = 0; i < serial_driver_count; i++){
        sbi_printf("%d: %s (%s)\n", i, multi_serial_drivers[i].name, multi_serial_drivers[i].enabled ? "\033[32mLoaded\033[0m" : "\033[31mFailed\033[0m");
    }
    sbi_printf("\n");
}

