#include <stdio.h>
#include <stdlib.h>
#include "mvm.h"
#include <string.h>
#include <assert.h>

#define MAXLEN (3000)


/*to initialise the map N.B., not the cells within map */
mvm* mvm_init(void)
{
  mvm* map;
  map = (mvm*) calloc(1, sizeof(mvm));

  if(map == NULL){
    ON_ERROR("Space allocation failed. Oups! \n");
  }

  map -> head = NULL;
  map -> numkeys = 0;

  return map;
}

int mvm_size(mvm* m){

  if(m == NULL){
    return 0;
  }

  return m -> numkeys;
}

/* Insert one key/value pair */
void mvm_insert(mvm* m, char* key, char* data){


  mvmcell *cell = (mvmcell*) calloc(1, sizeof(mvmcell));
  if(m == NULL || key == NULL || data == NULL){
    free(cell); /*to avoid memory leaks of empty cells */
    return;
  }

  cell -> key = key;
  cell -> data = data;
  cell -> next = NULL;

  if(m -> head == NULL){
    m -> head = cell;
  }
  else{
    cell -> next = m -> head;
    m -> head = cell;
  }

  m -> numkeys++;

}

/* Store list as a string "[key](value) [key](value) " etc.  */
char* mvm_print(mvm* m){

  mvmcell *p;
  char *str;
  char *strtmp;

  if(m == NULL){
    return NULL;
  }

  str = (char*) calloc(MAXLEN, sizeof(char));

  p = m -> head;

  while(p != NULL){
    strtmp = (char*) calloc(MAXLEN, sizeof(char));

    sprintf(strtmp, "[%s](%s) ", p -> key, p -> data);
    strcat(str, strtmp);

    free(strtmp);
    p = p -> next;
  }


  return str;
}
/* Remove one key/value */
void mvm_delete(mvm* m, char* key){

  typedef mvmcell* arrow;

  arrow *arrow_to_curr; /*will point to a pointer */
  mvmcell *mvmcurr;

  if(m == NULL || key == NULL){
    return;
  }

  arrow_to_curr = &(m -> head); /*to make arrow_to_curr
                                  a pointer to the 'first'
                                  pointer */
  mvmcurr = m -> head; /* initialise the array*/

  while(mvmcurr != NULL){
    if(strcmp(mvmcurr -> key, key) == 0){
      *arrow_to_curr = mvmcurr -> next; /* balance out the pointers*/
      free(mvmcurr);
      m -> numkeys--;
      return; /*to ONLY remove ONE key - otherwise
                all others with the same key would
                be removed*/
    }
    arrow_to_curr = &(mvmcurr -> next);
    mvmcurr = mvmcurr -> next;
  }
}

/* Return the corresponding value for a key */
char* mvm_search(mvm* m, char* key){

  mvmcell *p;

  if(m == NULL || key == NULL){
    return NULL;
  }

  p = m -> head;

  while(p != NULL){
    if(strcmp(key, p -> key) == 0){
      return p -> data;
    }
    p = p -> next;
  }
  return NULL;
}

/* Return *argv[] list of pointers to all values
stored with key, n is the number of values */
char** mvm_multisearch(mvm* m, char* key, int* n){

  mvmcell *curr_cell;
  char **output_p = NULL;

  *n = 0; /*to dereference n to make it an int type
            (as opposed to int* type) */
  if(m == NULL || key == NULL){
    return NULL;
  }

  curr_cell = m -> head;
  while(curr_cell != NULL){
    if(strcmp(curr_cell -> key, key) == 0){
      *n += 1; /*increase size then next allocate space for it */
      output_p = realloc(output_p, sizeof(char*)*(*n)); /* N.B, in
      brackets to distuinguigh pointer star and multiplication*/

      if(output_p == NULL){
        *n = 0;
        return NULL;
      }

      output_p[*n-1] = curr_cell -> data;
      /* *n-1 as indexing starts at 0 */
    }
    curr_cell = curr_cell -> next;
  }


  return output_p;
}

/* Free & set p to NULL */
void mvm_free(mvm** p){

  mvmcell *current;
  mvmcell *next;

  next = (**p).head; /*to double de-reference both the pointers */

  free(*p); /*to dereference p and free
              p,thus making it an mvm
              pointer which we can free
              as matches the mvm struct */

  *p = NULL;
  while(next != NULL){
    current = next;
    next = current -> next; /*next cell on from the current one*/

    free(current);
  }
}
