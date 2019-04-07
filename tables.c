/*---------------------------------------------------------------------------------------------
Program that allows to print some data in a human-readable table (like a sql table).
The idea is to use a matrix that contains the data then, when it comes to print that matrix
you'll have the data formatted correctly by calculating the max lenght of a string in a 
column. 
then use that lenght to define the space to dedicate for each field. Version 1.0


The data structure will be:
- A vector of matrix: a vector contains pointers to matrix that represent the single row:
  rows of the matrix represent wich column refers to (based to the header matrix) 
  columns represent the string data
- A pointer that points to the struct that contains everything
- A structure that contains everithing
- (TODO) Second structure that contains statistical data about the table 
- (TODO) Function to insert new fields in the martix
- (TODO) During the allocation of the table, expand it's size by 1/2 of the imput given
         by the user

 Version 2.0
TODO: introduce a new  program in C that allows to insert new options to the program.
Use a vector of pointers to strings and introduce a for loop to the menu_options function
that reads that vector.

TODO: introduce a promp parameter version where you can print a table given the name 

----------------------------------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define MAX_HEADER_LENGHT_NAME 30
#define ERROR_FILE_NOT_FOUND 1000
#define ERROR_ALLOCATION 1001


// Interface
struct mt{
  char **header; // Matrix header fields
  char ***mtx;   // Matrix of pointers to string
  int nc;        // Numbers of rows 
  int nr;        // Number of columns
  // Statistical data
  int nchars;    // Number of chars in the matrix
  int nnumbers;  // Number of numbers in the matrix
  int *max_lenght_column;
};

typedef struct mt *matrix;   // Pointer to the matrix


// Prototipes
void menu_options();
matrix create_table();
matrix load_table();
int print_to_destination(matrix M, FILE *fp);
int free_table(matrix M);
int error_handler(int e);
void update_char_number(char *string, matrix M, int i);


// Client

int main(){
  int choice = 1,check_if_defined = 0;
  FILE *fp; // Used if user decide to save the table
  matrix M;
  char ans = '1'; // contains the choice about the question below
  
  while(choice != '0'){
    menu_options();
    scanf("%d",&choice);    
    switch(choice){

    // CREATE TABLE
    case 1:
      if(check_if_defined){
        printf("Seems that another table was already defined.\
\n Do you wan to overwrite it? [Y/n]");
        scanf("%c",&ans);
        if (!(ans != '1')) // do something
	free_table(M);  // Free the table
	}
      M = create_table();
      check_if_defined = 1;
      break;


      // LOAD TABLE
    case 2:
      if(check_if_defined){
        printf("Seems that another table was already defined.\
\n Do you wan to overwrite it? [Y/n]");
        scanf("%c",&ans);
        if (!(ans != '1')) // do something
	free_table(M);  // Free the table 
      }
      M = load_table();
      check_if_defined = 1;
      break;


      //PRINT TABLE ON SCREEN
    case 3:
      if(check_if_defined){
        printf("Seems that no table was defined.\
\n Do you wan to create one? [Y/n]");
        scanf("%c",&ans);
        if (!(ans != '1')){ // do something
	M = create_table();  // Libera la matrice
	check_if_defined = 1;
	}
      }
      char *file_name;
      printf("Insert filename: ");
      scanf("%s",file_name);
      fp = fopen(file_name,"w");
      print_to_destination(M,fp);
      break;
      

      // PRINT TABLE ON FILE
    case 4:
      print_to_destination(M,stdout);
    default:
      break;
    }
  }
  return 0;
}

/* --------------------------- Implementation ---------------------------- */

/* 
   Function: menu_options
----------------------------------------
Function that prints all the available functionalities
*/
 void menu_options(){
  printf("Welcome to table generator 1.0\n");
  printf("What you want to do?\n\n");
  printf("1) Create table.\n");
  printf("2) Load table.\n");
  printf("3) Print table on screen.\n");
  printf("4) Print table on file.\n");
  printf("0) Exit.\n\n");
  printf(">");
 }


/*
  Function: create_table
----------------------------------------
Function that allows to create a new table and
populate it 

*/

matrix create_table(){
  matrix MI;
  if((MI = malloc(sizeof *MI)) == NULL)
    error_handler(ERROR_ALLOCATION);
  
  
  // Initializing statistical data
  MI->nchars = 0;
  MI->nnumbers = 0;
 
  int i,j,nc,nr;
  char *txtn; // Char read from input
  // Head fields
  printf("\nInsert how many columns this table will have: ");
  scanf("%d",&nc);

  char **h; 
  if((h = malloc(nc*sizeof(char *))) == NULL)
    error_handler(ERROR_ALLOCATION);

  MI->header = h;
  
  if((MI->max_lenght_column = (int *) malloc(nc*sizeof(int))) == NULL)
    error_handler(ERROR_ALLOCATION);
  MI->nc = nc;

  // Defining header fields
  for(i = 0; i < nc; i++){
    printf("\nInsert header name no.%d> ",i);
    scanf("%s",txtn);
    MI->header[i] = strdup(txtn); // Inserting in the header table
    MI->max_lenght_column[i] = strlen(txtn); // Initialization of the vector of length
  }
    
  // Rows definition
  
  printf("\nHow many fields do you want to insert? ");
  scanf("%d",&nr);
  MI->nr = nr;
  MI->mtx = (char ***) malloc(nc * sizeof(char*)); // Allocating the rows
  
  // Inserting data in the fields
  //Possible not saving because of not allocation to the string
  for(i = 0; i < nr;i++){
    MI->mtx[i] = (char **) malloc(nc * sizeof(char *)); //Allocating a matrix that contains elements of  a single row
    for(j = 0; j < nc; j++){
      printf("\nInsert field no.%d [Row:%d]\n> ",j,i);
      scanf("%s",txtn);
      update_char_number(txtn,MI,j);
      MI->mtx[i][j] = malloc(sizeof(char)); // Allocating space for the pointer to string
      MI->mtx[i][j] = strdup(txtn);
    }
  }

  return MI;
}


/*
  Function: update_char_number
----------------------------------------
This function updates statistical data about the table
such as the number of characters or how many numbers there are
*/

void update_char_number(char *string, matrix M, int i){
  int count = 0;
  if(strlen(string) > M->max_lenght_column[i]) // check if it has a longer len 
    M->max_lenght_column[i] = strlen(string);
  for (i = 0; string[i]!='\0';i++){
    // checks if string[i] is a integer
    if(string[i] >= '0' && string[i] <= '9' )
      M->nnumbers++;
    else M->nchars++;
  }
}


/*
  Function: load_table
----------------------------------------
This function load a table from a file

TODO: define format to read from file 
*/
matrix load_table(){

}


/*
  Function: print_to destination 
----------------------------------------
 Functions that allows to print the table on the screen
*/
int print_to_destination(matrix M,FILE *fp){
  if(fp == NULL){
    error_handler(ERROR_FILE_NOT_FOUND);
    return -1;
  }
  int i,j,how_many_spaces;
  // printing header
  for(i = 0; i<M->nc;i++){
    how_many_spaces = M->max_lenght_column[i] - strlen(M->header[i]);
    printf("| %s",M->header[i]);
    printf("%*c",how_many_spaces,' '); //print n. spaces 
  }
  printf("|\n"); // end of the row 


  // printing table
  
  for(i = 0; i<M->nr; i++){
    for(j = 0; j<M->nc; j++){
      how_many_spaces = M->max_lenght_column[i] - strlen(M->mtx[i][j]);
      printf("| %s",M->mtx[i][j]);
      printf("%*c",how_many_spaces,' '); //print n. spaces 
    }
    printf("|\n"); // end of the row 
  }
   
  return 0;
}


/*
  Function: free_table
----------------------------------------
Function that deallocate the matrix/table
*/

int free_table(matrix M){
  int i;
  // Deallocating matrix
  return 0;
}



/*
  Function: error_handler
----------------------------------------
Function that print messages of errors specifically for this code

//TODO: implementig other stuffs rather than just printing messages

*/
int error_handler(int e){
  switch(e){
  case ERROR_FILE_NOT_FOUND:
    printf("\n error 1000: unable to open the file.\n");
    break;
  case ERROR_ALLOCATION:
    printf("\n Unable to allocate memory. \n");
    break;
  }
  return 0;
}
