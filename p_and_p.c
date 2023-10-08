//This is a changed file
//TODO: check when reading a buffer no use of strcpy or strlen as they read past the buffer size DEFAULT_BUFFER_SIZE-1
//could use <read> func from <unistd.h> to read struct from file using fd ssize_tread(intfd, void*buf, size_tcount); -->   structMyStructmyStruct; ssize_tres = read(fd, &myStruct, sizeof(structMyStruct));
//fp to fd --> fdopen and fileno

#include <p_and_p.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    //TODO: ItemDetails Struct --> uint64_t is 8, the buffers are 512,  total 1032bytes
    FILE *fp;

    //open file
    fp = fdopen(fd, "w");
    if (fp == NULL) {
      fclose(fp);
      return 1;
    }

    //write nmemb to file
    uint64_t u64 = nmemb;
    
    size_t header_written = fwrite(&u64, sizeof(u64), 1, fp);
    if (header_written != 1) {
      fclose(fp);
    return 1;
    }
    
    //lseek or fseek to 64bits in. ie after the nmemb
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
  }


int saveItemDetailsToPath(const struct ItemDetails* arr, size_t nmemb, const char* filename) {
  return 0;
  //the same as saveItemDetails but uses filename parameter.
  //not required to implement, but may be useful for testing
    FILE *ofp = fopen(filename, "wb");

  if (ofp == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
}

int loadItemDetails(struct ItemDetails** ptr, size_t* nmemb, int fd) {
  return 0;
  //reads data from the .dat files
    //fread to read info from a struct

    // file open
    //do work
    // //TODO: fflush()
    //fclose(fp);
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

int saveCharacters(struct Character *arr, size_t nmemb, int fd) {
    //Full marks only needs functions that work with known valid files
    // Character struct --> the inventorySize says how many records are in the items carried array
    // When reading from file you need to know how much memory you are reading, its variable depending on inventory size
  
    //TODO: fwrite characters struct
    //TODO: STRUCT NEED TO BE ZEROED OUT FIRST then add in the name. Find a function that takes a value and copies it through the whole array
    
    
    // file open
    //do work
    // //TODO: fflush()
    //fclose(fp);
    return 0;
}

int loadCharacters(struct Character** ptr, size_t* nmemb, int fd) {
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
  //calls playGame()
}

void playGame(struct ItemDetails* ptr, size_t nmemb);

//TODO: REMOVE BEFORE SUBMITTING
int main(int argc, char *argv[]){printf("hello world\n");}
