#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 해시테이블의 노드(데이터 저장 단위)를 정의하는 구조체입니다.
typedef struct HashNode {
    char *key;              // (1) 데이터를 구분하기 위한 문자열 키입니다.
    int value;              // (2) 키에 대응하는 값 (여기서는 정수형)을 저장합니다.
    struct HashNode *next;  // (3) 동일한 해시 버킷에서 충돌이 발생할 경우, 다음 노드를 가리키기 위한 포인터입니다.
} HashNode;

// 해시테이블 자체를 나타내는 구조체를 정의합니다.
typedef struct HashTable {
    int size;           // (1) 해시테이블의 총 버킷(슬롯) 개수입니다.
    HashNode **table;   // (2) 각 버킷은 HashNode 포인터(연결 리스트의 첫 노드)를 저장하는 배열입니다.
} HashTable;

// 해시테이블을 생성하는 함수입니다. 입력받은 크기(size)만큼의 버킷을 가진 해시테이블을 동적할당합니다.
HashTable* createTable(int size) {
    // 해시테이블 구조체를 위한 메모리를 동적할당합니다.
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) {  // 메모리 할당 실패 여부를 확인합니다.
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    ht->size = size;   // 해시테이블의 크기를 설정합니다.
    // 버킷(포인터 배열)을 위한 메모리를 동적할당합니다.
    ht->table = malloc(sizeof(HashNode*) * size);
    if (ht->table == NULL) {  // 메모리 할당 실패 시 확인합니다.
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    // 모든 버킷을 아직 데이터가 없으므로 NULL로 초기화합니다.
    for (int i = 0; i < size; i++) {
        ht->table[i] = NULL;
    }
    return ht;  // 생성된 해시테이블의 주소를 반환합니다.
}

// 문자열 키를 받아서 해시값(버킷 인덱스)을 계산하는 함수입니다.
// 문자열의 각 문자 값을 인덱스와 곱하고, 테이블 크기로 나눈 나머지를 구합니다.
unsigned int hashFunction(const char* key, int size) {
    unsigned int hash = 0;  // 해시 값을 저장할 변수를 0으로 초기화합니다.
    // 문자열의 각 문자에 대해 반복합니다.
    for (int i = 0; key[i] != '\0'; i++) {
        // key[i]는 i번째 문자의 아스키 코드 값입니다.
        // i와 곱하여 가중치를 주고, 테이블 크기(size)로 나눈 나머지를 구하여 항상 유효한 인덱스로 만듭니다.
        hash = (hash + key[i] * i) % size;
    }
    return hash;  // 계산된 해시값을 반환합니다.
}

// 해시테이블에 (키, 값) 쌍을 저장하는 함수입니다.
void set(HashTable *ht, const char *key, int value) {
    // 키를 사용해 어떤 버킷(배열 인덱스)에 저장할지를 결정합니다.
    unsigned int index = hashFunction(key, ht->size);

    // 새로운 노드를 생성하기 위해 메모리를 동적할당합니다.
    HashNode *newNode = malloc(sizeof(HashNode));
    if (!newNode) {  // 메모리 할당 실패를 확인합니다.
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    // strdup 함수를 사용해 key 문자열을 새 메모리에 복사합니다.
    // (strdup은 문자열을 위한 메모리 할당과 복사를 동시에 수행합니다.)
    newNode->key = strdup(key);
    newNode->value = value;   // key에 대응하는 값을 저장합니다.
    // 현재 버킷에 이미 데이터가 존재한다면, 새 노드의 next가 기존 노드를 가리키도록 하여 연결 리스트(체인)로 만듭니다.
    newNode->next = ht->table[index];
    // 버킷의 시작점을 새롭게 생성한 노드로 변경합니다.
    ht->table[index] = newNode;
}

// 주어진 키에 해당하는 값을 검색해서 반환하는 함수입니다.
int get(HashTable *ht, const char *key) {
    // key를 해시 함수로 인덱스로 변환합니다.
    unsigned int index = hashFunction(key, ht->size);
    // 해당 인덱스(버킷)의 첫 번째 노드를 가져옵니다.
    HashNode *node = ht->table[index];
    // 연결 리스트(체인)를 따라가며 원하는 key를 찾습니다.
    while (node != NULL) {
        // 문자열 비교 함수 strcmp를 사용해서 두 문자열이 동일한지 확인합니다.
        if (strcmp(node->key, key) == 0) {
            return node->value;  // 일치하는 키를 찾으면 해당 값을 반환합니다.
        }
        node = node->next;  // 다음 노드로 이동합니다.
    }
    // 만약 키를 찾지 못하면 -1을 반환합니다. (-1은 "존재하지 않는 값"의 의미로 사용)
    return -1;
}



// 해시테이블이 사용한 모든 메모리를 해제하는 함수입니다.
void freeTable(HashTable *ht) {
    // 모든 버킷을 순회합니다.
    for (int i = 0; i < ht->size; i++) {
        HashNode *node = ht->table[i];
        // 버킷에 연결된 모든 노드를 순회하며 메모리 해제합니다.
        while (node) {
            HashNode *temp = node;
            node = node->next;
            // strdup로 할당된 key 문자열 메모리도 해제합니다.
            free(temp->key);
            free(temp);  // 노드 자신을 해제합니다.
        }
    }
    free(ht->table);  // 버킷 배열의 메모리를 해제합니다.
    free(ht);         // 해시테이블 구조체의 메모리를 해제합니다.
}

// main 함수는 해시테이블의 동작을 확인하기 위한 예제입니다.
int main() {
    // 크기가 50인 해시테이블을 생성합니다.
    HashTable *ht = createTable(50);

    // 데이터를 저장합니다. 각 호출은 (키, 값) 쌍을 해시테이블에 추가합니다.
    set(ht, "apple", 100);
    set(ht, "banana", 200);
    set(ht, "orange", 300);

    // "apple" 키로 값을 검색하고 결과에 따라 출력합니다.
    int value = get(ht, "apple");
    if (value != -1)
        printf("Key 'apple' 에 대한 값: %d\n", value);
    else
        printf("Key 'apple' 를 찾지 못했습니다.\n");

    // "banana" 키로 값을 검색하고 결과에 따라 출력합니다.
    value = get(ht, "banana");
    if (value != -1)
        printf("Key 'banana' 에 대한 값: %d\n", value);
    else
        printf("Key 'banana' 를 찾지 못했습니다.\n");

    // 없는 키 "grape"를 검색하여, 없음을 확인합니다.
    value = get(ht, "grape");
    if (value != -1)
        printf("Key 'grape' 에 대한 값: %d\n", value);
    else
        printf("Key 'grape' 를 찾지 못했습니다.\n");

    // 사용한 메모리를 모두 해제하여 메모리 누수를 방지합니다.
    freeTable(ht);
    return 0;
}



