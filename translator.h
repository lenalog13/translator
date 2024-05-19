#include "stringPars.h" 
#include "hash.h" 
#include <stdio.h> 
#include <string.h> 
#include <stdbool.h>
#include <math.h> 
#include <ctype.h>

char* size;

char* str_reverse(char* str) {
    if (str) {
        int len = strlen(str);
        for (int i = 0; i < len / 2; i++) {
            char temp = str[i];
            str[i] = str[len - i - 1];
            str[len - i - 1] = temp;
        }
    }
    return str;
}

char* decToHex(int decimal) {
    char* hexadecimal = (char*)malloc(sizeof(char) * 20);
    int index = 0;
    if (decimal == 0)
        hexadecimal[index++] = '0';
    else {
        while (decimal > 0) {
            int remainder = decimal % 16;
            if (remainder < 10)
                hexadecimal[index++] = remainder + '0';
            else
                hexadecimal[index++] = remainder - 10 + 'A';
            decimal /= 16;
        }
    }
    if (index == 0)
        hexadecimal[index++] = '0';
    hexadecimal[index] = '\0';
    str_reverse(hexadecimal);
    return hexadecimal;
}

int hexToDec(char* number) {
    int decNum = 0;
    for (int i = 0; i < strlen(number); i++) {
        if (isdigit(number[i]))
            decNum += (int)(number[i] - '0') * pow(16, strlen(number) - i - 1);
        if (!isdigit(number[i]) && number[i] != '-')
            decNum += (int)(number[i] - 'A' + 10) * pow(16, strlen(number) - i - 1);
    }
    return decNum;
}

void initMnemonicsTable(hashTable* mnemonics){
    add("START", "-1", 1, mnemonics);
    add("END", "-1", 1, mnemonics);
    add("BYTE", "-1", 1, mnemonics);
    add("WORD", "-1", 1, mnemonics);
    add("RESB", "-1", 1, mnemonics);
    add("RESW", "-1", 1, mnemonics);
    add("MOV", "10", 0, mnemonics);
    add("ADD", "11", 0, mnemonics);
    add("SUB", "12", 0, mnemonics);
    add("CMP", "13", 0, mnemonics);
    add("MUL", "14", 0, mnemonics);
    add("DIV", "15", 0, mnemonics);
    add("JE", "16", 0, mnemonics);
    add("LOOP", "17", 0, mnemonics);
    add("CALL", "18", 0, mnemonics);
    add("RET", "19", 0, mnemonics);
    add("HLT", "1A", 0, mnemonics);



}


void first_parse(FILE* fp, hashTable* names) {
    int counter = 0;
    int start = 0;
    char code_str[256];
    AssemblerString* asStr;

    while (fgets(code_str, 256, fp)) {
        DeleteNewStr(code_str); // Удаление символа новой строки
        asStr = AssemblerString_ctor(); // Создание новой структуры AssemblerString
        ParseString(code_str, asStr); // Разбор строки

        // Обработка метки
        if (asStr->label != NULL) {
            DataItem* check = search(asStr->label, names);
            if (check == NULL) {
                add(StringCopy(asStr->label, 0), decToHex(counter), 1, names);
            } else if (strcmp(check->key, asStr->label) == 0 && check->mark == 0) {
                check->mark = 1;
                check->info = decToHex(counter);
            }
        }
 
        counter += 3;
        
        // Обработка инструкций
        if (asStr->instruction != NULL) {
            if (strcmp(asStr->instruction, "START") == 0) {
                start = hexToDec(asStr->instruction);
                counter += start - 6; // Счетчик устанавливается на начальное значение
            } else if (strcmp(asStr->instruction, "END") == 0) {
                size = decToHex(counter - start); // Вычисление размера кода
            } else if (asStr->instruction != NULL) {
            const char splits2[] = ",";
            char* oper = malloc(sizeof(char) * strlen(asStr->instruction));
            if (strchr(asStr->instruction, ',') == NULL)
                strcpy(oper, asStr->instruction);
            else {
                for(int i = 0; i < 2; i++) {
                    if(i == 0)
                        oper = strtok(asStr->instruction, splits2);
                    else
                        oper = strtok(NULL, splits2);
                    if (strcmp(oper, "ax") != 0 && strcmp(oper, "bx") != 0 && strcmp(oper, "cx") != 0 &&
                        strcmp(oper, "dx") != 0) {
                        if (oper != NULL || i == 0) {
                            bool isDigit = true;
                            for (int i = 0; i < strlen(oper); i++) {
                                if (isdigit(oper[i]) == 0)
                                    isDigit = false;
                            }
                            if (!isDigit) {
                                DataItem *check = search(oper, names);
                                if (check == NULL) {
                                    char *temp = malloc(sizeof(char) * strlen(oper));
                                    strcpy(temp, oper);
                                    add(temp, decToHex(counter), 0, names);
                                }
                            }
                        }
                    }
                }
            }
        }
        

        // Обработка операндов
        if (asStr->operand != NULL) {
            DataItem* check = search(asStr->operand, names);
            if (check == NULL) {
                char* temp = malloc(sizeof(char) * strlen(asStr->operand));
                strcpy(temp, asStr->operand);
                add(temp, decToHex(counter), 1, names);

            }
            else if (strcmp(check->key, asStr->operand) == 0 && check->mark == 0){
                check->mark = 1;
                check->info = decToHex(counter);
            }
        }
    
        AssemblerString_dctor(asStr); // Освобождение памяти

    }
}
}


void second_parse(FILE* fp, hashTable* names, hashTable* mnemonics) {
    FILE* output_file = fopen("result.txt", "w");

    int start;
    int counter = 0;
    int obj_byte_counter = 0;
    char code_str[256];
    char obj_code_str[256], dump[256];
    obj_code_str[0] = '\0';
    dump[0] = '\0';

    while (fgets(code_str, 256, fp)) {
        AssemblerString* asStr = AssemblerString_ctor();
        ParseString(code_str, asStr);

        counter += 3;

        if (counter != 3)
            printf("Счётчик размещения: %s   ", decToHex(counter));

        // Проверка и обработка инструкций
        if (asStr->instruction) {
            if (strcmp(asStr->instruction, "START") == 0) {
                // Обработка инструкции START
                start = hexToDec(asStr->operand);
                counter += start - 6;
                fprintf(output_file, "Адрес начала: %sh\n", asStr->operand);
                fprintf(output_file, "Точка входа: %sh\n", asStr->operand);
                fprintf(output_file, "Объектный код:\n:02%s02", asStr->operand);
                 for (int i = 0; i < 4 - strlen(size); i++)
                    fprintf(output_file, "0");
                fprintf(output_file,"%sXX\n", size);
                continue;
            } else if (strcmp(asStr->instruction, "END") == 0) {
                // Обработка инструкции END
                strcat(obj_code_str, "XX\n");
                fputs(obj_code_str, output_file);
                fprintf(output_file, ":00%s01XX", decToHex(start));
                fprintf(output_file,"\nРазмер программы: %sh\n", size);
                fprintf(output_file," Бинарный дамп: %s", dump);
                return;
            } else {

                obj_byte_counter++;
                if (obj_byte_counter == 3){
                    obj_byte_counter = 1;
                    strcat(obj_code_str, "XX\n");
                    fputs(obj_code_str, output_file);
                    obj_code_str[0] = '\0';
                }
                if (obj_byte_counter == 1){
                    strcat(obj_code_str, ":03");
                    strcat(obj_code_str, decToHex(counter));
                    strcat(obj_code_str, "00");
                }
                if (obj_byte_counter == 2){
                    obj_code_str[2] = '6';
                }

                // Обработка остальных инструкций
                DataItem* mnemonic = search(asStr->instruction, mnemonics);
                if (mnemonic != NULL) {
                printf("Машинная команда: ");
                    if (mnemonic->mark != 1) {
                        printf("%s", mnemonic->info);
                        strcat(dump, mnemonic->info);
                        strcat(obj_code_str, mnemonic->info);
                    } else {
                        strcat(dump, "00");
                        strcat(obj_code_str, "00");
                        printf("00");

                    }
                } else {
                    printf("Ошибка: неизвестная инструкция %s\n", asStr->instruction);
                }
            }
        }

 if (asStr->operand){
            bool print = false;
            const char splits2[] = ",";
            char* oper = malloc(sizeof(char) * strlen(asStr->operand));
            if (strchr(asStr->operand, ',') == NULL) {
                strcpy(oper, asStr->operand);
                bool isDigit = true;
                for (int i = 0; i < strlen(oper); i++) {
                    if (isdigit(oper[i]) == 0)
                        isDigit = false;
                }
                if (!isDigit) {
                    if (search(oper, names) != NULL){
                        if (search(oper, names)->hashKey >= 0) {
                            printf("%s", search(oper, names)->info);
                            strcat(dump, search(oper, names)->info);
                            strcat(obj_code_str, search(oper, names)->info);
                            print = true;
                        }
                    }
                } else {
                    for (int i = 0; i < 4 - strlen(oper); i++) {
                        printf("0");
                        strcat(dump, "0");
                        strcat(obj_code_str, "0");
                    }
                    strcat(dump, oper);
                    strcat(obj_code_str, oper);
                    printf("%s", oper);
                    print = true;
                }
            }
            else {
                oper = strtok(asStr->operand, splits2);
                bool isDigit = true;
                for (int i = 0; i < strlen(oper); i++) {
                    if (isdigit(oper[i]) == 0)
                        isDigit = false;
                }
                if (!isDigit) {
                    if (search(oper, names) != NULL){
                        if (search(oper, names)->hashKey >= 0) {
                            strcat(dump, search(oper, names)->info);
                            strcat(obj_code_str, search(oper, names)->info);
                            printf("%s", search(oper, names)->info);
                            print = true;
                        }
                    }
                } else {
                    for (int i = 0; i < 4 - strlen(oper); i++) {
                        strcat(dump, "0");
                        strcat(obj_code_str, "0");
                        printf( "0");
                    }
                    strcat(dump, oper);
                    strcat(obj_code_str, oper);
                    printf("%s", oper);
                    print = true;
                }
                oper = strtok(NULL, splits2);
                if (oper != NULL) {
                    bool isDigit2 = true;
                    for (int i = 0; i < strlen(oper); i++) {
                        if (isdigit(oper[i]) == 0)
                            isDigit2 = false;
                    }
                    if (!isDigit2) {
                        if (search(oper, names) != NULL) {
                            if (search(oper, names)->hashKey >= 0) {
                                strcat(dump, search(oper, names)->info);
                                strcat(obj_code_str, search(oper, names)->info);
                                printf("%s", search(oper, names)->info);
                                print = true;
                            }
                        }
                    } else {
                        for (int i = 0; i < 4 - strlen(oper); i++) {
                            strcat(dump, "0");
                            strcat(obj_code_str, "0");
                            printf("0");
                        }
                        strcat(dump, oper);
                        strcat(obj_code_str, oper);
                        printf("%s", oper);
                        print = true;
                    }
                }
            }
            if (!print) {
                strcat(dump, "0000");
                strcat(obj_code_str, "0000");
                printf("0000");
            }
        } else if (asStr->instruction) {
            strcat(dump, "0000");
            strcat(obj_code_str, "0000");
            printf( "0000");
        }
        printf("\n");
    }
}
