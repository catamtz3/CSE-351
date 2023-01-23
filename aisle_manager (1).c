/*
 * CSE 351 Lab 1b (Manipulating Bits in C)
 *
 * Name(s):  Asher Lozano, Catalina Martinez
 * NetID(s): 1841966, 2065453
 *
 * ----------------------------------------------------------------------------
 * Overview
 * ----------------------------------------------------------------------------
 *  This is a program to keep track of the items in a small aisle of a store.
 *
 *  A store's aisle is represented by a 64-bit long integer, which is broken
 *  into 4 16-bit sections representing one type of item each. Note that since
 *  a section is 16-bits, it fits nicely into C's short datatype.
 *
 *  Aisle Layout:
 *
 *    Within an aisle, sections are indexed starting with the least-significant
 *    section being at index 0 and continuing up until one less than the number
 *    of sections.
 *
 *    Example aisle:
 *
 *                MSB                                                       LSB
 *                  +-------------+-------------+-------------+-------------+
 *                  |  Section 3  |  Section 2  |  Section 1  |  Section 0  |
 *                  +-------------+-------------+-------------+-------------+
 *                  |             |             |             |             |
 *      bit offset: 64            48            32            16            0
 *
 *  Section Layout:
 *
 *    A section in an aisle is broken into 2 parts. The 6 most significant bits
 *    represent a unique identifier for the type of item stored in that
 *    section. The rest of the bits in a section (10 least significant)
 *    indicate individual spaces for items in that section. For each of the 10
 *    bits/spaces, a 1 indicates that an item of the section's type is stored
 *    there and a 0 indicates that the space is empty.
 *
 *    Example aisle section: 0x651A
 *
 *                MSB                               LSB
 *                  +-----------+-------------------+
 *                  |0 1 1 0 0 1|0 1 0 0 0 1 1 0 1 0|
 *                  +-----------+-------------------+
 *                  | item id   | section spaces    |
 *      bit offset: 16          10                  0
 *
 *      In this example, the item id is 0b011001, and there are currently 4
 *      items stored in the section (at bit offsets 8, 4, 3, and 1) and 6
 *      vacant spaces.
 *
 *  Written by Porter Jones (pbjones@cs.washington.edu)
 */

#include "aisle_manager.h"
#include "store_util.h"

// the number of total bits in a section
#define SECTION_SIZE 16

// The number of bits in a section used for the item spaces
#define NUM_SPACES 10

// The number of bits in a section used for the item id
#define ID_SIZE 6

// The number of sections in an aisle
#define NUM_SECTIONS 4

// TODO: Fill in these with the correct hex values

// Mask for extracting a section from the least significant bits of an aisle.
// (aisle & SECTION_MASK) should preserve a section's worth of bits at the
// lower end of the aisle and set all other bits to 0. This is essentially
// extracting section 0 from the example aisle shown above.
#define SECTION_MASK 0x000000000000FFFF

// Mask for extracting the spaces bits from a section.
// (section & SPACES_MASK) should preserve all the spaces bits in a section and
// set all non-spaces bits to 0.
#define SPACES_MASK 0x03FF

// Mask for extracting the ID bits from a section.
// (section & ID_MASK) should preserve all the id bits in a section and set all
// non-id bits to 0.
#define ID_MASK 0xFC00


/* Given a pointer to an aisle and a section index, return the section at the
 * given index of the given aisle.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_section(unsigned long* aisle, int index) {
  // TODO: implement this method
  unsigned long aisle_data;
  unsigned short *section;

  aisle_data = *aisle;
  aisle_data = aisle_data >> (SECTION_SIZE * index);
  aisle_data &= SECTION_MASK;
  section = (unsigned short*) & aisle_data;
  return *section;
}

/* Given a pointer to an aisle and a section index, return the spaces of the
 * section at the given index of the given aisle. The returned short should
 * have the least 10 significant bits set to the spaces and the 6 most
 * significant bits set to 0.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_spaces(unsigned long* aisle, int index) {
  unsigned short spaces;

  spaces = get_section(aisle, index);
  spaces &= SPACES_MASK;
  return spaces;
}

/* Given a pointer to an aisle and a section index, return the id of the
 * section at the given index of the given aisle. The returned short should
 * have the least 6 significant bits set to the id and the 10 most significant
 * bits set to 0.
 *
 * Example: if the section is 0b0110010100011010, return 0b0000000000011001.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_id(unsigned long* aisle, int index) {
  unsigned short id;
  id = get_section(aisle, index);
  id &= ID_MASK;
  id >>= NUM_SPACES;
  return id;
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for section, set the section at the given index of
 * the given aisle to the new bit pattern.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_section(unsigned long* aisle, int index, unsigned short new_section) {
  long mask;
  int shift;
  long section;

  section = new_section;
  mask = SECTION_MASK;
  shift = index * SECTION_SIZE;
  mask <<= shift;
  mask = ~mask;
  section <<= shift;
  *aisle &= mask;
  *aisle |= section;
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for the spaces of the section, set the spaces for the
 * section at the given index of the given aisle to the new bit pattern. The
 * new spaces pattern should be in the 10 least significant bits of the given
 * bit pattern. If the new pattern uses bits outside the 10 least significant
 * bits, then the method should leave the spaces unchanged.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_spaces(unsigned long* aisle, int index, unsigned short new_spaces) {
  unsigned short spaces;
  unsigned short section;

  spaces = new_spaces & ~SPACES_MASK;
  if(spaces == 0){
    section = get_section(aisle, index);
    section >>= NUM_SPACES;
    section <<= NUM_SPACES;
    section = section ^ new_spaces;
    set_section(aisle, index, section);
  }

}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for the id of the section, set the id for the section
 * at the given index of the given aisle to the new bit pattern. The new id
 * pattern should be in the 6 least significant bits of the given bit pattern.
 * If the new pattern uses bits outside the 6 least significant bits, then the
 * method should leave the id unchanged.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_id(unsigned long* aisle, int index, unsigned short new_id) {
  // TODO: implement this method
  unsigned short id;
  unsigned short section;

  id = new_id & ~(ID_MASK >> NUM_SPACES);
  if(id == 0){
    section = get_section(aisle, index);
    new_id <<= NUM_SPACES;
    section <<= ID_SIZE;
    section >>= ID_SIZE;
    section = section ^ new_id;
    set_section(aisle, index, section);
  }
}

/* Given a pointer to an aisle, a section index, and a space index, toggle the
 * item in the given space index of the section at the given section index in
 * the given aisle. Toggling means that if the space was previously empty, it
 * should now be filled with an item, vice-versa.
 *
 * Can assume the section index is a valid index (0-3 inclusive).
 * Can assume the spaces index is a valid index (0-9 inclusive).
 */
void toggle_space(unsigned long* aisle, int index, int space_index) {
  unsigned short spaces_mask;
  unsigned short section;

  section = get_section(aisle, index);
  spaces_mask = SPACES_MASK << space_index;
  spaces_mask <<= (NUM_SPACES - 1 - space_index);
  spaces_mask &= ~ID_MASK;
  spaces_mask >>= (NUM_SPACES - 1 - space_index);
  section = section ^ spaces_mask;
  set_section(aisle, index, section); 
  
}

/* Given a pointer to an aisle and a section index, return the number of items
 * in the section at the given index of the given aisle.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short num_items(unsigned long* aisle, int index) {
  unsigned short section;
  unsigned short count;
  count = 0;
  section = get_section(aisle, index);
  

  for (int i = 0; i < NUM_SPACES; i++) {
    int counting = section & 1;
    count += counting;
    section >>= 1;
  }
  return count;
}

/* Given a pointer to an aisle, a section index, and the desired number of
 * items to add, add at most the given number of items to the section at the
 * given index in the given aisle. Items should be added to the least
 * significant spaces possible. If n is larger than or equal to the number of
 * empty spaces in the section, then the section should appear full after the
 * method finishes.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void add_items(unsigned long* aisle, int index, int n) {
  unsigned short new_section;
  int cap;

  cap = NUM_SPACES - num_items(aisle, index);
  new_section = get_section(aisle, index);

  //Case 1 if n is too big
  if ((int)cap <= n){
    for (int i = 0; i < NUM_SPACES; i++){
      int currItem = new_section & 1;
      if(currItem == 0){
        toggle_space(aisle, index, i);
      }
      new_section >>= 1;
    }
    //Case 2 available spaces in aisle
  } else{
    int i = 0;
    while(n > 0){
      int currItem = new_section & 1;
      if(currItem == 0){
        toggle_space(aisle, index, i);
        n--;
      }
      i++;
      new_section >>= 1;
    } 
  }
}

/* Given a pointer to an aisle, a section index, and the desired number of
 * items to remove, remove at most the given number of items from the section
 * at the given index in the given aisle. Items should be removed from the
 * least significant spaces possible. If n is larger than or equal to the
 * number of items in the section, then the section should appear empty after
 * the method finishes.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void remove_items(unsigned long* aisle, int index, int n) {
  unsigned short new_section;
  int holder;

  holder = num_items(aisle, index);
  new_section = get_section(aisle, index);

  //Case 1 n is too big
  if (n >= (int) holder){
    for (int i = 0; i < NUM_SPACES; i++){
      int currItem = new_section & 1;
      if(currItem == 1){
        toggle_space(aisle, index, i);
      }
      new_section >>= 1;
    }
    //other case
  } else {
    int i = 0;
    while (n > 0) {
      int currItem = new_section & 1;
      if(currItem == 1){
        toggle_space(aisle, index, i);
        n--;
      }
      i++;
      new_section >>= 1;
    }
  }
}

/* Given a pointer to an aisle, a section index, and a number of slots to
 * rotate by, rotate the items in the section at the given index of the given
 * aisle to the left by the given number of slots.
 *
 * Example: if the spaces are 0b0111100001, then rotating left by 2 results
 *          in the spaces     0b1110000101
 *
 * Can assume the index is a valid index (0-3 inclusive).
 * Can NOT assume n < NUM_SPACES (hint: find an equivalent rotation).
 */
void rotate_items_left(unsigned long* aisle, int index, int n) {
  unsigned short new_id;
  unsigned short new_section;
  unsigned short temp;
  //Gets vals and makes a section and a temp section to be able to shift the bits
  new_section = get_section(aisle, index);
  temp = get_section(aisle, index);
  new_id = get_id(aisle, index);
  new_id <<= NUM_SPACES;
  n = n % NUM_SPACES;
  // shifts to the left by n
  new_section <<= n;
  new_section &= SPACES_MASK;
  //shifts the temp bits to the right by the remainder of the 10 - n
  temp &= SPACES_MASK;
  temp >>= (NUM_SPACES - n);
 //Uses xor to set the new section
  new_section = new_section ^ temp;
  new_section = new_section ^ new_id;
  set_section(aisle, index, new_section);

}

/* Given a pointer to an aisle, a section index, and a number of slots to
 * rotate by, rotate the items in the section at the given index of the given
 * aisle to the right by the given number of slots.
 *
 * Example: if the spaces are 0b1000011110, then rotating right by 2 results
 *          in the spaces     0b1010000111
 *
 * Can assume the index is a valid index (0-3 inclusive).
 * Can NOT assume n < NUM_SPACES (hint: find an equivalent rotation).
 */
void rotate_items_right(unsigned long* aisle, int index, int n) {
  unsigned short new_id;
  unsigned short new_section;
  unsigned short temp;
  //Gets vals and makes a section and a temp section to be able to shift the bits
  new_section = get_section(aisle, index);
  temp = get_section(aisle, index);
  new_id = get_id(aisle, index);
  new_id <<= NUM_SPACES;
  n = n % NUM_SPACES;
  // shifts to the right by n
  new_section &= SPACES_MASK;
  new_section >>= n;
  //shifts the temp bits to the left by the remainder of the 10 - n
  temp <<= (NUM_SPACES - n);
  temp &= SPACES_MASK;
 //Uses xor to set the new section
  new_section = new_section ^ temp;
  new_section = new_section ^ new_id;
  set_section(aisle, index, new_section);
}