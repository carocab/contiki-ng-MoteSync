#include "decoder.h"

#include "contiki.h"
#include "stdio.h"
#include "math.h"

#define TICKS_PER_S 32768
#define TIMESLOTS_PER_S 100

#define PROPAGATION_CM_S 33100

struct my_node_t {
  uint8_t event_valid;
  uint8_t id;
  uint32_t x_pos; // cm
  uint32_t y_pos; // cm
  uint32_t event_asn_ls4b;
  uint32_t event_asn_ms1b;
  uint32_t event_offset; // ticks
};

static struct my_node_t my_nodes[8];
static uint8_t node_count;
static uint32_t max_dist;
static uint32_t min_ticks;

static uint32_t min_x_pos; // cm
static uint32_t min_y_pos; // cm

static uint32_t max_x_pos; // cm
static uint32_t max_y_pos; // cm

static uint8_t min_node;		
static uint32_t min_asn;
static uint32_t min_offset;

void decoder_receiver(const uint8_t *data)
{
	/* Create a pointer to the received data, adjust to the expected structure */
  	struct my_msg_t *msgPtr = (struct my_msg_t *) data;

	printf("ID: %x, X pos: %lu cm, Y pos: %lu cm, ASN: %lu, Offset: %lu ticks.\n", 
		msgPtr->id, msgPtr->x_pos, msgPtr->y_pos, msgPtr->event_asn_ls4b, msgPtr->event_offset);
	
	if (node_count == 0){
		printf("First node!\n");
		struct my_node_t new_node;
		new_node.event_valid = 1;
		new_node.id = msgPtr->id;
		new_node.x_pos = msgPtr->x_pos;
		new_node.y_pos = msgPtr->y_pos;
		new_node.event_asn_ls4b = msgPtr->event_asn_ls4b;
		new_node.event_offset = msgPtr->event_offset;
		my_nodes[node_count] = new_node;
		node_count++;
	} else {
		uint8_t is_new;
		is_new = 1;
		int i;		
		for (i = 0; i < node_count; i++){
			if(my_nodes[i].id == msgPtr->id){
				is_new = 0;
                		printf("Updating values for node %x: ", my_nodes[i].id);
                
                		my_nodes[i].x_pos = msgPtr->x_pos;
                		my_nodes[i].y_pos = msgPtr->y_pos;
                		
				if ((msgPtr->event_asn_ls4b >= my_nodes[i].event_asn_ls4b) && (msgPtr->event_offset > my_nodes[i].event_offset)){
		        		
					uint32_t delta_ticks = (TICKS_PER_S * (msgPtr->event_asn_ls4b - my_nodes[i].event_asn_ls4b)) / TIMESLOTS_PER_S
								+  msgPtr->event_offset - my_nodes[i].event_offset;
		        		
					printf("delta_ticks = %lu, ", delta_ticks);
					
		        		if (delta_ticks >= min_ticks){
		            			printf("value correct\n");
		            
		            			my_nodes[i].event_asn_ls4b = msgPtr->event_asn_ls4b;
		            			my_nodes[i].event_offset = msgPtr->event_offset;
		            
		            			my_nodes[i].event_valid = 1;
		        		} else {
		            			printf("too soon\n");
		        		}
				} else {
					printf("old event\n");
				}
			}
		}
		if (is_new == 1){
			printf("New node!\n");
			struct my_node_t new_node;
			new_node.event_valid = 1;
			new_node.id = msgPtr->id;
			new_node.x_pos = msgPtr->x_pos;
			new_node.y_pos = msgPtr->y_pos;
			new_node.event_asn_ls4b = msgPtr->event_asn_ls4b;
			new_node.event_offset = msgPtr->event_offset;
			my_nodes[node_count] = new_node;
			node_count++;
		}        
	}
}

static uint8_t is_compatible(uint32_t base){
	uint32_t x;
	uint32_t y;
	uint8_t n;
	uint8_t compatible;	

	for (x = min_x_pos; x <= max_x_pos; x++){
		for (y = min_y_pos; y <= max_y_pos; y++){
			
			compatible = 1;
			for (n = 0; n < node_count; n++){
				uint32_t d2 = (my_nodes[n].x_pos - x) * (my_nodes[n].x_pos - x)
					 + (my_nodes[n].y_pos - y) * (my_nodes[n].y_pos - y);
				uint32_t d = sqrt(d2); // cm
				
				uint32_t t = (TICKS_PER_S * (my_nodes[n].event_asn_ls4b - min_asn)) / TIMESLOTS_PER_S
								+  my_nodes[n].event_offset - min_offset;
				t += base;
				
				uint32_t r = (t * PROPAGATION_CM_S) / TICKS_PER_S;

				if (d > r){
					compatible = 0;
				} else {
					
				}
			}
			if (compatible){
				printf("\n--------------------------------------\n");
				printf("base = %lu, x = %lu, y = %lu\n", base, x, y);
				printf("--------------------------------------\n\n");
				return 1;
			}
			
			watchdog_periodic();
		}
	}
	
	return 0;
}

void decoder_process(void)
{ 
	if (node_count >= 3){
		uint8_t all_valid;
		all_valid = 1;
		
		printf("Calculating distance... ");

		max_dist = 0;
		uint8_t i;
		uint8_t j;
		for (i = 0; i < node_count; i++){
		    for (j = 0; j < node_count; j++){
		        uint32_t d2 = (my_nodes[i].x_pos - my_nodes[j].x_pos) * (my_nodes[i].x_pos - my_nodes[j].x_pos)
					 + (my_nodes[i].y_pos - my_nodes[j].y_pos) * (my_nodes[i].y_pos - my_nodes[j].y_pos);
			uint32_t d = sqrt(d2);
		        if (d > max_dist){
		            max_dist = d;
		        }
		    }
		    all_valid &= my_nodes[i].event_valid;            
		}

		min_ticks = TICKS_PER_S * max_dist / PROPAGATION_CM_S;

		printf("max_dist = %lu cm, min_ticks = %lu.\n", max_dist, min_ticks);		
		
		if (all_valid){
			printf("All data valid!\n");
			
			min_node = 0;
			min_asn = my_nodes[0].event_asn_ls4b;
			min_offset = my_nodes[0].event_offset;

			max_x_pos = 0;
			max_y_pos = 0;

			min_x_pos = max_dist;
			min_y_pos = max_dist;
		    	
		   	for (i = 0; i < node_count; i++){
		        	if (my_nodes[i].event_asn_ls4b < min_asn){
					min_node = i;
					min_asn = my_nodes[i].event_asn_ls4b;
					min_offset = my_nodes[i].event_offset;
				} else {
					if ((my_nodes[i].event_asn_ls4b == min_asn)&&(my_nodes[i].event_offset < min_offset)){
						min_node = i;						
						min_offset = my_nodes[i].event_offset;
					}
				}

				if (my_nodes[i].x_pos > max_x_pos){
					max_x_pos = my_nodes[i].x_pos;
				}

				if (my_nodes[i].y_pos > max_y_pos){
					max_y_pos = my_nodes[i].y_pos;
				}

				if (my_nodes[i].x_pos < min_x_pos){
					min_x_pos = my_nodes[i].x_pos;
				}

				if (my_nodes[i].y_pos < min_y_pos){
					min_y_pos = my_nodes[i].y_pos;
				}

		    	}
			
			printf("Closest node is %x, min_asn = %lu, min_offset = %lu\n", my_nodes[min_node].id, min_asn, min_offset);
			
			min_x_pos = 90;
			max_x_pos = 110;
			min_y_pos = 90;
			max_y_pos = 110;

			printf("Search interval x = (%lu,%lu) y = (%lu,%lu)\n", min_x_pos, max_x_pos, min_y_pos, max_y_pos);

			uint32_t base_delay;

			for (base_delay = 0; base_delay < max_dist; base_delay++){
				if (is_compatible(base_delay)){
					printf("Event found!\n");
					break;
				}					
			}
		}
		
	} else {
        	max_dist = 0;
        	printf("Not enouth nodes!\n");
	}
}
