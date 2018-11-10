/**
 * \file
 *         Basado en los ejemplos simple-udp-rpl y rpl-tsch
 *         ambos en [CONTIKI_DIR]/examples/ipv6/.
 *
 * \author Docentes RSI
 */

#include "contiki.h"
#include "sys/node-id.h"
#include "net/rpl/rpl.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/mac/tsch/tsch.h"
#include "net/rpl/rpl-private.h"
#include "simple-udp.h"
#include <stdio.h>
#include <stdlib.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define UDP_PORT 1234
#define SEND_INTERVAL (30 * CLOCK_SECOND)
#define SEND_TIME (rand() % (SEND_INTERVAL))

////////////////////////////////////////////////////////////////////////////

#include "data.h"

/* Create a structure and pointer to store the data to be sent as payload */
static struct my_msg_t msg;
static struct my_msg_t *msgPtr = &msg;

////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------*/

static struct simple_udp_connection unicast_connection;
/*---------------------------------------------------------------------------*/
PROCESS(node_process, "RPL Node");
AUTOSTART_PROCESSES(&node_process);

/*---------------------------------------------------------------------------*/
static void
print_network_status(void)
{
  int i;
  uint8_t state;
  uip_ds6_defrt_t *default_route;
#if RPL_WITH_STORING
  uip_ds6_route_t *route;
#endif /* RPL_WITH_STORING */
#if RPL_WITH_NON_STORING
  rpl_ns_node_t *link;
#endif /* RPL_WITH_NON_STORING */

  PRINTF("--- Network status ---\n");

  /* Our IPv6 addresses */
  PRINTF("- Server IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINTF("-- ");
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
    }
  }

  /* Our default route */
  PRINTF("- Default route:\n");
  default_route = uip_ds6_defrt_lookup(uip_ds6_defrt_choose());
  if(default_route != NULL) {
    PRINTF("-- ");
    PRINT6ADDR(&default_route->ipaddr);
    PRINTF(" (lifetime: %lu seconds)\n", (unsigned long)default_route->lifetime.interval);
  } else {
    PRINTF("-- None\n");
  }

#if RPL_WITH_STORING
  /* Our routing entries */
  PRINTF("- Routing entries (%u in total):\n", uip_ds6_route_num_routes());
  route = uip_ds6_route_head();
  while(route != NULL) {
    PRINTF("-- ");
    PRINT6ADDR(&route->ipaddr);
    PRINTF(" via ");
    PRINT6ADDR(uip_ds6_route_nexthop(route));
    PRINTF(" (lifetime: %lu seconds)\n", (unsigned long)route->state.lifetime);
    route = uip_ds6_route_next(route);
  }
#endif

#if RPL_WITH_NON_STORING
  /* Our routing links */
  PRINTF("- Routing links (%u in total):\n", rpl_ns_num_nodes());
  link = rpl_ns_node_head();
  while(link != NULL) {
    uip_ipaddr_t child_ipaddr;
    uip_ipaddr_t parent_ipaddr;
    rpl_ns_get_node_global_addr(&child_ipaddr, link);
    rpl_ns_get_node_global_addr(&parent_ipaddr, link->parent);
    PRINTF("-- ");
    PRINT6ADDR(&child_ipaddr);
    if(link->parent == NULL) {
      memset(&parent_ipaddr, 0, sizeof(parent_ipaddr));
      PRINTF(" --- DODAG root ");
    } else {
      PRINTF(" to ");
      PRINT6ADDR(&parent_ipaddr);
    }
    PRINTF(" (lifetime: %lu seconds)\n", (unsigned long)link->lifetime);
    link = rpl_ns_node_next(link);
  }
#endif

  PRINTF("----------------------\n");
}
/*---------------------------------------------------------------------------*/
static void
net_init(uip_ipaddr_t *br_prefix)
{
  uip_ipaddr_t global_ipaddr;

  if(br_prefix) { /* We are RPL root. Will be set automatically
                     as TSCH pan coordinator via the tsch-rpl module */
    memcpy(&global_ipaddr, br_prefix, 16);
    uip_ds6_set_addr_iid(&global_ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&global_ipaddr, 0, ADDR_AUTOCONF);
    rpl_set_root(RPL_DEFAULT_INSTANCE, &global_ipaddr);
    rpl_set_prefix(rpl_get_any_dag(), br_prefix, 64);
    rpl_repair_root(RPL_DEFAULT_INSTANCE);
  }

  NETSTACK_MAC.on();
}
/*---------------------------------------------------------------------------*/
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  printf("Data received on port %d from port %d with length %d\n",
         receiver_port, sender_port, datalen);
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;
  int i;
  uint8_t state;

  uip_ip6addr(&ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

  printf("IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
      printf("\n");
    }
  }
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(node_process, ev, data)
{
  static struct etimer print_timer, periodic_timer, send_timer;
  uip_ipaddr_t addr;
  uip_ip6addr(&addr, 0xfe80, 0x0, 0x0, 0x0, 0x212, 0x4b00, 0x60d, 0x0012);

  PROCESS_BEGIN();

  set_global_address();

  simple_udp_register(&unicast_connection, UDP_PORT, NULL, UDP_PORT, receiver);

#if WITH_TSCH
  /* TSCH role:
   * - role_6ln: simple node, will join any network, secured or not
   */
  static enum { role_6ln, role_6dr, role_6dr_sec } node_role;
  node_role = role_6ln;

  printf("Init: node starting with role %s\n",
         node_role == role_6ln ? "6ln" : (node_role == role_6dr) ? "6dr" : "6dr-sec");

  tsch_set_pan_secured(0);

  net_init(NULL);
#endif /* WITH_TSCH */

  /* Print out routing tables every minute */
  etimer_set(&print_timer, CLOCK_SECOND * 60);

  etimer_set(&periodic_timer, SEND_INTERVAL);

  while(1) {
    PROCESS_WAIT_EVENT();

    if (etimer_expired(&print_timer)) {
      print_network_status();
      etimer_reset(&print_timer);
    }
    if (etimer_expired(&periodic_timer)) {
      etimer_reset(&periodic_timer);
      etimer_set(&send_timer, SEND_TIME);
    }
    if (etimer_expired(&send_timer)) {
      printf("Sending unicast to ");
        	uip_debug_ipaddr_print(&addr);
        	printf("\n");
      
      /*-------------------------------------------*/
      /* SENDER                                    */
      /*-------------------------------------------*/
      
      msg.id = 15;
      msg.x_pos = 0;
      msg.y_pos = 0;
      
      msg.event_asn_ls4b = 0;
      msg.event_asn_ms1b = 0;
      
      msg.event_offset = 149;
      
      printf("ID: %d, X pos: %lu cm, Y pos: %lu cm, ASN: %lu, Offset: %lu ticks.\n", 
      	msg.id, msg.x_pos, msg.y_pos, msg.event_asn_ls4b, msg.event_offset);
      
      simple_udp_sendto(&unicast_connection, msgPtr, sizeof(msg), &addr);

      msg.id = 16;
      msg.x_pos = 0;
      msg.y_pos = 324;
      
      msg.event_asn_ls4b = 0;
      msg.event_asn_ms1b = 0;
      
      msg.event_offset = 255;
      
      printf("ID: %d, X pos: %lu cm, Y pos: %lu cm, ASN: %lu, Offset: %lu ticks.\n",
      	msg.id, msg.x_pos, msg.y_pos, msg.event_asn_ls4b, msg.event_offset);
      
      simple_udp_sendto(&unicast_connection, msgPtr, sizeof(msg), &addr);

      msg.id = 17;
      msg.x_pos = 500;
      msg.y_pos = 0;
      
      msg.event_asn_ls4b = 1;
      msg.event_asn_ms1b = 0;
      
      msg.event_offset = 95;
      
      printf("ID: %d, X pos: %lu cm, Y pos: %lu cm, ASN: %lu, Offset: %lu ticks.\n",
      	msg.id, msg.x_pos, msg.y_pos, msg.event_asn_ls4b, msg.event_offset);
      
      simple_udp_sendto(&unicast_connection, msgPtr, sizeof(msg), &addr);
      
      /*-------------------------------------------*/
      /* END SENDER                                */
      /*-------------------------------------------*/
      
    }
}
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
