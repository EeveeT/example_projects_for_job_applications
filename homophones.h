#ifndef HOMOPHONES_HEADER
#define HOMOPHONES_HEADER

#include "mvm.h"

enum MapLoadResult{
  FileLoaded,
  FileNotFound
};
typedef enum MapLoadResult MapLoadResult;
/*In the case of file not loading in */


/* Free's lines stored in the maps and both maps */
void clean_up(mvm* word_to_ph, mvm* ph_to_words);

/*Gets line from the file without the newline*/
bool get_line(char**line, FILE* file);

/*Loads in the data from cmudict.txt into two maps */
MapLoadResult load_maps(mvm** word_to_ph, mvm** ph_to_words, int n);

/*Gives us last n phonemes or returns NULL if less than n phonemes*/
char* last_n_phonemes(char* data, int n);

/*Splits the string and first half stays in input and second half
  is returned. No new allocation is made though leaving two strings
  in the same buffer so don't free the returned pointer!!*/
char* split_string(char* line);

/*Gets the rhyming words and returns a list and number of rhyming
  words through pointer to n*/
char** get_rhyming_words(
  mvm* word_to_phonemes,
  mvm* phonemes_to_word,
  char* word,
  int* no_of_rhyming_words
);

/*Testing funcitons which are silent on success (so hopefully silent!) */
void test_find_rhymes(void);
void test_split_string(void);
void test_last_phonemes(void);
void test(void);

#endif
