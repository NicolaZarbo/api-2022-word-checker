#include <string.h>
#include <stdbool.h>
#include "stdio.h"
#include "stdlib.h"

#define FROMFILE 1
#define INPUT fp

void salvaAmmissibile(char *word);
void nuovaPartita();
int leggiParolaPerConfronto();

void aggiungiAmmissibili();
bool accConfrontoLettera(int pos,char let1,char let2);
bool contieneAlmenoNecessarie(const char *parolaDelNodo);
bool contieneCaratteriNonPresenti(const char* parola, const char* nonPresenti, int quanteNP);
int posOfLetterInReferences(char lettera);
void confronto(char* parola);
void* eseguiComando();
bool presenteTraAmmissibili(char* parola);


//----------------------------strutture

typedef struct maskOfPos{
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


void salvaInCompatibili(char *parola);

//----------------globali
megaMask *totalMask;
char* parolanodo;
char* salvaPres;
char *nonPres;
int salvate;
int compatibili;//relativo alla partita
char* riferimento;
char* orderedRef;
int wordLen;
int tentativi;
int *confInPartita;//confronti fatti
FILE* fp;
refInfo * references;
char* mascheraAt;
int nPartite;
//------------------input


char *wordk;
int leggiParolaPerConfronto(){
    //wordk[0]= (char)getc(INPUT);

    char in=(char ) getc(INPUT);
    if(in==EOF){
        *confInPartita=tentativi+20;
        exit(0);
    }

    if(in == '+'){

        return 1;
    }
    wordk[0]=in;
    int i=1;
    in=(char ) getc(INPUT);
    while(in!='\n' && i<wordLen){
        wordk[i]=in;
        i++;
        in=(char ) getc(INPUT);
    }

    wordk[wordLen]= '\0';
    //if(strcmp(wordk,"sm_ks")==0){
      //  printf("ssss");
    //}
    if(presenteTraAmmissibili(wordk) ){
        confronto(wordk);
        *confInPartita=*confInPartita+1;
        return 2;
    }else{
        printf("not_exists\n");
        return 0;
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

TreeNode  *NULLO;
TreeNode *treeHead;
TreeNode *testaComp;

void deAllocaCompatibili(TreeNode *nodo){
    if(nodo->leftSon!=NULLO)
         deAllocaCompatibili(nodo->leftSon);
    if(nodo->rightSon!=NULLO)
        deAllocaCompatibili(nodo->rightSon);
    free(nodo);
}

void ruotaSX(TreeNode *nodo, TreeNode* testa){
    TreeNode *exFiglio=nodo->rightSon;
    nodo->rightSon=exFiglio->leftSon;
    if(exFiglio->leftSon != NULLO)
        exFiglio->leftSon->father=nodo;
    exFiglio->father=nodo->father;
    if(nodo->father==NULLO) {//aggiorna testa dell'albero
        if (testa == treeHead)
            treeHead = exFiglio;
        else testaComp=exFiglio;
    }
    else{
        if(nodo==nodo->father->leftSon)
            nodo->father->leftSon=exFiglio;
        else nodo->father->rightSon=exFiglio;
    }
    exFiglio->leftSon=nodo;
    nodo->father=exFiglio;
}
void ruotaDX(TreeNode *nodo, TreeNode* testa){
    TreeNode *exFiglio=nodo->leftSon;
    nodo->leftSon=exFiglio->rightSon;
    if(exFiglio->rightSon != NULLO)
        exFiglio->rightSon->father=nodo;
    exFiglio->father=nodo->father;
    if(nodo->father==NULLO) {//aggiorna testa dell'albero
        if (testa == treeHead)
            treeHead = exFiglio;
        else testaComp=exFiglio;
    }
    else{
        if(nodo==nodo->father->rightSon)
            nodo->father->rightSon=exFiglio;
        else nodo->father->leftSon=exFiglio;
    }
    exFiglio->rightSon=nodo;
    nodo->father=exFiglio;
}

/*void ruotaDxSx(TreeNode *nodo){
    ruotaDX(nodo);
    ruotaSX(nodo);
}
void ruotaSxDx(TreeNode *nodo){
    ruotaSX(nodo);
    ruotaDX(nodo);
}
 */
void sistemaInserimento(TreeNode* nuNodo, TreeNode *testa) {
    bool padreNero=nuNodo->father->isBlack;
    while (!padreNero) {
        TreeNode *nonno = nuNodo->father->father;
        if (nonno->leftSon == nuNodo->father) {
            bool nonnoNero=true;
            if(nonno->rightSon!=NULLO){
                nonnoNero=nonno->rightSon->isBlack;
            }
            if (!nonnoNero) {
                nonno->rightSon->isBlack = true;
                nonno->leftSon->isBlack = true;
                nonno->isBlack = false;
                nuNodo = nonno;
            } else {
                if (nuNodo == nuNodo->father->rightSon) {
                    nuNodo = nuNodo->father;
                    ruotaSX(nuNodo,testa);
                }
                nonno->isBlack = false;
                nuNodo->father->isBlack = true;
                ruotaDX(nonno,testa);
            }
        } else{
            bool nonnoNero=true;
            if(nonno->leftSon!=NULLO)
                nonnoNero=nonno->leftSon->isBlack;
            if(!nonnoNero){
                nonno->rightSon->isBlack = true;
                nonno->leftSon->isBlack = true;
                nonno->isBlack = false;
                nuNodo=nonno;
            } else{
                if(nuNodo == nuNodo->father->leftSon) {
                    nuNodo=nuNodo->father;
                    ruotaDX(nuNodo,testa);
                }
                nonno->isBlack = false;
                nuNodo->father->isBlack = true;
                ruotaSX(nonno,testa);
            }
        }
        if(nuNodo->father!=NULLO)
            padreNero=nuNodo->father->isBlack;
        else break;
    }
    testa->isBlack=true;
}

void scendiDaRootInsert(TreeNode *nuNodo, TreeNode *nodo){
    if(nodo!=NULLO) {//fixme perche lo ho messo??
        if (nuNodo->chiave > nodo->chiave) {
            if (nodo->rightSon != NULLO) {
                scendiDaRootInsert(nuNodo, nodo->rightSon);
                return;
            }
            nodo->rightSon = nuNodo;
        } else {
            if (nodo->leftSon != NULLO) {
                scendiDaRootInsert(nuNodo, nodo->leftSon);
                return;
            }
            nodo->leftSon = nuNodo;
        }
    }
    nuNodo->father=nodo;
    nuNodo->rightSon=NULLO;
    nuNodo->leftSon= NULLO;


}
void aggiungiNodo(int key,TreeNode *testaAlbero ){
    TreeNode *nuNodo= malloc(sizeof (TreeNode));
    nuNodo->chiave=key;
    nuNodo->isBlack=false;

    if(testaAlbero->chiave==-1){//possibile ottimizzazione, rimuovi questo case?
        nuNodo->father=NULLO;
        nuNodo->isBlack=true;
        nuNodo->leftSon=NULLO;
        nuNodo->rightSon=NULLO;
        if(testaAlbero==treeHead)
            treeHead=nuNodo;
        else testaComp=nuNodo;
    } else {
        scendiDaRootInsert(nuNodo, testaAlbero);
        sistemaInserimento(nuNodo, testaAlbero);
    }
}

TreeNode * minOfSubTree(TreeNode* nodo) {
    if(nodo->leftSon==NULLO)
        return nodo;
    return minOfSubTree(nodo->leftSon);
}
//solo per i compatibili
void sistemaCancellazione(TreeNode* nodoSost) {
    /*if(nodoSost->father==NULL&& nodoSost->leftSon==NULL&& nodoSost->rightSon==NULL) {
        testaComp = nodoSost;
        return;
    }*/
    while (nodoSost->father != NULLO && nodoSost->isBlack) {
        if (nodoSost == nodoSost->father->leftSon) {
            TreeNode *fratello = nodoSost->father->rightSon;
            if (!fratello->isBlack) {
                fratello->isBlack = true;
                nodoSost->father->isBlack = false;
                ruotaSX(nodoSost->father, testaComp);
                fratello = nodoSost->father->rightSon;
            }
            if (fratello->leftSon->isBlack && fratello->rightSon->isBlack) {
                fratello->isBlack = false;
                nodoSost = nodoSost->father;
            } else {
                if (fratello->rightSon->isBlack) {
                    fratello->leftSon->isBlack = true;
                    fratello->isBlack = false;
                    ruotaDX(fratello, testaComp);
                    fratello = nodoSost->father->rightSon;
                }
                fratello->isBlack = nodoSost->father->isBlack;
                nodoSost->father->isBlack = true;
                fratello->rightSon->isBlack = true;
                ruotaSX(nodoSost->father, testaComp);
                nodoSost->father = NULLO;//this is the new root
                treeHead = nodoSost;
                }

        } else {
            TreeNode *fratello = nodoSost->father->leftSon;
            if (!fratello->isBlack) {
                fratello->isBlack = true;
                nodoSost->father->isBlack = false;
                ruotaSX(nodoSost->father, testaComp);
                fratello = nodoSost->father->leftSon;
            }
            if (fratello->rightSon->isBlack && fratello->leftSon->isBlack) {
                fratello->isBlack = false;
                nodoSost = nodoSost->father;
            } else {
                if (fratello->leftSon->isBlack) {
                    fratello->rightSon->isBlack = true;
                    fratello->isBlack = false;
                    ruotaDX(fratello, testaComp);
                    fratello = nodoSost->father->leftSon;
                }
                fratello->isBlack = nodoSost->father->isBlack;
                nodoSost->father->isBlack = true;
                fratello->leftSon->isBlack = true;
                ruotaSX(nodoSost->father,testaComp);
                nodoSost->father = NULLO;//this is the new root
                treeHead = nodoSost;
            }
        }

    }
    nodoSost->isBlack = true;
    testaComp=nodoSost;
}
//per contrarre i compatibili
void cancellazioneNodoAlbero(TreeNode* nodo){
        //fixme rivedere
    bool wasBlack=nodo->isBlack;
    TreeNode *nodoSos;
   /* if(nodo->leftSon==NULLO&& nodo->rightSon==NULLO){
        if(nodo==testaComp) {
            testaComp->chiave=-1;
            return;
        }
        if(nodo->father->leftSon==nodo)
            nodo->father->leftSon=NULLO;
        else nodo->father->rightSon=NULLO;
        free(nodo);
        return;
    }*/
    if(nodo->leftSon==NULLO){
        nodoSos=nodo->rightSon;// fixme e se entrambi sono null????
        /*if(nodoSos!=NULLO){
            nodoSos->father=nodo->father;
        }*/
        if(nodo->father!=NULLO) {
            if (nodo->father->leftSon == nodo)
                nodo->father->leftSon = nodoSos;
            else nodo->father->rightSon = nodoSos;
        }
        nodoSos->father=nodo->father;

    }else if(nodo->rightSon==NULLO) {
        nodoSos=nodo->leftSon;
       /*if(nodoSos!=NULLO){
            nodoSos->father=nodo->father;
        }*/
        if(nodo->father!=NULLO) {
            if (nodo->father->leftSon == nodo)
                nodo->father->leftSon = nodoSos;
            else nodo->father->rightSon = nodoSos;
        }
        nodoSos->father=nodo->father;

    } else {
        TreeNode *min = minOfSubTree(nodo->rightSon);
        wasBlack = min->isBlack;
        nodoSos = min->rightSon;//fixme e se min non ha figli destri???
        if (min->father == nodo) {
            nodoSos->father = min;
        } else {
            if (min->father->leftSon == min)
                min->father->leftSon = nodoSos;//non dovrebbe mai entrare qui!!, rimuovere
            else min->father->rightSon = nodoSos;
            nodoSos->father = min->father;
            min->rightSon=nodo->rightSon;//capire meglio
            min->rightSon->father=min;
        }
        //    min->father->leftSon=min->rightSon;
        //  min->rightSon->father=min->father;


        //nodo->isBlack = wasBlack;
        if(nodo->father!=NULLO) {
            if (nodo->father->leftSon == nodo)//fixme e se è il root
                nodo->father->leftSon = min;
            else nodo->father->rightSon = min;
        }
        min->father = nodo->father;

        min->leftSon=nodo->leftSon;
        min->leftSon->father=min;
        //nodoSos = min;
        min->isBlack = nodo->isBlack;//???corretto?
        if(nodo->father==NULLO)
            testaComp=min;
    }
    if(testaComp->father!=NULLO)
        testaComp=NULLO;
    if(wasBlack)
        sistemaCancellazione(nodoSos);


   // free(nodo);//funzia??todo come faccio a deallocare, questo incasina tutto

}
char mappaInt64ToChar(int val){//todo ri-testare
    if(val<36){
        if(val==0)
            return 45;
        if(val>10) return (char)(val+54);
        return (char)(val+47);
    }
    if(val>37)
        return (char)(val+59);
    return 95;
}
int powerOfKFor(int number){
    int out=1;
    for (int i = 0; i < wordLen - 1; ++i) {
        out=out*64;
    }
    return out;
}
void chiaveToStringa(int chiave, char* target){
    int key=chiave;
    int esp= powerOfKFor(64);
    int res;
    for (int i = 0; i < wordLen; ++i) {
        res=key/(esp);//fixme ogni tanto accadono cose strane qui e va in segfault
        target[i]= mappaInt64ToChar(res);
        key-=res*esp;
        if(i != wordLen - 1)
            esp=esp/64;
    }
    target[wordLen]='\0';
}
char* debugKEY(int chiave){
    char* target= malloc(wordLen+1);
    int key=chiave;
    int esp= powerOfKFor(64);
    int res;
    for (int i = 0; i < wordLen; ++i) {
        res=key/(esp);
        target[wordLen - 1 - i]= mappaInt64ToChar(res);
        key-=res*esp;
        if(i == wordLen - 1)
            break;
        esp=esp/64;
    }
    target[wordLen]='\0';
    return target;
}
//solo per stampa filtrate
void attraversamentoOrdinato(TreeNode* vertice){
    if(vertice==NULLO){
        return;
    }
    attraversamentoOrdinato(vertice->leftSon);
    char nuf[wordLen+1];
    nuf[wordLen]='\0';
    chiaveToStringa((vertice->chiave),nuf);
  //  if(strcmp("_fah-",nuf)==0)
    //    printf("lol");
    printf("%s\n",nuf);

    attraversamentoOrdinato(vertice->rightSon);

}
int mappaCharToInt64(char x){
    if(x<91){
        if(x>64)
            return x-54;
        if(x>46)
            return x-47;
        return 0;
    }
    if(x>96)
        return x-59;
    return x-58;
}
//traduce stringa in chiave, se wordLen=5 e sizeof(int) è 4 byte => basta un int a parola!,
// in caso di problemi passerò a long o definire altro
int string2chiave(char* stringa){
    int outKey=0;
    int esp=1;
    for (int i = 0; i < wordLen; ++i) {
        outKey+=mappaCharToInt64(stringa[wordLen-1-i])*esp;
        esp=esp*64;
    }
    return outKey;
}
bool pres(int chiave, TreeNode* nodo){
    if(nodo==NULLO)
        return false;
    if(nodo->chiave==chiave)
        return true;
    if(nodo->chiave<chiave)
        return pres(chiave,nodo->rightSon);
    if(chiave<nodo->chiave)
        return pres(chiave,nodo->leftSon);
    return false;
}
bool presenteTraAmmissibili(char* parola){
    int keyOfParola= string2chiave(parola);
    return pres(keyOfParola,treeHead);
}
bool presenteTraComp(char* parola){
    int keyOfParola= string2chiave(parola);
    return pres(keyOfParola,testaComp);
}
TreeNode* trovaNodo(int val, TreeNode* nodo){
    if(nodo==NULLO)
        return NULLO;
    if(nodo->chiave<val && nodo->rightSon!=NULLO)
        return trovaNodo(val,nodo->rightSon);
    if(val<nodo->chiave && nodo->leftSon!=NULLO)
        return trovaNodo(val,nodo->leftSon);
    else return nodo;
}




/*
void testAlbero(){
    treeHead= malloc(sizeof (TreeNode));
    treeHead->chiave=-1;
    treeHead->leftSon=NULL;
    treeHead->rightSon=NULL;
    treeHead->father=NULL;
    wordLen=5;
    char* st= malloc(5*sizeof (char ));
    strcpy(st,"pigna");
    int val= string2chiave(st);
    aggiungiNodo(val,treeHead);
    strcpy(st,"Bigge");
    val= string2chiave(st);
    aggiungiNodo(val, treeHead);
    char nuf[6];
    chiaveToStringa(val,nuf);
    attraversamentoOrdinato(treeHead);
    TreeNode *target= trovaNodo(val,treeHead);
    cancellazioneNodoAlbero(target);
    attraversamentoOrdinato(treeHead);
}
 */















//-------------------memoria



bool isFromMem;
void recurAggComp(TreeNode* nodo,    char* nonPresenti, int nNonPresenti ){

    chiaveToStringa(nodo->chiave, parolanodo);
   // if(strcmp("sm_ks",wordk)==0)
     //   printf("eccoooooo   ");//todo fare in modo che le modifiche all'albero avvengano dopo aver seleczionato quelle da rimuovere, altrimenti alcuni nodi sembrano irraggiungibili
        //if(testaComp==NULLO || testaComp->father!=NULLO)
    TreeNode *sinistro=nodo->leftSon;
    TreeNode *destro=nodo->rightSon;


    chiaveToStringa(nodo->chiave, parolanodo);
    bool ammissibile=true;
    for (int j = 0; j < wordLen; ++j) {
        if(!accConfrontoLettera(j, wordk[j], parolanodo[j])) {
            ammissibile = false;
            break;
        }
    }

    if(ammissibile) {
        if (!contieneAlmenoNecessarie(parolanodo))
            ammissibile = false;
        bool xxxxxx=contieneAlmenoNecessarie(parolanodo);
    }
    if(ammissibile) {
        if (contieneCaratteriNonPresenti(parolanodo, nonPresenti, nNonPresenti))
            ammissibile = false;
    }
    TreeNode *succ=nodo->rightSon;
    //modifica compatibili
    if(ammissibile && isFromMem){
        salvaInCompatibili(parolanodo);
        if(testaComp==NULLO)
            testaComp=NULLO;
        compatibili++;
       // printf("%s    aggiunta\n",parolanodo);
    } else if(!ammissibile && !isFromMem) {
       if(presenteTraComp(parolanodo))
           printf("");
       else
           printf("no\n");
        compatibili--;
        bool test=presenteTraComp(parolanodo);
       // printf("%s  deleted\n",parolanodo);

        cancellazioneNodoAlbero(nodo);
        recurAggComp(testaComp,nonPresenti,nNonPresenti);
        //sinistro=testaComp->leftSon;
        //destro=testaComp->rightSon;
        //nodo=testaComp;
        //free(nodo);
        if (compatibili == 0) {
            printf("errore filtri, 0 parole compatibili\n");
            return;
        }

    }// else if(ammissibile && !isFromMem)
       // printf("%s   mantenuta\n",parolanodo);
    if(sinistro!=NULLO)
        recurAggComp(sinistro,   nonPresenti, nNonPresenti);

    if(destro!=NULLO)
        recurAggComp(destro,  nonPresenti, nNonPresenti);

}


void aggiornaCompatibili( char* nonPresenti, int nNonPresenti){
    //draft senza ottimizzazioni
    TreeNode *provenienza;
    if(compatibili == 0) {
        provenienza = treeHead;
        isFromMem=true;
    }else{
        provenienza = testaComp;
        isFromMem=false;
    }

    recurAggComp(provenienza , nonPresenti, nNonPresenti);
    printf("%d \n", compatibili);

}
bool rispettaTotalMask(char* string){
    //bool accettabile=true;
    if(!contieneAlmenoNecessarie(string))
        return false;
    int apparizioni[references->nUniche];//UUUUHH!
    for (int i = 0; i < references->nUniche; ++i) {
        apparizioni[i]=0;
    }
    for (int i = 0; i < wordLen; ++i) {
        if(totalMask->ofPos[i].sureValue!=string[i] && totalMask->ofPos[i].sureValue!=0)
            return false;
        if(totalMask->ofPos[i].notAppear[mappaCharToInt64(string[i])]=='1')
            return false;
    }
    return true;

}
void aggiornaCompatibiliNuovo(char* parolaNuova) {//l'ultimo elemento mancante
    if( *confInPartita ==0)
        return;
    if (rispettaTotalMask(parolaNuova) ) {
        salvaInCompatibili(parolaNuova);
    }
}


void aggiungiAmmissibili(){
    //char word[wordLen];
    int bo=1;

    do{

        int i=0;
        char in=(char)getc(INPUT);
        if(in!='+') {
            do {
                wordk[i] = in;
                in = (char)getc(INPUT);
                i++;
            } while (in != '\n' && i < wordLen);
            wordk[wordLen]='\0';
            salvaAmmissibile(wordk);
            aggiornaCompatibiliNuovo(wordk);
        }
        else{
            bo=0;
        }
    } while (bo==1);//fixme possono apparire comandi diversi da inserisci_fine?????????
    //legge inserisce_fine
    char in;
    do {
        in =(char) getc(INPUT);
    } while (in != '\n');
    //void (*esec)()= eseguiComando();
    //esec();
}



void salvaAmmissibile(char* word){
int chiave= string2chiave(word);
    aggiungiNodo(chiave,treeHead);
    salvate++;

}

void salvaInCompatibili(char* parola){
    int chiave=string2chiave(parola);
    aggiungiNodo(chiave,testaComp);

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
    //printf("badadd");
    return -1;
}

/**
 * inizializza la megaMask e references al primo giro
 */
void initPartita(){
    references= malloc(sizeof (riferimento));
    totalMask= malloc(sizeof(megaMask));
    nonPres= malloc(wordLen+1);
    nonPres[wordLen]='\0';
    posMask* posMaschera= malloc(wordLen * sizeof (posMask));//todo malloc the space for the structures
    for (int i = 0; i < wordLen; ++i) {
        char *initAr= malloc(64);//fixme make of bits
        for (int j = 0; j < 64; ++j) {
            initAr[j]=0;
        }
        posMaschera[i].notAppear=initAr;
        posMaschera[i].sureValue=0;
    }
    totalMask->ofPos=posMaschera;

    char *letSing= malloc(wordLen + 1);
    letSing[0]=orderedRef[0];
    int cc=0;
    int *nApp= malloc(wordLen * sizeof (int));
    nApp[0]=1;
    for (int i = 1; i < wordLen; ++i) {
        if(orderedRef[i]!=letSing[cc]){
            cc++;
            letSing[cc]=orderedRef[i];
            nApp[cc]=1;
        }else{
            nApp[cc]++;
        }
    }
    compatibili=0;
    letSing[cc+1]='\0';
    references->nUniche=cc+1;
    references->appearing=letSing;
    references->numberOfApp=nApp;
   // int *isMax= malloc(cc* sizeof(int));

    charInfo *arrInfo= malloc(sizeof (charInfo)*(cc+1));
    for (int i = 0; i < cc+1; ++i) {
        arrInfo[i].appears=0;
        arrInfo[i].letter=letSing[i];
        arrInfo[i].isMax=0;
    }
    totalMask->ofChars=arrInfo;
    nPartite++;
}

void initSuccessivi(){//todo qualcosa di molto brutto accade qui da qualche parte
    deAllocaCompatibili(testaComp);
    //rinnovo compatibili
    testaComp= malloc(sizeof (TreeNode));//si perde ogni volta la testa temporanea, si potrebbe riusare il puntatore
    testaComp->father=NULLO;
    testaComp->chiave=-1;
    //rinnovo strutture per confronti
    //posMask* posMaschera= totalMask->ofPos;
    for (int i = 0; i < wordLen; ++i) {
        char *initAr=totalMask->ofPos[i].notAppear;
        for (int j = 0; j < 64; ++j) {
            initAr[j]=0;
        }
        totalMask->ofPos[i].sureValue=0;
    }
    compatibili=0;
//    char *letSing= references->appearing;
    //int *nApp= references->numberOfApp;

    references->appearing[0]=orderedRef[0];
    int cc=0;

    references->numberOfApp[0]=1;
    for (int i = 1; i < wordLen; ++i) {//occhio a dimensioni e valore di cc
        if(orderedRef[i]!=references->appearing[cc]){
            cc++;
            references->appearing[cc]=orderedRef[i];
            references->numberOfApp[cc]=1;
        }else{
            references->numberOfApp[cc]++;
        }
    }
    references->appearing[cc+1]='\0';
    references->nUniche=cc+1;
    //charInfo *arrInfo=totalMask->ofChars;
    for (int i = 0; i < cc+1; ++i) {
        totalMask->ofChars[i].appears=0;
        totalMask->ofChars[i].letter=references->appearing[i];
        totalMask->ofChars[i].isMax=0;
    }
    nPartite++;
}





//-----------------controlli

//controlla se la lettera in questa posizione è accettabile
bool accConfrontoLettera(int pos,char let1,char let2){
    if(mascheraAt[pos]=='+'){
        return let1==let2;
    }
    return let1!=let2;
}
bool contieneAlmenoNecessarie(const char *parolaDelNodo){
    bool ok=true;//todo presenti deve essere un array ordinato come references.uniche
    int app;
    for (int i = 0; i < references->nUniche; ++i) {
        app=0;
        for (int j = 0; j < wordLen; ++j) {
            if(references->appearing[i] == parolaDelNodo[j] )
                app++;
        }
        if(!((app>=totalMask->ofChars[i].appears && totalMask->ofChars[i].isMax==0) || (totalMask->ofChars[i].isMax==1 && app==totalMask->ofChars[i].appears)))
            ok=false;
    }
    return ok;
}
bool contieneCaratteriNonPresenti(const char* parola, const char* nonPresenti, int quanteNP){
    for (int i = 0; i < wordLen; ++i) {
        for (int j = 0; j < quanteNP; ++j) {
            if(parola[i]==nonPresenti[j])
                return true;
        }
    }
    return false;
}

/**
 * crea nuova maschera e aggiorna quella completa
 * @param parola parola da confrontare con riferimento
 */
void confronto(char* parola){//fixme, qualcosa non funziona, capire perché
    int dime=references->nUniche;
    int presenti[dime];
   // nonPres[wordLen];
    for (int i = 0; i < references->nUniche; ++i) {
        presenti[i]=0;
    }
    int corrette=0;
    //todo controlla maschera agg per "-s9k0"

    int nNP=0;
    for(int i=0; i < wordLen; i++){
        nonPres[i]='0';
        int pos= posOfLetterInReferences(parola[i]);
        if(parola[i] == riferimento[i]){
            mascheraAt[i]='+';
          //  salvaPres[corrette]=parola[i];
            if(pos>0)
                presenti[pos]++;
            else printf("");//lmao
            corrette++;
            totalMask->ofPos[i].sureValue=parola[i];
        }else if(pos==-1){
            nonPres[nNP]=parola[i];
            nNP++;
            totalMask->ofPos[i].notAppear[mappaCharToInt64(parola[i])]=1;
        }
    }
    //salvaPres[corrette]='\0';
    nonPres[nNP]='\0';
    for(int i =0; i < wordLen; i++) {
        int prs = presenzeInRef(parola[i]);
        if (prs > 0) {
            int pos = posOfLetterInReferences(parola[i]);
            if (parola[i] != riferimento[i]) {
                if(pos>0 && pos<wordLen){
                    presenti[pos]++;
                    if (prs >= presenti[pos]){
                        corrette++;
                        mascheraAt[i] = '|';
                    }
                }
                else mascheraAt[i] = '/';
            }
        } else {
            mascheraAt[i] = '/';
        }
    }

    // aggiornamento maschera partita
    for (int i = 0; i < references->nUniche; ++i) {
        int pres=presenti[i];
        //charInfo *tmp=&totalMask->ofChars[i];
        totalMask->ofChars[i].letter=references->appearing[i];
        if(totalMask->ofChars[i].appears< pres)
            totalMask->ofChars[i].appears= pres;
        if (references->numberOfApp[i] < pres) {
            totalMask->ofChars[i].isMax = 1;
            totalMask->ofChars[i].appears=references->numberOfApp[i];
        }
    }
    if(corrette == wordLen){
        printf("ok\n");
        *confInPartita= tentativi + 1;
        return;
    }
    printf("%s\n",mascheraAt);
    aggiornaCompatibili(  nonPres, nNP);

}








//------------------comandi
void nuovaPartita(){
    //char buff[wordLen];
    compatibili=0;
    char in=(char)getc(INPUT);
    int i=0;
    if(in!='+'){
        do{
            riferimento[i] = in;
            in = (char)getc(INPUT);
            i++;
        }while (in != '\n');
    }
   // printf("%s\n", riferimento);
    in=(char)getc(INPUT);
    i=0;
    if(in!='+') {
         do{
            wordk[i] = in;
            in = (char)getc(INPUT);
            i++;
        }while (in != '\n' );
    }
    tentativi = atoi(wordk);

    char sw;
    orderedRef= strcpy(orderedRef, riferimento);
    for (int l = 0; l < wordLen; ++l){
        for (int j =l+1; j < wordLen; ++j){//fixme un sort con complessità migliore (non fondamentale, tanto la complessità e su wordLen, wordLen^2=25, klogk=15)
            if (orderedRef[l] > orderedRef[j]){
                sw = orderedRef[l];
                orderedRef[l] = orderedRef[j];
                orderedRef[j] = sw;
            }
        }
    }
    if(nPartite==0)
        initPartita();
    else initSuccessivi();

    *confInPartita=0;
    int con=0;
    while(con < tentativi){
        *confInPartita=con;
        int xx=leggiParolaPerConfronto();
        if(xx==1){
            void (*esec)()= eseguiComando();
            esec();
        }else if(xx==2)
            con++;

    }
    if(*confInPartita == tentativi)
        printf("ko\n");

}


void stampaFiltrate(){
    if(compatibili > 0){
        attraversamentoOrdinato(testaComp);
    }else
        attraversamentoOrdinato(treeHead);
}

void* eseguiComando(){
    char buf[20];
    char in= (char)getc(INPUT);
    int i=0;
    while (in!='\n'){
        buf[i]=in;
        in= (char)getc(INPUT);
        i++;
    }
    buf[i]='\0';
    void (*fnPointer)();
   // printf("+%s", buf );
    if(strcmp("inserisci_inizio", buf)==0){
        fnPointer=aggiungiAmmissibili;
    }
    else if(strcmp("stampa_filtrate",buf)==0) {
        fnPointer=stampaFiltrate;
    }
    else if(strcmp("nuova_partita",buf)==0) {
        fnPointer=nuovaPartita;
    }//else if(strcmp("inserisci_fine",buf)==0)
       // fnPointer=leggiParolaPerConfronto;
    else return NULL;
    return fnPointer;
}

int main(){

    //testAlbero();

    if(FROMFILE>0){
        if ((fp = fopen("C:/Users/User/CLionProjects/api-2022-word-checker/open_testcases/test1.txt", "r")) == NULL){
            printf("File cannot open");
            exit(1);
        }
    }

    parolanodo= malloc(wordLen+1);
    parolanodo[wordLen]='\0';

    wordk= malloc(wordLen+1);
    wordk[wordLen]='\0';
    NULLO= malloc(sizeof(TreeNode));
    NULLO->father=NULLO;
    NULLO->leftSon=NULLO;
    NULLO->rightSon=NULLO;
    NULLO->isBlack=true;
    NULLO->chiave=-1;
    confInPartita= malloc(sizeof (int));
    *confInPartita=0;
    char input[5];
    char in=(char)getc(INPUT);
    int i=0;
    while(in!='\n' && in!='+'){
        input[i]=in;
        in=(char)getc(INPUT);
        i++;
    }
    salvaPres= malloc(wordLen);
    salvaPres[wordLen]='\0';
    wordLen= atoi(input);
    testaComp= malloc(sizeof(TreeNode));
    testaComp->chiave=-1;

    treeHead= malloc(sizeof (TreeNode));
    treeHead->chiave=-1;
    orderedRef= malloc(wordLen + 1);
    orderedRef[wordLen]='\0';
    riferimento=malloc(wordLen + 1);
    riferimento[wordLen]='\0';
    mascheraAt= malloc(wordLen + 1);
    mascheraAt[wordLen]='\0';

    aggiungiAmmissibili();
    nuovaPartita();
    char rest= (char)getc(INPUT);
    while (rest=='+'){
         void (*ptr)()=eseguiComando();
         ptr();
         rest= (char)getc(INPUT);
    }

}

