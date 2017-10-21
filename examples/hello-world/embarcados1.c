#include "contiki.h"
#include <stdio.h>
#include "dev/leds.h"
#include "button-sensor.h"

#define LED_PING_EVENT (44)
#define LED_PONG_EVENT (45)

static struct etimer et_hello;
static struct etimer et_blink;
static struct etimer et_proc3;

static struct etimer et_time1;
static struct etimer et_time2;
static struct etimer et_time3;

PROCESS(old_hello_world_process, "Hello world process");
PROCESS(new_hello_world_process, "New Hello world process");
PROCESS(blink_process, "Blink Process");
PROCESS(proc3_process, "Processo 3");
PROCESS(process_ping_1, "Processo Ping Pong - 1");
PROCESS(process_ping_2, "Processo Ping Pong - 2");
PROCESS(process_ping_3, "Processo Ping Pong - 3");
PROCESS(pong_process, "Processo Ping Pong");
PROCESS(read_button_process, "read button process");

AUTOSTART_PROCESSES(&process_ping_1, &process_ping_2, &process_ping_3, &read_button_process, &pong_process);
//AUTOSTART_PROCESSES(&blink_process, &proc3_process);
//AUTOSTART_PROCESSES(&blink_process, &new_hello_world_process);
//AUTOSTART_PROCESSES(&old_hello_world_process, &new_hello_world_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(read_button_process, ev, data){
    PROCESS_BEGIN();

    while(1){
        if(ev == sensors_event){
            if(data == &button_left_sensor){
                printf("Left Button!\n");
                leds_toggle(LEDS_RED);
            }else if(data == &button_right_sensor){
                leds_toggle(LEDS_GREEN);
                printf("Right Button!\n");
            }

            process_post(&pong_process, LED_PING_EVENT, (void*)(&read_button_process));

        }else if(ev == LED_PONG_EVENT){
            printf("Recebido o Pong no processo dos botões!\n");
        }
        PROCESS_YIELD();
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(process_ping_1, ev, data){
    PROCESS_BEGIN();

    etimer_set(&et_time1, 2 * CLOCK_SECOND);

    while (1){
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_TIMER){
            printf("Chegamos no tempo de 2s no processo 1. Envia o Ping!\n");
            printf("Led Green!\n");
            leds_toggle(LEDS_GREEN);
            process_post(&pong_process, LED_PING_EVENT, (void*)(&process_ping_1));
            etimer_reset(&et_time1);

        }else if(ev == LED_PONG_EVENT){
            printf("Recebido o Pong no processo 1!\n");
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(process_ping_2, ev, data){
    PROCESS_BEGIN();

    etimer_set(&et_time2, 4 * CLOCK_SECOND);

    while (1){
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_TIMER){
            printf("Chegamos no tempo de 4s no processo 2. Envia o Ping!\n");
            process_post(&pong_process, LED_PING_EVENT, (void*)(&process_ping_2));
            etimer_reset(&et_time2);

        }else if(ev == LED_PONG_EVENT){
            printf("Recebido o Pong no processo 2!\n");
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(process_ping_3, ev, data){
    PROCESS_BEGIN();

    etimer_set(&et_time3, 10 * CLOCK_SECOND);

    while (1){
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_TIMER){
            printf("Chegamos no tempo de 10s no processo 3. Envia o Ping!\n");
            process_post(&pong_process, LED_PING_EVENT, (void*)(&process_ping_3));
            etimer_reset(&et_time3);

        }else if(ev == LED_PONG_EVENT){
            printf("Recebido o Pong no processo 3!\n");
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(pong_process, ev, data){
    PROCESS_BEGIN();
    while (1){
        if (ev == LED_PING_EVENT){
            struct process *data_from = (struct process*)data;
            printf("Recebido um ping!\n");
            printf("Envia o Pong para o processo %s. \n", data_from->name);
            process_post((struct process*)data, LED_PONG_EVENT, NULL);
        }
        PROCESS_YIELD();
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(proc3_process, ev, data){
    PROCESS_BEGIN();

    etimer_set(&et_proc3, 10 * CLOCK_SECOND);

    while (1){
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_TIMER){
            printf("Mensagem a cada 10 segundos!\n");
            etimer_reset(&et_proc3);
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data){
    PROCESS_BEGIN();

    etimer_set(&et_blink, 2 * CLOCK_SECOND);

    while (1){
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_TIMER){
            printf("Led Green!\n");
            leds_toggle(LEDS_GREEN);
            etimer_reset(&et_blink);
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(new_hello_world_process, ev, data){
    PROCESS_BEGIN();

    etimer_set(&et_hello, 4 * CLOCK_SECOND);

    while (1){
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_TIMER){
            printf("New Hello world!\n");
            etimer_reset(&et_hello);
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(old_hello_world_process, ev, data){
    PROCESS_BEGIN();

    printf("Old Hello, world\n");

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/

