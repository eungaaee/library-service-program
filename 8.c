#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG

enum file_or_struct_type { CLIENT, BOOK, BORROW, ALL };

FILE *client_fp, *book_fp, *borrow_fp;

void open_file(enum file_or_struct_type target_file) {
    if (target_file == CLIENT) {
        client_fp = fopen("./client.txt", "r");
        if (client_fp == NULL) {
            printf("Error: client.txt not found\n");
            exit(1);
        }
    } else if (target_file == BOOK) {
        book_fp = fopen("./book.txt", "r");
        if (book_fp == NULL) {
            printf("Error: book.txt not found\n");
            exit(1);
        }
    } else if (target_file == BORROW) {
        borrow_fp = fopen("./borrow.txt", "r");
        if (borrow_fp == NULL) {
            printf("Error: borrow.txt not found\n");
            exit(1);
        }
    } else if (target_file == ALL) {
        open_file(CLIENT);
        open_file(BOOK);
        open_file(BORROW);
    }
}

void close_file(enum file_or_struct_type target_file) {
    if (target_file == CLIENT) {
        fclose(client_fp);
    } else if (target_file == BOOK) {
        fclose(book_fp);
    } else if (target_file == BORROW) {
        fclose(borrow_fp);
    } else if (target_file == ALL) {
        close_file(CLIENT);
        close_file(BOOK);
        close_file(BORROW);
    }
}

struct client {
    int id;
    char password[20];
    char name[20];
    char address[50];
    char phone_number[20];
};

struct book {
    int id;
    char name[50];
    char publisher[20];
    char author[20];
    long long int ISBN;
    char location[50];
    char available;
};

struct borrow {
    int client_id;
    int book_id;
    time_t borrow_date;
    time_t return_date;
};

struct client_node {
    struct client data;
    struct client_node *next;
};
struct client_node *client_list_head;

struct book_node {
    struct book data;
    struct book_node *next;
};
struct book_node *book_list_head;

struct borrow_node {
    struct borrow data;
    struct borrow_node *next;
};
struct borrow_node *borrow_list_head;

void push_front(enum file_or_struct_type target_list, void *data_ptr) {
    if (target_list == CLIENT) {
        struct client_node *new_node = (struct client_node *)malloc(sizeof(struct client_node));
        new_node->data = *(struct client *)data_ptr;
        new_node->next = client_list_head;
        client_list_head = new_node;
    } else if (target_list == BOOK) {
        struct book_node *new_node = (struct book_node *)malloc(sizeof(struct book_node));
        new_node->data = *(struct book *)data_ptr;
        new_node->next = book_list_head;
        book_list_head = new_node;
    } else if (target_list == BORROW) {
        struct borrow_node *new_node = (struct borrow_node *)malloc(sizeof(struct borrow_node));
        new_node->data = *(struct borrow *)data_ptr;
        new_node->next = borrow_list_head;
        borrow_list_head = new_node;
    }
}

void initialize_lists() {
    open_file(ALL);

    struct client client_data;
    while (fscanf(client_fp, "%d | %s | %[^|]| %[^|]| %s", &client_data.id, client_data.password, client_data.name, client_data.address, client_data.phone_number) != EOF) {
		client_data.name[strlen(client_data.name) - 1] = '\0'; // 띄어쓰기 제거
		client_data.address[strlen(client_data.address) - 1] = '\0';
        push_front(CLIENT, &client_data);
    }

    struct book book_data;
    while (fscanf(book_fp, "%d | %[^|]| %[^|]| %[^|]| %lld | %[^|]| %c", &book_data.id, book_data.name, book_data.publisher, book_data.author, &book_data.ISBN, book_data.location, &book_data.available) != EOF) {
		book_data.name[strlen(book_data.name) - 1] = '\0';
		book_data.publisher[strlen(book_data.publisher) - 1] = '\0';
		book_data.author[strlen(book_data.author) - 1] = '\0';
		book_data.location[strlen(book_data.location) - 1] = '\0';
		push_front(BOOK, &book_data);
    }

    struct borrow borrow_data;
    while (fscanf(borrow_fp, "%d | %d | %ld | %ld", &borrow_data.client_id, &borrow_data.book_id, &borrow_data.borrow_date, &borrow_data.return_date) != EOF) {
		push_front(BORROW, &borrow_data);
    }

    close_file(ALL);
}

void sort_list(enum file_or_struct_type target_list) {
    if (target_list == CLIENT) {
        for (struct client_node *i = client_list_head; i != NULL; i = i->next) {
            for (struct client_node *j = i->next; j != NULL; j = j->next) {
                if (i->data.id > j->data.id) {
                    struct client temp = i->data;
                    i->data = j->data;
                    j->data = temp;
                }
            }
        }
    } else if (target_list == BOOK) {
        for (struct book_node *i = book_list_head; i != NULL; i = i->next) {
            for (struct book_node *j = i->next; j != NULL; j = j->next) {
                if (i->data.ISBN > j->data.ISBN) {
                    struct book temp = i->data;
                    i->data = j->data;
                    j->data = temp;
                }
            }
        }
    } else if (target_list == ALL) {
		sort_list(CLIENT);
		sort_list(BOOK);
	}
}

#ifdef DEBUG
void print_list(enum file_or_struct_type target_list) {
    if (target_list == CLIENT) {
        for (struct client_node *i = client_list_head; i != NULL; i = i->next) {
            printf("%d | %s | %s | %s | %s\n", i->data.id, i->data.password, i->data.name, i->data.address, i->data.phone_number);
        }
    } else if (target_list == BOOK) {
        for (struct book_node *i = book_list_head; i != NULL; i = i->next) {
            printf("%d | %s | %s | %s | %lld | %s | %c\n", i->data.id, i->data.name, i->data.publisher, i->data.author, i->data.ISBN, i->data.location, i->data.available);
        }
    } else if (target_list == BORROW) {
        for (struct borrow_node *i = borrow_list_head; i != NULL; i = i->next) {
            printf("%d | %d | %ld | %ld\n", i->data.client_id, i->data.book_id, i->data.borrow_date, i->data.return_date);
        }
    } else if (target_list == ALL) {
		print_list(CLIENT);
		putchar('\n');
		print_list(BOOK);
		putchar('\n');
		print_list(BORROW);
		putchar('\n');
	}
}
#endif

int main() {
    initialize_lists();
    sort_list(ALL);
	
#ifdef DEBUG
    print_list(ALL);
#endif

    return 0;
}