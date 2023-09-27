#include <p_and_p.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//TODO: watch lecs for spec discussion
//TODO: Look at week 9 lab, it has project info??
//Submit .c file, valid C11 code
//In struct Character, I will put inventorySize before inventory. (It could be left as is without impacting the code significantly. But it will make your life slightly easier if I put inventorySize first, plus it's better practice and a more logical order.)

int saveItemDetails(const struct ItemDetails* arr, size_t numEls, int fd) {
  return 0;
  //writes struct data into the .dat files in the expected format
  //take structs as input, produce file that matchs the 3.1 spec
  //?required to update the metadata in the file in the save function after appending the structs?
    // can write structs first, or meta data first.
    // look at fseek and lseek functions.
    // when writing a file the functions let you move forwards and backwards arounf the file so that you can write data at any position you like
}

int saveItemDetailsToPath(const struct ItemDetails* arr, size_t numEls, const char* filename) {
  return 0;
  //the same as saveItemDetails but uses filename parameter.
  //not required to implement, but may be useful for testing
}

int loadItemDetails(struct ItemDetails** ptr, size_t* numEls, int fd) {
  return 0;
  //reads data from the .dat files
}

int isValidName(const char *str) {
  return 0;
}

int isValidMultiword(const char *str) {
  return 0;
}

int isValidItemDetails(const struct ItemDetails *id) {
  return 0;
}

int isValidCharacter(const struct Character * c) {
  return 0;
}

int saveCharacters(struct Character *arr, size_t numEls, int fd) {
  return 0;
}

int loadCharacters(struct Character** ptr, size_t* numEls, int fd) {
  return 0;
}

int secureLoad(const char *filepath) {
  return 0;
  //find where to obtain ItemDetails pte and numEls variable in order to pass into playGame()
  //calls playGame()
}

void playGame(struct ItemDetails* ptr, size_t numEls);

