#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN     64
#define MAX_WORDS        10000
#define MAX_STOPWORDS    2000
#define MAX_LINE_PER_WORD 2000
#define READ_BUFFER_SIZE  65536
#define WRITE_BUFFER_SIZE 65536

typedef struct {
    char word[MAX_WORD_LEN];
    int count;
    int lines[MAX_LINE_PER_WORD];
    int line_count;
} IndexEntry;

IndexEntry index_table[MAX_WORDS];
int index_size = 0;

char stopwords[MAX_STOPWORDS][MAX_WORD_LEN];
int stopword_count = 0;

/* ----------------- HÀM TIỆN ÍCH ----------------- */

void to_lower_str(char *s) {
    for (; *s; ++s) {
        *s = (char)tolower((unsigned char)*s);
    }
}

int is_word_char(int c) {
    return isalpha((unsigned char)c);
}

/* Kiểm tra word có trong danh sách stopword không */
int is_stopword(const char *word) {
    for (int i = 0; i < stopword_count; ++i) {
        if (strcmp(stopwords[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Kiểm tra danh từ riêng:
   - Chữ cái đầu là chữ hoa
   - Không đứng ở đầu câu (starts_sentence = 0) */
int is_proper_noun(const char *original, int starts_sentence) {
    if (!original[0]) return 0;

    if (isupper((unsigned char)original[0]) && !starts_sentence) {
        return 1;
    }
    return 0;
}

/* Tìm vị trí word trong index_table (đã sắp xếp từ điển).
   Nếu chưa có, chèn mới, giữ bảng luôn được sắp xếp. */
int find_or_insert_word(const char *word) {
    int left = 0, right = index_size - 1;
    int mid, cmp;

    /* Tìm bằng binary search cho nhanh */
    while (left <= right) {
        mid = (left + right) / 2;
        cmp = strcmp(word, index_table[mid].word);
        if (cmp == 0) {
            return mid;
        } else if (cmp < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    /* Không tìm thấy, chèn tại vị trí 'left' */
    if (index_size >= MAX_WORDS) {
        fprintf(stderr, "Vuot qua so luong tu toi da (%d)\n", MAX_WORDS);
        exit(EXIT_FAILURE);
    }

    /* Dịch các phần tử từ left trở đi sang phải */
    for (int i = index_size; i > left; --i) {
        index_table[i] = index_table[i - 1];
    }

    /* Thêm từ mới */
    strncpy(index_table[left].word, word, MAX_WORD_LEN - 1);
    index_table[left].word[MAX_WORD_LEN - 1] = '\0';
    index_table[left].count = 0;
    index_table[left].line_count = 0;

    index_size++;
    return left;
}

/* Thêm lần xuất hiện của từ ở dòng line_number */
void add_occurrence(const char *word, int line_number) {
    int idx = find_or_insert_word(word);

    index_table[idx].count++;

    /* Kiểm tra xem line_number đã tồn tại trong mảng lines chưa */
    int line_exists = 0;
    for (int i = 0; i < index_table[idx].line_count; ++i) {
        if (index_table[idx].lines[i] == line_number) {
            line_exists = 1;
            break;
        }
    }

    /* Chỉ thêm line_number nếu chưa tồn tại */
    if (!line_exists) {
        if (index_table[idx].line_count >= MAX_LINE_PER_WORD) {
            fprintf(stderr, "Vuot qua so luong dong toi da cho tu '%s'\n", word);
            exit(EXIT_FAILURE);
        }
        index_table[idx].lines[index_table[idx].line_count++] = line_number;
    }
}

/* ----------------- ĐỌC STOPWORDS ----------------- */

void load_stopwords(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Khong mo duoc file stopword");
        exit(EXIT_FAILURE);
    }

    /* Sử dụng buffer lớn hơn để đọc nhanh hơn */
    char *buffer = (char*)malloc(READ_BUFFER_SIZE);
    if (!buffer) {
        fprintf(stderr, "Khong the cap phat bo nho\n");
        exit(EXIT_FAILURE);
    }
    setvbuf(f, buffer, _IOFBF, READ_BUFFER_SIZE);

    char buf[256];
    while (fgets(buf, sizeof(buf), f)) {
        /* Xóa \n, \r ở cuối dòng */
        buf[strcspn(buf, "\r\n")] = '\0';

        /* Bỏ qua dòng rỗng */
        if (buf[0] == '\0') continue;

        if (stopword_count >= MAX_STOPWORDS) {
            fprintf(stderr, "Vuot qua so luong stopword toi da (%d)\n", MAX_STOPWORDS);
            exit(EXIT_FAILURE);
        }

        to_lower_str(buf);
        strncpy(stopwords[stopword_count], buf, MAX_WORD_LEN - 1);
        stopwords[stopword_count][MAX_WORD_LEN - 1] = '\0';
        stopword_count++;
    }

    fclose(f);
    free(buffer);
}

/* ----------------- XỬ LÝ TỪ ĐỌC ĐƯỢC ----------------- */

void process_word(char *word_lower, char *word_original, int starts_sentence, int line_number) {
    if (word_lower[0] == '\0') return;

    /* Loại bỏ danh từ riêng */
    if (is_proper_noun(word_original, starts_sentence)) {
        return;
    }

    /* Loại bỏ stopword */
    if (is_stopword(word_lower)) {
        return;
    }

    /* Thêm vào bảng chỉ mục */
    add_occurrence(word_lower, line_number);
}

/* ----------------- XỬ LÝ VĂN BẢN ----------------- */

void process_text(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Khong mo duoc file van ban");
        exit(EXIT_FAILURE);
    }

    /* Sử dụng buffer lớn để tăng tốc độ đọc file */
    char *buffer = (char*)malloc(READ_BUFFER_SIZE);
    if (!buffer) {
        fprintf(stderr, "Khong the cap phat bo nho\n");
        exit(EXIT_FAILURE);
    }
    setvbuf(f, buffer, _IOFBF, READ_BUFFER_SIZE);

    int c;
    char word_lower[MAX_WORD_LEN];
    char word_original[MAX_WORD_LEN];
    int len = 0;
    int line_number = 1;

    int sentence_start = 1;          /* Đang ở đầu câu? */
    int word_starts_sentence = 0;    /* Cờ cho từ hiện tại */

    while ((c = fgetc(f)) != EOF) {
        if (c == '\r') {
            /* Bỏ qua hoặc có thể xử lý như newline tuỳ ý */
            continue;
        }

        if (is_word_char(c)) {
            /* Bắt đầu 1 từ mới */
            if (len == 0) {
                word_starts_sentence = sentence_start;
                sentence_start = 0;  /* đã có từ, không còn là đầu câu nữa */
            }

            if (len < MAX_WORD_LEN - 1) {
                word_original[len] = (char)c;
                word_lower[len] = (char)tolower((unsigned char)c);
                len++;
            }
            /* Nếu dài quá MAX_WORD_LEN - 1 thì bỏ bớt phần sau */

        } else {
            /* Kết thúc 1 từ (nếu đang có) */
            if (len > 0) {
                word_original[len] = '\0';
                word_lower[len] = '\0';

                process_word(word_lower, word_original, word_starts_sentence, line_number);

                len = 0;
            }

            /* Xử lý ký tự kết thúc: xuống dòng & dấu câu */
            if (c == '\n') {
                line_number++;
            }

            if (c == '.' || c == '?' || c == '!') {
                sentence_start = 1;
            }
        }
    }

    /* Nếu file kết thúc mà vẫn còn 1 từ đang dở */
    if (len > 0) {
        word_original[len] = '\0';
        word_lower[len] = '\0';
        process_word(word_lower, word_original, word_starts_sentence, line_number);
    }

    fclose(f);
    free(buffer);
}

/* ----------------- IN KẾT QUẢ ----------------- */

void print_index(void) {
    FILE *f = fopen("result.txt", "w");
    if (!f) {
        perror("Khong the mo file result.txt de ghi");
        exit(EXIT_FAILURE);
    }

    /* Sử dụng buffer lớn để tăng tốc độ ghi file */
    char *buffer = (char*)malloc(WRITE_BUFFER_SIZE);
    if (!buffer) {
        fprintf(stderr, "Khong the cap phat bo nho\n");
        exit(EXIT_FAILURE);
    }
    setvbuf(f, buffer, _IOFBF, WRITE_BUFFER_SIZE);

    for (int i = 0; i < index_size; ++i) {
        fprintf(f, "%s %d", index_table[i].word, index_table[i].count);
        
        for (int j = 0; j < index_table[i].line_count; ++j) {
            fprintf(f, ",%d", index_table[i].lines[j]);
        }
        
        fprintf(f, "\n");
    }

    fclose(f);
    free(buffer);
    printf("Da ghi ket qua vao file result.txt\n");
}

/* ----------------- MAIN ----------------- */

int main(int argc, char *argv[]) {
    const char *text_file = "vanban.txt";
    const char *stop_file = "stopw.txt";

    if (argc == 3) {
        text_file = argv[1];
        stop_file = argv[2];
    } else {
        printf("Su dung mac dinh: vanban.txt va stopw.txt\n");
        printf("Neu muon chi dinh file: %s <vanban.txt> <stopw.txt>\n\n", argv[0]);
    }

    load_stopwords(stop_file);
    process_text(text_file);
    print_index();

    return 0;
}
