/*the test words and accompanying phonemes are completeley random and
the phonemes are not from the file but just as I thought they'd be
and are only there for the sake of testing (in case you were wondering!)
they are probably inaccurate phonemes as I am quite dyslexic so I apologise
if they are horrendously inaccurate */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "homophones.h"  /*Please ensure to save homophones.h in the
                           same directory*/


void clean_up(mvm* word_to_ph, mvm* ph_to_words){

  mvmcell *current_cell;

  current_cell = word_to_ph -> head;
  while(current_cell != NULL){
    free(current_cell -> key);
    current_cell = current_cell -> next;
  }

  mvm_free(&word_to_ph);
  mvm_free(&ph_to_words);
}

bool get_line(char**line, FILE* file){

  bool success;
  int i;

  *line = (char*) calloc(256, sizeof(char));
  success = fgets(*line, 256, file) != NULL;

  /* Remove newline ('\r', '\n') from the string */
  for(i = 0; (*line)[i] != '\0'; i++){
    if((*line)[i] == '\n' || (*line)[i] == '\r'){
      (*line)[i] = '\0';
    }
  }

  return success;
}

MapLoadResult load_maps(mvm** word_to_ph, mvm** ph_to_words, int n){

  char* single_line;
  char* key;
  char* data;

  FILE* dictionary = fopen("cmudict.txt", "r");
  if(dictionary == NULL){
    return FileNotFound;
  }

  *word_to_ph = mvm_init();
  *ph_to_words = mvm_init();


  while(get_line(&single_line, dictionary)){
    data = split_string(single_line);

    if(data != NULL){
      key = single_line;
      data = last_n_phonemes(data, n);

      if(data != NULL){
        mvm_insert(*word_to_ph, key, data);
        mvm_insert(*ph_to_words, data, key);
      }
    }
  }
  fclose(dictionary);
  return FileLoaded;
}

char* last_n_phonemes(char* data, int n){

  int i;
  int phonemes;

  for(i = 0; data[i] != '\0'; i++){/*nothing to see here, folks*/}

  for(phonemes = 0; phonemes < n; i--){
    if(i < -1){
      return NULL;
      /*If we go past beginning of the string and while
        simply do not have n number of phonemes to return,
        we instead return a NULL pointer as cannot give
        user n number of phonemes. This also rules out seg fault
        as we can't encroach on memory that isn't ours*/
    }
    if(i == -1 || data[i] == ' '){
      phonemes++;
      /* Because space occurs before phoneme that precedes them
         (everything is backwards!), but char at beginning of
         the string does not preceed the phoneme and so need to
         go -1 instead of 0 to account for that phoneme. I
         overcame this by adding condition i=-1 so that we
         could have a phroneme without a preceeding space
         i.e., d⎵o⎵g has three phonemes but we would want to begin
         at d as we cant start at it's non-existent preceeding
         space. This only occurs when a rhyming word is the number
         of phonemes the user wants*/
    }
  }

  return &data[i+2];
  /*pointer to the char in data at i+2 because at end of loop
    we decrement i and then to get to the start of the phonemes,
    need to jump 2 characters --see diagram below :D

                  cat␀c ah0 t␀
                    ↑  ↑
                    i  phonemes
  */

}


char* split_string(char* line){
  char* data;
  int hash_index; /*how many chars into str # is */


  for(hash_index = 0; line[hash_index] != '#'; hash_index++){
      /* no need to store chars again as
         already in array*/
      if(line[hash_index] == '\0'){ /*if we reach the end of the str */
        return NULL;
      }
  }

  line[hash_index] = '\0';  /*set to null character */

  /*data = line + hash_index + 1; (first version, kept for my own reference)*/
  data = &line[hash_index + 1]; /* +1 as char* needs to go 1 beyond where the
                                    hash used to be*/
  return data;
  /*key IS line, as placing null pointer in middle of string
    splits it in half, and first bit (line) is the key :D*/
}

 char** get_rhyming_words(
  mvm* word_to_phonemes,
  mvm* phonemes_to_word,
  char* word,
  int* no_of_rhyming_words
){

  char* phonemes;
  char** rhyming_words;

  phonemes = mvm_search(word_to_phonemes, word);

  rhyming_words = mvm_multisearch(
    phonemes_to_word,
    phonemes,
    no_of_rhyming_words
  );

  return rhyming_words;
}

void test_find_rhymes(void){
  /*2 final phonemes */
  int test_num_rhym_words;
  char** list_of_rhym_words;
  mvm* test_w_t_ph;
  mvm* test_ph_t_w;
  char* test_word = "Christmas";

  /*check to see */
  test_w_t_ph = mvm_init();
  test_ph_t_w = mvm_init();
  mvm_insert(test_w_t_ph, test_word, "AH0 S");
  mvm_insert(test_ph_t_w, "AH0 S", test_word);
  mvm_insert(test_w_t_ph, "witness", "AH0 S");  /*just an example for testing */
  mvm_insert(test_ph_t_w, "AH0 S", "witness");
  mvm_insert(test_w_t_ph, "dreamt", "M T");
  mvm_insert(test_ph_t_w, "M T", "dreamt");

  list_of_rhym_words = get_rhyming_words(
    test_w_t_ph,
    test_ph_t_w,
    test_word,
    &test_num_rhym_words
  );
  assert(test_num_rhym_words == 2);
  /*n.b., map "backwards" as head inserts then pushes along */
  assert(strcmp(list_of_rhym_words[0], "witness") == 0);
  assert(strcmp(list_of_rhym_words[1], test_word) == 0);


}

void test_split_string(void){
  char test_string[256];
  char* test_string_phonemes;

  memcpy(test_string,"Christmas#CH R I S T M AH0 S", 29);
  test_string_phonemes = split_string(test_string);
  assert(test_string_phonemes != NULL); /*to check test is a str*/
  assert(strcmp(test_string, "Christmas") == 0);
  assert(strcmp(test_string_phonemes, "CH R I S T M AH0 S") == 0);

  memcpy(test_string,"#Rudolf", 8);
  test_string_phonemes = split_string(test_string);
  assert(test_string_phonemes != NULL);
  assert(strcmp(test_string, "") == 0);
  assert(strcmp(test_string_phonemes, "Rudolf") == 0);

  memcpy(test_string,"Reindeer#", 10);
  test_string_phonemes = split_string(test_string);
  assert(test_string_phonemes != NULL);
  assert(strcmp(test_string, "Reindeer") == 0);
  assert(strcmp(test_string_phonemes, "") == 0);

  memcpy(test_string,"Noel", 5);
  test_string_phonemes = split_string(test_string);
  assert(test_string_phonemes == NULL); /*check no return string*/
  assert(strcmp(test_string, "Noel") == 0);

}

void test_last_phonemes(void){
  assert(strcmp(last_n_phonemes("C A T", 2), "A T") == 0);
  assert(strcmp(last_n_phonemes("C A T", 3), "C A T") == 0);
  assert(last_n_phonemes("C A T", 4) == NULL);
  assert(strcmp(last_n_phonemes("D AI N O S AW", 5), "AI N O S AW") == 0);
  /*phoneme itself probably very wrong but just testing if function
  truely does just pick up on spaces and can 'club together' Multiple
  characters to one phoneme */
}

void test(void){

  test_find_rhymes();
  test_split_string();
  test_last_phonemes();

}

int main(int argc, char** argv){

  mvm* word_to_ph;
  mvm* ph_to_words;
  int n;
  int cnt;
  int no_of_rhyming_words;
  char** list_of_rhym_words;
  int cnt_rhyme;

  test();

  if(argc < 3 || strcmp(argv[1], "-n") != 0){
    fprintf(stderr, "Unable to run, please ensure you inputted "
                    "correctly. For example: \n"
                    "%s -n [number of phonemes] {word(s)}\n",
                     argv[0]);
    return -1;
  }
  n = atoi(argv[2]);

  if(load_maps(&word_to_ph, &ph_to_words, n) == FileNotFound){
    fprintf(stderr, "Unable to load dictionary\n");
    return -1;
  }

  for(cnt = 3; cnt < argc; cnt++){
    list_of_rhym_words = get_rhyming_words(
                            word_to_ph,
                            ph_to_words,
                            argv[cnt],
                            &no_of_rhyming_words
    );
    printf("%s (%s): ", argv[cnt], mvm_search(word_to_ph, argv[cnt]));
    for(cnt_rhyme = 0; cnt_rhyme < no_of_rhyming_words; cnt_rhyme++){
      printf("%s ", list_of_rhym_words[cnt_rhyme]);
    }
    printf("\n");
    free(list_of_rhym_words);
  }

  clean_up(word_to_ph, ph_to_words);

  return 0;
}
