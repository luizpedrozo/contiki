#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/rpl/rpl.h"
#include "dev/leds.h"

#include <string.h>

#define LED_TOGGLE_REQUEST (0x79)
#define LED_SET_STATE (0x7A)
#define LED_GET_STATE (0x7B)
#define LED_STATE (0x7C)

#define CONN_PORT (8802)

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN])

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *server_conn;

uint8_t ledCounter=0;

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&resolv_process,&udp_server_process);
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
    char buf[MAX_PAYLOAD_LEN];
    char* msg = (char*)uip_appdata;
    int i;

    if(uip_newdata()) {
        leds_toggle(LEDS_RED);
        ((char *)uip_appdata)[uip_datalen()] = 0;
        PRINTF("Server received: '%s' from ", msg);
        PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
        PRINTF("\n");

        //uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
        //PRINTF("Responding with message: ");
        //sprintf(buf, "Hello from the server! (%d)", ++seq_id);
        //PRINTF("%s\n", buf);

        switch (msg[0])
        {
        case LED_TOGGLE_REQUEST:
        {
            PRINTF("LED_TOGGLE_REQUEST\n");
            //Monta um LED_SET_STATE e envia para o nó solicitante
            uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
            server_conn->rport = UIP_UDP_BUF->destport;
            buf[0] = LED_SET_STATE;
            buf[1] = (ledCounter++)&0x03;
            uip_udp_packet_send(server_conn, buf, 2);
            PRINTF("Enviando LED_SET_STATE para [");
            PRINT6ADDR(&server_conn->ripaddr);
            PRINTF("]:%u\n", UIP_HTONS(server_conn->rport));
            /* Restore server connection to allow data from any node */
            uip_create_unspecified(&server_conn->ripaddr);
            server_conn->rport = 0;
            break;
        }
        case LED_STATE:
        {
            PRINTF("LED_STATE: %s %s\n",(msg[1]&LEDS_GREEN)?" (G) ":"  G  ",(msg[1]&LEDS_RED)?" (R) ":"  R  ");
            break;
        }
        default:
        {
            PRINTF("Comando Invalido: ");
            for(i=0;i<uip_datalen();i++)
            {
                PRINTF("0x%02X ",msg[i]);
            }
            PRINTF("\n");
            break;
        }
        }
    }

    uip_udp_packet_send(server_conn, buf, strlen(buf));
    /* Restore server connection to allow data from any node */
    memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));
    return;
}
/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Server IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
    }
  }
}
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(udp_server_process, ev, data)
{
#if UIP_CONF_ROUTER
  uip_ipaddr_t ipaddr;
  rpl_dag_t *dag;
#endif /* UIP_CONF_ROUTER */

  PROCESS_BEGIN();
  PRINTF("UDP server started\n");

#if RESOLV_CONF_SUPPORTS_MDNS
  resolv_set_hostname("contiki-udp-server");
  PRINTF("Setting hostname to contiki-udp-server\n");
#endif

#if UIP_CONF_ROUTER
  uip_ip6addr(&ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
#endif /* UIP_CONF_ROUTER */

  print_local_addresses();

//#if 1 //UIP_CONF_ROUTER
#if 0 //UIP_CONF_ROUTER - OFF - Only like client
  dag = rpl_set_root(RPL_DEFAULT_INSTANCE,
                     &uip_ds6_get_global(ADDR_PREFERRED)->ipaddr);
  if(dag != NULL) {
    uip_ip6addr(&ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
    rpl_set_prefix(dag, &ipaddr, 64);
    PRINTF("Created a new RPL dag with ID: ");
    PRINT6ADDR(&dag->dag_id);
    PRINTF("\n");
  }
#endif

  server_conn = udp_new(NULL, UIP_HTONS(CONN_PORT), NULL);
  udp_bind(server_conn, UIP_HTONS(CONN_PORT));

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/