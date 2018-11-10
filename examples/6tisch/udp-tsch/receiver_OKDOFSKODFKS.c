/**
 * \file
 *         Basado en los ejemplos simple-udp-rpl y rpl-tsch
 *         ambos en [CONTIKI_DIR]/examples/ipv6/.
 *
 * \author Remote Sync
 */

#include "contiki.h"
#include "node-id.h"
#include "net/rpl/rpl.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/mac/tsch/tsch.h"
#include "net/rpl/rpl-private.h"
#include "core/sys/clock.h"
#include "simple-udp.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define UDP_PORT 1234

////////////////////////////////////////////////////////////////////////////

#include "decoder.h"

////////////////////////////////////////////////////////////////////////////

static struct simple_udp_connection unicast_connection;
/*---------------------------------------------------------------------------*/
PROCESS(node_process, "RPL Node");
AUTOSTART_PROCESSES(&node_process);

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
  printf("Data received from ");
  uip_debug_ipaddr_print(sender_addr);
  printf(" on port %d from port %d with length %d.\n",
         receiver_port, sender_port, datalen);
  
  /*-------------------------------------------*/
  /* RECEIVER                                  */
  /*-------------------------------------------*/
  
  decoder_receiver(data);
  
  /*-------------------------------------------*/
  /* END RECEIVER                              */
  /*-------------------------------------------*/
}
/*---------------------------------------------------------------------------*/
static uip_ipaddr_t *
set_global_address(void)
{
  static uip_ipaddr_t ipaddr;
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

  return &ipaddr;
}
/*---------------------------------------------------------------------------*/
static void
create_rpl_dag(uip_ipaddr_t *ipaddr)
{
  struct uip_ds6_addr *root_if;

  root_if = uip_ds6_addr_lookup(ipaddr);
  if(root_if != NULL) {
    rpl_dag_t *dag;
    uip_ipaddr_t prefix;
    
    rpl_set_root(RPL_DEFAULT_INSTANCE, ipaddr);
    dag = rpl_get_any_dag();
    uip_ip6addr(&prefix, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
    rpl_set_prefix(dag, &prefix, 64);
    PRINTF("created a new RPL dag\n");
  } else {
    PRINTF("failed to create a new RPL DAG\n");
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(node_process, ev, data)
{
  static struct etimer print_timer;
  uip_ipaddr_t *ipaddr;

  PROCESS_BEGIN();

  ipaddr = set_global_address();

  create_rpl_dag(ipaddr);

  simple_udp_register(&unicast_connection, UDP_PORT, NULL, UDP_PORT, receiver);

#if WITH_TSCH
  /* 2 possible TSCH roles:
   * - role_6dr: DAG root, will advertise (unsecured) beacons
   * - role_6dr_sec: DAG root, will advertise secured beacons
   */
  static enum { role_6ln, role_6dr, role_6dr_sec } node_role;

  if(LLSEC802154_ENABLED) {
    node_role = role_6dr_sec;
  } else {
    node_role = role_6dr;
  }

  printf("Init: node starting with role %s\n",
         node_role == role_6ln ? "6ln" : (node_role == role_6dr) ? "6dr" : "6dr-sec");

  tsch_set_pan_secured(LLSEC802154_ENABLED);

  uip_ipaddr_t prefix;
  uip_ip6addr(&prefix, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
  net_init(&prefix);

#endif /* WITH_TSCH */

  /* Print timer */
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
