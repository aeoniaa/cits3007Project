#include <stdio.h>
#include <stdlib.h>

struct bank_account {
  int acct_num;
  char acct_name[20];
  double acct_balance;
};

int main() {
  // a bank_account instance
  struct bank_account account = {123456, "John Doe", 1000.50};

  const char * filename = "bank_account.bin";

  // open binary file for writing
  FILE *ofp = fopen(filename, "wb");

  if (ofp == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // write 'account' to file; there's 1 element to write,
  // which has size 'sizeof(struct bank_account)'.
  size_t els_written = fwrite(&account, sizeof(struct bank_account), 1, ofp);

  if (els_written != 1) {
    perror("Error writing to file");
    fclose(ofp);
    return 1;
  }

  fclose(ofp);

  printf("Bank account struct written to '%s'\n", filename);

  exit(EXIT_SUCCESS);
}
