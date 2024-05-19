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
 
        // НАДО ПРОВЕРИТЬ
        // Обработка инструкций
        if (asStr->instruction != NULL) {
            if (strcmp(asStr->instruction, "START") == 0) {
                start = hexToDec(asStr->instruction);
                counter = start; // Счетчик устанавливается на начальное значение
            } else if (strcmp(asStr->instruction, "END") == 0) {
                size = decToHex(counter - start); // Вычисление размера кода
                break; // Завершение обработки файла после инструкции END
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
        counter += 3; // Увеличение счетчика на размер инструкции

    //      printf("Метка: %20s, Оператор: %20s, Операнд: %20s, Комментарий: %s\n",
    //        asStr->label, asStr->instruction, asStr->operand, asStr->comment);

    }
}
}

/*
void second_parse(FILE* fp, hashTable* names, hashTable* mnemonics) {
    FILE* output_file = fopen("result.txt", "w");
//    if (!fp || !output_file) {
//        // Обработка ошибки открытия файла
//        return;
//    }

    int counter = 0;
    int obj_byte_counter = 0;
    char code_str[256];
    char obj_code_str[256], dump[256];
    obj_code_str[0] = '\0';
    dump[0] = '\0';

    while (fgets(code_str, 256, fp)) {
        AssemblerString* asStr = AssemblerString_ctor();
        ParseString(code_str, asStr);

        // Проверка и обработка метки
        if (asStr->label) {
            // Добавить обработку метки
        }

        // Проверка и обработка инструкций
        if (asStr->instruction) {
            if (strcmp(asStr->instruction, "START") == 0) {
                // Обработка инструкции START
            } else if (strcmp(asStr->instruction, "END") == 0) {
                // Обработка инструкции END
            } else {
                // Обработка остальных инструкций
            }
        }

        // Проверка и обработка операндов
        if (asStr->operand) {
            // Добавить обработку операндов
        }

        // Проверка и обработка комментариев
        if (asStr->comment) {
            // Добавить обработку комментариев
        }

        // Сборка объектного кода и вывод в файл
        // ...

        AssemblerString_dctor(asStr);
    }

    fclose(output_file);
}
*/

void second_parse(FILE* fp, hashTable* names, hashTable* mnemonics) {
    FILE* output_file = fopen("result.txt", "w");
    if (!fp || !output_file) {
        // Обработка ошибки открытия файла
        return;
    }

    int counter = 0;
    int obj_byte_counter = 0;
    char code_str[256];
    char obj_code_str[256], dump[256];
    obj_code_str[0] = '\0';
    dump[0] = '\0';

    while (fgets(code_str, 256, fp)) {
        AssemblerString* asStr = AssemblerString_ctor();
        ParseString(code_str, asStr);

        // Проверка и обработка метки
        if (asStr->label) {
            // Добавить обработку метки
            printf("Найдена метка: %s\n", asStr->label);
            // Пример: добавить метку в таблицу имен (names)
            if (search(asStr->label, names) == NULL) {
                DataItem* item = (DataItem*)malloc(sizeof(DataItem));
                item->key = strdup(asStr->label);
                item->info = strdup(decToHex(counter)); // или другой адрес/значение
                add(item->key, item->info, item->mark, names);
            //    insert(names, item->key, item->info);
            }
        }

        // Проверка и обработка инструкций
        if (asStr->instruction) {
            if (strcmp(asStr->instruction, "START") == 0) {
                // Обработка инструкции START
                int start = hexToDec(asStr->operand);
                counter = start;
                fprintf(output_file, "Адрес начала: %sh\n", asStr->operand);
                fprintf(output_file, "Точка входа: %sh\n", asStr->operand);
                continue;
            } else if (strcmp(asStr->instruction, "END") == 0) {
                // Обработка инструкции END
                strcat(obj_code_str, "XX\n");
                fputs(obj_code_str, output_file);
                fprintf(output_file, ":00%s01XX", decToHex(counter));
                fprintf(output_file, "\nРазмер программы: %s\n", dump);  // пример: вывод дампа
                break;
            } else {
                // Обработка остальных инструкций
                DataItem* mnemonic = search(asStr->instruction, mnemonics);
                if (mnemonic) {
                    printf("Машинная команда: %s\n", mnemonic->info);
                    strcat(obj_code_str, mnemonic->info);
                    obj_byte_counter++;
                    if (obj_byte_counter == 3) {
                        strcat(obj_code_str, "XX\n");
                        fputs(obj_code_str, output_file);
                        obj_code_str[0] = '\0';
                        obj_byte_counter = 0;
                    }
                } else {
                    printf("Ошибка: неизвестная инструкция %s\n", asStr->instruction);
                }
            }
        }

        // Проверка и обработка операндов
        if (asStr->operand) {
            // Добавить обработку операндов
            printf("Операнд: %s\n", asStr->operand);
            // Пример: добавление операнда в объектный код
            strcat(obj_code_str, asStr->operand);
        }

        // Проверка и обработка комментариев
        if (asStr->comment) {
            // Добавить обработку комментариев
            printf("Комментарий: %s\n", asStr->comment);
        }

        // Сборка объектного кода и вывод в файл
        // Пример: завершение строки объектного кода
        if (strlen(obj_code_str) > 0) {
            strcat(obj_code_str, "\n");
            fputs(obj_code_str, output_file);
            obj_code_str[0] = '\0';
        }

        counter += 3;  // Обновление счетчика, предположим, что каждая инструкция занимает 3 байта
        AssemblerString_dctor(asStr);
    }

    fclose(output_file);
}
