
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/adc-sensor.h"
#include "lpm.h"
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
static int16_t loadvalue = 0;
static int val_percent = 10;


//LPM
uint8_t pwm_request_max_pm(void){
    return LPM_MODE_DEEP_SLEEP;
}
void sleep_enter(void){
    leds_on(LEDS_RED);
}

void sleep_leave(void){
    leds_off(LEDS_RED);
}

LPM_MODULE(pwmdrive_module, pwm_request_max_pm, sleep_enter, sleep_leave, LPM_DOMAIN_PERIPH);

//Função de Inicialização
int16_t pwminit (int32_t freq) {
    uint32_t load = 0;

    ti_lib_ioc_pin_type_gpio_output(IOID_21);
    leds_off(LEDS_RED);

//LFP{
    /*//Enable GPT0 clocks under active mode
    ti_lib_prcm_peripheral_run_enable(PRCM_PERIPH_TIMER0);
    ti_lib_prcm_load_set();
    */

    //Enable GPT0 clocks under active, sleep, deep sleep
    ti_lib_prcm_peripheral_run_enable(PRCM_PERIPH_TIMER0);
    ti_lib_prcm_peripheral_sleep_enable(PRCM_PERIPH_TIMER0);
    ti_lib_prcm_peripheral_deep_sleep_enable(PRCM_PERIPH_TIMER0);
    ti_lib_prcm_load_set();
    while(!ti_lib_prcm_load_get());

    //Register with LPM. This will keep the PERIPH PD powered on
    //during deep sleep, allowing the pwm to keep working while the chip is
    //being power-cycled
    lpm_register_module(&pwmdrive_module);

//LFP}

    while(!ti_lib_prcm_load_get());

    /* Drive the I/O ID with GPT0 / Timer A */
    ti_lib_ioc_port_configure_set(IOID_21, IOC_PORT_MCU_PORT_EVENT0, IOC_STD_OUTPUT);

    /* GPT0 / Timer A: PWM, Interrupt Enable */
    ti_lib_timer_configure(GPT0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM);

    /* Stop the timers */
    ti_lib_timer_disable(GPT0_BASE, TIMER_A);
    ti_lib_timer_disable(GPT0_BASE, TIMER_B);

    if(freq > 0) {
        load = (GET_MCU_CLOCK / freq);

        ti_lib_timer_load_set(GPT0_BASE, TIMER_A, load);
        ti_lib_timer_match_set(GPT0_BASE, TIMER_A, load-1);

        /* Start */
        ti_lib_timer_enable(GPT0_BASE, TIMER_A);
    }
    return load;
}

void atualiza_sensor(void){

}

PROCESS(pwm_sensor, "Sensor com PWM");
PROCESS(read_tension, "Leitura de Tensão com ADC");

AUTOSTART_PROCESSES(&pwm_sensor, &read_tension);

PROCESS_THREAD(pwm_sensor, ev, data){

    PROCESS_BEGIN();

    static int16_t current_duty = 50, ticks = 0;

    printf("\nInicio do Processo PWM");

    loadvalue = pwminit(5000);

    printf("\nLoadvalue: %d", loadvalue);

    ticks = (current_duty * loadvalue) / 100;
    printf("\nTicks: %d", ticks);
    ti_lib_timer_match_set(GPT0_BASE, TIMER_A, loadvalue - ticks);

    while(1){
        PROCESS_YIELD();

        if(ev == sensors_event){

            printf("\nEvento Ocorreu!");

            if(data == &button_left_sensor){
                printf("\nLeft Button! -10%");

                if((current_duty - val_percent) >= 0)
                    current_duty = current_duty - val_percent;

            }else if(data == &button_right_sensor){
                printf("\nRight Button! +10%");

                if((current_duty + val_percent) <= 100)
                    current_duty = current_duty + val_percent;
            }

            if(current_duty == 0)
                ticks = 1;
            else{
                ticks = (current_duty * loadvalue) / 100;
            }

            printf("\nCurrent duty: %d", current_duty);
            printf("\nTicks atualizado: %d", ticks);

            ti_lib_timer_match_set(GPT0_BASE, TIMER_A, loadvalue - ticks);
        }else
            printf("\nNÃO FEZ NADA! ERRO");

    }

    PROCESS_END();
}

PROCESS_THREAD(read_tension, ev, data){
    PROCESS_BEGIN();

    printf("Inicio do Processo de Leitura do Tensor\n");

    //Declarando a estrutura do ADC Sensor
    static struct sensors_sensor *sensor;
    static int value_read = 0;
    static int16_t ticks = 0;
    static int percent_100 = 3320000;

    //ADC_SENSOR vai representar o AD que estará sendo utilizado
    sensor = sensors_find(ADC_SENSOR);

    etimer_set(&et_sensor, CLOCK_SECOND);

    while(1){
        PROCESS_YIELD();
        if (ev == PROCESS_EVENT_TIMER){
            //Ativar o ADC
            SENSORS_ACTIVATE(*sensor);

            //Configurar canal a ser utilizado, porta lógica
            sensor->configure(ADC_SENSOR_SET_CHANNEL, ADC_COMPB_IN_AUXIO7);

            value_read = sensor->value(ADC_SENSOR_VALUE);

            printf("\n\nLeitura do sensor: %d", value_read);
            printf("\nPercentual em 100 : %d", percent_100);

            val_percent = ( value_read * 100 ) / percent_100;
            if(val_percent < 5)
                val_percent = 1;
            else if(val_percent < 25)
                val_percent = 25;
            else if(val_percent < 50)
                val_percent = 50;
            else if(val_percent < 75)
                val_percent = 75;
            else
                val_percent = 100;

            ticks = (val_percent * loadvalue) / 100;

            printf("\nLoadValue: %d", loadvalue);
            printf("\nPercentual de leitura calculado: %d", val_percent);
            printf("\nTicks atualizado: %d", ticks);

            ti_lib_timer_match_set(GPT0_BASE, TIMER_A, loadvalue - ticks);

            //Desativar o ADC
            SENSORS_DEACTIVATE(*sensor);
            etimer_reset(&et_sensor);
        }
    }

    PROCESS_END();
}
