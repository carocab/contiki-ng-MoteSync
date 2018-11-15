/**
*   Proyecto RSI - MoteSync
*   Año: 2018
*   Grupo: Bentancour, Cabrera, Díaz
*   Basado en $(CONTIKI)/examples/6tisch/simple-node/
*   y $(CONTIKI)/examples/rpl-udp/udp-client.c
*/
#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "dev/user-sensor.h"
#include "net/ipv6/uiplib.h"
#include "sys/node-id.h"
#include "net/mac/tsch/tsch-asn.h"
#include <stdio.h>
#include "dev/user-sensor.h"
#include "sys/rtimer.h"

#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

/*---------------------------------------------------------------------------*/

#include "data.h"


/*---------------------------------------------------------------------------*/
/* Create a structure and pointer to store the data to be sent as payload */

static struct my_msg_t msg;
static struct my_msg_t *msgPtr = &msg;


/*---------------------------------------------------------------------------*/

static struct simple_udp_connection udp_conn;

struct data_sensor_t data_user_sensor;

/*---------------------------------------------------------------------------*/

PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);

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
  printf("Received response '%.*s' from ", datalen, (char *) data);
  uiplib_ipaddr_print(sender_addr);
 #if LLSEC802154_CONF_ENABLED
  printf(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL)); 
#endif
  printf("\n");

}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(udp_client_process, ev, data)
{
  static unsigned count;
  uip_ipaddr_t dest_ipaddr;
  
  PROCESS_BEGIN();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);
  SENSORS_ACTIVATE(user_sensor);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &user_sensor);
    data_user_sensor = get_data_sensor();
/*-------------------------------------------*/
      /* SENDER                                    */
      /*-------------------------------------------*/
      
      msg.id = node_id;
      msg.x_pos = 0;
      msg.y_pos = 0;
      
      msg.event_asn_ls4b = data_user_sensor.asn_ls4b;
      msg.event_asn_ms1b = data_user_sensor.asn_ms1b;
      
      msg.event_offset = (data_user_sensor.event_time - data_user_sensor.ref_time);    
      printf("ID: %x, X pos: %lu cm, Y pos: %lu cm, ASN: %lu, Offset: %lu ticks.\n", 
      msg.id, msg.x_pos, msg.y_pos, msg.event_asn_ls4b, msg.event_offset);
      
    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      /* Send to DAG root */
      printf("Sending request %u to ", count);
      uiplib_ipaddr_print(&dest_ipaddr);
      printf("\n"); 
      simple_udp_sendto(&udp_conn, msgPtr, sizeof(msg), &dest_ipaddr);
      count++;
    } else {
      printf("Not reachable yet\n");
    }
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
