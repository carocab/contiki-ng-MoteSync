#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "net/mac/tsch/tsch-asn.h"

////////////////////////////////////////////////////////////////////////////

#include "decoder.h"

////////////////////////////////////////////////////////////////////////////

#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

static struct simple_udp_connection udp_conn;

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  printf("Received request from ");
  uiplib_ipaddr_print(sender_addr);
  printf("\n");

  /*-------------------------------------------*/
  /* RECEIVER                                  */
  /*-------------------------------------------*/
  
  decoder_receiver(data);
  
  /*-------------------------------------------*/
  /* END RECEIVER                              */
  /*-------------------------------------------*/
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  static struct etimer print_timer;
  PROCESS_BEGIN();

  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);

  etimer_set(&print_timer, CLOCK_SECOND * 30);
  while(1) {
    PROCESS_WAIT_EVENT();

    if (etimer_expired(&print_timer)) {

      etimer_reset(&print_timer);
      
      /*-------------------------------------------*/
      /* RECIEVER                                  */
      /*-------------------------------------------*/
      
      decoder_process();
      
      /*-------------------------------------------*/
      /* END RECIEVER                              */
      /*-------------------------------------------*/
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
