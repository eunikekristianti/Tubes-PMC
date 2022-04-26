#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX_SIZE 4096

int bitsSize;    //banyaknya variabel
int limit;       //angka terbesar pada minterm

struct vector {           //menyimpan list dari kumpulan minterms
    int paired[MAX_SIZE];
};

struct Node {                   //menyimpan informasi terkait minterm
    struct Node* next;
    int hasPaired;
    int numberOfOnes;           //jumlah angka 1 pada minterm
    struct vector paired;
    int group;                  //kelompok berdasarkan jumlah angka 1 nya
    int binary[MAX_SIZE];
    int numberOfPairs;
};
typedef struct Node node;

struct implicantsTable {        //Tabel Prime Implicants
    int arr[MAX_SIZE][MAX_SIZE];
    int brr[MAX_SIZE][MAX_SIZE];
    int sum;
    int mintermCounter[MAX_SIZE];
} Table;

//deklarasi fungsi dan variabel
void add(int);
node* createNode(int);
void pair();
void display();
void displayTable();
node* createNodePair(node*, node*);
void binaryFill(node*, node*, node*);
void initTable();
int ifPairingPossible(node*, node*);
int ifDontCare(int);
int ifMintermPresentInImplicant(int, int);
void addPair(node*, node*);
void addToTable();
void analyseTable();
void binaryFill(node*, node*, node*);
void convertBinaryToMintermNotation(int);
int findMaxInTable(int*);
void initTable();
int numberOfImplicants(int, int*);
void pair();
void removeMintermsFromTable(int);

node *head, *head2;
int minterms;
int maxGroup, newMaxGroup;
int mintermsGiven[MAX_SIZE];
int dontCares[MAX_SIZE];

int main(void) {
    printf("Jumlah variabel (1 sampai 12): ");
    scanf("%d",&bitsSize);
    limit = pow(2,bitsSize);

    int i, temp;
    maxGroup = -1;
    newMaxGroup = -1;

    printf("Jumlah minterm: ");
    scanf("%d",&minterms);
    if (minterms==0){
        return 1;
    }
    for (i=0; i<limit; i++){
        mintermsGiven[i] = -1;
    }
    for (i=0; i<limit; i++){
        dontCares[i] = -1;
    }

    printf("Masukkan minterms (0 sampai %d):\n", limit-1);
    for(i=0; i<minterms; i++){
        scanf("%d",&temp);
        while (temp < 0 || temp >= limit){
            printf("Masukkan kembali dalam range 0 sampai %d\n", limit-1);
            scanf("%d",&temp);
        }
        mintermsGiven[temp] = 1;
        add(temp);
    }

    Table.sum=0;
    initTable();    //inisialisasi tabel dengan nilai -1
    pair();
    displayTable(); //menampilkan tabel prime implicants
    printf("\nPersamaan boolean yang telah disederhanakan: ");
    analyseTable();  //menentukan essential prime implicants dan menampilkan hasilnya
    return 1;
}

int ifDontCare(int i) {    //mengecek apakah minterm merupakan dont Care
    if (dontCares[i] == 1){
        return 1;
    }
    else {
        return 0;
    }
}

void add(int n) {    //membuat linked list untuk menyimpan minterm
    node *p, *q, *temp;
    p = createNode(n);
    if (p != NULL){
        if (head == NULL){
            head = p;
            head->next = NULL;
            return;
        }
        else {
            q = head;
            if (p->group < head->group){
                p->next = head;
                head = p;
                return;
            }
            while (q->next != NULL && ((q->next->group) <= (p->group))){
                q = q->next;
            }
            if (q->next == NULL){
                q->next = p;
                p->next = NULL;
            }
            else {
                temp = q->next;
                q->next = p;
                p->next = temp;
            }
        }
    }
}

void addPair(node *p, node *q) {   //membuat linked list untuk menyimpan matched pairs
    node *r, *temp;
    r = createNodePair(p,q);
    if (head2 == NULL){
        head2 = r;
    }
    else {
        temp = head2;
        while (temp->next != NULL){
            temp = temp->next;
        }
        temp->next = r;
    }
}

node* createNodePair(node *p, node *q) {    //membuat node baru
    int i, j;
    node *r;
    r = (node *)malloc(sizeof(node));
    if (r == NULL){
        printf("Proses gagal\n");
    }
    else {
        for (i = 0; i < p->numberOfPairs; i++){
            r->paired.paired[i] = p->paired.paired[i];
        }
        r->numberOfPairs = p->numberOfPairs * 2;
        for (j = 0; j < q->numberOfPairs; j++){
            r->paired.paired[i++] = q->paired.paired[j];
        }
        r->hasPaired = 0;
        r->next = NULL;
        r->group = p->group;
        binaryFill(p, q, r);
    }
    return r;
}

void displayTable() {   //menampilkan tabel prime implicants
    int i, j;
    printf("\nTabel Prime Implicants:\n");
    for (i = 0; i < Table.sum; i++){
        convertBinaryToMintermNotation(i);
        for (j = 0; j <= limit-1; j++){
            if (Table.brr[i][j] == 1){
                printf("   %d  ", j);
            }
        }
        printf("\n");
    }
}

void binaryFill(node *p,node *q,node *r) {      //mengisi r dengan nilai biner menggunakan p dan qnd q
    int c = bitsSize-1;
    while (c != -1){
        if (p->binary[c] == q->binary[c]){
            r->binary[c] = p->binary[c];
        }
        else {
            r->binary[c] = -1;
        }
        c--;
    }
}

node* createNode(int n) {   //membuat node untuk menyimpan data dari minterm
    int c = bitsSize-1;
    node *p;
    p = (node *)malloc(sizeof(node));
    if(p == NULL){
        printf("Proses gagal\n");
    }
    else {
        p->numberOfOnes = 0;
        p->paired.paired[0] = n;
        p->numberOfPairs = 1;
        while (n != 0){
            p->binary[c] = n%2;
            if (p->binary[c] == 1){
                p->numberOfOnes++;
            }
            c--;
            n = n/2;
        }
        while(c != -1){
            p->binary[c] = 0;
            c--;
        }
        p->hasPaired = 0;
    }
    p->group = p->numberOfOnes;
    if (p->group > maxGroup){
        maxGroup = p->group;
    }
    return p;
}

void initTable() {
    int i, j;
    for(j = 0; j <= limit-1; j++){
        for(i = 0; i <= limit-1; i++){
            Table.brr[j][i] = -1;
        }
    }
}

void display() {     //menampilkan minterms dan datanya
    int c = 1, count = 0, j = 0;
    node *p;
    p = head;
    while (p != NULL){
        j = 0;
        while(count < (p->numberOfPairs)){
            printf("%d,", p->paired.paired[count]);
            count++;
        }
        printf("\b");
        count = 0;
        printf("   ");
        while(j < bitsSize){
            if(p->binary[j] == -1){
                printf("%c",'-');
            }
            else {
                printf("%d",p->binary[j]);
            }
            j++;
        }
        printf("\n");
        c++;
        p = p->next;
    }
}

void pair() {    //membandingkan minterm
    node *p, *q;
    int oneMatched = 0;
    static int iteration = 1;
    p = head;
    q = p;
    printf("\nIterasi ke-%d:\n", iteration);
    iteration++;
    display();
    newMaxGroup = -1;
    while (p->group != maxGroup){
        q = q->next;
        while(q != NULL && (q->group == p->group)){
            q = q->next;
        }
        if (q == NULL){
            p = p->next;
            q = p;
            continue;
        }
        else {
            if (q->group != (p->group+1)){
                p = p->next;
                q = p;
                continue;
            }
            if (ifPairingPossible(p,q)) {
                oneMatched = 1;
                p->hasPaired = 1;
                q->hasPaired = 1;
                addPair(p,q);
                if ((p->group) > newMaxGroup){
                    newMaxGroup = p->group;
                }
            }
        }
    }
    addToTable();
    if (oneMatched) {
        head = head2;
        head2 = NULL;
        maxGroup = newMaxGroup;
        pair();
    }
}

void addToTable(){
    int i, j, k, allMatched;
    node *p;
    p = head;
    while (p != NULL){
        if (!(p->hasPaired)){
            if (Table.sum!=0){
                for (j = 0; j < Table.sum; j++){
                    allMatched = 1;
                    for (k = 0; k < p->numberOfPairs; k++){
                        if (Table.brr[j][p->paired.paired[k]] == 1){
                            continue;
                        }
                        else {
                            allMatched = 0;
                            break;
                        }
                    }
                    if (allMatched == 1){
                        break;
                    }
                }
                if (allMatched == 1){
                    p = p->next;
                    continue;
                }
            }
            i = bitsSize-1;
            while (i != -1){
                Table.arr[Table.sum][i] = p->binary[i];
                i--;
            }
            for (i=0; i < p->numberOfPairs; i++){
                if (ifDontCare(p->paired.paired[i]) == 1){     //dont care tidak dimasukkan ke dalam tabel
                    Table.brr[Table.sum][p->paired.paired[i]] = -1;
                    continue;
                }
                Table.mintermCounter[Table.sum]++;
                Table.brr[Table.sum][p->paired.paired[i]] = 1;
            }
            Table.sum++;
        }
        p = p->next;
    }
}

int findMaxInTable(int *row) {      //mencari prime implicant dengan unused minterm terbanyak
    int i, greatest = -1;
    for (i = 0; i < Table.sum; i++){
        if (Table.mintermCounter[i] > greatest){
            *row = i;
            greatest = Table.mintermCounter[i];
        }
    }
    return greatest;
}

void analyseTable() {     //menentukan essential prime implicants dan menampilkan hasilnya
    int i, j, greatestRow, ifFirst = 1;
    int essentialPrimeImplicant[limit];
    int temp;
    for (i = 0; i <= limit-1; i++){
        essentialPrimeImplicant[i] = -1;
    }
    for (i = 0; i <= limit-1; i++){
        if (mintermsGiven[i] == 1){
            if (numberOfImplicants(i,&temp) == 1){
                essentialPrimeImplicant[i] = temp;
            }
        }
    }
    for (i = 0; i <= limit-1; i++){
        if (essentialPrimeImplicant[i] != -1){
            if (ifFirst != 1){
                printf(" + ");
            }
            else {
                ifFirst = 0;
            }
            convertBinaryToMintermNotation(essentialPrimeImplicant[i]);
            removeMintermsFromTable(essentialPrimeImplicant[i]);
            for (j = i+1; j <= limit-1; j++){
                if (essentialPrimeImplicant[j] == essentialPrimeImplicant[i]){
                    essentialPrimeImplicant[j] = -1;
                }
            }
            essentialPrimeImplicant[i] = -1;
        }
    }
    while (findMaxInTable(&greatestRow) != 0){
        if (ifFirst != 1){
            printf(" + ");
        }
        else {
            ifFirst = 0;
        }
        convertBinaryToMintermNotation(greatestRow);
        removeMintermsFromTable(greatestRow);
    }
    printf("\b\n");
}

int ifMintermPresentInImplicant(int minterm, int implicant) { //mengecek apakah minterm ada pada tabel
    if (Table.brr[implicant][minterm] == 1){
        return 1;
    }
    else {
        return 0;
    }
}

void removeMintermsFromTable(int n) {   //menghapus minterm dari tabel
    int i, j;
    for (i = 0; i <= limit-1; i++){
        if (Table.brr[n][i] == 1){
            mintermsGiven[i] =- 1;
            for(j = 0; j < Table.sum; j++){
                if(Table.brr[j][i] == 1){
                    Table.brr[j][i] = -1;
                    Table.mintermCounter[j]--;
                }
            }
        }
    }
}

int numberOfImplicants(int n, int *temp) {    //returns in how many implicants a particular minterm is present
    int i, j;
    int count = 0;
    for (i = 0; i < Table.sum; i++){
        if (Table.brr[i][n] == 1){
            j = i;
            count++;
        }
    }
    *temp = j;
    return count;
}

void convertBinaryToMintermNotation(int n) {  //mengkonversi bilangan biner ke dalam variabel dan menampilkannya
    int c = 0;
    char charactersNormal[] = {'A','B','C','D','E','F','G','H','I','J','K','L'};
    char charactersComplement[] = {'a','b','c','d','e','f','g','h','i','j','k','l'};
    while (c != bitsSize){
        if (Table.arr[n][c] != -1){
            if (Table.arr[n][c] == 1){
                printf("%c", charactersNormal[c]);
            }
            else {
                printf("%c", charactersComplement[c]);
            }
        }
        c++;
    }
}

int ifPairingPossible(node *a, node *b) {  //mengecek apakah 2 bilangan biner hanya berbeda 1 angka
    int c = bitsSize-1;
    int ifOneDissimilar = 0;
    while (c != -1){
        if (a->binary[c] != b->binary[c]){
            if (ifOneDissimilar){
                return 0;
            }
            else {
                ifOneDissimilar=1;
            }
        }
        c--;
    }
    return 1;
}
