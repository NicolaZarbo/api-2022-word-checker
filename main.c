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
bool ammissibile(char* parola);
void aggiungiAmmissibili();
bool accConfrontoLettera(char maskMode,char let1,char let2);
bool contieneAlmenoNecessarie(char *parola, int* infoSt);
bool contieneCaratteriNonPresenti(char* parola, char* nonPresenti, int quanteNP);
int posOfLetterInReferences(char i);
void confronto(char* parola);
void* eseguiComando();


//----------------------------strutture
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


//----------------globali
megaMask totalMask;
int quant;
char ** stringhe;
char** compatibili;
int ammesse;
char* ref;
char* orderedRef;
int k;
int n;
int confInPartita;
FILE* fp;
char *parolaConfronto;
refInfo references;

//------------------input


void  getString(char* toWhere){
    if(FROMFILE>0){
        fgets(toWhere, 1024, fp);
    }else{
        fgets(toWhere, 1024, stdin);
    }
}
void leggiParolaPerConfronto(){
    char word[k];
    int paroleUsate=0;//funny cosa semi ricorsiva
    word[0]= getc(stdin);
    if(word[0]=='+'){
        void (*esec)()= eseguiComando();//fixme esegui funzione ricevuta da comando
        esec();
        return ;
    }
    for (int i = 1; i < k; i++) {
        word[i] = getc(stdin);
    }
    if(ammissibile(word)){
        confronto(word);
    }else{
        printf("%s","not_exist");
    }
    confInPartita++;
}


//-------------------memoria


void aggiornaCompatibili(char  maschera[], char* parola, int* presentiInNumero, char* nonPresenti, int nNonPresenti){
    //draft senza ottimizzazioni
    int dim;
    char** provenienza;
    if(ammesse==0) {
        provenienza = stringhe;
        dim = quant;
    }else{
        provenienza=compatibili;
        dim=ammesse;
    }
    for (int i = 0; i < dim; ++i) {
        char* stringa=provenienza[i];
        bool ammissibile=true;
        for (int j = 0; j < k; ++j) {
            if(!accConfrontoLettera(maschera[j],parola[j],stringa[j]))
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
            compatibili[ammesse]=stringa;
        }

    }

}
/*
void addWords(){
    char read;
    read = (char)getc(INPUT);//lookout
    while (read!='+'){
        char word[k];
        for (int i = 1; i < k; ++i) {
            word[i]=read;
            read = (char)getc(INPUT);
        }
        salvaInMemoria(word);
        read = (char)getc(INPUT);
    }
    eseguiComando();
    // printf("%s", input );//fixme , controlla comando
}
 */
void aggiungiAmmissibili(){
    char word[k];
    int bo=1;

    do{
        word[0]= (char)getc(INPUT);
        if(word[0]=='+')
            bo=0;
        else {
            for (int i = 1; i < k; i++) {
                word[i] =(char) getc(INPUT);
            }
            salvaInMemoria(word);
        }
    } while (bo==1);
    void (*esec)()= eseguiComando();
    esec();
}
//controlla se è presente tra le ammissibili
bool ammissibile(char* parola){
    for (int i = 0; i < quant; ++i) {
        if(strcmp(compatibili[i], parola) == 0)
            return true;
    }
    return false;
}

void salvaInMemoria(char* word){
    if(stringhe==NULL){
        printf("error allocation");
        return;
    }
    printf("%s", word);
    stringhe[quant]= malloc(k*sizeof (char));
    quant++;


    //todo trovare una struttura con inserimento decentemente veloce,
    // che possa essere attraversata per intero velocemente, o ,
    // invece dell0ultima proprietà tenere dei puntatori
    // alle parole ammesse

    //dopo il primo giro di ammissibilità controllare sempre tra lista di parole compatibili
}

//------------------strutture utils


//restituisce quante volte la lettera appare
int presenzeInRef(char lettera){
    for (int i = 0; i < references.nUniche; ++i) {
        if(references.appearing[i]==lettera){
            return references.numberOfApp[i];
        }
    }
    return 0;
}
int posOfLetterInReferences(char lettera){
    for (int i = 0; i < references.nUniche; ++i) {
        if(references.appearing[i]==lettera){
            return i;
        }
    }
    return -1;
}
void orderString(char* str, char* buf){//todo test
    char sw;

    buf=str;
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
void initMegaMask(){
    posMask posMaschera[k];
    for (int i = 0; i < k; ++i) {
        char initAr[54];//fixme make of bits
        posMaschera[i].notAppear=initAr;
        posMaschera[i].sureValue=0;
    }
    totalMask.ofPos=posMaschera;
    char x=orderedRef[0];
    int cc=1;
    char letSing[k];
    letSing[0]=x;
    int nApp[k];
    nApp[0]=1;
    for (int i = 1; i < k; ++i) {
        if(orderedRef[i]!=x){
            cc++;
            x=orderedRef[i];
            letSing[cc]=x;
            nApp[cc]=1;
        }else{
            nApp[cc]++;
        }
    }
    references.nUniche=cc;
    references.appearing=letSing;
    references.numberOfApp=nApp;//fixme trim arrays to cc length
    charInfo arrInfo[cc];
    for (int i = 0; i < cc; ++i) {
        arrInfo[i].appears=0;
        arrInfo[i].letter=letSing[i];
        arrInfo[i].isMax=0;
    }
    totalMask.ofChars=arrInfo;
}




//-----------------controlli

//controlla se la lettera in questa posizione è accettabile
bool accConfrontoLettera(char maskMode,char let1,char let2){
    if(maskMode=='+'){
        return let1==let2;
    }
    return let1!=let2;
}
bool contieneAlmenoNecessarie(char *parola, int* infoSt){

    char presenti[references.nUniche];
    for (int i = 0; i < references.nUniche; ++i) {
        presenti[i]=0;
    }

    for (int i = 0; i < k; ++i) {
        int pos=posOfLetterInReferences(parola[i]);
        if(pos>=0){
            presenti[pos]++;
        }

    }
    for (int i = 0; i < references.nUniche; ++i) {
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
    char maschera[k];
    int presenti[references.nUniche];
    char nonPresenti[k];
    for (int i = 0; i < references.nUniche; ++i) {
        presenti[i]=0;
    }

    int nNP=0;
    for(int i=0;i<k;i++){
        nonPresenti[i]='0';
        int pos= posOfLetterInReferences(parola[i]);
        if(parola[i]==ref[i]){
            maschera[i]='+';
            presenti[pos]++;
        }else if(pos==-1){
            nonPresenti[nNP]=parola[i];
            nNP++;
        }
    }
    for(int i =0; i<k; i++){
        int prs=presenzeInRef(parola[i]);
        if(prs>0){
            int pos= posOfLetterInReferences(parola[i]);
            if(parola[i]!=ref[i]) {
                presenti[pos]++;
                if (prs >= presenti[pos])
                    maschera[i] = '|';
                else maschera[i] = '/';
            }
        }else{
            maschera[i]='/';
        }
    }
    printf("%s",maschera);
    aggiornaCompatibili(maschera, parola, presenti, nonPresenti, nNP);
}


//------------------comandi
void nuovaPartita(){
    //todo
    char buf[k];
    n=getc(stdin);
    ref= fgets(buf,k,stdin);
    orderString(ref,orderedRef);
    initMegaMask();
    confInPartita=0;
    while(confInPartita<n){
        leggiParolaPerConfronto();
    }
}




void stampaFiltrate(){
    //todo confronta con "stampa_filtrate" e return 0 se non corretto

    //todo trova tutte le parale che rispettano le condizioni
}

void* eseguiComando(){
    char *buf = NULL;//legge il comando da eseguire,15 -> lettere max comando fixme
    getString(buf);
    void (*fnPointer)();
    printf("%s", buf );
    if(strcmp("inserisci_inizio", buf)==0){
        fnPointer=aggiungiAmmissibili;
    }
    else if(strcmp("stampa_filtrate",buf)==0) {
        fnPointer=stampaFiltrate;
    }
    else if(strcmp("nuova_partita",buf)==0) {
        fnPointer=nuovaPartita;
    }else{
        fnPointer=leggiParolaPerConfronto;
    }
    return fnPointer;//fixme dare delle funzioni in prim ordine

}

int main(){
    if(FROMFILE>0){
        if ((fp = fopen("C:/Users/User/CLionProjects/api-2022-word-checker/open_testcases/insert.txt", "r")) == NULL){
            printf("File cannot open");
            exit(1);
        }
    }

    aggiungiAmmissibili();
    char input[12];
    getString(input);
    k= atoi(input);
    stringhe= malloc(1000*sizeof(char )*k );
    compatibili = malloc(1000 * sizeof(char ) * k);
    aggiungiAmmissibili();
    while (1>0){}// finché puo leggere


}

