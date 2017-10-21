
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/adc-sensor.h"
#include "lib/sensors.h"
#include "random.h"
#include "button-sensor.h"
#include "board-peripherals.h"
#include "ti-lib.h"
#include <stdio.h>
#include <stdint.h>

static struct etimer et_led;
static struct etimer et_sensor;
static int contador = 0, led1 = 0, led2 = 0;

/*---------------------------------------------------------------------------*/
//PROCESS(cont_led, "Contador de Led 1");
PROCESS(read_tension, "Leitura de Tensão com ADC");
//AUTOSTART_PROCESSES(&cont_led);
AUTOSTART_PROCESSES(&read_tension);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(read_tension, ev, data){
    PROCESS_BEGIN();

    printf("Inicio do Processo de Leitura do Tensor\n");

    //Declarando a estrutura do ADC Sensor
    static struct sensors_sensor *sensor;

    //ADC_SENSOR vai representar o AD que estará sendo utilizado
    sensor = sensors_find(ADC_SENSOR);

    etimer_set(&et_sensor, CLOCK_SECOND);

    while(1){
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_TIMER){
            //Ativar o ADC
            SENSORS_ACTIVATE(*sensor);

            //Configurar canal a ser utilizado, porta lógica
            sensor->configure(ADC_SENSOR_SET_CHANNEL, ADC_COMPB_IN_AUXIO0);

            printf("Leitura do sensor: %d\n", sensor->value(ADC_SENSOR_VALUE));

            //Desativar o ADC
            SENSORS_DEACTIVATE(*sensor);
            etimer_reset(&et_sensor);
        }
    }

    PROCESS_END();
}
/*
PROCESS_THREAD(cont_led, ev, data){

    PROCESS_BEGIN();

    printf("Inicio do contador para o Led 1\n");

    IOCPinTypeGpioOutput(BOARD_IOID_DIO12);
    IOCPinTypeGpioOutput(BOARD_IOID_DIO15);

    etimer_set(&et_led, CLOCK_SECOND);

    while(1){
        PROCESS_WAIT_EVENT();

        if (ev == PROCESS_EVENT_TIMER){
            printf("\n\nChegamos no tempo de 1s no 1º Led!\n");

            contador++;

            printf("Contador: %d \n", contador);

            led1 = contador & 0b01;
            led2 = (contador & 0b10)>>1;

            printf("Led 1: %d \n", led1);
            printf("Led 2: %d \n", led2);

            GPIO_writeDio(BOARD_IOID_DIO12, led1);
            GPIO_writeDio(BOARD_IOID_DIO15, led2);

            etimer_reset(&et_led);
        }

    }
    PROCESS_END();
}
*/
