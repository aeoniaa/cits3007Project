/**
 * @file p_and_p.c
 * @author Brigitte Gredziuk 23460936
 * @date 2023
 * @brief File containing functions for CITS3007 Secure Coding Project.
 * 
 * p_and_p.c contains functions for saveItemDetails, loadItemDetails to serialize and deserialize ItemDetails structs to files,
 * saveCharacter, loadCharacter functions to serialize and deserialize Character structs to files
 * and validation functions for what defines a 'name', 'multiword', and the structs ItemDetails and Character.
 */

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
#include <pwd.h>
#include <grp.h>
#include <assert.h> //TODO: delete before submittion
#include <check.h> //TODO: delete before submittion



/**
 * @brief Serializes an array of @c ItemDetails structs
 * 
 * Takes a pointer to an array of @c ItemDetails structs, the number of structs in the array to be serialized, 
 * and the file descriptor for the serialisation destination. Saves the array in the ItemDetails file format,  
 * starting with a 64-bit unsigned integer @p nmemb indicating number of @c ItemDetails structs serialized. 
 * Before serialization, each @c ItemDetails struct to be added is validated using @c isValidItemDetails() function.
 * If an invalid @c ItemDetails struct is found, the function stops and returns 1 without serialization occurring.
 * @param arr Pointer to a series of @c ItemDetails structs to be saved
 * @param nmemb Number of @c ItemDetails structs held in @p arr to be saved
 * @param fd File descriptor of the file data will be serialized to.
 * @return 1 if error occurs during serialization. On success, returns 0.
 * @note It is up to the caller to ensure the number of ItemDetails structs in @p arr is reflected in @p nmemb, otherwise this function is not guaranteed to work accurately.
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
    if (res != 1) {
      fclose(fp);
      return 1;
    }
  }

  size_t header_written = fwrite(&nmemb, sizeof(nmemb), 1, fp);
  if (header_written != 1) {
    fclose(fp);
  return 1;
  }
  
  if (fseek(fp, sizeof(uint64_t), SEEK_SET) != 0){
    fclose(fp);
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

int saveItemDetailsToPath(const struct ItemDetails* arr, size_t nmemb, const char* filename);

/**
 * @brief Deserialises an array of @c ItemDetails structs.
 * 
 * This function takes a file descriptor @p fd of the file to be deserialized, writes the number of structs deserialized to @p nmemb,
 * and writes the @c ItemDetails structs into @p ptr. On success, this function allocates enough memory to store the number of records
 * contained in the file @p fd. On failure, this memory is not allocated (freed). This memory is allocated to @p ptr
 * The address to the allocated memory is written to @p ptr. From @p fd, the header containing the number of @c ItemDetails structs to be 
 * loaded is written to @p nmemb, and the structs are written to @p ptr. 
 * @param ptr Address of a pointer-to-ItemDetails structwhich points to the memory where the @c ItemDetails structs are written.
 * @param nmemb Address of a size_t containing the number of @c ItemDetails structs loaded from file @p fd.
 * @param fd Pointer to a file of ItemDetails file format.
 * @return 1 if error occurs during deserialization. On success, returns 0.
 * @note The memory allocated to @p ptr in this function is to be freed by the caller.
*/
int loadItemDetails(struct ItemDetails** ptr, size_t* nmemb, int fd) {
  if (read(fd, nmemb, sizeof(uint64_t)) != sizeof(uint64_t)) {
      return 1;
  }

  *ptr = (struct ItemDetails*)malloc(sizeof(struct ItemDetails) * (*nmemb));
  if (*ptr == NULL) {
    free(*ptr);
    return 1;
  }

  if (read(fd, *ptr, sizeof(struct ItemDetails) * (*nmemb)) != sizeof(struct ItemDetails) * (*nmemb)) {
    free(*ptr); 
    return 1;
  }

  for (size_t i = 0; i < *nmemb; i++) {
    if (!isValidItemDetails(&(*ptr)[i])) {
      free(*ptr);
      return 1;
    }
  }

  fsync(fd);
  return 0;
}

/**
 * @brief Checks if string is a valid name field
 * 
 * A valid name has graphical representation, with no other characters permitted including whitespace and control characters.
 * To be valid, a string must be max length DEFAULT_BUFFER_SIZE, with the block containing a NUL terminated string of length at most DEFAULT_BUFFER_SIZE-1.
 * @param str The string to be validated.
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
 * 
 * A valid multi-word field can have a graphical representation, but may also contain space characters so long as the space characters
 * are not at the beginning or end of the string.  * To be valid, a string must be max length DEFAULT_BUFFER_SIZE, with the block 
 * containing a NUL terminated string of length at most DEFAULT_BUFFER_SIZE-1.
 * @param str The string to be validated.
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
 * 
 * A valid ItemDetails struct consists of a 64-bit unsigned integer as the @c itemID, the item's unique identifier; a block of characters size 
 * DEFAULT_BUFFER_SIZE containing a @c name-field as defined in @c isValidName() representing the @c itemName; and a block of characters size 
 * DEFAULT_BUFFER_SIZE containing a @c name-field as defined in @c isValidMultiword() representing the @c itemDesc;
 * To be valid, neither @c itemName nor @c itemDesc should be empty strings.
 * @param id The ItemDetails struct to be validated.
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
 * 
 * A valid Character struct consists of a 64-bit unsigned integer as the @c characterID; an 8-bit unsigned integer as the @c socialClass 
 * specifiying a member of the @c CharacterSocialClass enum; a block of characters size DEFAULT_BUFFER_SIZE containing a @c name-field 
 * as defined in @c isValidName() representing the @c profession; a block of characters size DEFAULT_BUFFER_SIZE containing a @c name-field 
 * as defined in @c isValidMultiword() representing the @c name; and a 64-bit unsigned integer indicating the number of items carried by the 
 * character @c inventorySize less than or equal to @c MAX_ITEMS. The inventory of the character is held in @c inventory array containing 
 * 
 * @c ItemCarried structs. A valid character must contain maximum MAX_ITEMS in total across all items carried quantity; and an amount of 
 * @c ItemCarried structs equal to @c inventorySize. Furthermore, to be valid, neither @c profession nor @c name should be empty strings.
 * @param c The Character struct to be validated
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

/**
 * @brief Serializes an array of @c Character structs
 * 
 * Takes a pointer to an array of @c Character structs, the number of structs in the array to be serialized, 
 * and the file descriptor for the serialisation destination. Saves the array in the Character file format,  
 * starting with a 64-bit unsigned integer @p nmemb indicating number of @c Character structs serialized. 
 * Before serialization, each @c Character struct to be added is validated using @c isValidCharacter() function.
 * If an invalid @c Character struct is found, the function stops and returns 1 without serialization occurring.
 * @param arr Pointer to a series of @c Character structs to be saved
 * @param nmemb Number of @c Character structs held in @p arr to be saved
 * @param fd File descriptor of the file data will be serialized to.
 * @return 1 if error occurs during serialization. On success, returns 0.
 * @note It is up to the caller to ensure the number of Character structs in @p arr is reflected in @p nmemb, otherwise this function is not guaranteed to work accurately.
*/
//FIXME:SEGMENTATION　FAULT
int saveCharacters(struct Character *arr, size_t nmemb, int fd) {
  FILE *fp;

  fp = fdopen(fd, "wb"); 
  if (fp == NULL) {
      fclose(fp);
      return 1;
  }

  printf("1\n");
  if (fwrite(&nmemb, sizeof(nmemb), 1, fp) != 1) {
      fclose(fp);
      return 1;
  }

  printf("2\n");

  for (size_t i = 0; i < nmemb; i++) { 
    //TODO: add validation
    int res = isValidCharacter(&arr[i]);
    printf("a\n");
    if (res != 1) {
      fclose(fp);
      printf("b\n");
      return 1;
    }

    printf("3\n");
  //stack-buffer-overflow
  //   size_t sizeOfCharStruct = sizeof(struct Character); //1208, does not include ItemCarried size
  //   //printf("size of struct ItemCarried: %ld\n", sizeof(struct ItemCarried));
  //   sizeOfCharStruct += (sizeof(struct ItemCarried) * arr[i].inventorySize); //sizeof(struct ItemCarried) = 16
  //   if (fwrite(&arr[i], sizeOfCharStruct, 1, fp) != 1) {
  //       fclose(fp);
  //       return 1;
  //   }
  // }

  //CAUSES STACK BUFFER OVERFLOW
    printf("sizeOfCharStruct: %ld\n", sizeof(struct Character));
    printf("sizeOfItemStruct: %ld\n", sizeof(struct ItemCarried));
    printf("sizeOfInventory: %ld\n", sizeof(struct ItemCarried)*arr[i].inventorySize);
  //   if (fwrite(&arr[i], sizeof(struct Character) + (sizeof(struct ItemCarried)*arr[i].inventorySize), 1, fp) != 1) {
  //     fclose(fp);
  //     return 1;
  //   }
  // }


    //WRITE THE START OF EACH CHARACTER
    if (fwrite(&arr[i], (sizeof(struct Character) - sizeof(arr[i].inventory)), 1, fp) != 1) {
      fclose(fp);
      return 1;
    }

    struct ItemCarried inventoryToWrite[arr[i].inventorySize];
    for (size_t j = 0; j < arr[i].inventorySize; j++) {
      inventoryToWrite[j] = arr[i].inventory[j];
    }
    if (fwrite(&inventoryToWrite, sizeof(inventoryToWrite), 1, fp) != 1) {
      fclose(fp);
      return 1;
    }

  //   if (fwrite(arr[i].inventory, , arr[i].inventorySize, fp) != arr[i].inventorySize) {
  //     fclose(fp);
  //     return 1;
  //   }
  // }
  }

  fflush(fp);
  fclose(fp);
  return 0;
}

/**
 * @brief Deserialises an array of @c Character structs.
 * 
 * This function takes a file descriptor @p fd of the file to be deserialized, writes the number of structs deserialized to @p nmemb,
 * and writes the @c Character structs into @p ptr. On success, this function allocates enough memory to store the number of records
 * contained in the file @p fd. On failure, this memory is not allocated (freed). This memory is allocated to @p ptr
 * The address to the allocated memory is written to @p ptr. From @p fd, the header containing the number of @c Character structs to be 
 * loaded is written to @p nmemb, and the structs are written to @p ptr. 
 * @param ptr Address of a pointer-to-Character struct which points to the memory where the @c Character structs are written.
 * @param nmemb Address of a size_t containing the number of @c Character structs loaded from file @p fd.
 * @param fd Pointer to a file of Character file format.
 * @return 1 if error occurs during deserialization. On success, returns 0.
 * @note The memory allocated to @p ptr in this function is to be freed by the caller.
*/
//FIXME: This function
int loadCharacters(struct Character** ptr, size_t* nmemb, int fd) {
  if (read(fd, nmemb, sizeof(uint64_t)) != sizeof(uint64_t)) {
    return 1;
  }

  //FIXME: Does not allow for variable size of character struct
  *ptr = (struct Character*)malloc(sizeof(struct Character) * (*nmemb));
  if (*ptr == NULL) {
    return 1;
  }

  // Read the characters from the file and store them in the allocated memory
  if (read(fd, *ptr, sizeof(struct Character) * (*nmemb)) != sizeof(struct Character) * (*nmemb)) {
      free(*ptr); 
      return 1;
  }

  fsync(fd);
  return 0;
}

/**
 * @brief Loads the game after checking user holds correct permissions to load the ItemDetails database.
 *  
 * The function checks the running process's permissions to ensure the executable it was launched from was a setUID executable owned by user @c pitchpoltadmin.
 * It attempts to acquire appropriate permissions for opening the ItemDetails database, by setting the effective userID is to the userID of pitchpoltadmin.
 * It opens the @c ItemDetails database to a file descriptor, drops permissions, then loads the database from the specified file and calls the function
 * playGame() using the loaded data and the number of items in the loaded data.
 * @param filepath The file path of the @c ItemDetails database file
 * @return 1 if an error occurs during deserialization, 2 if an error occurs while aquiring or dropping permissions, or 0 otherwise.
 * @note This function assumes it is called after the executable has temporarily dropped privileges
 * @note This function assumes the ruid, rgid, euid and egid have dropped privilege, and the privileged uid and gid of @c pitchpoltadmin are saved in suid and sgid respectively.
 */
int secureLoad(const char *filepath) {
  struct passwd *user_info = getpwuid(geteuid());
  if (user_info == NULL || strcmp(user_info->pw_name, "pitchpoltadmin") != 0) {
    return 2; 
  }

  uid_t ruid, euid, suid;
  if(getresuid(&ruid, &euid, &suid) != 0){
    return 2;
  }
  if(setresuid(-1, suid, euid) != 0) {
    return 2;
  }

  gid_t rgid, egid, sgid;
  if(getresgid(&rgid, &egid, &sgid) != 0){
    return 2;
  }
  if(setresgid(-1, sgid, egid) != 0) {
    return 2;
  }

  int fd = open(filepath, O_RDONLY);
  if (fd == -1) {
    close(fd);
    return 1;
  }    

  if (setgid(getgid()) != 0) {
    return 2; 
  }
  if (setregid(-1, 0) == 0) {
    return 2;
  }

  if (setuid(getuid()) != 0) {
    return 2; 
  }
  if (setreuid(-1, 0) == 0) {
    return 2;
  }

  size_t nmemb = 0;
  struct ItemDetails * loadedItems = NULL;
  
  int res = loadItemDetails(&loadedItems, &nmemb, fd);
  if (res != 0) {
    close(fd);
    return 1;
  }

  close(fd);
  playGame(loadedItems, nmemb);
  free(loadedItems);
  return 0;
}

//TODO: RESET  BEFORE SUBMITTING: void playGame(struct ItemDetails* ptr, size_t nmemb);
void playGame(struct ItemDetails* ptr, size_t nmemb){
  printf("playGame done\n");
}





//TODO: DELETE THESE FUNCTIONS: THEY WERE FOR CHECKING

int open_with_fileno(const char * infile_path) {
  FILE *ifp = fopen(infile_path, "rb");
  //if (ifp == NULL)
    //printf(__FILE__, __LINE__, "couldn't open file");
  

  int fd = fileno(ifp);

  // if (fd == -1)
  //   printf(__FILE__, __LINE__, "couldn't get fd for file");

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


//AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH USE PROVIDED CHARACTER TEST FILES FOR TESTING LOAD CHARACTER
//TODO: REMOVE BEFORE SUBMITTING
//TODO: remove prints before submitting
int main(int argc, char *argv[]){
  printf("hello world\n");

//from check.ts loadItemDetails
  const char * infile_path = "test-data/items002.dat";
  int item001fd = open_with_fileno(infile_path);
  printf("opened file\n");
  
  size_t numItems = 0;
  struct ItemDetails * loaditemsArr = NULL;
  int res = loadItemDetails(&loaditemsArr, &numItems, item001fd);
  if (res != 0) {return 1;}
  printf("numItems (modified by loadItemDetails() ): %ld\n", numItems);
  free(loaditemsArr);


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

    res = saveItemDetails(itemArr, itemArr_size, saveItemfd);
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

//     struct Character arr[] = {
//         {1, MENDICANT, "Profession 1", "Character 1", 2, {{1, 5}, {2, 10}}},
//         {2, LABOURER, "Profession 2", "Character 2", 1, {{3, 3}}},
//         // Add more characters as needed
//     };
// //     struct Character arr[] = { {
// //   .characterID = 1, //64bit
// //   .socialClass = MERCHANT, //8bit
// //   .profession = "inn-keeper", //DEFAULT_BUFFER_SIZE = 512
// //   .name = "Edgar Crawford", //DEFAULT_BUFFER_SIZE = 512
// // //  .inventorySize = 0, //64bit
// // //   .inventory = NULL
// // // } };

// // //FIXME: works with empty inventory
// //   .inventorySize = 1, //64bit
// //     .inventory = {
// //       { .itemID = 200648657395984580, //64bit
// //         .quantity = 1 //64bit
// //       }
// //     }
// //   } 
  
// // };

struct Character arr[] = { {
  .characterID = 1,
  .socialClass = MERCHANT,
  .profession = "inn-keeper",
  .name = "Edgar Crawford",
  .inventorySize = 1,
  .inventory = {
    { .itemID = 200648657395984580,
      .quantity = 1
    }
  }
} };

  printf("size of character arr[0] = %ld\n", sizeof(arr[0]));
  printf("size of character arr[1] = %ld\n", sizeof(arr[1]));

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

  res = slurp_file("tmp.dat", "rb", &Afile_conts, &Afile_size);
  assert(res == 0);


  //FIXME: file size bad
  const size_t Aexpected_size = sizeof(uint64_t) + sizeof(arr[0]);

  //fprintf(stderr, "%s:%d: actual file_size = %zu\n", __FILE__, __LINE__, file_size);

  printf("Afilesize: %ld\n", Afile_size);
  printf("AExpectedfilesize: %ld\n", Aexpected_size);
  //assert(Afile_size == Aexpected_size); //"size of written file should eq expected size"

   // metadata should be `1`
  size_t Aactual_read_metadata = 0;
  memcpy(&Aactual_read_metadata, Afile_conts, sizeof(uint64_t));
  assert(Aactual_read_metadata == nmembSAVECHAR); //"size of written metadata should be as expected");

  // following the metadata should be our struct
  struct Character Aactual_read_item = { 0 };
  memcpy(&Aactual_read_item, Afile_conts + sizeof(uint64_t), sizeof(arr[0]));
  //assert(Aactual_read_item == arr[0]); //"size of written metadata should be as expected"

  assert_characters_are_equal(&Aactual_read_item, &(arr[0]));

  if (Afile_conts != NULL){
    free(Afile_conts);
  }
}
