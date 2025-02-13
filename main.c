#include "trie.h"
#include <ctype.h>

size_t split_file_words(FILE *file, char ***buf, size_t *buf_size){
    if (*buf_size == 0){
        *buf_size = 256;
        *buf = malloc(sizeof(char) * (*buf_size));
        if (*buf == NULL){
            fprintf(stderr, "unable to allocate memory for buf\n");
            return 0;
        }
    }

    int c;
    size_t buf_len, curr_len, curr_size;

    curr_len = 0, buf_len = 0;
    curr_size = 128;
    char *curr = malloc(sizeof(char) * curr_size);
    if (curr == NULL){
        fprintf(stderr, "unable to allocate memory for curr\n");
        free(buf);
        return 0;
    }

    while((c = fgetc(file)) != EOF){
        if (isspace(c)){
            if (buf_len >= *buf_size){
                (*buf_size) *= 2;
                *buf = realloc(buf, sizeof(char) * (*buf_size));
                if (*buf == NULL){
                    fprintf(stderr, "unable to reallocate memory for buf\n");
                    free(curr);
                    return 0;
                }
            }
            curr[curr_len] = '\0';
            (*buf)[buf_len++] = strdup(curr);
            curr_len = 0;
        }else{
            if (curr_len >= curr_size){
                curr_size *= 2;
                curr = realloc(curr, sizeof(char) * curr_size);
                if (curr == NULL){
                    fprintf(stderr, "unable to reallocate memory for curr\n");
                    free(buf);
                    return 0;
                }
            }
            curr[curr_len++] = c;
        }
    }

    free(curr);
    return buf_len;
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

int spell_check_file(Trie *trie, char *filename){
    FILE *file = fopen(filename, "r");

    Trie *temp;
    char **buf = NULL;
    size_t i, buf_size, buf_len;
    buf_size = 0;
    buf_len = split_file_words(file, &buf, &buf_size);

    fclose(file);
    if (!buf_len){
        fprintf(stderr, "unable to read file %s\n", filename);
        trie_free(trie);
        return 1;
    }

    for (i = 0; i < buf_len; i++){
        temp = trie_search(trie, buf[i], strlen(buf[i]));
        if (temp && temp->word_count){
            printf("%s spelled correctly\n", buf[i]);
            continue;
        }
        printf("%s not spelled correctly\n", buf[i]);
        free(buf[i]);
    }
    free(buf);
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

    char **buf = NULL;
    size_t i, buf_size, buf_len;
    buf_size = 0;
    buf_len = split_file_words(file, &buf, &buf_size);

    fclose(file);
    if (!buf_len){
        fprintf(stderr, "unable to read source %s\n", argv[1]);
        trie_free(trie);
        return 1;
    }

    for (i = 0; i < buf_len; i++){
        trie_add(trie, buf[i], strlen(buf[i]));
        free(buf[i]);
    }
    free(buf);

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
