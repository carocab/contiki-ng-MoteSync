

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/* MAC Address */
#define IEEE_802154_MAC_ADDRESS {0x00, 0x12, 0x74, 0x08, 0x0, 0x8, 0x0, 0x8}

/*Conf RPL*/
#define RPL_MRHOF_CONF_MAX_LINK_METRIC 10000

/* Set to enable TSCH security */

#define WITH_SECURITY 0

/* USB serial takes space, free more space elsewhere */
#define SICSLOWPAN_CONF_FRAG 0
#define UIP_CONF_BUFFER_SIZE 160

/* Configure CC1200 radio */
#undef  NETSTACK_CONF_RADIO
#define NETSTACK_CONF_RADIO         cc1200_driver
#define CC1200_CONF_USE_GPIO2       1 //en la wiki decia 0
#define CC1200_CONF_USE_RX_WATCHDOG 0
#define ANTENNA_SW_SELECT_DEF_CONF  ANTENNA_SW_SELECT_SUBGHZ
#define NETSTACK_CONF_RADIO         cc1200_driver
/*******************************************************/
/******************* Configure TSCH ********************/
/*******************************************************/

/* IEEE802.15.4 PANID */
#define IEEE802154_CONF_PANID 0xaaaa

/* Do not start TSCH at init, wait for NETSTACK_MAC.on() */
#define TSCH_CONF_AUTOSTART 1

/* 6TiSCH minimal schedule length.
 * Larger values result in less frequent active slots: reduces capacity and saves energy. */
#define TSCH_SCHEDULE_CONF_DEFAULT_LENGTH 3

#if WITH_SECURITY

/* Enable security */
#define LLSEC802154_CONF_ENABLED 1

#endif /* WITH_SECURITY */


/* Enable TSCH statistics */
#define TSCH_STATS_CONF_ON 1

/* Enable periodic RSSI sampling for TSCH statistics */
#define TSCH_STATS_CONF_SAMPLE_NOISE_RSSI 0

/* Reduce the TSCH stat "decay to normal" period to get printouts more often */
#define TSCH_STATS_CONF_DECAY_INTERVAL (60 * CLOCK_SECOND)

#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE TSCH_HOPPING_SEQUENCE_2_2



/*******************************************************/
/************* Other system configuration **************/
/*******************************************************/

/* Logging */
#define LOG_CONF_LEVEL_RPL                         LOG_LEVEL_WARN
//#define LOG_CONF_LEVEL_TCPIP                       LOG_LEVEL_WARN
//#define LOG_CONF_LEVEL_IPV6                        LOG_LEVEL_WARN
//#define LOG_CONF_LEVEL_6LOWPAN                     LOG_LEVEL_WARN
//#define LOG_CONF_LEVEL_MAC                         LOG_LEVEL_INFO
//#define LOG_CONF_LEVEL_FRAMER                      LOG_LEVEL_INFO
//#define TSCH_LOG_CONF_PER_SLOT                     1

#endif /* PROJECT_CONF_H_ */
