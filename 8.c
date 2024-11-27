#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

enum e_file_or_struct_type { CLIENT, BOOK, BORROW, ALL_FILE_OR_STRUCT };

enum e_search_book_by { ID, NAME, PUBLISHER, AUTHOR, ISBN, LOCATION, AVAILABLE, SEARCH_ALL };
enum e_data_type { INT, LONGLONG, STRING };
enum e_search_book_mode { STANDARD_INFO, BORROW_INFO };

FILE *client_fp, *book_fp, *borrow_fp;

void open_file(enum e_file_or_struct_type target_file, char *mode) {
    if (target_file == CLIENT) {
        client_fp = fopen("./client.txt", mode);
        if (client_fp == NULL) {
            printf("Error: client.txt not found\n");
            exit(1);
        }
    } else if (target_file == BOOK) {
        book_fp = fopen("./book.txt", mode);
        if (book_fp == NULL) {
            printf("Error: book.txt not found\n");
            exit(1);
        }
    } else if (target_file == BORROW) {
        borrow_fp = fopen("./borrow.txt", mode);
        if (borrow_fp == NULL) {
            printf("Error: borrow.txt not found\n");
            exit(1);
        }
    } else if (target_file == ALL_FILE_OR_STRUCT) {
        open_file(CLIENT, mode);
        open_file(BOOK, mode);
        open_file(BORROW, mode);
    }
}

void close_file(enum e_file_or_struct_type target_file) {
    if (target_file == CLIENT) {
        fclose(client_fp);
    } else if (target_file == BOOK) {
        fclose(book_fp);
    } else if (target_file == BORROW) {
        fclose(borrow_fp);
    } else if (target_file == ALL_FILE_OR_STRUCT) {
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
    long long ISBN;
    char location[50];
    char available;
};

struct borrow {
    int user_id;
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

void push_front(enum e_file_or_struct_type target_list, void *data_ptr) {
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

void erase(enum e_file_or_struct_type target_list, void *prev_node) {
	if (target_list == CLIENT) {
		struct client_node *target_node; // 삭제할 노드

		if (prev_node == NULL) { // 맨 앞 노드를 삭제하는 경우
			target_node = client_list_head;
			client_list_head = client_list_head->next;
		} else {
			target_node = ((struct client_node *)prev_node)->next;
			((struct client_node *)prev_node)->next = target_node->next;
		}

		free(target_node); // 삭제하는 노드의 메모리 해제
	} else if (target_list == BOOK) {
		struct book_node *target_node;

		if (prev_node == NULL) {
			target_node = book_list_head;
			book_list_head = book_list_head->next;
		} else {
			target_node = ((struct book_node *)prev_node)->next;
			((struct book_node *)prev_node)->next = target_node->next;
		}

		free(target_node);
	} else if (target_list == BORROW) {
		struct borrow_node *target_node;

		if (prev_node == NULL) {
			target_node = borrow_list_head;
			borrow_list_head = borrow_list_head->next;
		} else {
			target_node = ((struct borrow_node *)prev_node)->next;
			((struct borrow_node *)prev_node)->next = target_node->next;
		}

		free(target_node);
	}
}

void initialize_lists() {
    open_file(ALL_FILE_OR_STRUCT, "r");

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
    while (fscanf(borrow_fp, "%d | %d | %ld | %ld", &borrow_data.user_id, &borrow_data.book_id, &borrow_data.borrow_date, &borrow_data.return_date) != EOF) {
		push_front(BORROW, &borrow_data);
    }

    close_file(ALL_FILE_OR_STRUCT);
}

void sort_list(enum e_file_or_struct_type target_list) {
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
    } else if (target_list == ALL_FILE_OR_STRUCT) {
		sort_list(CLIENT);
		sort_list(BOOK);
	}
}

#ifdef DEBUG
void print_list(enum e_file_or_struct_type target_list) {
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
            printf("%d | %d | %ld | %ld\n", i->data.user_id, i->data.book_id, i->data.borrow_date, i->data.return_date);
        }
    } else if (target_list == ALL_FILE_OR_STRUCT) {
		print_list(CLIENT);
		putchar('\n');
		print_list(BOOK);
		putchar('\n');
		print_list(BORROW);
		putchar('\n');
	}
}
#endif

void sync_file(enum e_file_or_struct_type target_file) {
	open_file(target_file, "w");

	if (target_file == CLIENT) {
		for (struct client_node *i = client_list_head; i != NULL; i = i->next) {
			fprintf(client_fp, "%d | %s | %s | %s | %s\n", i->data.id, i->data.password, i->data.name, i->data.address, i->data.phone_number);
		}
	} else if (target_file == BOOK) {
		for (struct book_node *i = book_list_head; i != NULL; i = i->next) {
			fprintf(book_fp, "%d | %s | %s | %s | %lld | %s | %c\n", i->data.id, i->data.name, i->data.publisher, i->data.author, i->data.ISBN, i->data.location, i->data.available);
		}
	} else if (target_file == BORROW) {
		for (struct borrow_node *i = borrow_list_head; i != NULL; i = i->next) {
			fprintf(borrow_fp, "%d | %d | %ld | %ld\n", i->data.user_id, i->data.book_id, i->data.borrow_date, i->data.return_date);
		}
	}

	close_file(target_file);
}

// user
void print_book_info(struct book book, enum e_search_book_mode mode) {
    if (mode == STANDARD_INFO) {
        printf("\n도서명: %s\n", book.name);
        printf("출판사: %s\n", book.publisher);
        printf("저자명: %s\n", book.author);
        printf("ISBN: %lld\n", book.ISBN);
        printf("소장처: %s\n", book.location);
        printf("대여가능 여부: %c(%d/%d)\n", book.available, 0, 0); // TODO: 대여된 총 권수, 보유하고 있는 총 권수 어떻게 알아내지
        printf("** Y는 대여가능, N은 대여불가를 의미\n");
        printf("** (x/y) : (대여된 총 권수 / 보유하고 있는 총 권수)\n");
    } else if (mode == BORROW_INFO) {
        printf("\n도서번호 : %d\n", book.id);
        printf("도서명 : %s\n", book.name);
        // 대여일자 및 반납일자는 효율을 위해 이 함수의 호출위치에서 출력
    }
}

void find_book(enum e_search_book_by by, ...) { // (search by, target's data type, target)
    if (by == SEARCH_ALL) { // 5. 전체검색
        for (struct book_node *i = book_list_head; i != NULL; i = i->next) {
            print_book_info(i->data, STANDARD_INFO);
        }
        return;
    }

    // 1. - 4.
    va_list ap;
    va_start(ap, by);
    enum e_data_type type = va_arg(ap, enum e_data_type);
    
    int int_target;
    long long longlong_target;
    char *str_target;

    if (type == INT)
        int_target = va_arg(ap, int);
    else if (type == LONGLONG)
        longlong_target = va_arg(ap, long long);
    else if (type == STRING)
        str_target = va_arg(ap, char *);
    
    enum e_search_book_mode mode = va_arg(ap, enum e_search_book_mode);
    
    for (struct book_node *i = book_list_head; i != NULL; i = i -> next) {
        if (by == NAME) {
            if (strcmp(i->data.name, str_target) == 0)
                print_book_info(i->data, mode);
        } else if (by == PUBLISHER) {
            if (strcmp(i->data.publisher, str_target) == 0)
                print_book_info(i->data, mode);
        } else if (by == ISBN) {
            if (i->data.ISBN == longlong_target)
                print_book_info(i->data, mode);
        } else if (by == AUTHOR) {
            if (strcmp(i->data.author, str_target) == 0)
                print_book_info(i->data, mode);
        } else if (by == ID) {
            if (i->data.id == int_target)
                print_book_info(i->data, mode);
        }
    }
}

void book_search() {
    while (1) {
        printf("\n>> 도서 검색 <<\n");
        printf("1. 도서명 검색\t\t2. 출판사 검색\n3. ISBN 검색\t\t4. 저자명 검색\n5. 전체 검색\t\t6. 이전메뉴\n\n");
        
        int select;
        printf("번호를 선택하세요: ");
        scanf("%d", &select);

        while (getchar() != '\n');

        if (select == 1) {
            char target_name[50];
            printf("\n도서명을 입력하세요: ");
            scanf("%[^\n]", target_name);

            find_book(NAME, STRING, target_name, STANDARD_INFO);
        } else if (select == 2) {
            char target_publisher[20];
            printf("\n출판사를 입력하세요: ");
            scanf("%[^\n]", target_publisher);

            find_book(PUBLISHER, STRING, target_publisher, STANDARD_INFO);
        } else if (select == 3) {
            long long target_ISBN;
            printf("\nISBN을 입력하세요: ");
            scanf("%lld", &target_ISBN);

            find_book(ISBN, LONGLONG, target_ISBN, STANDARD_INFO);
        } else if (select == 4) {
            char target_author[20];
            printf("\n저자명을 입력하세요: ");
            scanf("%[^\n]", target_author);

            find_book(AUTHOR, STRING, target_author, STANDARD_INFO);
        } else if (select == 5) {
            printf("\n전체 검색 결과\n");
            find_book(SEARCH_ALL);
        } else if (select == 6) {
            break;
        }
    }
}

void my_borrow_info(int id) {
    open_file(BORROW, "r");

    for (struct borrow_node *i = borrow_list_head; i != NULL; i = i->next) {
        if (i->data.user_id == id) {
            find_book(ID, INT, i->data.book_id, BORROW_INFO);

            /* // localtime() 함수는 내부에서 static 메모리를 사용하므로 여러번 호출하면 서로의 값이 덮어씌워져버린다.
            struct tm *borrow_tm = localtime(&(i->data.borrow_date));
            struct tm *return_tm = localtime(&(i->data.return_date));
            printf("대여일자 : %.4d. %.2d. %.2d.\n", borrow_tm->tm_year + 1900, borrow_tm->tm_mon + 1, borrow_tm->tm_mday);
            printf("반납일자 : %.4d. %.2d. %.2d.\n", return_tm->tm_year + 1900, return_tm->tm_mon + 1, return_tm->tm_mday); */
            struct tm *time_data;

            time_data = localtime(&(i->data.borrow_date));
            printf("대여일자 : %.4d. %.2d. %.2d.\n", time_data->tm_year + 1900, time_data->tm_mon + 1, time_data->tm_mday);
            time_data = localtime(&(i->data.return_date));
            printf("반납일자 : %.4d. %.2d. %.2d.\n", time_data->tm_year + 1900, time_data->tm_mon + 1, time_data->tm_mday);
        }
    }

    close_file(BORROW);
}

void change_password(struct client_node *user) {
    char verify[20];
    printf("\n현재 비밀번호: ");
    while (getchar() != '\n');
    scanf("%[^\n]", verify);

    if (strcmp(verify, user->data.password) != 0) {
        fprintf(stderr, "현재 비밀번호와 일치하지 않습니다.\n");
    } else {
        printf("새 비밀번호: ");
        while (getchar() != '\n');
        scanf("%[^\n]", user->data.password);

        printf("비밀번호 변경이 완료되었습니다.\n");
    }
}

void change_address(struct client_node *user) {
    printf("\n새 주소: ");
    while (getchar() != '\n');
    scanf("%[^\n]", user->data.address);

    printf("주소 변경이 완료되었습니다.\n");
}

void change_phone_number(struct client_node *user) {
    printf("\n새 전화번호: ");
    while (getchar() != '\n');
    scanf("%[^\n]", user->data.phone_number);

    printf("전화번호 변경이 완료되었습니다.\n");
}

void account_setting(int id) {
    struct client_node *user;
    for (user = client_list_head; user != NULL; user = user->next) {
        if (user->data.id == id) break;
    }

    while (1) {
        printf("\n>> 개인정보 수정 <<\n");
        printf("1. 비밀번호 변경\t\t2. 주소 변경\n3. 전화번호 변경\t\t4. 이전메뉴\n\n");

        int select;
        printf("번호를 선택하세요: ");
        scanf("%d", &select);

        if (select == 1) {
            change_password(user);
        } else if (select == 2) {
            change_address(user);
        } else if (select == 3) {
            change_phone_number(user);
        } else if (select == 4) {
            break;
        }
    }

    sync_file(CLIENT);
}

_Bool delete_account(int id) {
    char y_n;
    fprintf(stderr, "정말 탈퇴하시겠습니까? (y / n) : ");
    while (getchar() != '\n');
    scanf("%c", &y_n);
    
    if (y_n == 'n') return 0;
    
    struct client_node *user, *prev = NULL;
    for (user = client_list_head; user != NULL; user = user->next) {
        if (user->data.id == id) {
            erase(CLIENT, prev);
            printf("탈퇴가 완료되었습니다.\n");
            return 1;
        }
        prev = user;
    }

    return -1;
}

void user_menu(int id) {
    while (1) {
        printf("\n>> 회원 메뉴 <<\n");
        printf("1. 도서 검색\t\t2. 내 대여 목록\n3. 개인정보 수정\t4. 회원 탈퇴\n5. 로그아웃\t\t6. 프로그램 종료\n\n");

        int select;
        printf("번호를 선택하세요: ");
        scanf("%d", &select);

        if (select == 1) {
            book_search();
        } else if (select == 2) {
            my_borrow_info(id);
        } else if (select == 3) {
            account_setting(id);
        } else if (select == 4) {
            if (delete_account(id)) return;
        } else if (select == 5) {
            return;
        } else if (select == 6) {
            exit(0);
        }
    }
}

// administrator
void add_book() {
    struct book* book_data = (struct book*)malloc(sizeof(struct book));

    printf(">> 도서 등록 <<\n");
    printf("도서명: ");
    scanf("%s", book_data->name);
    printf("출판사: ");
    scanf("%s", book_data->publisher);
    printf("저자명: ");
    scanf("%s", book_data->author);
    printf("ISBN: ");
    scanf("%lld", &(book_data->ISBN));
    printf("소장처: ");
    scanf("%s", book_data->location);

    book_data->available = 'Y';

    struct book_node* lastNode = book_list_head;
    while (lastNode->next != NULL)
        lastNode = lastNode->next;

    book_data->id = (lastNode->data.id) + 1;
    printf("\n자동입력 사항\n\n");
    printf("대여가능 여부: %c\n", book_data->available);
    printf("도서번호: %d\n\n", book_data->id);
    printf("등록하시겠습니까? ");

    if (getchar() == 'Y') {
        push_front(BOOK, book_data);  // 책 연결 리스트에 추가
        sort_list(BOOK);                    // 책 리스트 정렬
        sync_file(BOOK);                    // 파일에도 동기화

        printf("등록이 완료되었습니다.\n");
    }
}

void delete_book() {
    int erasing_book_id;
    struct book memo;

    printf(">> 도서 삭제 <<\n");
    printf("1. 도서명 검색\t\t2. ISBN 검색\n\n");

    int select;
    printf("검색 번호를 입력하세요: ");
    scanf("%d", &select);

    struct book_node *tmp = book_list_head;

    switch (select) {
        case 1:
            char search_by_name[50];
            printf("도서명을 입력하세요: ");
            scanf("%s", search_by_name);

            printf("\n\n>>검색 결과 <<\n");
            printf("도서번호: ");

            while (1) {  // 검색된 책 번호 일렬로 출력
                if (strcmp(tmp->data.name, search_by_name) == 0) {
                    printf("%d(삭제 가능 여부 : %c)", tmp->data.id, tmp->data.available);
                    memo = tmp->data;
                }
                tmp = tmp->next;
                if (tmp == NULL) break;

                printf(", ");  // 쉼표처리
            }

            printf("\n도서명: %s\n", memo.name);
            printf("출판사: %s\n", memo.publisher);
            printf("저자명: %s\n", memo.author);
            printf("ISBN: %lld\n", memo.ISBN);
            printf("소장처: %s\n\n", memo.location);

            printf("삭제할 도서의 번호를 입력하세요: ");
            scanf("%d", &erasing_book_id);

            tmp = book_list_head;
            while (tmp->data.id != erasing_book_id && tmp->next != NULL) tmp = tmp->next;

            if (tmp->next == NULL && tmp->data.id != erasing_book_id)
                printf("해당 도서가 존재하지 않습니다.\n");
            else if (tmp->data.available == 'N') {
                printf("이 도서는 삭제할 수 없습니다.\n");
            } else {
                erase(BOOK, tmp);
                printf("도서가 삭제되었습니다.\n");
                sync_file(BOOK);
            }

            break;
        case 2:
            long long search_by_ISBN;
            printf("ISBN을 입력하세요: ");
            scanf("%lld", &search_by_ISBN);

            printf("\n\n>>검색 결과 <<\n");
            printf("도서번호: ");

            while (tmp->next != NULL) {  // 검색된 책 번호 일렬로 출력
                if (tmp->data.ISBN == search_by_ISBN) {
                    printf("%d(삭제 가능 여부 : %c)", tmp->data.id, tmp->data.available);
                    memo = tmp->data;
                    break;
                }
                tmp = tmp->next;
            }
            printf("\n도서명: %s\n", memo.name);
            printf("출판사: %s\n", memo.publisher);
            printf("저자명: %s\n", memo.author);
            printf("ISBN: %lld\n", memo.ISBN);
            printf("소장처: %s\n\n", memo.location);

            printf("삭제할 도서의 번호를 입력하세요: ");
            scanf("%d", &erasing_book_id);

            tmp = book_list_head;
            while (tmp->data.id != erasing_book_id && tmp->next != NULL) tmp = tmp->next;

            if (tmp->next == NULL && tmp->data.id != erasing_book_id)
                printf("해당 도서는 존재하지 않습니다.\n");
            else if (tmp->data.available == 'Y') {
                erase(BOOK, tmp);
                printf("해당 도서가 삭제되었습니다.\n");
                sync_file(BOOK);
            } else {
                printf("이 도서는 삭제할 수 없습니다.\n");
            }

            break;
        default:
            printf("유효하지 않은 번호입니다.\n");
    }
}

void admin_menu() {
    printf("\n>> 관리자 메뉴 <<\n");
    printf("1. 도서 등록\t\t2. 도서 삭제\n3. 도서 대여\t\t4. 도서 반납\n5. 도서 검색\t\t6. 회원 목록\n7. 로그아웃\t\t8. 프로그램 종료\n\n");

    int select;
    printf("번호를 선택하세요: ");
    scanf("%d", &select);

    if (select == 1) {
        add_book();
    } else if (select == 2) {
        delete_book();
    } else if (select == 3) {
        // borrow_book();
    } else if (select == 4) {
        // return_book();
    } else if (select == 5) {
        // search_book();
    } else if (select == 6) {
        // search_client();
    } else if (select == 7) {
        return;
    } else if (select == 8) {
        exit(0);
    }
}

// account
void signup() {
	printf("\n>> 회원 가입 <<\n");
	printf("학번, 비밀번호, 이름, 주소, 전화번호를 입력하세요.\n\n");

	struct client client_data;

	printf("학번: ");
	scanf("%d", &client_data.id);
	printf("비밀번호: ");
	scanf("%s", client_data.password);
	printf("이름: ");
	scanf("%s", client_data.name);
	printf("주소: ");
	scanf("%s", client_data.address);
	printf("전화번호: ");
	scanf("%s", client_data.phone_number);

	for (struct client_node *i = client_list_head; i != NULL; i = i->next) {
		if (i->data.id == client_data.id) {
            fprintf(stderr, "\n이미 동일한 학번의 계정이 존재합니다.\n\n");
            return;
        }
	}

	push_front(CLIENT, &client_data);
    sort_list(CLIENT);
	sync_file(CLIENT);

	printf("\n회원가입이 되셨습니다.\n\n");
}

void login() {
    char id[9], password[20];

	printf("\n>> 로그인 <<\n");
	printf("학번: ");
	scanf("%s", id);
	printf("비밀번호: ");
	scanf("%s", password);

	if (strcmp("admin", id) == 0 && strcmp("yuoung14", password) == 0) {
        admin_menu();
        return;
    }

    for (struct client_node *i = client_list_head; i != NULL; i = i->next) {
        if (i->data.id == atoi(id) && strcmp(i->data.password, password) == 0) {
            user_menu(i->data.id);
            return;
        }
    }

    fprintf(stderr, "\n학번이나 비밀번호가 올바르지 않습니다.\n\n");
}

int main() {
    initialize_lists();
    sort_list(ALL_FILE_OR_STRUCT);
	
#ifdef DEBUG
	print_list(CLIENT);
	putchar('\n');
	erase(CLIENT, NULL); // 맨 앞 노드 삭제
	for (struct client_node *i = client_list_head; i != NULL; i = i->next) {
		if (i->next->data.id == 20230004) {
			erase(CLIENT, i); // 20230004 삭제
			break;
		}
	}
    print_list(CLIENT);

	save_to_file(CLIENT);
#endif

	while (1) {
        printf(">> 도서 관리 프로그램 <<\n");
        printf("1. 회원 가입\t\t2. 로그인\t\t3. 프로그램 종료\n\n");
        printf("번호를 선택하세요: ");

        int select;
        scanf("%d", &select);

		if (select == 1) {
			signup();
		} else if (select == 2) {
			login();
		} else if (select == 3) {
			break;
		}
	}

    return 0;
}