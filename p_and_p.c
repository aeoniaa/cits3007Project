//This is a changed file

#include <p_and_p.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * serializes an array of ItemDetails structs
 * ItemDets.dat file format: 64-bit unsigned integer indicating number of ItemDetails structs to follow in the file 
 * followed by multiple ItemDetails struct: itemID(64bit unsigned integer) + itemName(DEFAULT_BUFFER_SIZE 512) + itemDesc(DEFAULT_BUFFER_SIZE 512)
 * \param arr
 * \param numEls
 * \param fd
 * \return 1 if error, 0 otherwise
*/
int saveItemDetails(const struct ItemDetails* arr, size_t numEls, int fd){
    //TODO: DOES THE FD NEED TO BE ZEROED OUT FIRST
    FILE *fp;

    //open file
    fp = fdopen(fd, "w");
    if( fp == NULL ) {
      fclose(fp);
      return 1;
    }

    //write numEls to file
    uint64_t u64 = numEls;
    if(fwrite(&u64, sizeof(u64), 1, fp) != 1){
      fclose(fp);
      return 1;
    }
    
    //lseek or fseek to 64bits in. ie after the numEls
    if (fseek(fp, sizeof(u64), SEEK_SET) != 0){
      fclose(fp);
      return 1;
    }

    //write the structs
    size_t els_written = fwrite(&arr, sizeof(struct ItemDetails), numEls, fp);
    if (els_written != numEls) {
      fclose(fp);
    return 1;
    }

    //fflush()
    

    fclose(fp);
    return 0;
  }


int saveItemDetailsToPath(const struct ItemDetails* arr, size_t numEls, const char* filename) {
  return 0;
  //the same as saveItemDetails but uses filename parameter.
  //not required to implement, but may be useful for testing
    FILE *ofp = fopen(filename, "wb");

  if (ofp == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
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

int main(int argc, char *argv[]){printf("hello world\n");}
