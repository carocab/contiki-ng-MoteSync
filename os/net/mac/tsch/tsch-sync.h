/**
*   Proyecto RSI - MoteSync
*   Año: 2018
*
*   path: $(CONTIKI)/os/net/mac/tsch/tsch-sync.h
*   biref: Modulo para obtener copia de ASN y slot_start. 
*   Implmentación de las funciones en $(CONTIKI)/os/net/mac/tsch/tsch-slot-operation.c
*/

#ifndef TSCH_SYNC_H_
#define TSCH_SYNC_H_

/********** Functions *********/
#include "net/mac/tsch/tsch-asn.h"
struct tsch_asn_t get_asn(void); 

uint32_t get_slot_start(void);

#endif /*TSCH_SYNC_H_ */