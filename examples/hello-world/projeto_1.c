/*
//EXEMPLO1
#include "contiki.h"
#include "sys/timer.h"
#include <stdio.h> // For printf()

static struct timer nt;

//---------------------------------------------------------------------------
PROCESS(test_timer_process, "Test Timer Process");
AUTOSTART_PROCESSES(&test_timer_process);
//---------------------------------------------------------------------------
PROCESS_THREAD(test_timer_process, ev, data){
  PROCESS_BEGIN();

  static u_int32_t ticks = 0;

  timer_set(&nt, CLOCK_SECOND);


  while(!timer_expired(&nt)) {
  ticks++;
  }
  printf (" timer,_ticks:_\t %ld\n", ticks);


  PROCESS_END();
}
//---------------------------------------------------------------------------
*/
/*//EXEMPLO2
#include "contiki.h"
#include "sys/ctimer.h"
#include <stdio.h> //For Printf("");

//Função que será chamada após a execução do timer
static void ctimer_callback_ex( void *ptrValor){
    printf("Passou aqui\n");
}

static struct ctimer ct;

//---------------------------------------------------------------------------
PROCESS(test_timer_process, "Test Timer Process");
AUTOSTART_PROCESSES(&test_timer_process);
//---------------------------------------------------------------------------
PROCESS_THREAD(test_timer_process, ev, data){
  PROCESS_BEGIN();

  static u_int32_t valorQualquer = 20;

  //CLOCK_SECOND * 2 - significa uma espera de 2 segundos e em seguida chama a função ctimer_callback_ex
  ctimer_set(&ct, CLOCK_SECOND * 2, ctimer_callback_ex, &valorQualquer );

  while(1){
      //Libera o processador após a execução do timer
      PROCESS_YIELD();
  }
  PROCESS_END();
}
//---------------------------------------------------------------------------
*/

//EXEMPLO 3 - Botões e Leds
#include "contiki.h"
#include <stdio.h> //For Printf("");
#include "dev/button-sensor.h"
#include "dev/leds.h"

//---------------------------------------------------------------------------
PROCESS(test_sensor, "Test Timer Process");
AUTOSTART_PROCESSES(&test_sensor);
//---------------------------------------------------------------------------
PROCESS_THREAD(test_sensor, ev, data){
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);

  while(1){
      PROCESS_YIELD(); //Libera o processador

      if(ev == sensors_event){
          if(data == &button_left_sensor){
              printf("Botao Esquerdo.\n");
              leds_toggle(LEDS_GREEN);
          }else if(data == &button_right_sensor){
              printf("Botao Direito.\n");
              leds_toggle(LEDS_RED);
          }
      }
  }
  SENSORS_DEACTIVATE(button_sensor);
  PROCESS_END();
}
//---------------------------------------------------------------------------
