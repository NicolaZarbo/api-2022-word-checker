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
//char ** stringhe;
//char** compatibili;
int ammesse;
char* ref;
char* orderedRef;
int k;
int n;
int confInPartita;//confronti fatti
FILE* fp;
char *parolaConfronto;
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
        /*
        word[0]= (char)getc(INPUT);
        if(word[0]=='+')
            bo=0;
        else {
            for (int i = 1; i < k; i++) {
                word[i] =(char) getc(INPUT);
            }
            */
        //getString(word);
        int i=0;
        char in=getc(INPUT);
        //word[0]=in;
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

    //dopo il primo giro di ammissibilità controllare sempre tra lista di parole compatibili
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
    //free(presenti);
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
    //orderString(ref,orderedRef);
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
    if(FROMFILE>0){
        if ((fp = fopen("C:/Users/User/CLionProjects/api-2022-word-checker/open_testcases/test1.txt", "r")) == NULL){
            printf("File cannot open");
            exit(1);
        }
    }

    //aggiungiAmmissibili();
    char input[5];
    //getString(input);
    char in=getc(INPUT);
    int i=0;
    while(in!='\n' && in!='+'){
        input[i]=in;
        in=getc(INPUT);
        i++;
    }

    k= atoi(input);
    //stringhe= malloc(1000*sizeof(char )*k );
    headSv= malloc(sizeof (Nodo));
    codaSv=headSv;
    headComp=  malloc(sizeof (Nodo));
    codaComp=headComp;
    orderedRef= malloc(k+1);//*sizeof(char)
    orderedRef[k]='\0';
    ref=malloc(k+1);//*sizeof(char)
    ref[k]='\0';
    mascheraAt= malloc(k+1);//*sizeof(char)
    mascheraAt[k]='\0';

    aggiungiAmmissibili();
    char rest= getc(INPUT);
    while (rest=='+'){
         void (*ptr)()=eseguiComando();
         ptr();
         rest= getc(INPUT);
    }// finché puo leggere


}

