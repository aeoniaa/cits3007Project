
#include <stdio.h>
#include <stdlib.h>

struct bank_account {
  int acct_num;
  char acct_name[20];
  double acct_balance;
};

int main() {
  // a bank_account instance to store the read data
  struct bank_account account;

  const char * filename = "bank_account.bin";

  // open the binary file for reading
  FILE *file = fopen(filename, "rb");

  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // read from the file
  size_t els_read = fread(&account, sizeof(struct bank_account), 1, file);

  if (els_read != 1) {
    perror("Error reading from file");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  fclose(file);

  // display the account information
  printf("Account Number: %d\n", account.acct_num);
  printf("Account Name: %s\n", account.acct_name);
  printf("Account Balance: %.2f\n", account.acct_balance);

  return 0;
}
