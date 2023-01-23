 /*
 * CSE 351 Lab 1b (Manipulating Bits in C)
 *
 * Name(s): Catalina Martinez, Asher Lozano
 * NetID(s): 2065453, 1841966
 *
 * This is a file for managing a store of various aisles, represented by an
 * array of 64-bit integers. See aisle_manager.c for details on the aisle
 * layout and descriptions of the aisle functions that you may call here.
 *
 * Written by Porter Jones (pbjones@cs.washington.edu)
 */

#include <stddef.h>  // To be able to use NULL
#include "aisle_manager.h"
#include "store_client.h"
#include "store_util.h"

// Number of aisles in the store
#define NUM_AISLES 10

// Number of sections per aisle
#define SECTIONS_PER_AISLE 4

// Number of items in the stockroom (2^6 different id combinations)
#define NUM_ITEMS 64

// Global array of aisles in this store. Each unsigned long in the array
// represents one aisle.
unsigned long aisles[NUM_AISLES];

// Array used to stock items that can be used for later. The index of the array
// corresponds to the item id and the value at an index indicates how many of
// that particular item are in the stockroom.
int stockroom[NUM_ITEMS];


/* Starting from the first aisle, refill as many sections as possible using
 * items from the stockroom. A section can only be filled with items that match
 * the section's item id. Prioritizes and fills sections with lower addresses
 * first. Sections with lower addresses should be fully filled (if possible)
 * before moving onto the next section.
 */
void refill_from_stockroom() {
  // TODO: implement this function
  int numItems;
  for(int i =0; i < NUM_AISLES; i++){
	  for(int j =0; j< SECTIONS_PER_AISLE;j++){
		  numItems =10- (num_items(aisles+i,j));
		  if(numItems >=stockroom[get_id(aisles+i,j)]){
			  add_items(aisles+i,j,stockroom[get_id(aisles+i,j)]);
			  stockroom[get_id(aisles+i,j)] = 0;
		  }
		  else if(stockroom[get_id(aisles+i,j)] != 0){
			  add_items(aisles+i,j,numItems);
			  stockroom[get_id(aisles+i,j)] = stockroom[get_id(aisles+i,j)]-numItems;
		  }
}
}
}

/* Remove at most num items from sections with the given item id, starting with
 * sections with lower addresses, and return the total number of items removed.
 * Multiple sections can store items of the same item id. If there are not
 * enough items with the given item id in the aisles, first remove all the
 * items from the aisles possible and then use items in the stockroom of the
 * given item id to finish fulfilling an order. If the stockroom runs out of
 * items, you should remove as many items as possible.
 */
int fulfill_order(unsigned short id, int num) {
  // TODO: implement this function
  unsigned short getid = 0;
  unsigned short stock = 0;
  int count = 0;
  int max = num;
  for(int i = 0; i < NUM_AISLES; i++){
	  for(int j = 0; j < SECTIONS_PER_AISLE;j++){
		  getid= get_id(aisles+i,j);
		  if(getid==id){
			  stock = num_items(aisles+i,j);
			  remove_items(aisles+i,j,num);
			  num-=stock;
			  count+=stock;
		  }if(num <0) return max;
	  }
  }
  if(num<=stockroom[id] && num >0){
	  stockroom[id]-=num;
	  count+=num;
  }else if(stockroom[id] <num && num>0){
	  count+=stockroom[id];
	  num-=stockroom[id];
	  stockroom[id] = 0;
  }
  return count;

}

/* Return a pointer to the first section in the aisles with the given item id
 * that has no items in it or NULL if no such section exists. Only consider
 * items stored in sections in the aisles (i.e., ignore anything in the
 * stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* empty_section_with_id(unsigned short id) {
  // TODO: implement this function
  unsigned long* aisle;
  for(int i = 0; i < NUM_AISLES; i++){
	  aisle = (aisles+i);
	  for(int j = 0; j <SECTIONS_PER_AISLE;j++){
		if(get_id(aisle,j)==id){
			if(num_items(aisle, j)==0){
				return (unsigned short*)(aisle)+j;
			}
		}
	  }
  }
  return NULL;

}

/* Return a pointer to the section with the most items in the store. Only
 * consider items stored in sections in the aisles (i.e., ignore anything in
 * the stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short* section_with_most_items() {
  // TODO: implement this function
  unsigned long* aisle;
  unsigned short* res;
  unsigned short max = 0;
  for(int i =NUM_AISLES-1;i>=0;i--){
	aisle = aisles+i;
	for(int j = SECTIONS_PER_AISLE-1; j >=0; j--){
		if(num_items(aisle,j)>= max){
			max = num_items(aisle,j);
			res = (unsigned short*)(aisle)+j;
		}
	}
  }
  return res;

}
