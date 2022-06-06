#include<stdio.h>
#include<stdlib.h>
#include <string.h>

#define STRING_BUFFER_SIZE 100
#define LINE_BUFFER_SIZE 255

// types
struct CourseStruct {
    char name[STRING_BUFFER_SIZE];       //declaring course struct
    int creditHours;
    char code[STRING_BUFFER_SIZE];
    char department[STRING_BUFFER_SIZE];
    char topics[STRING_BUFFER_SIZE][100];
    int topicsCount;
};
typedef struct CourseStruct *Course;


struct HashStruct {
    int elementsCount;
    int collisionCount;  //declaring hash struct
    Course *table;
    int size;
};
typedef struct HashStruct *Hash;


// Function types
int readAndInsertCoursesFromFile();

void printCourse(Course);

void printMenu();

void insertNewCourse();

int getHashSize(int elementsCount);


void emptyHash(Hash hash, int size);

int countCoursesFromFile();

void printHashTables();

int searchDoubleHashing(char *);

void delete(char *courseName);

void saveHashTables();

void saveHashTable(FILE *, Hash);

int searchLinearProbing(char *courseName);

int isHalfFull(Hash hash);

void insertDoubleHashing(Course);

void insertLinearProbing(Course);

void buildNewHash(int);
// globals

Hash doubleHashing, linearProbing;

int main() {
    int coursesCount = countCoursesFromFile();
    doubleHashing = (Hash) malloc(sizeof(struct HashStruct));    // declaring two hash
    linearProbing = (Hash) malloc(sizeof(struct HashStruct));
    int initialHashSize = getHashSize(coursesCount);     //get the size of the hash table

    emptyHash(doubleHashing, initialHashSize); //emptying
    emptyHash(linearProbing, initialHashSize);
    readAndInsertCoursesFromFile();
    printMenu();
    return 0;
}

void printMenu() {
    int menuOption = -1;
    while (menuOption != 9) {
        printf("1. Print hashed tables (including empty spots).\n"
               "2. Print out table size and the load factor.\n"
               "3. Print out the used hash functions.\n"
               "4. Insert a new record to hash table (insertion will be done on"
               "both hash tables).\n"
               "5. Search for a specific word (specify which table to search in).\n"
               "6. Delete a specific record (from both tables).\n"
               "7. Compare between the two methods in terms of number of\n"
               "collisions occurred.\n"
               "8. Save hash table back to a file named saved_courses.txt (of the\n"
               "double hashing)\n"
               "9. Exit\n"
               "-------------------------------------------------------------------\n");
        scanf("%d", &menuOption);
        fflush(stdin);
        switch (menuOption) {
            case 1: {
                printHashTables();
                break;
            }
            case 2: {
                double doubleHashingLoadFactor = ((double) doubleHashing->elementsCount / (double) doubleHashing->size);
                printf("Double Hashing: Table Size: %d, Load Factor: %.2f\n", doubleHashing->size,
                       doubleHashingLoadFactor);
                double linearProbingLoadFactor = ((double) linearProbing->elementsCount / (double) linearProbing->size);
                printf("Linear Probing: Table Size: %d, Load Factor: %.2f\n", linearProbing->size,
                       linearProbingLoadFactor);
                break;
            }
            case 3:
                printf("-----------------------------------------------\n");
                // % is used to escape another % character
                printf("Double Hashing Function: h(k) = ((k %% %d) + (i*(1 + k %% %d))) %% %d\n", doubleHashing->size,
                       doubleHashing->size - 1, doubleHashing->size);
                printf("Linear Probing Function: h(k) = ((k % %%d) + i) % %%d // i=0,1,2,etc... \n",
                       linearProbing->size, linearProbing->size);
                printf("-----------------------------------------------\n");
                break;
            case 4: {
                insertNewCourse();
                break;
            }
            case 5: {
                char courseName[STRING_BUFFER_SIZE];
                printf("Enter Course To Search For:\n");
                fflush(stdin);
                fgets(courseName, STRING_BUFFER_SIZE, stdin);
                courseName[strcspn(courseName, "\n")] = 0;
                int method;
                fflush(stdin);
                printf("Enter option 1 for Double Hashing, 2 for Linear Probing: \n");
                scanf("%d", &method);
                int foundedIndex = -1;
                if (method == 1) {
                    foundedIndex = searchDoubleHashing(courseName);

                } else if (method == 2) {
                    foundedIndex = searchLinearProbing(courseName);
                }
                if (foundedIndex == -1) {
                    printf("not found ...\n");
                    return;
                }
                printf("index in hash table = %d\n", foundedIndex);
                if (method == 1) {
                    printCourse(doubleHashing->table[foundedIndex]);
                } else if (method == 2) {
                    printCourse(linearProbing->table[foundedIndex]);
                }
                break;
            }
            case 6: {
                char courseName[STRING_BUFFER_SIZE];
                printf("Enter Course  Name To Delete:\n");
                // skip \n
                fflush(stdin);
                fgets(courseName, STRING_BUFFER_SIZE, stdin);
                courseName[strcspn(courseName, "\n")] = 0;
                delete(courseName);
                break;
            }

            case 7:
                printf("Double Hashing collisions: %d\n", doubleHashing->collisionCount);
                printf("Linear Probing collisions: %d\n", linearProbing->collisionCount);
                break;
            case 8:
                saveHashTables();
                break;

            case 9:
                break;
            default:
                printf("wrong option please check again...\n");
        }
    }
}

void saveHashTable(FILE *fptr, Hash hash) {
    for (int i = 0; i < hash->size; ++i) {
        if (hash->table[i] != NULL) {
            fprintf(fptr, "%d. name:%s, credit hours:%d, code:%s, department: %s,topics:", i, hash->table[i]->name,
                    hash->table[i]->creditHours, hash->table[i]->code,
                    hash->table[i]->department);
            for (int j = 0; j < hash->table[i]->topicsCount; ++j) {
                fprintf(fptr, "%s", hash->table[i]->topics[j]);
                if (j != hash->table[i]->topicsCount - 1) {
                    fprintf(fptr, ",");
                }
            }
            fprintf(fptr, "\n");
        } else {
            fprintf(fptr, "%d. NULL\n", i);
        }
    }
}

void saveHashTables() {
    FILE *fptr;
    fptr = fopen("saved_courses.txt", "w");
    if (fptr == NULL) {
        printf("there is no  file with that name\n");
        return;
    }
    fprintf(fptr, "-------------------------------Double Hashing Table  --------------------------\n");
    saveHashTable(fptr, doubleHashing);
    fprintf(fptr, "\n------------------------------- Linear Probing Hash Table --------------------------\n");
    saveHashTable(fptr, linearProbing);
    fclose(fptr);
}

void insertNewCourse() {
    int creditHours = -1, topicsCount;
    char name[STRING_BUFFER_SIZE], code[STRING_BUFFER_SIZE], department[STRING_BUFFER_SIZE];
    Course course = (Course) malloc(sizeof(struct CourseStruct));


    printf("Enter Course Name \n");
    fgets(name, STRING_BUFFER_SIZE, stdin);
    // remove new line
    name[strcspn(name, "\n")] = 0;       //inserting new course and getting its details
    strcpy(course->name, name);
    fflush(stdin);

    printf("Enter Course Code \n");
    fgets(code, STRING_BUFFER_SIZE, stdin);
    code[strcspn(code, "\n")] = 0;
    strcpy(course->code, code);
    fflush(stdin);

    printf("Enter Course Department \n");
    fgets(department, STRING_BUFFER_SIZE, stdin);
    department[strcspn(department, "\n")] = 0;
    strcpy(course->department, department);
    fflush(stdin);

    printf("Enter Course Credit Hours \n");
    scanf("%d", &creditHours);
    course->creditHours = creditHours;
    fflush(stdin);

    printf("Enter Topics Count \n");
    scanf("%d", &topicsCount);
    course->topicsCount = topicsCount;
    fflush(stdin);

    char topic[100];
    for (int i = 0; i < topicsCount; ++i) {
        printf("Enter Topic %d\n", i + 1);
        fgets(topic, STRING_BUFFER_SIZE, stdin);
        topic[strcspn(topic, "\n")] = 0;
        strcpy(course->topics[i], topic);
    }
    insertDoubleHashing(course);
    insertLinearProbing(course);
}

int countCoursesFromFile() {
    FILE *fptr;
    int coursesCount = 0;
    fptr = fopen("offered_courses.txt", "r");
    if (fptr == NULL) {
        printf("there is no  file with that name");
        return -1;
    }
    char lineBuffers[LINE_BUFFER_SIZE];
    while (fgets(lineBuffers, LINE_BUFFER_SIZE, fptr) != NULL) {
        // remove new line at the end
        lineBuffers[strcspn(lineBuffers, "\n")] = 0;
        if (lineBuffers[0] == '#' || strcmp(lineBuffers, "") == 0 || strcmp(lineBuffers, "\n") == 0) {
            continue;
        }
        coursesCount++;
    }
    return coursesCount;
}

int readAndInsertCoursesFromFile() {
    FILE *fptr;
    int coursesCount = 0;
    fptr = fopen("offered_courses.txt", "r");
    if (fptr == NULL) {
        printf("there is no  file with that name");
        return -1;
    }
    char lineBuffers[LINE_BUFFER_SIZE];

    char *tempToken;
    int currentTopic;
    struct CourseStruct *newCourse;
    while (fgets(lineBuffers, LINE_BUFFER_SIZE, fptr) != NULL) {
        // remove new line at the end
        lineBuffers[strcspn(lineBuffers, "\n")] = 0;
        if (lineBuffers[0] == '#' || strcmp(lineBuffers, "") == 0 || strcmp(lineBuffers, "\n") == 0) {
            continue;
        }
        newCourse = (Course) malloc(sizeof(struct CourseStruct));
        strcpy(newCourse->name, strtok(lineBuffers, ":"));
        newCourse->creditHours = atoi(strtok(NULL, "#"));
        strcpy(newCourse->code, strtok(NULL, "#"));
        strcpy(newCourse->department, strtok(NULL, "/"));
        tempToken = strtok(NULL, ",");
        currentTopic = 0;
        while (tempToken != NULL) {
            strcpy(newCourse->topics[currentTopic], tempToken);
            currentTopic++;
            tempToken = strtok(NULL, ",");
        }
        newCourse->topicsCount = currentTopic;
        insertDoubleHashing(newCourse);
        insertLinearProbing(newCourse);
        coursesCount++;
    }
    fclose(fptr);
    return coursesCount;
}

void printCourse(Course course) {
    printf("name:%s, credit hours:%d, code:%s, department: %s, topics: ", course->name, course->creditHours,
           course->code,
           course->department);
    for (int i = 0; i < course->topicsCount; ++i) {
        printf("%s", course->topics[i]);
        if (i != course->topicsCount - 1) {
            printf(",");
        }
    }
    printf("\n");
}

int getHashSize(int elementsCount) {///get the first bigger prime number
// table size is first bigger prime than elements count * 2
    int limit = elementsCount * 2;
    int nextPrime = limit + 1;
    int is_prime = 0;
    int i;
    while (1) { // finding the prime number
        i = 2;
        while (i <= limit) {     //checking if its the wanted number
            if (nextPrime % i == 0) {
                is_prime = 0;
                break;
            } else
                is_prime = 1;
            i++;
        }
        if (is_prime == 1)   //the prime number is found
            return nextPrime;
        nextPrime++;   //see if the next number is prime

    }
}

void emptyHash(Hash hash, int size) {
    hash->elementsCount = 0;
    hash->size = size;
    hash->collisionCount = 0;
    hash->table = (Course *) malloc(size * sizeof(Course));
    for (int i = 0; i < hash->size; ++i) {
        hash->table[i] = NULL;
    }
}

int isHalfFull(Hash hash) {
    // check load factor
    return ((double) hash->elementsCount / (double) hash->size) > 0.7 ;
}

int searchDoubleHashing(char *courseName) {
    int key = 0;
    int j = 0;
    while (courseName[j] != '\0') {
        key += courseName[j];
        j++;
    }
    int i = 1;
    int hash1 = key % doubleHashing->size;
    int index = hash1;
    int hash2= 1 + key % (doubleHashing->size - 1);




    if (doubleHashing->table[index] != NULL &&
        strcmp(doubleHashing->table[index]->name, courseName) == 0) {// found from first time
        return index;
    }
    while (doubleHashing->table[index] == NULL ? 1 : strcmp(doubleHashing
                                                                    ->table[index]->name, courseName) != 0) {  //if the course not found in that index
        {
            index = (hash1 + i * hash2) % doubleHashing->size;
            if (index == hash1) {
                return -1;
            }
            i++;
        }
    }
    if (index == doubleHashing->size) { // end reached without finding it
        return -1;
    }
    return index;
}

int searchLinearProbing(char *courseName) {
    int key = 0;
    int j = 0;
    while (courseName[j] != '\0') {
        key += courseName[j];
        j++;
    }
    int i = 1;
    int index = key % linearProbing->size; //getting index
    int hash1 = index;
    if (linearProbing->table[index] != NULL &&
        strcmp(linearProbing->table[index]->name, courseName) == 0) {// found from first time
        return index;
    }

    while (linearProbing->table[index] == NULL ? 1 : strcmp(linearProbing->table[index]->name, courseName) != 0) {  //if the course not found in the index we continue to search
        {
            index = (hash1 + i) % linearProbing->size;
            if (index == hash1) {
                return -1;
            }
            i++;
        }
    }
    if (index == linearProbing->size) { // end reached without finding it
        return -1;
    }
    return index;
}

Course cloneCourse(Course c) {
    if (c == NULL) {
        return NULL;
    }
    Course newCourse = (Course) malloc(sizeof(struct CourseStruct));
    strcpy(newCourse->name, c->name);
    strcpy(newCourse->code, c->code);
    strcpy(newCourse->department, c->department);
    newCourse->topicsCount = c->topicsCount;
    newCourse->creditHours = c->creditHours;
    for (int i = 0; i < c->topicsCount; ++i) {
        strcpy(newCourse->topics[i], c->topics[i]);
    }
    return newCourse;
}

void buildNewHash(int method) {   // build new hash, and return old entries
    Hash hash = NULL;
    if (method == 1) { // method -> 1 -> double hashing
        hash = doubleHashing;

    } else if (method == 2) {// method -> 2 -> linear probing

        hash = linearProbing;
    } else {
        return;
    }
    int oldSize = hash->size;
    Course *tempTable = (Course *) malloc(oldSize * sizeof(Course));
    int newSize = getHashSize(oldSize);
    // save old entries
    for (int i = 0; i < oldSize; ++i) {
        tempTable[i] = cloneCourse(hash->table[i]);
    }
    // create table using new size
    hash->table = (Course *) malloc(newSize * sizeof(Course));
    hash->elementsCount = 0;
    hash->size = newSize;
    // empty table
    for (int i = 0; i < newSize; ++i) {
        hash->table[i] = NULL;
    }
    for (int i = 0; i < oldSize; ++i) {
        if (tempTable[i] == NULL) {
            continue;
        }
        if (method == 1) {
            insertDoubleHashing(tempTable[i]);
        } else {                                                         // used to know which insert method to use

            insertLinearProbing(tempTable[i]);
        }
    }
}

void delete(char *courseName) {
    int index1 = searchDoubleHashing(courseName);
    int index2 = searchLinearProbing(courseName);
    if (index1 > -1) {
        doubleHashing->table[index1] = NULL;
    }
    if (index2 > -1) {
        linearProbing->table[index2] = NULL;
    }
}

// get hash index for method 1
// h(k) = k % hashSize + i*(1 + key % (hashSize - 1))
int getDoubleHashingIndex(char *courseName) {
    int key = 0;
    int j = 0;
    while (courseName[j] != '\0') {
        key += courseName[j];
        j++;
    }
    int i = 1;  // collision intial value
    int hashedIndex1 = key % doubleHashing->size;
    int hashedIndex2 = 1 + key % (doubleHashing->size - 1);
    int index = hashedIndex1;
    while (doubleHashing->table[index] != NULL)  ///apply second hash function because of collision
    {
        // increase collisions
        doubleHashing->collisionCount++;
        index = (hashedIndex1 + i * hashedIndex2) % doubleHashing->size;
        i++;
    }
    return index;
}

// get hash index for linear probing
int getLinearProbingIndex(char *courseName) {
    int key = 0;
    int j = 0, i = 0;
    while (courseName[j] != '\0') {
        key += courseName[j];
        j++;
    }
    int initialIndex = key % linearProbing->size;
    int index = initialIndex;
    while (linearProbing->table[index] != NULL) {   //when collision happen will enter this loop
        index = (initialIndex + i) % linearProbing->size;
        linearProbing->collisionCount++;
        i++;
    }

    return index;
}


void insertDoubleHashing(Course course) {
    /// when hash is half full we need to rehash
    if (isHalfFull(doubleHashing)) {
        buildNewHash(1);
    }
    int index = getDoubleHashingIndex(course->name);
    doubleHashing->table[index] = course;   //adding to the table
    doubleHashing->elementsCount++;
}

void insertLinearProbing(Course course) {
    if (isHalfFull(linearProbing)) {
        buildNewHash(2);
    }
    int index = getLinearProbingIndex(course->name);
    linearProbing->table[index] = course;    //adding to the table
    linearProbing->elementsCount++;
}

void printHashTables() {
    printf("-----------------Double Hashing Table---------------------------\n");
    for (int i = 0; i < doubleHashing->size; ++i) {
        printf("%d. ", i);
        if (doubleHashing->table[i]) {
            printCourse(doubleHashing->table[i]);
        } else {
            printf("NULL\n");
        }
    }
    printf("------------------------------------------------------\n");

    printf("-----------------Linear Probing Table--------------------------\n");
    for (int i = 0; i < linearProbing->size; ++i) {
        printf("%d. ", i);
        if (linearProbing->table[i]) {
            printCourse(linearProbing->table[i]);
        } else {
            printf("NULL\n");
        }
    }
    printf("------------------------------------------------------\n");
}
