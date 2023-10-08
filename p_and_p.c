//This is a changed file
//TODO: check when reading a buffer no use of strcpy or strlen as they read past the buffer size DEFAULT_BUFFER_SIZE-1
//could use <read> func from <unistd.h> to read struct from file using fd ssize_tread(intfd, void*buf, size_tcount); -->   structMyStructmyStruct; ssize_tres = read(fd, &myStruct, sizeof(structMyStruct));
//fp to fd --> fdopen and fileno
#define _POSIX_C_SOURCE 200809L

#include <p_and_p.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>


/**
 * serializes an array of ItemDetails structs
 * ItemDets.dat file format: 64-bit unsigned integer indicating number of ItemDetails structs to follow in the file 
 * followed by multiple ItemDetails struct: itemID(64bit unsigned integer) + itemName(DEFAULT_BUFFER_SIZE 512) + itemDesc(DEFAULT_BUFFER_SIZE 512)
 * //TODO: pre-conditions
 * \param arr
 * \param nmemb
 * \param fd
 * \return 1 if error, 0 otherwise
*/
int saveItemDetails(const struct ItemDetails* arr, size_t nmemb, int fd) {
    //ItemDetails Struct --> uint64_t is 8, the buffers are 512,  total 1032bytes
    FILE *fp;

    //open file
    fp = fdopen(fd, "w");
    if (fp == NULL) {
      fclose(fp);
      return 1;
    }

    //write nmemb to file
    //TODO: what is nmemb???? numItems is its original name
    uint64_t u64 = nmemb;
    
    size_t header_written = fwrite(&u64, sizeof(u64), 1, fp);
    if (header_written != 1) {
      fclose(fp);
    return 1;
    }
    
    //lseek or fseek to 64bits in. ie after the nmemb
    //TODO: change sizeof() to 64bits
    if (fseek(fp, sizeof(u64), SEEK_SET) != 0){
      fclose(fp);
      return 1;
    }

    //TODO: memset() STRUCT NEED TO BE ZEROED OUT FIRST -> if itemdetails doesnt have exact known things (ie, theres no pointers)
    //write the structs
    size_t els_written = fwrite(&arr, sizeof(struct ItemDetails), nmemb, fp);
    if (els_written != nmemb) {
      fclose(fp);
    return 1;
    }

    //TODO: fflush() --> check if contains buffered inout or output yet to be fully read or written
    fclose(fp);
    return 0;

    //TODO: incorporate validation functions into this function where applicable. return an error if encounter an invalid struct or file record.
  }


// int saveItemDetailsToPath(const struct ItemDetails* arr, size_t nmemb, const char* filename) {
//   return 0;
//   //the same as saveItemDetails but uses filename parameter.
//   //not required to implement, but may be useful for testing
//     FILE *ofp = fopen(filename, "wb");

//   if (ofp == NULL) {
//     perror("Error opening file");
//     exit(EXIT_FAILURE);
//   }
// }

/**
 * @brief 
 * On success, this function allocates enough memory to store the number of records contained in the file
 * The address to the allocated memory is written to ptr.
 * The memory allocated in this function is to be freed by the caller.
 * All records contained in the file are written into the memory allocated
 * @param ptr the address of a pointer-to-ItemDetails struct, points to the memory with the ItemDetails structs loaded
 * @param nmemb the address of a size_t, the number of items loaded from file
 * @param fd file being deserialized
 * @return 1 if error occurs. No memory will be allocated. On success, return 0.
*/
int loadItemDetails(struct ItemDetails** ptr, size_t* nmemb, int fd) {
  return 0;
    //read header for number of items to load --> write num or records to nmemb
    //allocate memory for amount of items --> pointer = ptr
    //write records to that memory
    //ptr = pointer to that memory 
    //nmemb = number of items loaded

    //fread to read info from a struct

    //file header: Number of items: a 64-bit unsigned integer indicating the number of ItemDetails structs that follow in the file.
    //ItemDetails: 64bit int ID + 512byte Name buffer + 512byte Desc buffer


    // file open
    //do work
    // //TODO: fflush()
    //fclose(fp);

    //TODO: incorporate validation functions into this function where applicable. return an error if encounter an invalid struct or file record.
    //return 1 if error occurs. No memory should be allocated (allocated memory freed)
}

/**
 * @brief Checks if string is a valid name field
 * A valid name has graphical representation, with no other characters permitted.
 * Must be max length DEFAULT_BUFFER_SIZE.
 * @return 1 if valid, 0 otherwise
*/
int isValidName(const char *str) {
  //CHECK REQUIREMENTS empty string is valid as doesn't contain any non graphical characters
  //isgraph(); and only graph
  //no whitespace or control characters
  //length = DEFAULT_BUFFER_SIZE
  //DEFAULT_BUFFER_SIZE-1 content, +1 nul terminator
  //undefined after first nul byte

  if (str == NULL){
    return 0;
  }

  //strnlen returns num bytes excluding terminating \0
  size_t len = strnlen(str, DEFAULT_BUFFER_SIZE); 

  if (len >= DEFAULT_BUFFER_SIZE) {
    return 0;
  }

  for (size_t i = 0; i < len; i++) {
    if (!isgraph(str[i]) || isspace(str[i]) || iscntrl(str[i])) {
      return 0;
    }
  }

  return 1;
}

/**
 * @brief Checks if string is a valid multi-word field
 * @return 1 if valid, 0 otherwise
*/
int isValidMultiword(const char *str) {
  //CHECK REQUIREMENTS empty string is valid
  //isgraph() + space characters
  //first and last characts must not be space
  //length = DEFAULT_BUFFER_SIZE
  //DEFAULT_BUFFER_SIZE-1 content, +1 nul terminator
  //undefined after first nul byte
  if (str == NULL){
    return 0;
  }

  //strnlen returns num bytes excluding terminating \0
  size_t len = strnlen(str, DEFAULT_BUFFER_SIZE); 

  if (len >= DEFAULT_BUFFER_SIZE) {
    return 0;
  }

  for (size_t i = 0; i < len; i++) {
    if (!isgraph(str[i]) && !isspace(str[i])) {
      return 0;
    }
  }

  if (isspace(str[0]) || isspace(str[len - 1])) {
    return 0;
  }

  return 1;
}

/**
 * @brief Checks if an ItemDetails struct is valid
 * @return 1 if valid, 0 otherwise
*/
int isValidItemDetails(const struct ItemDetails *id) {
  //uint64_t itemID
  //name = NUL-terminated string of length at most DEFAULT_BUFFER_SIZE-1 (name field)
  //desc = NUL-terminated string of length at most DEFAULT_BUFFER_SIZE-1 (multi-word field)
  
  //name = name field
  //desc = multi-word field
  //no empty strings
  if (id == NULL) {
    return 0;
  }

  if (!isValidName(id->name)) {
    return 0;
  }

  if (!isValidMultiword(id->desc)) {
    return 0;
  }

  if (id->name[0] == '\0' || id->desc[0] == '\0') {
    return 0;
  }

  return 1;
}

/**
 * @brief Checks if a Character struct is valid
 * @return 1 if valid, 0 otherwise
*/
int isValidCharacter(const struct Character * c) {
  //uint64_t characterID; 
  //enum CharacterSocialClass socialClass; //enum CharacterSocialClass {   MENDICANT,   LABOURER,   MERCHANT,   GENTRY,   ARISTOCRACY };
  //char profession[DEFAULT_BUFFER_SIZE]; -->NUL-terminated string of length at most DEFAULT_BUFFER_SIZE-1. is name field, must not be empty string
  //char name[DEFAULT_BUFFER_SIZE]; --> NUL-terminated string of length at most DEFAULT_BUFFER_SIZE-1. is multi-word field, must not be empty string
  //size_t inventorySize; 64-bit unsigned integer indicating num of items carried by character
  //struct ItemCarried inventory[MAX_ITEMS];
      //what is ItemCarried --> //struct ItemCarried {   uint64_t itemID;   size_t quantity; };
      //itemID = 64bit unsigned int
      //quantity = 64bit unsigned int
      //sum of all 'quantity' fields in the character's *inventory* must not exceed MAX_ITEMS.
      //inventorySize is less than or equal to MAX_ITEMS
  //check number of array elements currently containing valid ItemCarried records is stored in the 'inventorySize' field
  
  //Although the inventory field of each Character struct always contains MAX_ITEMS elements, only the used portion of the inventory (that is, inventorySize many elements) is written to (or read from) a character file.
  
  if (c == NULL) {
    return 0;
  }

  if (c->socialClass < MENDICANT || c->socialClass > ARISTOCRACY) { 
    return 0;
  }

  if (!isValidName(c->profession) || c->profession[0] == '\0') {
    return 0;
  }

  if (!isValidMultiword(c->name) || c->name[0] == '\0') {
    return 0;
  }

  if (c->inventorySize > MAX_ITEMS) {
    return 0;
  }

  size_t totalQuantity = 0;
  for (size_t i = 0; i < c->inventorySize; i++) {
    totalQuantity += c->inventory[i].quantity;
  }
  if (totalQuantity > MAX_ITEMS) {
    return 0;
  }

  return 1;
}

//SHOULD BEHAVE AS ITEM DETAILS DOES
int saveCharacters(struct Character *arr, size_t nmemb, int fd) {
    //Full marks only needs functions that work with known valid files
    // Character struct --> the inventorySize says how many records are in the items carried array
    // When reading from file you need to know how much memory you are reading, its variable depending on inventory size
  
    //TODO: fwrite characters struct
    //TODO: STRUCT NEED TO BE ZEROED OUT FIRST then add in the name. Find a function that takes a value and copies it through the whole array
    
    //Header = 64-bit unsigned int indicating number of CHaracter structs stored in the file, and total number of character to be loaded
    //characterId = A 64-bit, unsigned integer value representing the unique identifier of the character.
    //socialClass: An 8-bit, unsigned integer representing the character’s social class. Each value (from 0 to 4) specifies one of the enumerated members of the CharacterSocialClass enum.
    //profession: A block of characters of length DEFAULT_BUFFER_SIZE, containing the character’s profession. This is a name field.
    //name: A block of characters of length DEFAULT_BUFFER_SIZE, containing the character’s name. This is a multi-word field.
    //inventorySize:  indicating the number of items carried by the character. anything beyonf is not considered part of the inventory
    //structs amount = inventorySize
      //itemID: A 64-bit, unsigned integer value representing the unique identifier of the item class.
      //quantity: A 64-bit, unsigned integer value indicating the quantity of the item carried by the character.



    // file open
    //do work
    // //TODO: fflush()
    //fclose(fp);
    return 0;
    //TODO: validate using isValidCharacter()
}

//SHOULD BEHAVE AS ITEM DETAILS DOES
int loadCharacters(struct Character** ptr, size_t* nmemb, int fd) {
    //Size of the Character array: A 64-bit, unsigned integer value indicating the number of Character structs stored in the file. This is the total number of characters to be loaded.

     // file open
    //do work
    // //TODO: fflush()
    //fclose(fp);
    return 0;
}

int secureLoad(const char *filepath) {
  return 0;
  //find where to obtain ItemDetails pte and nmemb variable in order to pass into playGame()
  //setuid: secure coding cookbook, labs
  //calls playGame() --> //TODO: make stub with empty body in a test_utils.c
  /**
   * Forum post explaining functions
   * 5.2 of the paper link - use the easy, powerful one?
   * If calling non-standard funcs, invoked in right way -> lab explains how to invoke them in the right way, otherwise moodle will fail
   * 
  The specification doesn't say whether the pitchpoltadmin account userID is zero, 
  so you'll have to make a reasonable assumption about this (which you should document in your submitted code). 
  Given what has been discussed in lectures and labs, would zero be a sensible choice for the pitchpoltadmin account?
  
  */
  //10 marks --> look at project spec!!!!!!
}

void playGame(struct ItemDetails* ptr, size_t nmemb);

//TODO: REMOVE BEFORE SUBMITTING
int main(int argc, char *argv[]){printf("hello world\n");}
