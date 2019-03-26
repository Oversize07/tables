/*---------------------------------------------------------------------------------------------
Program that allows to print some data in a human-readable table (like a sql table).
The idea is to use a matrix that contains the data then, when it comes to print that matrix
you'll have the data formatted correctly by calculating the max lenght of a string in a column 
then use that lenght to define the space to dedicate for each field. Version 1.0


The data structure will be:
- A pointer that points to the struct that contains everything
- A structure that contains everithing
- (TODO) Second structure that contains statistical data about the table 


 Version 2.0
TODO: introduce a new  program in C that allows to insert new options to the program.
Use a vector of pointers to strings and introduce a for loop to the menu_options function
that reads that vector.

TODO: introduce a promp parameter version where you can print a table given the name 

----------------------------------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAX_HEADER_LENGHT_NAME 30
#define FILE_NOT_FOUND 1000



// Interface
struct mt{
  char **header; // MAtrix header fields
  char **mtx;    // Matrix
  int nc;        // Numbers of rows 
  int nr;        // Number of columns
  // Statistical data
  int nchars;    // Number of chars in the matrix
  int nnumbers;  // Number of numbers in the matrix
};

typedef struct mt *matrix;   // Pointer to the matrix


// Prototipes
void menu_options();
int create_table();
int print_to_destination(matrix M, FILE *fp);
int max_length_row(char **mtx, int rl);
int free_table(matrix M);
int error_handler(int e);



// Client

int main(){
  int choice,check_if_defined = 0;
  menu_options();
  scanf("%d",&choice);
  while(1){
    switch(choice){

    // CREATE TABLE
    case 1:
      if(check_if_defined){
        char a = 1; // contains the choice about the question below
        printf("Seems that another table was already defined.\
\n Do you wan to overwrite it? [Y/n]");
        scanf("%c",a);
        if !(a != 1) // do something
	free_table(M);  // Free the table
      }
      matrix M = create_table(&M);
      check_if_defined = 1;
      break;


      // LOAD TABLE
    case 2:
      if(check_if_defined){
        char a = 1; // contains the choice about the question below
        printf("Seems that another table was already defined.\
\n Do you wan to overwrite it? [Y/n]");
        scanf("%c",a);
        if !(a != 1) // do something
	free_table(M);  // Free the table 
      }
      matrix M = load_table(M);
      check_if_defined = 1;
      break;


      //PRINT TABLE ON SCREEN
    case 3:
      if(check_if_defined){
        char a = 1; // contains the choice about the question below
        printf("Seems that no table was defined.\
\n Do you wan to create one? [Y/n]");
        scanf("%c",a);
        if !(a != 1){ // do something
	matrix M = create_table(M);  // Libera la matrice
	check_if_defined = 1;
	}
      }
      char *file_name;
      FILE fp;
      pritnf("Insert filename: ");
      scanf("%s",file_name);
      fp = fopen(file_name,"w");
      print_to_destination(M,fp);
      break;
      

      // PRINT TABLE ON FILE
    case 4:
      FILE *fp = fopen(stdout,"w");
      print_to_destination(M,fp);
    default:
      goto exit;
    }
  }
 exit:
  return 0;
}

/* --------------------------- Implementation ---------------------------- */

// Function that prints all the available functionalities
void menu_options(){
  printf("Welcome to table generator 1.0\n");
  printf("What you want to do?\n\n");
  pritnf("1) Create table.\n");
  printf("2) Load table.");
  printf("3) Print table on screen.\n");
  printf("4) Print table on file.");
  printf("0) Exit.\n\n");
}


// Functions thta allows to create a table
// TODO: fill the table
matrix create_table(){
  matrix M = malloc(sizeof M);
  int i,nc;
  char *txtn; // Char read from input
  // Head fields
  printf("\nInsert how many columns this table will have: ");
  scanf("%d",&nc);

  M->header = (char **) malloc(nc * sizeof(char*));
  M->nc = nc;
  
  for(i = 0; i < nc; i++){
    printf("\nInserisci nome header no.%d> ",i);
    scanf("%s",txtn);
    M->header[i] = strdup(txtn); // Inserting in the header table
  }
    
  // Rows definition
  
  // Inserting data in the fields

  return M;
}


// Function that find the length of the longest elements in a vector of strings
int max_length_row(char **mtx, int rl){
  int i, longest_length;

  for(i = 0; i<rl; i++){
    if(strlen(mtx[i])>longest_length)
      longest_length = strlen(mtx[i]);
   }
  return longest_length;

}// Done

// Functions that allows to print the table on the screen
int print_to_destination(matrix M,FILE *fp){
  if(fp == NULL){
    error_handler(FILE_NOT_FOUND);
    return -1;
  }
  int longest_length = max_length_row(M->mtx,M->nr);

  
  // printf("%c",how_many," "); print n. spaces
  return 0;
}

int free_table(matrix M){
  int i;
  // Deallocating matrix
  return 0;
}

int error_handler(int e){
  switch(e){
  case FILE_NOT_FOUND:
    printf("\n error 1000: unable to open the file.\n");
    return 0;
  }

}
