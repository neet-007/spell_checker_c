#include "trie.h"
#include <ctype.h>

int split_file_words_apply(FILE *file, Trie *trie, int apply(Trie *trie, char *value, size_t value_size)){
    int c;
    size_t buf_len, curr_len, curr_size;

    curr_len = 0, buf_len = 0;
    curr_size = 128;
    char *curr = malloc(sizeof(char) * curr_size);
    if (curr == NULL){
        fprintf(stderr, "unable to allocate memory for curr\n");
        return 0;
    }

    while((c = fgetc(file)) != EOF){
        if (isspace(c)){
            curr[curr_len] = '\0';
            apply(trie, curr, curr_len);
            curr_len = 0;
        }else{
            if (curr_len >= curr_size){
                curr_size *= 2;
                curr = realloc(curr, sizeof(char) * curr_size);
                if (curr == NULL){
                    fprintf(stderr, "unable to reallocate memory for curr\n");
                    return 0;
                }
            }
            curr[curr_len++] = c;
        }
    }

    free(curr);

    return 1;
}

int spell_check_loop(Trie *trie){
    char buf[256];
    Trie *temp;
    size_t i;

    while (fgets(buf, sizeof(buf) - 1, stdin)){
        i = 0;
        while (buf[i] != '\n' && buf[i] != '\0'){
            i++;
        }

        if (buf[i] == '\n'){
            buf[i] = '\0';
        }

        temp = trie_search(trie, buf, strlen(buf));
        if (temp && temp->word_count){
            printf("%s spelled correctly\n", buf);
            continue;
        }
        printf("%s not spelled correctly\n", buf);
    }

    trie_free(trie);
    return 0;
}

int spell_check_file_apply(Trie *trie, char *value, size_t value_size){
    Trie *temp = trie_search(trie, value, value_size);
    if (temp && temp->word_count){
        printf("%s spelled correctly\n", value);
        return 1;
    }

    printf("%s not spelled correctly\n", value);
    return 0;
}

int spell_check_file(Trie *trie, char *filename){
    FILE *file = fopen(filename, "r");
    if (file == NULL){
        fprintf(stderr, "unable to read file %s\n", filename);
        trie_free(trie);
        return 0;
    }

    int res = split_file_words_apply(file, trie, spell_check_file_apply);
    fclose(file);
    if (!res){
        fprintf(stderr, "unable to add words file %s", filename);
        trie_free(trie);
        return 1;
    }

    trie_free(trie);

    return 0;
}

int main(int argc, char *argv[]){
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: spell_checker <source> [spellcheck]\n");
        fprintf(stderr, "Check spelling in a text file.\n\n");
        fprintf(stderr, "Arguments:\n");
        fprintf(stderr, "  <source>     Required: Input text file for dictionary (default: system dictionary)\n");
        fprintf(stderr, "  [spellcheck] Optional: Input text file to spell check\n");

        return 0;
    }

    Trie *trie;
    trie = trie_build("", 1);
    if (trie == NULL){
        fprintf(stderr, "unable to initialize trie %s", get_trie_package_error_message());
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL){
        fprintf(stderr, "unable to open file %s", argv[1]);
        trie_free(trie);
        return 1;
    }

    int res = split_file_words_apply(file, trie, trie_add);
    fclose(file);
    if (!res){
        fprintf(stderr, "unable to add words file %s", argv[1]);
        trie_free(trie);
        return 1;
    }

    if (argc == 2){
        return spell_check_loop(trie);
    }

    if (argc == 3){
        return spell_check_file(trie, argv[2]);
    }

    trie_free(trie);

    fprintf(stderr, "Usage: spell_checker <source> [spellcheck]\n");
    fprintf(stderr, "Check spelling in a text file.\n\n");
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "  <source>     Required: Input text file for dictionary (default: system dictionary)\n");
    fprintf(stderr, "  [spellcheck] Optional: Input text file to spell check\n");

    return 1;
}
