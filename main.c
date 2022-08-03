#include <string.h>
#include <stdbool.h>
#include "stdio.h"
#include "stdlib.h"

#define FROMFILE 1
#define MAX_LENGTH 1000
#define INPUT fp

void salvaInMemoria(char *word);
void nuovaPartita();
void leggiParolaPerConfronto();

void aggiungiAmmissibili();
bool accConfrontoLettera(int pos,char let1,char let2);
bool contieneAlmenoNecessarie(char *parola, int* infoSt);
bool contieneCaratteriNonPresenti(char* parola, char* nonPresenti, int quanteNP);
int posOfLetterInReferences(char i);
void confronto(char* parola);
void* eseguiComando();


//----------------------------strutture
typedef struct listaNodo{
    struct listaNodo *successivo;
    char* stringa;
}Nodo;
typedef struct mascheraOfPos{
    char sureValue;
    char* notAppear;//fixme make this int and use a map char to int related to this alphabet
}posMask;
typedef struct charInfo{
    int appears;
    int isMax;//fixme optimise
    char letter;
}charInfo;
typedef struct megaMask{
    posMask* ofPos;
    charInfo* ofChars;
}megaMask;
typedef struct refInfo{
    char* appearing;
    int* numberOfApp;
    int nUniche;
} refInfo;

bool ammissibile(char* parola, Nodo*nodo);

void salvaInCompatibili(char *stringa);

//----------------globali
megaMask *totalMask;
int quant;
int ammesse;
char* ref;
char* orderedRef;
int k;
int n;
int confInPartita;//confronti fatti
FILE* fp;
refInfo * references;
int whatthedogdoing;
Nodo *headSv;
Nodo *codaSv;
Nodo *headComp;
Nodo *codaComp;
char* mascheraAt;
//------------------input


void  getString(char* toWhere){
    char in=getc(INPUT);
    int i=0;
    while(in!='\n' && in!='+'){
        toWhere[i]=in;
        in=getc(INPUT);
        i++;
    }

    printf(" parola   %s\n",toWhere);
}

void leggiParolaPerConfronto(){
    whatthedogdoing=1;
    char word[k];
    word[0]= getc(INPUT);
    if(word[0]=='+'){
        void (*esec)()= eseguiComando();//fixme esegui funzione ricevuta da comando
        esec();
        return ;
    }
    for (int i = 1; i < k; i++) {
        word[i] = getc(INPUT);
    }
    getc(INPUT);
    if(ammissibile(word, headSv)){
        confronto(word);
        confInPartita++;
    }else{
        printf("not_exists\n");
    }
}




//-----------------------------------------------------------------------------RB Tree

typedef struct nodoAlbero{
    bool isBlack;
    struct nodoAlbero* leftSon;
    struct nodoAlbero* rightSon;
    struct nodoAlbero* father;
    int chiave;//£ parola[i]*64^i;
}TreeNode;

TreeNode *treeHead;
TreeNode *testaComp;



void ruotaSX(TreeNode *nodo){
    TreeNode *oldFather=nodo->father;
    oldFather->rightSon=nodo->leftSon;
    if(oldFather->father==NULL){
        nodo->father=NULL;
        oldFather->father=nodo;
    }else if(oldFather->father->leftSon==oldFather){
        oldFather->father->leftSon=nodo;
    } else{
        oldFather->father->rightSon=nodo;
    }
    oldFather->father=nodo;
    nodo->leftSon=oldFather;
}
void ruotaDX(TreeNode *nodo){
    TreeNode *oldFather=nodo->father;
    oldFather->leftSon=nodo->rightSon;
    if(oldFather->father==NULL){
        nodo->father=NULL;
        oldFather->father=nodo;
    }else if(oldFather->father->leftSon==oldFather){
        oldFather->father->leftSon=nodo;
    } else{
        oldFather->father->rightSon=nodo;
    }
    oldFather->father=nodo;
    nodo->rightSon=oldFather;
}

void ruotaDxSx(TreeNode *nodo){
    ruotaDX(nodo);
    ruotaSX(nodo);
}
void ruotaSxDx(TreeNode *nodo){
    ruotaSX(nodo);
    ruotaDX(nodo);
}
void sistemaInserimento(TreeNode* nuNodo){
    while (!nuNodo->father->isBlack){
        TreeNode * nonno=nuNodo->father->father;
        if(nonno->leftSon==nuNodo->father) {
            if (!nonno->rightSon->isBlack) {
                nonno->rightSon->isBlack = true;
                nonno->leftSon->isBlack = true;
                nonno->isBlack = false;
                nuNodo=nonno;//assegno nonno a nuNodo????
            } else{
                if (nuNodo == nuNodo->father->rightSon) {
                    nuNodo=nuNodo->father;//assegna padre a nuNodo????
                    ruotaSX(nuNodo);
                }
                nonno->isBlack = false;
                nuNodo->father->isBlack = true;
                ruotaDX(nonno);
            }
        } else{
            if(!nonno->leftSon->isBlack){
                nonno->rightSon->isBlack = true;
                nonno->leftSon->isBlack = true;
                nonno->isBlack = false;
                nuNodo=nonno;//assegno nonno a nuNodo????
            } else{
                if(nuNodo == nuNodo->father->leftSon) {
                    nuNodo=nuNodo->father;//assegna padre a nuNodo????
                    ruotaDX(nuNodo);
                }
                nonno->isBlack = false;
                nuNodo->father->isBlack = true;
                ruotaSX(nonno);
            }
        }
    }
}

void scendiDaRootInsert(TreeNode *nuNodo, TreeNode *nodo){
    if(nuNodo->chiave>nodo->chiave){
        if(nodo->rightSon!=NULL) {
            scendiDaRootInsert(nuNodo, nodo->rightSon);
            return;
        }
        nodo->rightSon=nuNodo;
    } else{
        if(nodo->leftSon!=NULL) {
            scendiDaRootInsert(nuNodo, nodo->leftSon);
            return;
        }
        nodo->leftSon=nuNodo;
    }
    nuNodo->father=nodo;
    nuNodo->rightSon=NULL;
    nuNodo->leftSon= NULL;

    sistemaInserimento(nuNodo);
}
void aggiungiNodo(int key){
    TreeNode *nuNodo= malloc(sizeof (TreeNode));
    nuNodo->chiave=key;
    nuNodo->isBlack=false;

    if(treeHead->chiave==-1){//possibile ottimizzazione, rimuovi questo case?
        treeHead = nuNodo;
        nuNodo->father=NULL;
        nuNodo->isBlack=true;
        nuNodo->leftSon=NULL;
        nuNodo->rightSon=NULL;
    } else scendiDaRootInsert(nuNodo,treeHead);
}

TreeNode * minOfSubTree(TreeNode* nodo) {
    if(nodo->leftSon==NULL)
        return nodo;
    return minOfSubTree(nodo->leftSon);
}
void sistemaCancellazione(TreeNode* nodoSost) {
    while (nodoSost->father == NULL || !nodoSost->isBlack) {
        if (nodoSost == nodoSost->father->leftSon) {
            TreeNode *fratello = nodoSost->father->rightSon;
            if (!fratello->isBlack) {
                fratello->isBlack = true;
                nodoSost->father->isBlack = false;
                ruotaSX(nodoSost->father);
                fratello = nodoSost->father->rightSon;
            }
            if (fratello->leftSon->isBlack && fratello->rightSon->isBlack) {
                fratello->isBlack = false;
                nodoSost = nodoSost->father;
            } else {
                if (fratello->rightSon->isBlack) {
                    fratello->leftSon->isBlack = true;
                    fratello->isBlack = false;
                    ruotaDX(fratello);
                    fratello = nodoSost->father->rightSon;
                }
                fratello->isBlack = nodoSost->father->isBlack;
                nodoSost->father->isBlack = true;
                fratello->rightSon->isBlack = true;
                ruotaSX(nodoSost->father);
                nodoSost->father = NULL;//this is the new root
                treeHead = nodoSost;
            }
        } else {
            TreeNode *fratello = nodoSost->father->leftSon;
            if (!fratello->isBlack) {
                fratello->isBlack = true;
                nodoSost->father->isBlack = false;
                ruotaSX(nodoSost->father);
                fratello = nodoSost->father->leftSon;
            }
            if (fratello->rightSon->isBlack && fratello->leftSon->isBlack) {
                fratello->isBlack = false;
                nodoSost = nodoSost->father;
            } else {
                if (fratello->leftSon->isBlack) {
                    fratello->rightSon->isBlack = true;
                    fratello->isBlack = false;
                    ruotaDX(fratello);
                    fratello = nodoSost->father->leftSon;
                }
                fratello->isBlack = nodoSost->father->isBlack;
                nodoSost->father->isBlack = true;
                fratello->leftSon->isBlack = true;
                ruotaSX(nodoSost->father);
                nodoSost->father = NULL;//this is the new root
                treeHead = nodoSost;
            }
        }
        nodoSost->isBlack = true;
    }
}
//per contrarre i compatibili
void cancellazioneNodoAlbero(TreeNode* nodo){
        //fixme rivedere
    bool wasBlack=nodo->isBlack;
    TreeNode *nodoSos;
    if(nodo->leftSon==NULL){
        nodoSos=nodo->rightSon;// fixme e se entrambi sono null????
        nodoSos->father=nodo->father;
        if(nodo->father->leftSon==nodo)
            nodo->father->leftSon=nodoSos;
        else nodo->father->rightSon=nodoSos;
    }else if(nodo->rightSon==NULL) {
            nodoSos=nodo->leftSon;
            nodoSos->father=nodo->father;
            if (nodo->father->leftSon == nodo)
                nodo->father->leftSon = nodoSos;
            else nodo->father->rightSon = nodoSos;


    } else{
            TreeNode *min= minOfSubTree(nodo->rightSon);
            wasBlack=min->isBlack;
            TreeNode *temp=min->rightSon;
            if(min==nodo->leftSon||min==nodo->rightSon){
                temp->father=min;
            }else{
                min->father->leftSon=min->rightSon;
                min->rightSon->father=min->father;


                nodo->isBlack=wasBlack;
                if (nodo->father->leftSon == nodo)
                    nodo->father->leftSon = min;
                else nodo->father->rightSon = min;
                min->father=nodo->father;
            }
            nodoSos=min;
    }
    nodoSos->isBlack=wasBlack;//???corretto?
    if(wasBlack)
        sistemaCancellazione(nodoSos);
    free(nodo);//funzia??
}
char mappaInt64ToChar(int val){
    if(val<26){
        if(val==0)
            return 45;
        return (char)(val+64);
    }
    if(val>27)
        return (char)(val+69);
    return 95;
}
int powerOfKFor(int number){
    int out=1;
    for (int i = 0; i < k-1; ++i) {
        out=out*64;
    }
    return out;
}
void chiaveToStringa(int chiave, char* target){
    int key=chiave;
    int esp= powerOfKFor(64);
    int res;
    for (int i = 0; i < k; ++i) {
        res=key/(esp);
        target[k-1-i]= mappaInt64ToChar(res);
        key-=res*esp;
        esp=esp/64;//occhio a 1/64
    }
}

void attraversamentoOrdinato(TreeNode* vertice){
    if(vertice==NULL){
        return;
    }
    attraversamentoOrdinato(vertice->leftSon);
    char nuf[6];
    chiaveToStringa((vertice->chiave),nuf);
    printf("%s\n",nuf);//todo modifica chiave-Stringa

    attraversamentoOrdinato(vertice->rightSon);

}//todo scrivere versione per scorrimento in funzione "ammissibile()"

int mappaCharToInt64(char x){
    if(x<91){
        if(x>64)
            return x-64;
        return 0;
    }
    if(x>96)
        return x-69;
    return x-68;
}
//traduce stringa in chiave, se k=5 e sizeof(int) è 4 byte => basta un int a parola!
int string2chiave(char* stringa){
    int outKey=0;
    int esp=1;
    for (int i = 0; i < k; ++i) {
        outKey+=mappaCharToInt64(stringa[i])*esp;
        esp=esp*64;
    }
    return outKey;
}




void testAlbero(){
    treeHead= malloc(sizeof (TreeNode));
    treeHead->chiave=-1;
    treeHead->leftSon=NULL;
    treeHead->rightSon=NULL;
    k=5;
    char* st= malloc(5*sizeof (char ));
    strcpy(st,"pigna");
    int val= string2chiave(st);
    aggiungiNodo(val);
    strcpy(st,"Bigge");
    val= string2chiave(st);
    aggiungiNodo(val);
    char nuf[6];
    chiaveToStringa(val,nuf);
    printf("%s\n",nuf);//todo modifica chiave-Stringa
    attraversamentoOrdinato(treeHead);
}















//-------------------memoria



void aggiornaCompatibili( char* parola, int* presentiInNumero, char* nonPresenti, int nNonPresenti){
    //draft senza ottimizzazioni
    Nodo *provenienza;
    if(ammesse==0) {
        provenienza = headSv;
    }else{
        provenienza=headComp;
    }
    while (provenienza->stringa!=NULL){
        char* stringa=provenienza->stringa;
        bool ammissibile=true;
        for (int j = 0; j < k; ++j) {
            if(!accConfrontoLettera(j,parola[j],stringa[j]))
                ammissibile=false;
        }
        if(ammissibile) {
            if (!contieneAlmenoNecessarie(stringa, presentiInNumero))
                ammissibile = false;
        }
        if(ammissibile) {
            if (contieneCaratteriNonPresenti(stringa, nonPresenti, nNonPresenti))
                ammissibile = false;
        }
        if(ammissibile){
            salvaInCompatibili(stringa);
        }
        provenienza=provenienza->successivo;
    }
    printf("%d \n",ammesse);

}

void aggiungiAmmissibili(){
    whatthedogdoing=0;
    char word[k];
    int bo=1;

    do{

        int i=0;
        char in=getc(INPUT);
        if(in!='+') {
            do {
                word[i] = in;
                in = getc(INPUT);
                i++;
            } while (in != '\n' && i < k);
            salvaInMemoria(word);
        }
        else{
            bo=0;
        }
    } while (bo==1);//fixme possono apparire comandi diversi da inserisci_fine?????????
    void (*esec)()= eseguiComando();
    esec();
}
//controlla se è presente tra le ammissibili
bool ammissibile(char* parola, Nodo* nodo){
    if(nodo->stringa==NULL)
        return false;
    if(strcmp(nodo->stringa,parola)==0){
        return true;
    }
    return ammissibile(parola, nodo->successivo);
}


void salvaInMemoria(char* word){//con lista
    Nodo *nuovo= malloc(sizeof (Nodo));
    char* str= malloc(k*sizeof (char));
    codaSv->stringa= strcpy(str,word);
    quant++;
    nuovo->stringa=NULL;
    codaSv->successivo=nuovo;
    codaSv=nuovo;


    //todo trovare una struttura con inserimento decentemente veloce,
    // che possa essere attraversata per intero velocemente, o ,
    // invece dell0ultima proprietà tenere dei puntatori
    // alle parole ammesse


}

void salvaInCompatibili(char* parola){
    Nodo *nuovo= malloc(sizeof (Nodo));
    char* str= malloc(k*sizeof (char));
    codaComp->stringa=strcpy(str,parola);
    ammesse++;
    codaComp->successivo=nuovo;
    nuovo->stringa=NULL;
    codaComp=nuovo;
}


//------------------strutture utils


//restituisce quante volte la lettera appare
int presenzeInRef(char lettera){
    for (int i = 0; i < references->nUniche; ++i) {
        if(references->appearing[i]==lettera){
            return references->numberOfApp[i];
        }
    }
    return 0;
}
int posOfLetterInReferences(char lettera){
    for (int i = 0; i < references->nUniche; ++i) {
        if(references->appearing[i]==lettera){
            return i;
        }
    }
    return -1;
}
void orderString(char* str, char* buf){//todo test
    char sw;

    buf= strcpy(buf,str);
    for (int i = 0; i < k; ++i){
        for (int j = i + 1; j < k; ++j){
            if (buf[i] > buf[j]){
                sw = buf[i];
                buf[i] = buf[j];
                buf[j] = sw;
            }
        }
    }

}
/**
 * inizializza la megaMask e references
 */
void initPartita(){
    references= malloc(sizeof (ref));
    totalMask= malloc(sizeof(megaMask));
    posMask* posMaschera= malloc(k*sizeof (posMask));//todo malloc the space for the structures
    for (int i = 0; i < k; ++i) {
        char initAr[54];//fixme make of bits
        posMaschera[i].notAppear=initAr;
        posMaschera[i].sureValue=0;
    }
    totalMask->ofPos=posMaschera;

    char *letSing= malloc(k+1);
    letSing[0]=orderedRef[0];
    int cc=0;
    int *nApp= malloc(k*sizeof (int));
    nApp[0]=1;
    for (int i = 1; i < k; ++i) {
        if(orderedRef[i]!=letSing[cc]){
            cc++;
            letSing[cc]=orderedRef[i];
            nApp[cc]=1;
        }else{
            nApp[cc]++;
        }
    }
    letSing[cc+1]='\0';
    references->nUniche=cc+1;
    references->appearing=letSing;
    references->numberOfApp=nApp;//fixme trim arrays to cc length
    charInfo *arrInfo= malloc(sizeof (charInfo)*(cc+1));
    for (int i = 0; i < cc; ++i) {
        arrInfo[i].appears=0;
        arrInfo[i].letter=letSing[i];
        arrInfo[i].isMax=0;
    }
    totalMask->ofChars=arrInfo;
}





//-----------------controlli

//controlla se la lettera in questa posizione è accettabile
bool accConfrontoLettera(int pos,char let1,char let2){
    if(mascheraAt[pos]=='+'){
        return let1==let2;
    }
    return let1!=let2;
}
bool contieneAlmenoNecessarie(char *parola, int* infoSt){

    char presenti[references->nUniche];
    for (int i = 0; i < references->nUniche; ++i) {
        presenti[i]=0;
    }

    for (int i = 0; i < k; ++i) {
        int pos=posOfLetterInReferences(parola[i]);
        if(pos>=0){
            presenti[pos]++;
        }

    }
    for (int i = 0; i < references->nUniche; ++i) {
        if(infoSt[i]!=presenti[i])
            return false;
    }
    return true;
}
bool contieneCaratteriNonPresenti(char* parola, char* nonPresenti, int quanteNP){
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < quanteNP; ++j) {
            if(parola[i]==nonPresenti[j])
                return true;
        }
    }
    return false;
}

/**
 * crea nuova maschera e aggiorna quella completa
 * @param parola parola da confrontare con ref
 */
void confronto(char* parola){
    //char maschera[k];
    int dime=references->nUniche;
    int presenti[dime];
    char nonPresenti[k];
    for (int i = 0; i < references->nUniche; ++i) {
        presenti[i]=0;
    }
    int corrette=0;

    int nNP=0;
    for(int i=0;i<k;i++){
        nonPresenti[i]='0';
        int pos= posOfLetterInReferences(parola[i]);//fixme doesnt work
        if(parola[i]==ref[i]){
            mascheraAt[i]='+';
            presenti[pos]++;
            corrette++;
            totalMask->ofPos[i].sureValue=parola[i];
        }else if(pos==-1){
            nonPresenti[nNP]=parola[i];
            nNP++;
            totalMask->ofPos[i].notAppear+=parola[i];
        }
    }
    nonPresenti[nNP]='\0';
    for(int i =0; i<k; i++) {
        int prs = presenzeInRef(parola[i]);
        if (prs > 0) {
            int pos = posOfLetterInReferences(parola[i]);
            if (parola[i] != ref[i]) {
                presenti[pos]++;
                if (prs >= presenti[pos])
                    mascheraAt[i] = '|';
                else mascheraAt[i] = '/';
            }
        } else {
            mascheraAt[i] = '/';
        }
    }

    // aggiornamento maschera partita
    for (int i = 0; i < references->nUniche; ++i) {
        int pres=presenti[i];
        charInfo *tmp=&totalMask->ofChars[i];
        tmp->letter=references->appearing[i];
        tmp->appears= pres;
        if (references->numberOfApp[i] == pres) {
            tmp->isMax = 1;
        }
    }
    if(corrette==k){
        printf("ok\n");
        confInPartita=n+1;
        return;
    }
    printf("%s\n",mascheraAt);
    aggiornaCompatibili( parola, presenti, nonPresenti, nNP);

}








//------------------comandi
void nuovaPartita(){
    //todo
    char buff[k];
    ammesse=0;
    char in=getc(INPUT);
    int i=0;
    if(in!='+'){
        do{
            ref[i] = in;
            in = getc(INPUT);
            i++;
        }while (in != '\n');
    }
    printf("%s\n",ref);
    in=getc(INPUT);
    i=0;
    if(in!='+') {
         do{
            buff[i] = in;
            in = getc(INPUT);
            i++;
        }while (in != '\n' );
    }
    n = atoi(buff);
    char sw;

    orderedRef= strcpy(orderedRef,ref);
    for (int l = 0; l < k; ++l){
        for (int j =l+1; j < k; ++j){//fixme +1 ha senso??
            if (orderedRef[l] > orderedRef[j]){
                sw = orderedRef[l];
                orderedRef[l] = orderedRef[j];
                orderedRef[j] = sw;
            }
        }
    }

    initPartita();
    confInPartita=0;
    while(confInPartita<n){
        leggiParolaPerConfronto();
    }
    if(confInPartita==n)
        printf("ko\n");

}


void stampaCompList(Nodo*nodo){
    if(nodo->stringa==NULL)
        return;
    printf("%s\n",nodo->stringa);
    stampaCompList(nodo->successivo);
}

void stampaFiltrate(){
    if(ammesse>0){
        stampaCompList(headComp);
    }else
        stampaCompList(headSv);
}

void* eseguiComando(){
    char buf[20];//legge il comando da eseguire,15 -> lettere max comando fixme

    fgets(buf,20,INPUT);
    void (*fnPointer)();
    printf("+%s", buf );
    if(strcmp("inserisci_inizio\n", buf)==0){
        fnPointer=aggiungiAmmissibili;
    }
    else if(strcmp("stampa_filtrate\n",buf)==0) {
        fnPointer=stampaFiltrate;
    }
    else if(strcmp("nuova_partita\n",buf)==0) {
        fnPointer=nuovaPartita;
    }else{
        if(whatthedogdoing==0){
            fnPointer=aggiungiAmmissibili;
        }else{
            fnPointer=leggiParolaPerConfronto;
        }
    }
    return fnPointer;//fixme dare delle funzioni in prim ordine

}

int main(){
    testAlbero();
    /*
    if(FROMFILE>0){
        if ((fp = fopen("C:/Users/User/CLionProjects/api-2022-word-checker/open_testcases/test1.txt", "r")) == NULL){
            printf("File cannot open");
            exit(1);
        }
    }

    char input[5];
    char in=getc(INPUT);
    int i=0;
    while(in!='\n' && in!='+'){
        input[i]=in;
        in=getc(INPUT);
        i++;
    }

    k= atoi(input);
    headSv= malloc(sizeof (Nodo));
    codaSv=headSv;
    headComp=  malloc(sizeof (Nodo));
    codaComp=headComp;
    orderedRef= malloc(k+1);
    orderedRef[k]='\0';
    ref=malloc(k+1);
    ref[k]='\0';
    mascheraAt= malloc(k+1);
    mascheraAt[k]='\0';

    aggiungiAmmissibili();
    char rest= getc(INPUT);
    while (rest=='+'){
         void (*ptr)()=eseguiComando();
         ptr();
         rest= getc(INPUT);
    }
     */
}

