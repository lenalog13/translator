
#include "translator.h" 




int main()
{   
    hashTable* mnemonics = newHashTable();
    initMnemonicsTable(mnemonics);
    hashTable* names = newHashTable();

    FILE* fp; // Объявление файла
    fp = fopen("code.asm", "r"); // Открытие файла
    if (fp == NULL) {
        printf("Ошибка при открытии файла.\n"); // Проверка возможности открытия файла
        return 1; // При невозможности открытия программа завершается
    }
    first_parse(fp, names);
    fclose(fp);
    fp = fopen("code.asm", "r"); // Открытие файла
    if (fp == NULL) {
        printf("Ошибка при открытии файла.\n"); // Проверка возможности открытия файла
        return 1; // При невозможности открытия программа завершается
    }
    second_parse(fp, names, mnemonics);
}