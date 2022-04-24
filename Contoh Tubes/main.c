#include <stdio.h>
#include <conio.h>
#include <malloc.h>
#define bitsSize 4     //defines the number of the variables  in the function
#define limit 16        //defines the maximum number of the minterms-1 (2^bitSize-1)

struct vector            //stores the list of all minterms grouped
    {
    int paired[limit];
    };

struct Node                  //node declaration to hold the information about minterms such as number of pairs and number of pairs formed
    {
    struct Node* next;    //link to next node in the list
    int hasPaired;            //stores 1 oe 0 to denote the state of pairing
    int numberOfOnes;   //stores the number of ones in the minterm
    struct vector paired; //struct vector to denote other paired minterms
    int group;                  //denote the group according to the number of ones
    int binary[bitsSize];   //stores the binary of the till then formed paired expression
    int numberOfPairs;    //stores how many paires have been formed 4 denoted 2 paires or 1 quad
    };
typedef struct Node node;

struct implicantsTable         //Prime Implicants Table
    {
    int arr[limit][bitsSize];
    int brr[limit][limit];
    int top;                             //denotes how many prime implicants have been added till now
    int mintermCounter[limit];    //stores the number of the minterms in the particular prime implicant
    } Table;

//declarations of function prototype   .....names are descriptive somewhat of what they do

void add(int);
node* createNode(int);
void pair();
void display();
void displayTable();
node* createNodePair(node*,node*);
void binaryFill(node*,node*,node*);
void initTable();
int ifPairingPossible(node*,node*);
int ifDontCare(int);
int ifMintermPresentInImplicant(int,int);
void addPair(node*,node*);
void addToTable();
void analyseTable();
void binaryFill(node*,node*,node*);
void convertBinaryToMintermNotation(int);
int findMaxInTable(int*);
void initTable();
int numberOfImplicants(int,int*);
void pair();
void removeMintermsFromTable(int);


node *head,*head2;
int minterms;       //stores the number of minterms given excluding dont cares
int maxGroup,newMaxGroup;   /*denotes at a particular pairing level
                      what is the maximum group present and also calculates the next maximum group */

int mintermsGiven[limit];  //stores given minterms
int dontCares[limit];     //stores dont cares

int main(void)
    {
    int i,temp,dontCare=0,dontCareNumber;
    maxGroup=-1;
    newMaxGroup=-1;
    printf("Enter the numbe of minterms. excluding Don't Care..\n");
    scanf("%d",&minterms);
    if(minterms==0)
        return 1;
    for(i=0; i<limit; i++)
        mintermsGiven[i]=-1;
    for(i=0; i<limit; i++)
        dontCares[i]=-1;
    printf("Enter the minterms (not Don't Care)....\n");
    for(i=0; i<minterms; i++)
        {
        scanf("%d",&temp);
        mintermsGiven[temp]=1;
        add(temp);
        }
    printf("Are there any don't care conditions........Enter 1 for yes 0 otherwise.....\n");
    scanf("%d",&dontCare);
    if(dontCare==1)
        {
        printf("Enter the number of don't care conditions......\n");
        scanf("%d",&dontCareNumber);
        printf("Enter the dont care minterms ........\n");
        for(i=0; i<dontCareNumber; i++)
            {
            scanf("%d",&temp);
            dontCares[temp]=1;
            add(temp);
            }
        }
    Table.top=0;
    initTable();    //initialise the Prime Implicants Table with all cells -1 to denote empty.
    pair();           //do the pairing
    displayTable(); //display the Prime Implicants Table
    printf("Simplified Boolean Expression is.......");
    analyseTable();  //Analyse the table and print the result
    getch();
    return 1;
    }
int ifDontCare(int i)    //checks if a particular minterm is a dont Care
    {
    if(dontCares[i]==1)
        return 1;
    else
        return 0;
    }
void add(int n)     //creates a linked list to store given minterms
    {
    node *p,*q,*temp;
    p=createNode(n);
    if(p!=NULL)
        {
        if(head==NULL)
            {
            head=p;
            head->next=NULL;
            return;
            }
        else
            {
            q=head;
            if(p->group<head->group)
                {
                p->next=head;
                head=p;
                return;
                }
            while(q->next!=NULL&&((q->next->group)<=(p->group)))
                {
                q=q->next;
                }
            if(q->next==NULL)
                {
                q->next=p;
                p->next=NULL;
                }
            else
                {
                temp=q->next;
                q->next=p;
                p->next=temp;
                }
            }
        }
    }
void addPair(node *p,node *q)   //create a linked list to store the paired minterms
    {
    node *r,*temp;
    r=createNodePair(p,q);
    if(head2==NULL)
        {
        head2=r;
        }
    else
        {
        temp=head2;
        while(temp->next!=NULL)
            temp=temp->next;
        temp->next=r;
        }
    }
node* createNodePair(node *p,node *q)    //creates a new node using given nodes
    {
    int i,j;
    node *r;
    r=(node *)malloc(sizeof(node));
    if(r==NULL)
        printf("Insertion Failed.....\n");
    else
        {
        for(i=0; i<p->numberOfPairs; i++)
            {
            r->paired.paired[i]=p->paired.paired[i];
            }
        r->numberOfPairs=p->numberOfPairs*2;
        for(j=0; j<q->numberOfPairs; j++)
            {
            r->paired.paired[i++]=q->paired.paired[j];
            }
        r->hasPaired=0;
        r->next=NULL;
        r->group=p->group;
        binaryFill(p,q,r);
        }
    return r;
    }
void displayTable()   //display the prime implicants table
    {
    int i,j;
    printf("Prime Implicants Table.............\n");
    for(i=0; i<Table.top; i++)
        {
        convertBinaryToMintermNotation(i);
        for(j=0; j<=limit-1; j++)
            {

            if(Table.brr[i][j]==1)
                printf("   %d  ",j);
            }
        printf("\n");
        }
    }
void binaryFill(node *p,node *q,node *r)         /*fills the binary values in r
                                                    using p and q.If both bits are same keep as it is else put -1. */
    {
    int c=bitsSize-1;
    while(c!=-1)
        {
        if(p->binary[c]==q->binary[c])
            {
            r->binary[c]=p->binary[c];
            }
        else
            {
            r->binary[c]=-1;
            }
        c--;
        }
    }
node* createNode(int n)    //creates a node to store the minterm data
    {
    int c=bitsSize-1;
    node *p;
    p=(node *)malloc(sizeof(node));
    if(p==NULL)
        printf("Insertion Failed.....\n");
    else
        {
        p->numberOfOnes=0;
        p->paired.paired[0]=n;
        p->numberOfPairs=1;
        while(n!=0)
            {
            p->binary[c]=n%2;
            if(p->binary[c]==1)
                p->numberOfOnes++;
            c--;
            n=n/2;
            }
        while(c!=-1)
            {
            p->binary[c]=0;
            c--;
            }
        p->hasPaired=0;
        }
    p->group=p->numberOfOnes;
    if(p->group>maxGroup)
        maxGroup=p->group;
    return p;
    }
void initTable()
    {
    int i,j;
    for(j=0; j<=limit-1; j++)
        for(i=0; i<=limit-1; i++)
            {
            Table.brr[j][i]=-1;
            }
    }
void display()     //displays the minterms and their pairing and binary values at each pass
    {
    int c=1,count=0,j=0;
    node *p;
    p=head;
    while(p!=NULL)
        {
        j=0;
        while(count<(p->numberOfPairs))
            {
            printf("%d,",p->paired.paired[count]);
            count++;
            }
        printf("\b");
        count=0;
        printf("   ");
        while(j<bitsSize)
            {
            if(p->binary[j]==-1)
                printf("%c",'-');
            else
                printf("%d",p->binary[j]);
            j++;
            }
        printf("\n");
        c++;
        p=p->next;
        }
    }
void pair()    //does the pairing work
    {
    node *p,*q;
    int oneMatched=0;
    static int iteration=1;  //stores the iteration or pass count
    p=head;
    q=p;
    printf("Iteration  %d........\n",iteration);
    iteration++;
    display();
    newMaxGroup=-1;
    while(p->group!=maxGroup)
        {
        q=q->next;
        while(q!=NULL&&(q->group==p->group))
            {
            q=q->next;
            }
        if(q==NULL)
            {
            p=p->next;
            q=p;
            continue;
            }
        else
            {
            if(q->group!=(p->group+1))
                {
                p=p->next;
                q=p;
                continue;
                }
            if(ifPairingPossible(p,q))        /*checks if pairing possible and if yes put hasPaired value 1 and
                                                     add them to the new linked list...*/
                {
                oneMatched=1;
                p->hasPaired=1;
                q->hasPaired=1;
                addPair(p,q);
                if((p->group)>newMaxGroup)
                    newMaxGroup=p->group;
                }
            }
        }
    addToTable();
    if(oneMatched)      //checks if atleast one pair has been formed else it returns
        {
        head=head2;
        head2=NULL;
        maxGroup=newMaxGroup;
        pair();
        }
    }
void addToTable()
    {
    int i,j,k,allMatched;
    node *p;
    p=head;
    while(p!=NULL)
        {
        if(!(p->hasPaired))
            {
            if(Table.top!=0) //checking for duplicates
                {
                for(j=0; j<Table.top; j++)
                    {
                    allMatched=1;
                    for(k=0; k<p->numberOfPairs; k++)
                        {
                        if(Table.brr[j][p->paired.paired[k]]==1)
                            continue;
                        else
                            {
                            allMatched=0;
                            break;
                            }
                        }
                    if(allMatched==1)
                        {
                        break;
                        }
                    }
                if(allMatched==1)
                    {
                    p=p->next;
                    continue;
                    }
                }
            i=bitsSize-1;
            while(i!=-1)
                {
                Table.arr[Table.top][i]=p->binary[i];
                i--;
                }
            for(i=0; i<p->numberOfPairs; i++)
                {
                if(ifDontCare(p->paired.paired[i])==1)      //if minterms is in dont care dont add it to table
                    {
                    Table.brr[Table.top][p->paired.paired[i]]=-1;
                    continue;
                    }
                Table.mintermCounter[Table.top]++;
                Table.brr[Table.top][p->paired.paired[i]]=1;
                }
            Table.top++;
            }
        p=p->next;
        }
    }
int findMaxInTable(int *row)       /*finds the prime implicant which has the greatest number of minterms
                                                     unused at that point and returns the number and row is assigned the row
                                                     number..*/
    {
    int i,greatest=-1;
    for(i=0; i<Table.top; i++)
        {
        if(Table.mintermCounter[i]>greatest)
            {
            *row=i;
            greatest=Table.mintermCounter[i];
            }
        }
    return greatest;
    }
void analyseTable()      //does the analysing work  of the table
    {
    int i,j,k,greatestRow,ifFirst=1;
    int essentialPrimeImplicant[limit];      //stores the row number of all essential prime implicants
    int temp,c;
    for(i=0; i<=limit-1; i++)
        essentialPrimeImplicant[i]=-1;
    for(i=0; i<=limit-1; i++)
        {
        if(mintermsGiven[i]==1)
            {
            if(numberOfImplicants(i,&temp)==1)
                {
                essentialPrimeImplicant[i]=temp;
                }
            }
        }
    for(i=0; i<=limit-1; i++)
        {
        if(essentialPrimeImplicant[i]!=-1)
            {
            if(ifFirst!=1)
                printf(" + ");
            else
                ifFirst=0;
            convertBinaryToMintermNotation(essentialPrimeImplicant[i]);

            removeMintermsFromTable(essentialPrimeImplicant[i]);
            for(j=i+1; j<=limit-1; j++)
                {
                if(essentialPrimeImplicant[j]==essentialPrimeImplicant[i])
                    essentialPrimeImplicant[j]=-1;
                }
            essentialPrimeImplicant[i]=-1;

            }
        }
    while(findMaxInTable(&greatestRow)!=0)
        {

        if(ifFirst!=1)
            printf(" + ");
        else
            ifFirst=0;
        convertBinaryToMintermNotation(greatestRow);

        removeMintermsFromTable(greatestRow);
        }
    printf("\b");

    }
int ifMintermPresentInImplicant(int minterm,int implicant) //checks if a particular minterm is present in  a particular implicant row
    {
    if(Table.brr[implicant][minterm]==1)
        return 1;
    else
        return 0;
    }
void removeMintermsFromTable(int n)    /*given a implicant row it deletes all the minterms from it as
                                       well as delete all the minterms from that respective columns too...*/
    {
    int i,j;
    for(i=0; i<=limit-1; i++)
        {
        if(Table.brr[n][i]==1)
            {
            mintermsGiven[i]=-1;

            for(j=0; j<Table.top; j++)
                {
                if(Table.brr[j][i]==1)
                    {
                    Table.brr[j][i]=-1;
                    Table.mintermCounter[j]--;
                    }
                }
            }
        }
    }
int numberOfImplicants(int n,int *temp)     //returns in how many implicants a particular minterm is present
    {
    int i,j;
    int count=0;
    for(i=0; i<Table.top; i++)
        {
        if(Table.brr[i][n]==1)
            {
            j=i;
            count++;
            }
        }
    *temp=j;
    return count;
    }
void convertBinaryToMintermNotation(int n)   //converts and prints the binary into a variable notation
    {
    int c=0;
    /*you can change the variable symbols to use your own and will need to increase if you have to
     use more than 8 variables......*/
    char charactersNormal[]= {'A','B','C','D','E','F','G','H'};
    char charactersComplement[]= {'a','b','c','d','e','f','g','h'};
    while(c!=bitsSize)
        {
        if(Table.arr[n][c]!=-1)
            {
            if(Table.arr[n][c]==1)
                printf("%c",charactersNormal[c]);
            else
                printf("%c",charactersComplement[c]);
            }
        c++;
        }
    }
int ifPairingPossible(node *a,node *b)   //checks if there is a change of only one bit
    {
    int c=bitsSize-1;
    int ifOneDissimilar=0;
    while(c!=-1)
        {
        if(a->binary[c]!=b->binary[c])
            {
            if(ifOneDissimilar)
                return 0;
            else
                ifOneDissimilar=1;

            }
        c--;
        }
    return 1;
    }
