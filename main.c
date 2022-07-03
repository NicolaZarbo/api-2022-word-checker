#include <string.h>
#include "stdio.h"
#include "stdlib.h"

char* ref;
char* orderedRef;
int k;
int n;

void salvaInMemoria(char *word);
int nuovaPartita(char* word);
char* ordinaStringa(char* word);
int leggiParoleInArrivo();
int isNuovaPartita(char*cmd);
void aggiungiAmmissibili();

int main(){
    k= getc(stdin);
    aggiungiAmmissibili();
    char buf[10];
    char* cmd=fgets(buf,12,stdin);
    if(nuovaPartita(cmd)!=1)
        return -1;
    //while{ leggo altri comandi }
}
int nuovaPartita(char* word){
    if(strcmp(word, "nuova_partita")!=0)//fixme is strcmp ok?
        return -1;
    //todo
    char buf[k];
    ref= fgets(buf,k,stdin);
    orderedRef=ordinaStringa(ref);
    n=getc(stdin);
    int i=0;
    while(i<n){
        i+=leggiParoleInArrivo();
    }
    return 1;
}
char* ordinaStringa(char* word){
    char* res=(char*) malloc(k);
    //todo ref ordinata su res
    return res;
}
void trovaAmmissibiliCompatibili(char*  maschera){
    //todo applica una maschera a tutti gli ammissibili per filtrarli
    // la parola testata viene messa in una lista
    // viene salvata la grandezza della lista dei filtrati e usati
}


int letteraPresente(char  lettera){//1 presente ; 0 no
        //todo se presente nella parola 1 else 0
}
void stampaMaschera(char* maschera){
    //todo stampa la maschera, se legge diverso da / o | stampa +
    // poi stampa il numero di ammissibili compatibili
}
void confronto(char* parola){
    char maschera[k];
    for(int i =0; i<k; i++){
        if(ref[i]==parola[i])
            maschera[i]=parola[i];
        else {
            if(letteraPresente(parola[i])==1)
                maschera[i]='|';
            else
                maschera[i]='/';
        }
    }
    trovaAmmissibiliCompatibili(maschera);
    stampaMaschera(maschera);
}

int stampaFiltrate(){
    //todo confronta con "stampa_filtrate" e return 0 se non corretto

    //todo trova tutte le parale che rispettano le condizioni
}

int isInserisciInizio(char* cmd){
    aggiungiAmmissibili();
    //todo leggi inserisci fine, oppure no lmao

}
void aggiungiAmmissibili(){
    char word[k];
    int bool=1;
    do{
        word[0]= getc(stdin);
        if(word[0]=='+')
            bool=0;
        else {
            for (int i = 1; i < k; i++) {
                word[i] = getc(stdin);
            }
            salvaInMemoria(word);
        }
    } while (bool==1);
}

void eseguiComando(){
    char buf[15];//legge il comando da eseguire,15 -> lettere max comando
    char *cmd= fgets(buf,15,stdin);
   // if(isNuovaPartita(cmd)==1) {
     //   nuovaPartita(cmd);
      //  return;
   // }
    if(isInserisciInizio(cmd)==1)
        return;
    if(stampaFiltrate()==1)
        return;
}

int leggiParoleInArrivo(){
    char word[k];
        word[0]= getc(stdin);
        if(word[0]=='+')
            eseguiComando();
        else {
            for (int i = 1; i < k; i++) {
                word[i] = getc(stdin);
            }
            confronto(word);
        }
}

void salvaInMemoria(char* word){
    //todo trovare una struttura con inserimento decentemente veloce,
    // che possa essere attraversata per intero velocemente, o ,
    // invece dell0ultima proprietà tenere dei puntatori
    // alle parole ammesse

    //dopo il primo giro di ammissibilità controllare sempre tra lista di parole ammissibili
}

