#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "net/mac/tsch/tsch-asn.h"

////////////////////////////////////////////////////////////////////////////

#include "decoder.h"
#include "dev/user-sensor.h"
#include "sys/rtimer.h"
#include "sys/node-id.h"

static struct my_msg_t msg;

struct data_sensor_t data_user_sensor;

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

  SENSORS_ACTIVATE(user_sensor);
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

    if (ev == sensors_event && data == &user_sensor){
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
      printf("ID: %d, X pos: %lu cm, Y pos: %lu cm, ASN: %lu, Offset: %lu ticks.\n", 
      msg.id, msg.x_pos, msg.y_pos, msg.event_asn_ls4b, msg.event_offset);

    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
