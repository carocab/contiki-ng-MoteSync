#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/* USB serial takes space, free more space elsewhere */
#define SICSLOWPAN_CONF_FRAG 0
#define UIP_CONF_BUFFER_SIZE 160

/*******************************************************/
/******************* Configure TSCH ********************/
/*******************************************************/

/* Do not start TSCH at init, wait for NETSTACK_MAC.on() */
#define TSCH_CONF_AUTOSTART 1

/* 6TiSCH minimal schedule length.
 * Larger values result in less frequent active slots: reduces capacity and saves energy. */
#define TSCH_SCHEDULE_CONF_DEFAULT_LENGTH 3


/* Enable TSCH statistics */
#define TSCH_STATS_CONF_ON 1

/* Enable periodic RSSI sampling for TSCH statistics */
#define TSCH_STATS_CONF_SAMPLE_NOISE_RSSI 0

#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE TSCH_HOPPING_SEQUENCE_2_2



#endif /* PROJECT_CONF_H_ */
