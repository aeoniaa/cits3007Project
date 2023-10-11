//TODO: remove prints before submitting
//TODO: no use of strcpy or strlen as they read past the buffer size DEFAULT_BUFFER_SIZE-1
//could use <read> func from <unistd.h> to read struct from file using fd ssize_tread(intfd, void*buf, size_tcount); -->   structMyStructmyStruct; ssize_tres = read(fd, &myStruct, sizeof(structMyStruct));
//fp to fd --> fdopen and fileno
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include "test_utils.h"
#include <p_and_p.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <assert.h>
#include <check.h>



/**
 * serializes an array of ItemDetails structs
 * ItemDets.dat file format: 64-bit unsigned integer indicating number of ItemDetails structs to follow in the file 
 * followed by multiple ItemDetails struct: itemID(64bit unsigned integer) + itemName(DEFAULT_BUFFER_SIZE 512) + itemDesc(DEFAULT_BUFFER_SIZE 512)
 * validates each ItemDetails struct attempted to be written is valid. If not, stop, close file and return 1 (error)
 * //TODO: pre-conditions
 * \param arr
 * \param nmemb
 * \param fd
 * \return 1 if error, 0 otherwise
*/
int saveItemDetails(const struct ItemDetails* arr, size_t nmemb, int fd) {
  FILE *fp;

  fp = fdopen(fd, "w");
  if (fp == NULL) {
    fclose(fp);
    return 1;
  }

  for (size_t i = 0; i < nmemb; i++) {
    int res = isValidItemDetails(&arr[i]);
    //printf("Item details %s is valid\n", arr[i].name);
    if (res != 1) {
      fclose(fp);
      //printf("Error: invlaid item details detected");
      return 1;
    }
  //   printf("Item %zu:\n", i + 1);
  //   printf("\tItem ID: %lu\n", arr[i].itemID);
  //   printf("\tName: %s\n", arr[i].name);
  //   printf("\tDescription: %s\n", arr[i].desc);
  }

  size_t header_written = fwrite(&nmemb, sizeof(nmemb), 1, fp);
  if (header_written != 1) {
    fclose(fp);
  return 1;
  }
  
  if (fseek(fp, sizeof(uint64_t), SEEK_SET) != 0){
    fclose(fp);
    //printf("a\n");
    return 1;
  }

  size_t els_written = 0;
  els_written = fwrite(arr, sizeof(struct ItemDetails), nmemb, fp);
  if (els_written != nmemb) {
    fclose(fp);
  return 1;
  }

  fflush(fp);
  fclose(fp);
  return 0;
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
 * Does not close fd. 
 * @param ptr the address of a pointer-to-ItemDetails struct, points to the memory with the ItemDetails structs loaded
 * @param nmemb the address of a size_t, the number of items loaded from file
 * @param fd file being deserialized
 * @return 1 if error occurs. No memory will be allocated. On success, return 0.
*/
int loadItemDetails(struct ItemDetails** ptr, size_t* nmemb, int fd) {
    //read header for number of items to load --> write num or records to nmemb
    //allocate memory for amount of items
    //write records to that memory
    //ptr = pointer to that memory 
    //nmemb = number of itemDetail structs to be loaded

    //fread() to read info from a struct

    //file header: Number of items: a 64-bit unsigned integer indicating the number of ItemDetails structs that follow in the file.
    //ItemDetails: 64bit int ID + 512byte Name buffer + 512byte Desc buffer

    // //TODO: fflush()
    //fclose(fp);

    
    //return 1 if error occurs. No memory should be allocated (allocated memory freed)


    // Read the number of records (nmemb) from the file header
    if (read(fd, nmemb, sizeof(uint64_t)) != sizeof(uint64_t)) {
        perror("Failed to read the header"); //TODO: get rid of perror(). do dif error handling
        return 1;
    }

    // Allocate memory for the records
    *ptr = (struct ItemDetails*)malloc(sizeof(struct ItemDetails) * (*nmemb));

    if (*ptr == NULL) {
        perror("Memory allocation failed"); //TODO: get rid of perror(). do dif error handling
        return 1;
    }

    //TODO: possibly need to fseek here to 64bits in.
    // Read the records from the file and store them in the allocated memory
    if (read(fd, *ptr, sizeof(struct ItemDetails) * (*nmemb)) != sizeof(struct ItemDetails) * (*nmemb)) {
        perror("Failed to read item details"); //TODO: get rid of perror(). do dif error handling
        free(*ptr); // Free the allocated memory in case of an error
        return 1;
    }

    fsync(fd); // no called correctly //TODO:  error handling
    //close(fd); //""
    return 0; // Success
    //TODO: incorporate validation functions into this function where applicable. return an error if encounter an invalid struct or file record.

}

/**
 * @brief Checks if string is a valid name field
 * A valid name has graphical representation, with no other characters permitted.
 * Must be max length DEFAULT_BUFFER_SIZE.
 * @return 1 if valid, 0 otherwise
*/
int isValidName(const char *str) {
  
  if (str == NULL){
    return 0;
  }

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
  if (str == NULL){
    return 0;
  }

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
  if (id == NULL) {
    return 0;
  }

  if (!isValidName(id->name) || id->name[0] == '\0') {
    return 0;
  }

  if (!isValidMultiword(id->desc) || id->desc[0] == '\0') {
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

//SHOULD BEHAVE AS ITEM DETAILS   DOES
//FIXME:SEGMENTATION　FAULT
int saveCharacters(struct Character *arr, size_t nmemb, int fd) {
    //Full marks only needs functions that work with known valid files
    // Character struct --> the inventorySize says how many records are in the items carried array
    // When reading from file you need to know how much memory you are reading, its variable depending on inventory size
  
    //TODO: fwrite characters struct
    
    //Header = 64-bit unsigned int indicating number of Character structs stored in the file, and total number of character to be loaded
    //characterId = A 64-bit, unsigned integer value representing the unique identifier of the character.
    //socialClass: An 8-bit, unsigned integer representing the character’s social class. Each value (from 0 to 4) specifies one of the enumerated members of the CharacterSocialClass enum.
    //profession: A block of characters of length DEFAULT_BUFFER_SIZE, containing the character’s profession. This is a name field.
    //name: A block of characters of length DEFAULT_BUFFER_SIZE, containing the character’s name. This is a multi-word field.
    //inventorySize:  indicating the number of items carried by the character. anything beyonf is not considered part of the inventory
    //structs amount = inventorySize
      //itemID: A 64-bit, unsigned integer value representing the unique identifier of the item class.
      //quantity: A 64-bit, unsigned integer value indicating the quantity of the item carried by the character.



  FILE *fp;

  fp = fdopen(fd, "wb"); // Open in binary write mode
  if (fp == NULL) {
      fclose(fp);
      return 1;
  }

  // Write the header: 64-bit unsigned integer indicating the number of Character structs
  if (fwrite(&nmemb, sizeof(nmemb), 1, fp) != 1) {
      fclose(fp);
      return 1;
  }

  // Write each Character struct and its associated ItemCarried data
  for (size_t i = 0; i < nmemb; i++) { 
    int res = isValidCharacter(&arr[i]);
    //printf("Item details %s is valid\n", arr[i].name);
    if (res != 1) {
      fclose(fp);
      return 1;
    }

    size_t sizeOfCharStruct = 1208;
    sizeOfCharStruct += (sizeof(struct ItemCarried) * arr[i].inventorySize);
      // Write the Character struct
    if (fwrite(&arr[i], sizeOfCharStruct, 1, fp) != 1) {
        fclose(fp);
        return 1;
    }
  }

  //     // Write the associated ItemCarried data based on inventorySize
  //   if (fwrite(arr[i].inventory, sizeof(struct ItemCarried), arr[i].inventorySize, fp) != arr[i].inventorySize) {
  //       fclose(fp);
  //       return 1;
  //   }
  // }

  fflush(fp);
  fclose(fp);
  return 0;

}
// //PREVIOUSVERSION
//   FILE *fp;
//
//   fp = fdopen(fd, "w");
//   if (fp == NULL) {
//     //printf("Error opening fd as fp\n");
//     fclose(fp);
//     return 1;
//   }
// //printf("aaaaaaaaaaaaaaaa\n");
//   //TODO: 
//
//   //Tfind char struct size of each char struct. using inventory size * number of items in inventory
//   //add to runningSizeCountOfAllCharacterStructs
//
//   size_t header_written = fwrite(&nmemb, sizeof(nmemb), 1, fp);
//   if (header_written != 1) {
//     //printf("Failed to write header\n");
//     fclose(fp);
//   return 1;
//   }
//   // if (header_written ==1 ) {
//   //   //printf("header written worked\n");
//   // }
//
//
//  // printf("dddddddddddddddddddddd\n");
//   //FIXME: this is where the seg fault occurs
//   int res = fseek(fp, sizeof(uint64_t), SEEK_SET);
//   if (res != 0){
//     fclose(fp);
//     printf("fseek failed --> closing fp \n");
//     return 1;
//   }
// //printf("eeeeeeeeeeeeeeeeeeeeeeeeeeee\n");
//   size_t sizeOfArr = 0;
//   size_t characters_written = 0;
// for (size_t i = 0; i < nmemb; i++) {
//     //FIXME:
//     // printf("abababa\n");
//     // int res = isValidCharacter(&arr[i]);
//     // printf("Character %s is valid\n", arr[i].name);
//     // if (res != 1) {
//     //   fclose(fp);
//     //   printf("Error: invlaid character detected");
//     //   return 1;
//     // }
//     //printf("bbbbbbbbbbbbbbbbb\n");
//     //FIXME: THIS IS THE PROBLEM: Size of character array incoming not calculated properly
//     size_t charStructSize = sizeof(uint64_t) + sizeof(uint8_t) + DEFAULT_BUFFER_SIZE + DEFAULT_BUFFER_SIZE + sizeof(uint64_t);
//     charStructSize += arr[i].inventorySize * (sizeof(uint64_t) * 2);
//     sizeOfArr += charStructSize;
//     printf("Item %zu:\n", i + 1);
//     printf("\tCharacter ID: %ld\n", arr[i].characterID);
//     printf("\tSocialClass: %d\n", arr[i].socialClass);
//     printf("\tProfession: %s\n", arr[i].profession);
//     printf("\tName: %s\n", arr[i].name);
//     printf("\tInventorySize: %ld\n", arr[i].inventorySize);
//
//     printf("size of char struct: %ld\n", charStructSize);
//     size_t els_written = 0;
//     els_written = fwrite(arr, charStructSize, 1, fp);
//     if (els_written != 1) {
//       fclose(fp);
//       printf("els_written failed --> closing fp \n");
//     return 1;
//     }
//     characters_written += 1;
//   }
//   printf("size of char struct: %ld\n", sizeOfArr);
//   if (characters_written != nmemb) {
//       fclose(fp);
//       printf("characters_written failed , wrote %ld, instead of %ld characters--> closing fp \n", characters_written, nmemb);
//     return 1;
//   }
//   //printf("ccccccccccccccc\n");
// 
//   //TODO: NEED TO WRITE EACH MEMBER INDIVIDUALLY ACCORDING TO ITS SIZE
//   // els_written = fwrite(arr, sizeOfArr, nmemb, fp);
//   // if (els_written != nmemb) {
//   //   fclose(fp);
//   //   printf("els_written failed --> closing fp \n");
//   // return 1;
//   // }
// //printf("ffffffffffffffffffffffffffffff\n");
//   fflush(fp);
// //printf("ggggggggggggggggggggggggggg\n");
//   fclose(fp);
// printf("hhhhhhhhhhhhhhhhhhh\n");
//   return 0;


//TODO: validation

//SHOULD BEHAVE AS ITEM DETAILS DOES
//FIXME: NO TESTS AVAILABLE
int loadCharacters(struct Character** ptr, size_t* nmemb, int fd) {
    //Size of the Character array: A 64-bit, unsigned integer value indicating the number of Character structs stored in the file. This is the total number of characters to be loaded.    
    // Read the number of characters (nmemb) from the file header
    if (read(fd, nmemb, sizeof(uint64_t)) != sizeof(uint64_t)) {
        perror("Failed to read the header");
        return 1;
    }

    //FIXME: PROBLEM:
    // Allocate memory for the characters
    *ptr = (struct Character*)malloc(sizeof(struct Character) * (*nmemb));
    
    // Allocate memory for the records
    //*ptr = (struct ItemDetails*)malloc(sizeof(struct ItemDetails) * (*nmemb));

    if (*ptr == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    // Read the characters from the file and store them in the allocated memory
    if (read(fd, *ptr, sizeof(struct Character) * (*nmemb)) != sizeof(struct Character) * (*nmemb)) {
        perror("Failed to read character details");
        free(*ptr); // Free the allocated memory in case of an error
        return 1;
    }

    return 0; // Success
    // //TODO: fflush()
    //fclose(fp);
}
    //TODO: possibly need to fseek here to 64bits in.


//FIXME: DO THIS!!!
int secureLoad(const char *filepath) {
  printf("%s\n", filepath);
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

//TODO: DELETE THESE FUNCTIONS: THEY WERE FOR CHECKING

int open_with_fileno(const char * infile_path) {
  FILE *ifp = fopen(infile_path, "rb");
  if (ifp == NULL)
    printf(__FILE__, __LINE__, "couldn't open file");
  

  int fd = fileno(ifp);

  if (fd == -1)
    printf(__FILE__, __LINE__, "couldn't get fd for file");

  return fd;
}

// check whether two ItemDetails structs are equal. returns 1 if they are,
// 0 otherwise.
void assert_itemDetails_are_equal(const struct ItemDetails *id1, const struct ItemDetails *id2) {
  assert(id1->itemID == id2->itemID); //, "ItemID for id1 and id2 should be equal");
  int res = strcmp(id1->name, id2->name);
  if (res != 0) printf("id1->name != id2->name");
  res = strcmp(id1->desc, id2->desc);
  //if (res == 0) printf("id1->desc, id2->desc");
}

void assert_characters_are_equal(const struct Character *id1, const struct Character *id2) {
  assert(id1->characterID == id2->characterID); //, "ItemID for id1 and id2 should be equal");
  int res = strcmp(id1->name, id2->name);
  if (res != 0) printf("id1->name !=  id2->name");
  assert(id1->inventorySize == id2->inventorySize);
  //if (res == 0) printf("id1->desc, id2->desc");
  printf("assertCharactersEqual passed");
}

// read the contents of `filename` into malloc'd memory.
// the caller should free `*file_conts`.
int slurp_file(
  const char * filename, const char *mode, char **file_conts, size_t *file_size) {
    FILE *file = fopen(filename, mode);
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_sizel = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_sizel == -1) {
        perror("Error getting file size");
        fclose(file);
        return 1;
    }

    *file_size = (size_t) file_sizel;

    // memory for contents
    assert(file_conts != NULL);
    *file_conts = malloc( (size_t) file_sizel);

    char * file_conts_ = *file_conts;

    if (file_conts_ == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return 1;
    }

    // Read the file contents into the array
    size_t bytes_read = fread(file_conts_, 1, (size_t) file_sizel, file);

    if (bytes_read != (size_t) file_sizel) {
        perror("Error reading file");
        free(file_conts_);
        fclose(file);
        return 1;
    }

    fclose(file);

    return 0;
}


//TODO: REMOVE BEFORE SUBMITTING
int main(int argc, char *argv[]){
  printf("hello world\n");

//from check.ts loadItemDetails
  // const char * infile_path = "test-data/items001.dat";
  // int item001fd = open_with_fileno(infile_path);
  // printf("opened file\n");
  //
  //
  // size_t numItems = 0;
  // struct ItemDetails * itemsArr = NULL;
  // int res = loadItemDetails(&itemsArr, &numItems, item001fd);
  //
  // printf("res of loadItemDetails: %d\n", res);
  // printf("numItems (modified by loadItemDetails() ): %ld\n", numItems);

//my loadItemDetails
    // // Sample usage of loadItemDetails
    // int fd = open("your_file.dat", O_RDONLY); // Open the file for reading
    // if (fd == -1) {
    //     perror("Failed to open the file");
    //     return 1;
    // }
    // struct ItemDetails* loadedItems = NULL;
    // size_t numItems = 0;
    // 
    // if (loadItemDetails(&loadedItems, &numItems, fd) != 0) {
    //     fprintf(stderr, "Error: Failed to load item details\n");
    //     return 1;
    // }
    // if (loadItemDetails(&loadedItems, &numItems,
    // // Now you have the loadedItems array with numItems elements
    //
    // // Remember to free the allocated memory when you're done with it
    // free(loadedItems);
    //
    // return 0;


  //SAVE ITEM DETAILS CHECKS
  struct ItemDetails itemArr[] = {
      { .itemID = 16602759796824695000UL, .name = "telescope",      .desc = "brass with wooden tripod, 25x30x60 in." }
    };
    size_t itemArr_size = sizeof(itemArr)/sizeof(struct ItemDetails);

    char* file_conts = NULL;
    size_t file_size = 0;

    FILE *ofp = fopen("tmp.dat", "wb");
    assert(ofp != NULL);
    int saveItemfd = fileno(ofp);
    assert(saveItemfd != -1);

    int res = saveItemDetails(itemArr, itemArr_size, saveItemfd);
    assert(res == 0);
    fclose(ofp);

    res = slurp_file("tmp.dat", "rb", &file_conts, &file_size);
    assert(res == 0);

    const size_t expected_size = sizeof(uint64_t) + sizeof(struct ItemDetails);

    //fprintf(stderr, "%s:%d: actual file_size = %zu\n", __FILE__, __LINE__, file_size);
    assert(file_size == expected_size); //"size of written file should eq expected size"

    // metadata should be `1`
    size_t actual_read_metadata = 0;
    memcpy(&actual_read_metadata, file_conts, sizeof(size_t));
    assert(actual_read_metadata == itemArr_size); //"size of written metadata should be as expected");

    // following the metadata should be our struct
    struct ItemDetails actual_read_item = { 0 };
    memcpy(&actual_read_item, file_conts + sizeof(size_t), sizeof(struct ItemDetails));
    assert(actual_read_metadata == itemArr_size); //"size of written metadata should be as expected"

    assert_itemDetails_are_equal(&actual_read_item, &(itemArr[0]));

    if (file_conts != NULL)
      free(file_conts);





//   //SAVECHARACTER

    // struct Character arr[] = {
    //     {1, MENDICANT, "Profession 1", "Character 1", 2, {{1, 5}, {2, 10}}},
    //     {2, LABOURER, "Profession 2", "Character 2", 1, {{3, 3}}},
    //     // Add more characters as needed
    // };
    struct Character arr[] = { {
  .characterID = 1, //64bit
  .socialClass = MERCHANT, //8bit
  .profession = "inn-keeper", //DEFAULT_BUFFER_SIZE = 512
  .name = "Edgar Crawford", //DEFAULT_BUFFER_SIZE = 512
  .inventorySize = 0, //64bit
//   .inventory = NULL
// } };

//FIXME: works with empty inventory
.inventorySize = 1, //64bit
  .inventory = {
    { .itemID = 200648657395984580, //64bit
      .quantity = 1 //64bit
    }
  }
} };

    size_t nmembSAVECHAR = sizeof(arr) / sizeof(arr[0]);

  char* Afile_conts = NULL;
  size_t Afile_size = 0;

  FILE *Aofp = fopen("tmp.dat", "wb");
  assert(Aofp != NULL);
  int saveCharfd = fileno(Aofp);
  assert(saveCharfd != -1);

if (saveCharacters(arr, nmembSAVECHAR, saveCharfd) != 0) {
        fprintf(stderr, "Error: Failed to save characters\n");
        return 1;
    }
    printf("passed save character\n");

  fclose(Aofp);

  printf("a\n");

  res = slurp_file("tmp.dat", "rb", &Afile_conts, &Afile_size);
  assert(res == 0);
  printf("b\n");


  //FIXME: file size bad
  const size_t Aexpected_size = sizeof(uint64_t) + sizeof(arr[0]);

  //fprintf(stderr, "%s:%d: actual file_size = %zu\n", __FILE__, __LINE__, file_size);
  printf("size of arr[] = %ld\n", sizeof(arr[0]));
  printf("Afilesize: %ld\n", Afile_size);
  printf("AExpectedfilesize: %ld\n", Aexpected_size);
  assert(Afile_size == Aexpected_size); //"size of written file should eq expected size"


  //CAN I PRINT OUT FILE_CONTS???
printf("c\n");
   // metadata should be `1`
  size_t Aactual_read_metadata = 0;
  memcpy(&Aactual_read_metadata, Afile_conts, sizeof(uint64_t));
  assert(Aactual_read_metadata == nmembSAVECHAR); //"size of written metadata should be as expected");

  // following the metadata should be our struct
  struct Character Aactual_read_item = { 0 };
  memcpy(&Aactual_read_item, Afile_conts + sizeof(uint64_t), sizeof(arr[0]));
  //assert(Aactual_read_item == arr[0]); //"size of written metadata should be as expected"

  printf("d\n");

  assert_characters_are_equal(&Aactual_read_item, &(arr[0]));

  printf("e\n");
  if (Afile_conts != NULL){
    free(Afile_conts);
  }


//FROM CHATGPT TESTING
    // // Create an array of Character structures
    // struct Character characters[2];

    // // Initialize character data
    // characters[0].characterID = 1;
    // characters[0].socialClass = MERCHANT;
    // strcpy(characters[0].profession, "Merchant");
    // strcpy(characters[0].name, "John Doe");
    // characters[0].inventorySize = 2;
    // characters[0].inventory[0].itemID = 101;
    // characters[0].inventory[0].quantity = 3;
    // characters[0].inventory[1].itemID = 102;
    // characters[0].inventory[1].quantity = 5;

    // characters[1].characterID = 2;
    // characters[1].socialClass = LABOURER;
    // strcpy(characters[1].profession, "Labourer");
    // strcpy(characters[1].name, "Jane Smith");
    // characters[1].inventorySize = 1;
    // characters[1].inventory[0].itemID = 201;
    // characters[1].inventory[0].quantity = 8;

    // // Save the Character data to a binary file
    // FILE *fp = fopen("tmp.dat", "wb");
    // if (fp == NULL) {
    //     perror("Failed to create file");
    //     return 1;
    // }

    // if (saveCharacters(characters, 2, fileno(fp)) != 0) {
    //     perror("Failed to save characters");
    //     return 1;
    // }

    // close(fp);

    // // Read the data back from the file to verify correctness
    // struct Character loadedCharacters[2];
    // fp = fopen("tmp.dat", "r");
    // if (fp == NULL) {
    //     perror("Failed to open file for reading");
    //     return 1;
    // }

    // if (read(fd, loadedCharacters, sizeof(loadedCharacters)) != sizeof(loadedCharacters)) {
    //     perror("Failed to read characters from file");
    //     return 1;
    // }

    // close(fd);

    // // Compare the loaded data with the original data
    // if (memcmp(characters, loadedCharacters, sizeof(characters)) == 0) {
    //     printf("Character data successfully saved and loaded.\n");
    // } else {
    //     printf("Character data mismatch after loading.\n");
    // }

    // return 0;
}
