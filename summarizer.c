#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Word {
    char* text;
    int count;
    struct Word* next;
} Word;

typedef struct Sentence {
    char* text;
    int score;
    int index;
} Sentence;

// Read entire stdin
char* read_input() {
    size_t size = 1024;
    size_t len = 0;
    char* buffer = malloc(size);
    if (!buffer) return NULL;

    int c;
    while ((c = getchar()) != EOF) {
        if (len + 1 >= size) {
            size *= 2;
            char* new_buffer = realloc(buffer, size);
            if (!new_buffer) {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
        buffer[len++] = (char)c;
    }
    buffer[len] = '\0';
    return buffer;
}

void normalize_word(char* word) {
    int i = 0, j = 0;
    while (word[i]) {
        char c = tolower(word[i]);
        if (isalnum(c)) {
            word[j++] = c;
        }
        i++;
    }
    word[j] = '\0';
}

void add_word(Word** head, char* text) {
    Word* current = *head;
    while (current != NULL) {
        if (strcmp(current->text, text) == 0) {
            current->count++;
            return;
        }
        current = current->next;
    }
    Word* new_word = (Word*)malloc(sizeof(Word));
    new_word->text = strdup(text);
    new_word->count = 1;
    new_word->next = *head;
    *head = new_word;
}

int get_word_count(Word* head, char* word) {
    Word* current = head;
    while (current != NULL) {
        if (strcmp(current->text, word) == 0) {
            return current->count;
        }
        current = current->next;
    }
    return 0;
}

char** split_into_words(char* sentence, int* word_count) {
    char* copy = strdup(sentence);
    if (!copy) return NULL;

    char* token;
    char** words = NULL;
    int capacity = 0;
    int size = 0;
    const char* delimiters = " \t\n\r";

    token = strtok(copy, delimiters);
    while (token != NULL) {
        char* word = strdup(token);
        normalize_word(word);

        if (strlen(word) > 0) {
            if (size >= capacity) {
                capacity = (capacity == 0) ? 4 : capacity * 2;
                words = realloc(words, capacity * sizeof(char*));
            }
            words[size++] = word;
        } else {
            free(word);
        }

        token = strtok(NULL, delimiters);
    }

    free(copy);
    *word_count = size;
    return words;
}

Sentence* split_into_sentences(char* text, int* sentence_count) {
    char* copy = strdup(text);
    if (!copy) return NULL;

    char* token;
    Sentence* sentences = NULL;
    int capacity = 0;
    int size = 0;
    const char* delimiters = ".!?";

    token = strtok(copy, delimiters);
    while (token != NULL) {
        while (isspace(*token)) token++;
        char* end = token + strlen(token) - 1;
        while (end > token && isspace(*end)) end--;
        *(end + 1) = '\0';

        if (strlen(token) == 0) {
            token = strtok(NULL, delimiters);
            continue;
        }

        if (size >= capacity) {
            capacity = (capacity == 0) ? 4 : capacity * 2;
            sentences = realloc(sentences, capacity * sizeof(Sentence));
        }

        sentences[size].text = strdup(token);
        sentences[size].score = 0;
        sentences[size].index = size;
        size++;

        token = strtok(NULL, delimiters);
    }

    free(copy);
    *sentence_count = size;
    return sentences;
}

int compare_scores(const void* a, const void* b) {
    const Sentence* s1 = (const Sentence*)a;
    const Sentence* s2 = (const Sentence*)b;
    return s2->score - s1->score;
}

int compare_index(const void* a, const void* b) {
    const Sentence* s1 = (const Sentence*)a;
    const Sentence* s2 = (const Sentence*)b;
    return s1->index - s2->index;
}

void free_words(Word* head) {
    while (head != NULL) {
        Word* temp = head;
        head = head->next;
        free(temp->text);
        free(temp);
    }
}

void free_sentences(Sentence* sentences, int count) {
    for (int i = 0; i < count; i++) {
        free(sentences[i].text);
    }
    free(sentences);
}

int main() {
    char* input = read_input();
    if (!input || strlen(input) == 0) {
        printf("No input provided.\n");
        free(input);
        return 1;
    }

    int num_sentences;
    Sentence* sentences = split_into_sentences(input, &num_sentences);
    if (num_sentences == 0) {
        printf("No sentences found.\n");
        free(input);
        return 0;
    }

    Word* word_frequencies = NULL;

    for (int i = 0; i < num_sentences; i++) {
        int num_words;
        char** words = split_into_words(sentences[i].text, &num_words);
        for (int j = 0; j < num_words; j++) {
            add_word(&word_frequencies, words[j]);
            free(words[j]);
        }
        free(words);
    }

    for (int i = 0; i < num_sentences; i++) {
        int num_words;
        char** words = split_into_words(sentences[i].text, &num_words);
        for (int j = 0; j < num_words; j++) {
            sentences[i].score += get_word_count(word_frequencies, words[j]);
            free(words[j]);
        }
        free(words);
    }

    qsort(sentences, num_sentences, sizeof(Sentence), compare_scores);

    int top_n = num_sentences * 0.3;
    if (top_n < 1) top_n = 1;

    qsort(sentences, top_n, sizeof(Sentence), compare_index);

    for (int i = 0; i < top_n; i++) {
        printf("%s. ", sentences[i].text);
    }
    printf("\n");

    free(input);
    free_sentences(sentences, num_sentences);
    free_words(word_frequencies);

    return 0;
}
