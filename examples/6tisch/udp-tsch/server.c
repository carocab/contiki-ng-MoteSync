#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "net/mac/tsch/tsch-asn.h"


#define WITH_SERVER_REPLY  0
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

static struct simple_udp_connection udp_conn;

/**
  * Variables para alamcenar el inicio de timeslot y el ASN
  */

extern uint32_t ref;
extern struct tsch_asn_t event_ASN; 


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
  //char buf[UIPLIB_IPV6_MAX_STR_LEN];
  //uiplib_ipaddr_snprint(buf, sizeof(buf), sender_addr);
  printf("Received request '%.*s' from ", datalen, (char *) data);
  //printf("%s", buf);
  uiplib_ipaddr_print(sender_addr);
  printf("\n");
#if WITH_SERVER_REPLY
  /* send back the same string to the client as an echo reply */
  printf("Sending response.\n");
  simple_udp_sendto(&udp_conn, data, datalen, sender_addr);
#endif /* WITH_SERVER_REPLY */
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
