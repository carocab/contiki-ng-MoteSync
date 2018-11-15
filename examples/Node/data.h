#ifndef DATA_H
#define DATA_H

/*---------------------------------------------------------------------------*/
/* This data structure is used to store the packet content (payload) */
struct my_msg_t {
  uint16_t id;
  uint32_t x_pos; // cm
  uint32_t y_pos; // cm
  uint32_t event_asn_ls4b;
  uint32_t event_asn_ms1b;
  uint32_t event_offset; // ticks
};

/*---------------------------------------------------------------------------*/

#endif
