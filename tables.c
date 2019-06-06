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
- (TODO) Introduce log file
 Version 2.0
TODO: introduce a new  program in C that allows to insert new options to the program.
Use a vector of pointers to strings and introduce a for loop to the menu_options function
that reads that vector.
TODO: introduce a promp parameter version where you can print a table given the name 
----------------------------------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#define DEF_S 32
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
int print_to_screen(matrix M);
int print_to_file(matrix M, char *file_name);
matrix drop_table(matrix M);
int error_handler(int e, char *message);
void update_char_number(char *string, matrix M, int i);
int query(matrix M);

// Client

int main(){
  int choice = 1,check_if_defined = 0;
  FILE *fp; // Used if user decide to save the table
  matrix M;
  char ans; // contains the choice about the question below
  printf("\n\nWelcome to table generator 1.0\n");
  while(choice != 0){
    menu_options();
    scanf("%d",&choice);    
    switch(choice){
      // EXIT
    case 0:
      printf("\nOkay, see you soon!\n\n");
      break;
      
    // CREATE TABLE
    case 1:
      if(check_if_defined == 1){
         
       do{
         scanf("%*[^\n]%*1c");
	printf("Seems that another table was already defined.\n Do you wan to overwrite it? [Y/n]");
	  scanf("%c",&ans);
	  ans = tolower(ans);
       }while(!(ans == 'y' || ans == 'n'));

	if (!(ans != 'y')){ // The user want to drop the previous table
	  M = drop_table(M);  // Free the table
	  M = create_table();
	  check_if_defined = 1;
        }else{ // The user doesn't want to overwrite the table
	  printf("Okay, then I will not do anything");
        }
      }else{      
        M = create_table();
        check_if_defined = 1;
      }
      break;


      // LOAD TABLE
    case 2:
      if(check_if_defined == 1){

	do{
	  scanf("%*[^\n]%*1c");
	  printf("Seems that another table was already defined.\n Do you wan to overwrite it? [Y/n]");
	  scanf("%c",&ans);
	  ans = tolower(ans);
        }while(!(ans == 'y' || ans == 'n' ));

	if (!(ans != 'y')){ // // The user want to drop the previous table
	  M = drop_table(M);  // Free the table 
	  if((M = load_table())) break; // Something went wrong
	  check_if_defined = 1;
        }else // The user doesn't want to overwrite the table
	  printf("Okay, then I will not do anything");
      }else{ // Table not previously defined, proceed to load the matrix from file
	char *file_name = malloc(MAX_HEADER_LENGHT_NAME*sizeof(char));
	printf("Insert filename: ");
        scanf("%s",file_name);
	FILE *fp;
	if(((fp = fopen(file_name,"r"))) == NULL ){
	  printf("Ops, your file was not found.. \n");
	  error_handler(ERROR_FILE_NOT_FOUND,NULL);
	  break;
	}
	
        if((M = load_table(fp)) == NULL) break; // Something went wrong
        check_if_defined = 1;
      }
      break;


      //PRINT TABLE ON FILE
    case 3:
      ans = 'n';
      if(check_if_defined != 1){
        do{
	scanf("%*[^\n]%*1c");
	  printf("Seems that no table was defined.\n Do you wan to create one? [Y/n]");
	  scanf("%c",&ans);
	  ans = tolower(ans);
        }while(!(ans == 'y' || ans == 'n'));

	if (ans == 'y'){ // do something
	  M = create_table();  // Create the matrix
	  check_if_defined = 1;	
	  char *file_name = malloc(MAX_HEADER_LENGHT_NAME*sizeof(char));
	  printf("Insert filename: ");
	  scanf("%s",file_name);
	  if( access(file_name, F_OK) ){
	    // There is another file with that name
	    do{
	      scanf("%*[^\n]%*1c");
	      printf("Seems that there is already another file with that name.\n Do you wan to overwrite it? [Y/n]");
	      scanf("%c",&ans);
	      ans = tolower(ans);
	    }while(!(ans == 'y' || ans == 'n'));

	    if (ans == 'y'){ // do something
	      char *command = malloc(MAX_HEADER_LENGHT_NAME*sizeof(char));
	      command = strcat("rm ",file_name);
	      system(command);
	    }else{
	      printf("Okay, then i will not do anything");
	      break;
	    }
	  }else{
	    char *command = malloc(MAX_HEADER_LENGHT_NAME*sizeof(char));
	    command = strcat("touch ", file_name);
	    system(command);
	  }
	  if(print_to_file(M,file_name))  break; // Something went wrong
        }
	else // The user doesn't want to create a table
	  printf("Okay, then I will not print anything");
        
      }else{ // Table was previously defined
        char *file_name = malloc(MAX_HEADER_LENGHT_NAME*sizeof(char));
        printf("Insert filename: ");
        scanf("%s",file_name);
        if( access(file_name, F_OK) != -1 ){
	    // There is another file with that name
	    do{
	      scanf("%*[^\n]%*1c");
	      printf("Seems that there is already another file with that name.\n Do you wan to overwrite it? [Y/n]");
	      scanf("%c",&ans);
	      ans = tolower(ans);
	    }while(!(ans == 'y' || ans == 'n'));

	    if (ans == 'y'){ // do something
	      char *command;
	      command = strcat("rm ",file_name);
	      system(command);
	    }else{
	      printf("Okay, then i will not do anything");
	      break;
	    }
        }
        if(print_to_file(M,file_name)) break; // Something went wrong
      }
      break;
      

      // PRINT TABLE ON SCREEN
    case 4:
      if(check_if_defined != 1){

	do{
	  scanf("%*[^\n]%*1c");
	  printf("Seems that no table was defined.\n Do you wan to create it? [Y/n]");
	  scanf("%c",&ans);
	  ans = tolower(ans);
        }while(!(ans == 'y' || ans == 'n'));

	if (ans == 'y'){ // do something
	  M = create_table();  // Create the matrix
	  check_if_defined = 1;
        }else // The user doesn't want to create table
	  printf("Okay, then I will not do anything");
        
      }else
	  if(print_to_screen(M)) break; // Something went wrong
      
      break;

      // QUERY THE MATRIX
    case 5:
      if(check_if_defined != 1){

	do{
	  scanf("%*[^\n]%*1c");
	  printf("Seems that no table was defined.\n Do you wan to create it? [Y/n]");
	  scanf("%c",&ans);
	  ans = tolower(ans);
        }while(!(ans == 'y' || ans == 'n'));

	if (ans == 'y'){ // do something
	  M = create_table();  // Create the matrix
	  check_if_defined = 1;
        }else // The user doesn't want to create table
	  printf("Okay, then I will not do anything");
        
      }else
	  if(query(M)) break; // Something went wrong
      
      break;
      

      // DEFAULT: do nothing
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
  printf("\n\nWhat you want to do?\n\n");
  printf("1) Create table.\n");
  printf("2) Load table.\n");
  printf("3) Print table on file.\n");
  printf("4) Print table on screen.\n");
  printf("5) Query. (Alpha)\n");
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
  matrix M;
  if((M = malloc(sizeof *M)) == NULL){
    error_handler(ERROR_ALLOCATION,"Structure allocation failed in function: create_table");
    return NULL;
  }
  
  // Initializing statistical data
  M->nchars = 0;
  M->nnumbers = 0;
 
  int i,j,nc,nr = 0;
  char txtn[DEF_S]; // Char read from input
  // Header fields
  printf("\nInsert how many fields this table will have: ");
  if(scanf("%d",&nc) != 1)
    do{
      scanf("%*[^\n]%*1[\n]"); // Ignore all characters untill /n then ignore /n char
      printf("\n Please insert an integer \n>");    
    }while(!scanf("%d",&nc));
    
 
  if((M->header = malloc(nc*sizeof(char *))) == NULL){
    error_handler(ERROR_ALLOCATION,"Header allocation failed in function: create_table");
    return NULL;
  }

  if((M->max_lenght_column = (int *) malloc(nc*sizeof(int))) == NULL){
    error_handler(ERROR_ALLOCATION,"max_lenght_column allocation failed in function: create_table");
    return NULL;
  }
  M->nc = nc;

  // Defining header fields
  for(i = 0; i < nc; i++){
    printf("\nInsert header name [%d]> ",i+1);
    scanf("%s",txtn);
    M->header[i] = strdup(txtn); // Inserting in the header table
    M->max_lenght_column[i] = strlen(txtn); // Initialization of the vector of length
  }
    
  // Rows definition
  
  printf("\nHow many rows do you want to insert? ");
  if(scanf("%d",&nr) != 1)
    do{
      scanf("%*[^\n]%*1[\n]"); // Ignore all characters untill /n then ignore /n char
      printf("\n Please insert an integer \n>");
    }while(!scanf("%d\n",&nr));
    
  M->nr = nr;
  M->mtx = (char ***) malloc(nc * sizeof(char*)); // Allocating the rows
  
  // Inserting data in the fields
  //Possible not saving because of not allocation to the string
  for(i = 0; i < nr;i++){
    M->mtx[i] = (char **) malloc(nc * sizeof(char *)); //Allocating a matrix that contains elements of  a single row
    for(j = 0; j < nc; j++){
      printf("\nInsert field %s [Row:%d]\n> ",M->header[j],i+1);
      scanf("%s",txtn);
      update_char_number(txtn,M,j);
      M->mtx[i][j] = strdup(txtn);
    }
  }

  return M;
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
1° row: nr nc
next nc rows: header name fields
the next nr*nc rows in the file there will be the fields of the matrix
*/
matrix load_table(FILE *fp){  
  matrix M;

  // Allocating new structure 
  if( ((M = malloc(sizeof *M))) == NULL){
    error_handler(ERROR_ALLOCATION,"Structure allocation failed in function: load_table");
    return NULL;
  }
 
  int i,j,nc,nr;
  char txtn[DEF_S]; // Char read from input

  
  // Header fields
  
  fscanf(fp,"%d %d\n", &nr,&nc); // reading sizes from the first row of the file 

  M->nc = nc;
  M->nr = nr;
  
  if( ((M->header = malloc(nc*sizeof(char *)))) == NULL){
    error_handler(ERROR_ALLOCATION,"Header allocation failed in function: load_table");
    return NULL;
  }

  if( ((M->max_lenght_column = (int *) malloc(nc*sizeof(int)))) == NULL){
    error_handler(ERROR_ALLOCATION,"max_lenght_column allocation failed in function: load_table");
    return NULL;
  }

  // Defining header fields
  for(i = 0; i < nc; i++){
    fscanf(fp,"%s\n",txtn);
    M->header[i] = strdup(txtn); // Inserting in the header table
    M->max_lenght_column[i] = strlen(txtn); // Initialization of the vector of length
  }


  M->mtx = (char ***) malloc(nc * sizeof(char*)); // Allocating the rows
 
  for(i = 0; i < nr;i++){
    M->mtx[i] = (char **) malloc(nc * sizeof(char *)); //Allocating a matrix that contains elements of  a single row
    for(j = 0; j < nc; j++){ // reading single row      
      fscanf(fp,"%s\n",txtn);
      M->mtx[i][j] = strdup(txtn);
    }
  }

  return M;
}


/*
  Function: print_to_screen 
----------------------------------------
 Functions that allows to print the table on the screen
*/
int print_to_screen(matrix M){
  int i,j,h,how_many_spaces,count = 1;
  for(i = 0;i<M->nc;i++) count+=M->max_lenght_column[i]+3; // length table orizontally in chars 

  h = M->max_lenght_column[0] + 2; // how many ─ will print
  
  printf("\n\n ┌");
  for(i = 1,j = 1;i<count-1;i++,h--)
    if(h == 0){ // print a "t" 
      h = M->max_lenght_column[j] + 3; // how many ─ will print again
      printf("┬");
      j++;
    }else 
      printf("─");
 
  printf("┐\n");
  
  // printing header
  for(i = 0; i<M->nc;i++){
    how_many_spaces = M->max_lenght_column[i] - strlen(M->header[i]);
    printf(" │ %s%*.s",M->header[i],how_many_spaces,' ');
    //printf("%*.c",how_many_spaces,' '); //print n. spaces 
  }
  printf(" │\n "); // end of the row 

  // Printing line

  h = M->max_lenght_column[0] + 2; // how many ─ will print

  printf("├");
  for(i = 1,j = 1;i<count-1;i++,h--)
    if(h == 0){ // Print a cross
      h = M->max_lenght_column[j] + 3; // how many ─ will print again
      printf("┼"); 
      j++;
    }else printf("─");
  printf("┤\n");
  
  // printing table
  
  for(i = 0; i<M->nr; i++){
    for(j = 0; j<M->nc; j++){
      how_many_spaces = M->max_lenght_column[j] - strlen(M->mtx[i][j]);
      printf(" │ %s%*.s",M->mtx[i][j],how_many_spaces,' ');
      //printf("%*.c",how_many_spaces,' '); //print n. spaces 
    }
    printf(" │\n"); // end of the row 
  }


  h = M->max_lenght_column[0] + 2; // how many ─ will print

  printf(" └");
  for(i = 1,j = 1;i<count-1;i++,h--)
    if(h == 0){ // print an upside down "t"
      h = M->max_lenght_column[j] + 3; // how many ─ will print again
      printf("┴");
      j++;
    }else printf("─");
  printf("┘\n\n");
  return 0;
}




/*
  Function: print_to_file
----------------------------------------
 Functions that allows to print the table on the screen
*/
int print_to_file(matrix M,char *file_name){
  /*if(fp == NULL){
    error_handler(ERROR_FILE_NOT_FOUND,NULL);
    return -1;
    }*/

  FILE *fp = fopen(file_name,"w");
    
  // Saving sizes on the first row
  fprintf(fp,"%d %d\n",M->nr,M->nc);
  
  int i,j;
  
  // Saving header
  for(i = 0; i<M->nc;i++){
    fprintf(fp,"%s\n",M->header[i]); 
  } 


  // Saving table
  
  for(i = 0; i<M->nr; i++)
    for(j = 0; j<M->nc; j++)
      fprintf(fp,"%s\n",M->mtx[i][j]);
    
  fclose(fp);
  return 0;
}

/*
  Function: drop_table
----------------------------------------
Function that deallocate the matrix/table
TODO: check if not empty for each
*/

matrix drop_table(matrix M){
  int i,j;
  // Deallocating max_lenght_columns
  free(M->max_lenght_column);

  // Deallocating header
  for(i = 0; i< M->nc;i++)
    free(M->header[i]);
  free(M->header);

  // Deallocating matrix
  for (i = 0; i< M->nc; i++){
    for (j = 0; j<M->nr; j++)
      free(M->mtx[i][j]);
    free(M->mtx[i]);
  }
  free(M->mtx);

  // Deallocating the struct
  free(M);

  return NULL;
}


/*
  Function: query
----------------------------------------
Function that allows to query the matrix
*/

int query(matrix M){
  return -1; // Its still alpha tho

  

}
/*
  Function: error_handler
----------------------------------------
Function that print messages of errors specifically for this code
//TODO: implementig other stuffs rather than just printing messages
*/
 int error_handler(int e, char *message){
   if(message == NULL) message = "NaN";
   switch(e){
  case ERROR_FILE_NOT_FOUND:
    printf("\n error 1000: unable to open the file.\n");
    break;
  case ERROR_ALLOCATION:
    printf("\n Unable to allocate memory.\nMessage (if any): %s",message);
    break;
  }
  return 0;
}
