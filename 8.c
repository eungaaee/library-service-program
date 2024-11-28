#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

enum FileOrStructType { CLIENT, BOOK, BORROW, ALL_FILE_OR_STRUCT };

enum FindBookBy { BOOK_ID, BOOK_NAME, PUBLISHER, AUTHOR, ISBN, LOCATION, AVAILABLE, SEARCH_ALL_BOOK };
enum DataType { INT, LONGLONG, STRING };
enum SearchBookMode { STANDARD_INFO, ALL_INFO, BORROW_INFO };
enum FindClientBy { CLIENT_NAME, CLIENT_ID, SEARCH_ALL_CLIENT };

FILE *client_fp, *book_fp, *borrow_fp;

void OpenFile(enum FileOrStructType target_file, char *mode) {
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
        OpenFile(CLIENT, mode);
        OpenFile(BOOK, mode);
        OpenFile(BORROW, mode);
    }
}

void CloseFile(enum FileOrStructType target_file) {
    if (target_file == CLIENT) {
        fclose(client_fp);
    } else if (target_file == BOOK) {
        fclose(book_fp);
    } else if (target_file == BORROW) {
        fclose(borrow_fp);
    } else if (target_file == ALL_FILE_OR_STRUCT) {
        CloseFile(CLIENT);
        CloseFile(BOOK);
        CloseFile(BORROW);
    }
}

struct Client {
    int id;
    char password[20];
    char name[20];
    char address[50];
    char phone_number[20];
};

struct Book {
    int id;
    char name[50];
    char publisher[20];
    char author[20];
    long long ISBN;
    char location[50];
    char available;
};

struct Borrow {
    int client_id;
    int book_id;
    time_t borrow_date;
    time_t return_date;
};

struct ClientNode {
    struct Client data;
    struct ClientNode *next;
};
struct ClientNode *client_list_head;

struct BookNode {
    struct Book data;
    struct BookNode *next;
};
struct BookNode *book_list_head;

struct BorrowNode {
    struct Borrow data;
    struct BorrowNode *next;
};
struct BorrowNode *borrow_list_head;

void PushFront(enum FileOrStructType target_list, void *data_ptr) {
    if (target_list == CLIENT) {
        struct ClientNode *new_node = (struct ClientNode *)malloc(sizeof(struct ClientNode));
        new_node->data = *(struct Client *)data_ptr;
        new_node->next = client_list_head;
        client_list_head = new_node;
    } else if (target_list == BOOK) {
        struct BookNode *new_node = (struct BookNode *)malloc(sizeof(struct BookNode));
        new_node->data = *(struct Book *)data_ptr;
        new_node->next = book_list_head;
        book_list_head = new_node;
    } else if (target_list == BORROW) {
        struct BorrowNode *new_node = (struct BorrowNode *)malloc(sizeof(struct BorrowNode));
        new_node->data = *(struct Borrow *)data_ptr;
        new_node->next = borrow_list_head;
        borrow_list_head = new_node;
    }
}

void Erase(enum FileOrStructType target_list, void *prev_node) {
	if (target_list == CLIENT) {
		struct ClientNode *target_node; // 삭제할 노드

		if (prev_node == NULL) { // 맨 앞 노드를 삭제하는 경우
			target_node = client_list_head;
			client_list_head = client_list_head->next;
		} else {
			target_node = ((struct ClientNode *)prev_node)->next;
			((struct ClientNode *)prev_node)->next = target_node->next;
		}

		free(target_node); // 삭제하는 노드의 메모리 해제
	} else if (target_list == BOOK) {
		struct BookNode *target_node;

		if (prev_node == NULL) {
			target_node = book_list_head;
			book_list_head = book_list_head->next;
		} else {
			target_node = ((struct BookNode *)prev_node)->next;
			((struct BookNode *)prev_node)->next = target_node->next;
		}

		free(target_node);
	} else if (target_list == BORROW) {
		struct BorrowNode *target_node;

		if (prev_node == NULL) {
			target_node = borrow_list_head;
			borrow_list_head = borrow_list_head->next;
		} else {
			target_node = ((struct BorrowNode *)prev_node)->next;
			((struct BorrowNode *)prev_node)->next = target_node->next;
		}

		free(target_node);
	}
}

void InitializeLists() {
    OpenFile(ALL_FILE_OR_STRUCT, "r");

    struct Client client_data;
    while (fscanf(client_fp, "%d | %s | %[^|]| %[^|]| %s", &client_data.id, client_data.password, client_data.name, client_data.address, client_data.phone_number) != EOF) {
		client_data.name[strlen(client_data.name) - 1] = '\0'; // 띄어쓰기 제거
		client_data.address[strlen(client_data.address) - 1] = '\0';
        PushFront(CLIENT, &client_data);
    }

    struct Book book_data;
    while (fscanf(book_fp, "%d | %[^|]| %[^|]| %[^|]| %lld | %[^|]| %c", &book_data.id, book_data.name, book_data.publisher, book_data.author, &book_data.ISBN, book_data.location, &book_data.available) != EOF) {
		book_data.name[strlen(book_data.name) - 1] = '\0';
		book_data.publisher[strlen(book_data.publisher) - 1] = '\0';
		book_data.author[strlen(book_data.author) - 1] = '\0';
		book_data.location[strlen(book_data.location) - 1] = '\0';
		PushFront(BOOK, &book_data);
    }

    struct Borrow borrow_data;
    while (fscanf(borrow_fp, "%d | %d | %ld | %ld", &borrow_data.client_id, &borrow_data.book_id, &borrow_data.borrow_date, &borrow_data.return_date) != EOF) {
		PushFront(BORROW, &borrow_data);
    }

    CloseFile(ALL_FILE_OR_STRUCT);
}

void SortList(enum FileOrStructType target_list) {
    if (target_list == CLIENT) {
        for (struct ClientNode *i = client_list_head; i != NULL; i = i->next) {
            for (struct ClientNode *j = i->next; j != NULL; j = j->next) {
                if (i->data.id > j->data.id) {
                    struct Client temp = i->data;
                    i->data = j->data;
                    j->data = temp;
                }
            }
        }
    } else if (target_list == BOOK) {
        for (struct BookNode *i = book_list_head; i != NULL; i = i->next) {
            for (struct BookNode *j = i->next; j != NULL; j = j->next) {
                if (i->data.ISBN > j->data.ISBN) {
                    struct Book temp = i->data;
                    i->data = j->data;
                    j->data = temp;
                }
            }
        }
    } else if (target_list == ALL_FILE_OR_STRUCT) {
		SortList(CLIENT);
		SortList(BOOK);
	}
}

#ifdef DEBUG
void print_list(enum e_file_or_struct_type target_list) {
    if (target_list == CLIENT) {
        for (struct ClientNode *i = client_list_head; i != NULL; i = i->next) {
            printf("%d | %s | %s | %s | %s\n", i->data.id, i->data.password, i->data.name, i->data.address, i->data.phone_number);
        }
    } else if (target_list == BOOK) {
        for (struct BookNode *i = book_list_head; i != NULL; i = i->next) {
            printf("%d | %s | %s | %s | %lld | %s | %c\n", i->data.id, i->data.name, i->data.publisher, i->data.author, i->data.ISBN, i->data.location, i->data.available);
        }
    } else if (target_list == BORROW) {
        for (struct BorrowNode *i = borrow_list_head; i != NULL; i = i->next) {
            printf("%d | %d | %ld | %ld\n", i->data.client_id, i->data.book_id, i->data.borrow_date, i->data.return_date);
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

void SyncFile(enum FileOrStructType target_file) {
	OpenFile(target_file, "w");

	if (target_file == CLIENT) {
		for (struct ClientNode *i = client_list_head; i != NULL; i = i->next) {
			fprintf(client_fp, "%d | %s | %s | %s | %s\n", i->data.id, i->data.password, i->data.name, i->data.address, i->data.phone_number);
		}
	} else if (target_file == BOOK) {
		for (struct BookNode *i = book_list_head; i != NULL; i = i->next) {
			fprintf(book_fp, "%d | %s | %s | %s | %lld | %s | %c\n", i->data.id, i->data.name, i->data.publisher, i->data.author, i->data.ISBN, i->data.location, i->data.available);
		}
	} else if (target_file == BORROW) {
		for (struct BorrowNode *i = borrow_list_head; i != NULL; i = i->next) {
			fprintf(borrow_fp, "%d | %d | %ld | %ld\n", i->data.client_id, i->data.book_id, i->data.borrow_date, i->data.return_date);
		}
	}

	CloseFile(target_file);
}

// user
void PrintBookInfo(struct Book book, enum SearchBookMode mode) {
    if (mode == STANDARD_INFO) {
        printf("\n도서명: %s\n", book.name);
        printf("출판사: %s\n", book.publisher);
        printf("저자명: %s\n", book.author);
        printf("ISBN: %lld\n", book.ISBN);
        printf("소장처: %s\n", book.location);
        // 대여가능 여부 및 (대여됨/전체) 는 효율을 위해 이 함수의 호출 위치에서 출력
    } else if (mode == ALL_INFO) {
        printf("\n도서번호: %d\n", book.id);
        printf("도서명: %s\n", book.name);
        printf("출판사: %s\n", book.publisher);
        printf("저자명: %s\n", book.author);
        printf("ISBN: %lld\n", book.ISBN);
        printf("소장처: %s\n", book.location);
        printf("대여가능 여부: %c\n", book.available);
    } else if (mode == BORROW_INFO) {
        printf("\n도서번호 : %d\n", book.id);
        printf("도서명 : %s\n", book.name);
        // 대여일자 및 반납일자는 효율을 위해 이 함수의 호출 위치에서 출력
    }
}

int BookFinder(enum FindBookBy by, ...) { // (find by, target's data type, target)
    system("clear");

    int borrowed_count = 0, total_count = 0;

    if (by == SEARCH_ALL_BOOK) { // 5. 전체검색
        for (struct BookNode *i = book_list_head; i != NULL; i = i->next) {
            PrintBookInfo(i->data, ALL_INFO);
            putchar('\n');
            
            total_count++;
        }
        return total_count;
    }

    // 1. - 4.
    va_list ap;
    va_start(ap, by);
    enum DataType type = va_arg(ap, enum DataType);
    
    int int_target;
    long long longlong_target;
    char *str_target;

    if (type == INT)
        int_target = va_arg(ap, int);
    else if (type == LONGLONG)
        longlong_target = va_arg(ap, long long);
    else if (type == STRING)
        str_target = va_arg(ap, char *);
    
    enum SearchBookMode mode = va_arg(ap, enum SearchBookMode);
    
    for (struct BookNode *i = book_list_head; i != NULL; i = i -> next) {
        if (by == BOOK_NAME) {
            if (strcmp(i->data.name, str_target) == 0) {
                PrintBookInfo(i->data, mode);

                if (i->data.available == 'N') borrowed_count++;
                total_count++;
            }
        } else if (by == PUBLISHER) {
            if (strcmp(i->data.publisher, str_target) == 0) {
                PrintBookInfo(i->data, mode);
                
                if (i->data.available == 'N') borrowed_count++;
                total_count++;
            }
        } else if (by == ISBN) {
            if (i->data.ISBN == longlong_target) {
                PrintBookInfo(i->data, mode);

                if (i->data.available == 'N') borrowed_count++;
                total_count++;
            }
        } else if (by == AUTHOR) {
            if (strcmp(i->data.author, str_target) == 0) {
                PrintBookInfo(i->data, mode);

                if (i->data.available == 'N') borrowed_count++;
                total_count++;
            }
        } else if (by == BOOK_ID) {
            if (i->data.id == int_target) {
                PrintBookInfo(i->data, mode);

                if (i->data.available == 'N') borrowed_count++;
                total_count++;
            }
        }

        if (i->next == NULL && total_count > 0) {
            printf("\n대여가능 여부: %c(%d/%d)\n", borrowed_count < total_count ? 'Y' : 'N', borrowed_count, total_count);
        }
    }

    return total_count;
}

void SearchBook() {
    while (1) {
        system("clear");

        printf(">> 도서 검색 <<\n");
        printf("1. 도서명 검색\t\t2. 출판사 검색\n3. ISBN 검색\t\t4. 저자명 검색\n5. 전체 검색\t\t6. 이전 메뉴\n\n");
        
        int select;
        printf("번호를 선택하세요: ");
        scanf("%d", &select);

        int result_count;

        if (select == 1) {
            char target_name[50];
            printf("\n도서명을 입력하세요: ");
            getchar();
            scanf("%[^\n]", target_name);

            result_count = BookFinder(BOOK_NAME, STRING, target_name, STANDARD_INFO);
        } else if (select == 2) {
            char target_publisher[20];
            printf("\n출판사를 입력하세요: ");
            getchar();
            scanf("%[^\n]", target_publisher);

            result_count = BookFinder(PUBLISHER, STRING, target_publisher, STANDARD_INFO);
        } else if (select == 3) {
            long long target_ISBN;
            printf("\nISBN을 입력하세요: ");
            scanf("%lld", &target_ISBN);

            result_count = BookFinder(ISBN, LONGLONG, target_ISBN, STANDARD_INFO);
        } else if (select == 4) {
            char target_author[20];
            printf("\n저자명을 입력하세요: ");
            getchar();
            scanf("%[^\n]", target_author);

            result_count = BookFinder(AUTHOR, STRING, target_author, STANDARD_INFO);
        } else if (select == 5) {
            printf("\n전체 검색 결과\n");
            result_count = BookFinder(SEARCH_ALL_BOOK);
        } else if (select == 6) {
            break;
        }

        if (result_count == 0) fprintf(stderr, "\n검색 결과가 없습니다.\n");
    }
}

void PrintDate(time_t t) {
    struct tm *t_struct = localtime(&t);
    char *wday[7] = {"일", "월", "화", "수", "목", "금", "토"};

    printf("%d년 %d월 %d일 %s요일\n", t_struct->tm_year + 1900, t_struct->tm_mon + 1, t_struct->tm_mday, wday[t_struct->tm_wday]);
}

void ClientBorrowInfo(int id) {
    system("clear");

    printf(">> 내 대여 목록 <<\n");

    for (struct BorrowNode *i = borrow_list_head; i != NULL; i = i->next) {
        if (i->data.client_id == id) {
            BookFinder(BOOK_ID, INT, i->data.book_id, BORROW_INFO); // BORROW_INFO: 도서번호, 도서명 출력

            /* // localtime() 함수는 내부에서 static 메모리를 사용하므로 여러번 호출하면 서로의 값이 덮어씌워져버린다.
            struct tm *borrow_tm = localtime(&(i->data.borrow_date));
            struct tm *return_tm = localtime(&(i->data.return_date));
            printf("대여일자 : %.4d. %.2d. %.2d.\n", borrow_tm->tm_year + 1900, borrow_tm->tm_mon + 1, borrow_tm->tm_mday);
            printf("반납일자 : %.4d. %.2d. %.2d.\n", return_tm->tm_year + 1900, return_tm->tm_mon + 1, return_tm->tm_mday); */
            printf("대여일자 : ");
            PrintDate(i->data.borrow_date);
            printf("반납일자 : ");
            PrintDate(i->data.return_date);
        }
    }
}

_Bool IsClientBorrowed(int id) {
    for (struct BorrowNode *i = borrow_list_head; i != NULL; i = i->next) {
        if (i->data.client_id == id) {
            return 1;
        }
    }

    return 0;
}

void ChangePassword(struct ClientNode *user) {
    char verify[20];
    printf("\n현재 비밀번호: ");
    getchar();
    scanf("%[^\n]", verify);

    if (strcmp(verify, user->data.password) != 0) {
        fprintf(stderr, "현재 비밀번호와 일치하지 않습니다.\n");
    } else {
        printf("새 비밀번호: ");
        getchar();
        scanf("%[^\n]", user->data.password);
        SyncFile(CLIENT);

        printf("비밀번호 변경이 완료되었습니다.\n");
    }
    getchar();
}

void ChangeAddress(struct ClientNode *user) {
    printf("\n새 주소: ");
    getchar();
    scanf("%[^\n]", user->data.address);
    SyncFile(CLIENT);

    printf("주소 변경이 완료되었습니다.\n");
    getchar();
}

void ChangePhoneNumber(struct ClientNode *user) {
    printf("\n새 전화번호: ");
    getchar();
    scanf("%[^\n]", user->data.phone_number);
    SyncFile(CLIENT);

    printf("전화번호 변경이 완료되었습니다.\n");
    getchar();
}

void AccountSetting(int id) {
    struct ClientNode *user;
    for (user = client_list_head; user != NULL; user = user->next) {
        if (user->data.id == id) break;
    }

    while (1) {
        system("clear");

        printf(">> 개인정보 수정 <<\n");
        printf("1. 비밀번호 변경\t\t2. 주소 변경\n3. 전화번호 변경\t\t4. 이전메뉴\n\n");

        int select;
        printf("번호를 선택하세요: ");
        scanf("%d", &select);

        if (select == 1) {
            ChangePassword(user);
        } else if (select == 2) {
            ChangeAddress(user);
        } else if (select == 3) {
            ChangePhoneNumber(user);
        } else if (select == 4) {
            break;
        }
    }
}

_Bool DeleteAccount(int id) {
    system("clear");

    char y_n;
    printf("정말 탈퇴하시겠습니까? (y / n) : ");
    getchar();
    scanf("%c", &y_n);
    
    if (IsClientBorrowed(id)) {
        fprintf(stderr, "대여중인 책이 있어서 탈퇴할 수 없습니다.\n");
        return 0;
    } else if (y_n == 'n') return 0;
    
    struct ClientNode *user, *prev = NULL;
    for (user = client_list_head; user != NULL; user = user->next) {
        if (user->data.id == id) {
            Erase(CLIENT, prev);
            SyncFile(CLIENT);
            printf("탈퇴가 완료되었습니다.\n");
            return 1;
        }
        prev = user;
    }

    return -1;
}

void UserMenu(int id) {
    while (1) {
        system("clear");

        printf(">> 회원 메뉴 <<\n");
        printf("1. 도서 검색\t\t2. 내 대여 목록\n3. 개인정보 수정\t4. 회원 탈퇴\n5. 로그아웃\t\t6. 프로그램 종료\n\n");

        int select;
        printf("번호를 선택하세요: ");
        scanf("%d", &select);

        if (select == 1) {
            SearchBook();
        } else if (select == 2) {
            ClientBorrowInfo(id);
        } else if (select == 3) {
            AccountSetting(id);
        } else if (select == 4) {
            if (DeleteAccount(id)) return;
        } else if (select == 5) {
            return;
        } else if (select == 6) {
            exit(0);
        }
    }
}

// administrator
void AddBook() {
    system("clear");

    struct Book book_data;

    printf(">> 도서 등록 <<\n");

    printf("도서명: ");
    getchar();
    scanf("%[^\n]", book_data.name);

    printf("출판사: ");
    getchar();
    scanf("%[^\n]", book_data.publisher);

    printf("저자명: ");
    getchar();
    scanf("%[^\n]", book_data.author);

    printf("ISBN: ");
    scanf("%lld", &(book_data.ISBN));

    printf("소장처: ");
    getchar();
    scanf("%[^\n]", book_data.location);

    book_data.available = 'Y';

    struct BookNode* last_node;
    for (last_node = book_list_head; last_node->next != NULL; last_node = last_node->next);

    book_data.id = last_node->data.id + 1;

    printf("\n자동입력 사항\n\n");
    printf("대여가능 여부: %c\n", book_data.available);
    printf("도서번호: %d\n\n", book_data.id);
    printf("등록하시겠습니까? ");

    getchar();
    if (getchar() == 'Y') {
        PushFront(BOOK, &book_data);
        SortList(BOOK);
        SyncFile(BOOK);

        printf("등록이 완료되었습니다.\n");
    }
}

void DeleteBook() {
    system("clear");

    printf(">> 도서 삭제 <<\n");
    printf("1. 도서명 검색\t\t2. ISBN 검색\n\n");

    int select;
    printf("검색 번호를 입력하세요: ");
    scanf("%d", &select);

    struct Book book_info;
    _Bool is_first = 1;

    if (select == 1) {
        char target_name[50];
        printf("도서명을 입력하세요: ");
        getchar();
        scanf("%[^\n]", target_name);

        printf("\n\n>> 검색 결과 <<\n");
        printf("도서번호: ");

        for (struct BookNode *i = book_list_head; i != NULL; i = i->next) {
            if (strcmp(i->data.name, target_name) == 0) {
                if (is_first) {
                    book_info = i->data;
                    is_first = 0;
                } else {
                    printf(", ");
                }

                printf("%d(삭제 가능 여부 : %c)", i->data.id, i->data.available);
            }
        }
    } else if (select == 2) {
        long long target_ISBN;
        printf("ISBN을 입력하세요: ");
        scanf("%lld", &target_ISBN);

        printf("\n\n>> 검색 결과 <<\n");
        printf("도서 번호: ");

        for (struct BookNode *i = book_list_head; i != NULL; i = i->next) {
            if (i->data.ISBN == target_ISBN) {
                if (is_first) {
                    book_info = i->data;
                    is_first = 0;
                } else {
                    printf(", ");
                }

                printf("%d(삭제 가능 여부 : %c)", i->data.id, i->data.available);
            }
        }
    }

    if (is_first == 1) {
        printf("\n도서관에서 제공하지 않는 도서입니다.\n");
        return;
    }

    printf("\n도서명: %s\n", book_info.name);
    printf("출판사: %s\n", book_info.publisher);
    printf("저자명: %s\n", book_info.author);
    printf("ISBN: %lld\n", book_info.ISBN);
    printf("소장처: %s\n\n", book_info.location);

    int erasing_book_id;
    printf("삭제할 도서의 번호를 입력하세요: ");
    scanf("%d", &erasing_book_id);

    struct BookNode *i, *prev;
    for (i = book_list_head, prev = NULL; i != NULL; i = i->next) {
        if (i->data.id == erasing_book_id) {
            if (i->data.available == 'N') {
                printf("이 도서는 삭제할 수 없습니다.\n");
            } else {
                Erase(BOOK, prev);
                SyncFile(BOOK);
                printf("도서가 삭제되었습니다.\n");
            }
            return;
        }
        prev = i;
    }

    if (i == NULL) printf("해당 도서가 존재하지 않습니다.\n");
}

void BorrowBook() {
    system("clear");

    printf(">> 도서 대여 <<\n");
    printf("1. 도서명 검색\t\t2. ISBN 검색\n\n");

    int select;
    printf("검색 번호를 입력하세요: ");
    scanf("%d", &select);

    struct Book book_info;
    _Bool is_first = 1;

    if (select == 1) {
        char target_name[50];
        printf("도서명을 입력하세요: ");
        getchar();
        scanf("%[^\n]", target_name);

        printf("\n\n>> 검색 결과 <<\n");
        printf("도서번호: ");

        for (struct BookNode *i = book_list_head; i != NULL; i = i->next) {
            if (strcmp(i->data.name, target_name) == 0) {
                if (is_first) {
                    book_info = i->data;
                    is_first = 0;
                } else {
                    printf(", ");
                }

                printf("%d(대여 가능 여부 : %c)", i->data.id, i->data.available);
            }
        }
    } else if (select == 2) {
        long long target_ISBN;
        printf("ISBN을 입력하세요: ");
        scanf("%lld", &target_ISBN);

        printf("\n\n>> 검색 결과 <<\n");
        printf("도서 번호: ");

        for (struct BookNode *i = book_list_head; i != NULL; i = i->next) {
            if (i->data.ISBN == target_ISBN) {
                if (is_first) {
                    book_info = i->data;
                    is_first = 0;
                } else {
                    printf(", ");
                }

                printf("%d(대여 가능 여부 : %c)", i->data.id, i->data.available);
            }
        }
    }

    if (is_first == 1) {
        printf("도서관에서 제공하지 않는 도서입니다.\n");
        return;
    }

    printf("\n도서명: %s\n", book_info.name);
    printf("출판사: %s\n", book_info.publisher);
    printf("저자명: %s\n", book_info.author);
    printf("ISBN: %lld\n", book_info.ISBN);
    printf("소장처: %s\n\n", book_info.location);

    int client_id;
    printf("학번을 입력하세요: ");
    scanf("%d", &client_id);

    struct ClientNode *target_client_node;

    for (struct ClientNode *i = client_list_head; i != NULL; i = i->next) {
        if (i->data.id == client_id) {
            target_client_node = i;
            break;
        } else if (i->next == NULL && i->data.id != client_id) {
            fprintf(stderr, "\n학번이 올바르지 않습니다.\n");
            return;
        }
    }

    int book_id;
    printf("도서번호를 입력하세요: ");
    scanf("%d", &book_id);

    struct BookNode *target_book_node;

    for (struct BookNode *i = book_list_head; i != NULL; i = i->next) {
        if (i->data.id == book_id) {
            target_book_node = i;
            break;
        } else if (i->next == NULL && i->data.id != book_id) {
            fprintf(stderr, "\n도서번호가 올바르지 않습니다.\n");
            return;
        }
    }

    printf("이 도서를 대여합니까? ");
    getchar();
    if (getchar() == 'N') return;
    else if (target_book_node->data.available == 'N') {
        fprintf(stderr, "\n이 도서는 대여할 수 없습니다.\n");
        return;
    }
    

    // BORROW: 대여 목록 업데이트
    struct Borrow borrow_data;
    borrow_data.client_id = target_client_node->data.id;
    borrow_data.book_id = target_book_node->data.id;

    time_t now = time(NULL);
    borrow_data.borrow_date = now; // 대여한 날 기록
    now += 24 * 60 * 60 * 30; // 24시간 * 60분 * 60초 == 1일
    struct tm *t = localtime(&now);
    if (t->tm_wday == 0) now += 24 * 60 * 60; // 일요일이면 반납기간을 하루 더 주기 (31일)
    borrow_data.return_date = now; // 반납해야할 날 기록

    PushFront(BORROW, &borrow_data);
    SortList(BORROW);
    SyncFile(BORROW);

    // BOOK: 대여 가능 여부 업데이트
    target_book_node->data.available = 'N';
    SyncFile(BOOK);

    printf("\n도서가 대여 되었습니다.\n");
}

void ReturnBook() {
    system("clear");

    int client_id;
    printf("학번을 입력하세요: ");
    scanf("%d", &client_id);

    printf("\n>> 회원의 대여 목록 <<\n");
    ClientBorrowInfo(client_id);

    int book_id;
    printf("\n반납할 도서번호를 입력하세요: ");
    scanf("%d", &book_id);

    printf("\n도서 반납처리를 할까요? ");
    getchar();
    if (getchar() == 'N') return;

    // BORROW 데이터에 반납 반영하기
    struct BorrowNode *prev = NULL;

    for (struct BorrowNode *i = borrow_list_head; i != NULL; i = i->next) {
        if (i->data.book_id == book_id) break;
        else if (i->next == NULL && i->data.book_id != book_id) {
            fprintf(stderr, "\n도서가 존재하지 않습니다.\n");
            return;
        }
        prev = i;
    }

    Erase(BORROW, prev);
    SyncFile(BORROW);

    // BOOK 데이터에 반납 반영하기
    for (struct BookNode *i = book_list_head; i != NULL; i = i->next) {
        if (i->data.id == book_id) {
            i->data.available = 'Y';
            break;
        }
    }

    SyncFile(BOOK);

    printf("\n도서가 반납 되었습니다.\n");
}

void PrintClientInfo(struct Client client) {
    printf("학번 : %d\n", client.id);
    printf("이름 : %s\n", client.name);
    printf("주소 : %s\n", client.address);
    printf("전화번호 : %s\n", client.phone_number);
}

void ClientFinder(enum FindClientBy by, ...) { // (search by, target's data type, target)
    if (by == SEARCH_ALL_CLIENT) { // 3. 전체검색
        for (struct ClientNode *i = client_list_head; i != NULL; i = i->next) {
            PrintClientInfo(i->data);
            putchar('\n');
        }
        return;
    }

    // 1. 이름 검색, 2. 학번 검색
    va_list ap;
    va_start(ap, by);
    enum DataType type = va_arg(ap, enum DataType);
    
    int int_target;
    char *str_target;

    if (type == INT)
        int_target = va_arg(ap, int);
    else if (type == STRING)
        str_target = va_arg(ap, char *);
    
    for (struct ClientNode *i = client_list_head; i != NULL; i = i -> next) {
        if (by == CLIENT_NAME) {
            if (strcmp(i->data.name, str_target) == 0)
                PrintClientInfo(i->data);
        } else if (by == CLIENT_ID) {
            if (i->data.id == int_target) {
                PrintClientInfo(i->data);
                break; // 같은 학번이 여러 개일 수 없다.
            }
        }
    }
}

void SearchClient() {
    while (1) {
        system("clear");

        printf(">> 회원 목록 <<\n");
        printf("1. 이름 검색\t\t2. 학번 검색\n3. 전체 검색\t\t4. 이전 메뉴\n\n");
        
        int select;
        printf("번호를 선택하세요: ");
        scanf("%d", &select);

        if (select == 1) {
            char target_name[50];
            printf("\n회원의 이름을 입력하세요: ");
            getchar();
            scanf("%[^\n]", target_name);

            ClientFinder(CLIENT_NAME, STRING, target_name);
        } else if (select == 2) {
            int target_id;
            printf("\n회원의 학번을 입력하세요: ");
            scanf("%d", &target_id);

            ClientFinder(CLIENT_ID, INT, target_id);
        } else if (select == 3) {
            printf("\n전체 검색 결과\n");
            ClientFinder(SEARCH_ALL_CLIENT);
        } else if (select == 4) {
            break;
        }
    }
}

void AdminMenu() {
    while (1) {
        system("clear");

        printf(">> 관리자 메뉴 <<\n");
        printf("1. 도서 등록\t\t2. 도서 삭제\n3. 도서 대여\t\t4. 도서 반납\n5. 도서 검색\t\t6. 회원 목록\n7. 로그아웃\t\t8. 프로그램 종료\n\n");

        int select;
        printf("번호를 선택하세요: ");
        scanf("%d", &select);

        if (select == 1) {
            AddBook();
        } else if (select == 2) {
            DeleteBook();
        } else if (select == 3) {
            BorrowBook();
        } else if (select == 4) {
            ReturnBook();
        } else if (select == 5) {
            SearchBook();
        } else if (select == 6) {
            SearchClient();
        } else if (select == 7) {
            return;
        } else if (select == 8) {
            exit(0);
        }
    }
}

// account
void SignUp() {
    system("clear");

	printf(">> 회원 가입 <<\n");
	printf("학번, 비밀번호, 이름, 주소, 전화번호를 입력하세요.\n\n");

	struct Client client_data;

	printf("학번: ");
	scanf("%d", &client_data.id);

	printf("비밀번호: ");
	scanf("%s", client_data.password);

	printf("이름: ");
    getchar();
	scanf("%[^\n]", client_data.name);

	printf("주소: ");
    getchar();
	scanf("%[^\n]", client_data.address);

	printf("전화번호: ");
    getchar();
	scanf("%[^\n]", client_data.phone_number);

	for (struct ClientNode *i = client_list_head; i != NULL; i = i->next) {
		if (i->data.id == client_data.id) {
            fprintf(stderr, "\n이미 동일한 학번의 계정이 존재합니다.\n\n");
            return;
        }
	}

	PushFront(CLIENT, &client_data);
    SortList(CLIENT);
	SyncFile(CLIENT);

	printf("\n회원가입이 되셨습니다.\n\n");
}

void LogIn() {
    system("clear");

    char id[9], password[20];

	printf(">> 로그인 <<\n");
	printf("학번: ");
	scanf("%s", id);
	printf("비밀번호: ");
	scanf("%s", password);

	if (strcmp("admin", id) == 0 && strcmp("yuoung14", password) == 0) {
        AdminMenu();
        return;
    }

    for (struct ClientNode *i = client_list_head; i != NULL; i = i->next) {
        if (i->data.id == atoi(id) && strcmp(i->data.password, password) == 0) {
            UserMenu(i->data.id);
            return;
        }
    }

    fprintf(stderr, "\n학번이나 비밀번호가 올바르지 않습니다.\n\n");
}

int main() {
    InitializeLists();
    SortList(ALL_FILE_OR_STRUCT);

	while (1) {
        system("clear");

        printf(">> 도서 관리 프로그램 <<\n");
        printf("1. 회원 가입\t\t2. 로그인\t\t3. 프로그램 종료\n\n");
        printf("번호를 선택하세요: ");

        int select;
        scanf("%d", &select);

		if (select == 1) {
			SignUp();
		} else if (select == 2) {
			LogIn();
		} else if (select == 3) {
			break;
		}
	}

    return 0;
}