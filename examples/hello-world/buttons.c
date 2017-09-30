
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "random.h"
#include "button-sensor.h"
#include "board-peripherals.h"

#include "ti-lib.h"

#include <stdio.h>
#include <stdint.h>

/*---------------------------------------------------------------------------*/
PROCESS(read_button_process, "read button process");
AUTOSTART_PROCESSES(&read_button_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(read_button_process, ev, data)
{
    PROCESS_BEGIN();
    printf("Read Button Demo\n");

    while(1){
        PROCESS_YIELD();

        if(ev == sensors_event){
            if(data == &button_left_sensor){
                printf("Left Button!\n");
                leds_toggle(LEDS_RED);
            }
            else if(data == &button_right_sensor){
                leds_toggle(LEDS_GREEN);
                printf("Right Button!\n");
            }
        }
        PROCESS_END();
    }
    return 0;
}
