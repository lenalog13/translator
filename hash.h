#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>

#define TABLE_SIZE 79

typedef struct 
{ 
    int hashKey; 
    char* key;
    int mark;
    char* info;

} DataItem;

typedef struct
{
    DataItem* hashArray[TABLE_SIZE]; 
    int originalHashIndexes[TABLE_SIZE]; 
    int indexOHI;
    int itemsCount;
    int maxHashIndex;
    int collisions;

}hashTable;

hashTable* newHashTable()
{
    hashTable* HT = (hashTable*)malloc(sizeof(hashTable)); 

    for (int i = 0; i < TABLE_SIZE; i++){
        DataItem* newItem = (DataItem*)malloc(sizeof(DataItem)); 
        newItem->key = " ";
        newItem->mark = -1;
        newItem->info = " ";
        HT->hashArray[i] = newItem; 
    }

    HT->indexOHI = 0; 
    HT->itemsCount = 0; 
    HT->maxHashIndex = 0; 
    HT->collisions = 0; 
    return HT;

}

int hashCode(char* key, hashTable* HT) 
{ 
    int hash = 0;

    for (int i = 0; key[i] != '\0'; i++) {
        hash += key[i]; 
    }

    hash %= TABLE_SIZE;

    if (HT != NULL) { 
        HT->originalHashIndexes[HT->indexOHI] = hash; 
        HT->indexOHI++;
    }

    return hash; 
}

DataItem* search(char* key, hashTable* HT) 
{ 
    int hashIndex;
    int selfIndex;
    hashIndex = hashCode(key, NULL); 
    selfIndex = hashIndex - 1;
    bool firststep = true;

    while (HT->hashArray[hashIndex] != NULL && selfIndex != hashIndex) {

        if (firststep) { 
            firststep = false; selfIndex++;
        }

        if (hashIndex < 0) return NULL;

        if (strcmp(HT->hashArray[hashIndex]->key, key) == 0) {
        return HT->hashArray[hashIndex]; 
        }

        if (hashIndex == TABLE_SIZE - 1) hashIndex = 0;
        else ++hashIndex;
    }

    return NULL; 
}

void add(char* key, char* info, int mark, hashTable* HT) 
{
    DataItem* item = (DataItem*)malloc(sizeof(DataItem));
    int hashIndex;
    item->key = key;
    hashIndex = hashCode(key, HT);
    int selfIndex = hashIndex - 1;
    bool firststep = true;
    bool hasCollision = false;
    if (hashIndex < 0) return;

    while (HT->hashArray[hashIndex] != NULL && 
       strcmp(HT->hashArray[hashIndex]->key, " ") != 0 &&
       strcmp(HT->hashArray[hashIndex]->key, key) != 0 && 
       selfIndex != hashIndex)
    { 
        if (firststep){
            firststep = false;
            selfIndex++; 
        }

        if (!hasCollision) 
        { 
            HT->collisions++; 
            hasCollision = true;
        }

        if (hashIndex == TABLE_SIZE - 1)
            hashIndex = 0; else
        ++hashIndex; 
    }
    
    if (selfIndex == hashIndex) return; item->mark = mark;
    item -> info = info;
    item->hashKey = hashIndex;
    if (hashIndex > HT->maxHashIndex) HT->maxHashIndex = hashIndex; 
    HT->hashArray[hashIndex] = item;
    HT->itemsCount++;

}

void print(hashTable* HT) 
{ 
    int i;

    for (i = 0; i < TABLE_SIZE; i++) 
    {
        if (HT->hashArray[i]->mark != -1) {
            printf("%d: %8s", i, HT->hashArray[i]->key);
            printf("\n"); 
        }
    }

printf("кол-во коллизий: %d ", HT->collisions); 
}

