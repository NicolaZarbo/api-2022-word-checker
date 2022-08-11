#include <string.h>
#include <stdbool.h>
#include "stdio.h"
#include "stdlib.h"

#define FROMFILE 0
#define INPUT stdin
#define REMOVEDBLOCK 10
#define ADDEDBLOCK 5
#define BUTTAINFILE 0

void salvaAmmissibile(char *word);
void nuovaPartita();
int leggiParolaPerConfronto();

void aggiungiAmmissibili();
bool accConfrontoLettera(int pos,char let1,char let2);
bool contieneAlmenoNecessarie(const char *parolaDelNodo);
bool contieneCaratteriNonPresenti(const char* parola, const char* nonPresenti, int quanteNP);
int posOfLetterInReferences(char lettera);
void confronto();
void* eseguiComando();
bool presenteTraAmmissibili(char* parola);


//----------------------------strutture

enum constrType{presInPosizione,quantiPresenti};
typedef struct constraintQuant{
    void (*con)();
    short precedenza;
    char lettera;
    short quant;
    bool isMax;
} quantCon;
typedef struct conPos{
    void  (*con)();
    short precedenza;
    char lettera;
    short pos;
    bool appare;
}posCon;
struct constPosList{
    posCon * nnd;
    struct constPosList *succ;
};
struct constQuanList{
    quantCon * qnd;
    struct constQuanList *succ;
};

typedef struct maskOfPos{
    char sureValue;
    char* notAppear;
}posMask;
typedef struct charInfo{
    int appears;
    int isMax;//fixme optimise
}charInfo;
typedef struct megaMask{
    posMask* ofPos;
    charInfo* ofChars;
}megaMask;
typedef struct refInfo{
    char* appearing;
    int* numberOfApp;
    int nUniche;
    char* appariscenti;
} refInfo;

void salvaInCompatibili(char *parola);

//----------------globali
megaMask *totalMask;
struct constPosList *maggiorePrecedenzaPos;
int dimMPP;
struct constPosList *nuoviPos;
int dimNP;
struct constQuanList *maggiorePrecQuant;
int dimMPQ;
struct constQuanList *nuoviQuant;
int dimNQ;
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
char* countConfronto;
//------------------input

char *wordk;
// ----------------
//----debug
FILE *ott;
int lineS;



//-----------------------------------------------------------------------------RB Tree

typedef struct nodoAlbero{
    bool isBlack;
    struct nodoAlbero* leftSon;
    struct nodoAlbero* rightSon;
    struct nodoAlbero* father;
    char* nParola;
}TreeNode;

TreeNode  *NULLO;
TreeNode *nullHead;
TreeNode *treeHead;
TreeNode *testaComp;

bool constraintRun(const char * parola);
void attraversamentoOrdinato(TreeNode* vertice);
void recycleIsGood(TreeNode* rimosso);
bool rispettaTotalMask(char* string);

void customPrinter(char* word){
    for (int i = 0; i < wordLen; ++i) {
        if(BUTTAINFILE)
            fprintf(ott,"%c",word[i]);
        printf("%c",word[i]);
    }
    if(BUTTAINFILE)
        fprintf(ott,"\n");
    printf("\n");

}
void copiaParola(char* dest, const char*from){
    for (int i = 0; i < wordLen; ++i) {
        dest[i]=from[i];
    }
}

bool stringMaggiore(const char* primo, const char* secondo){
    return strncmp(primo, secondo, wordLen)>0;
   /* for (int i = 0; i < wordLen; ++i) {
        if(primo[i]!=secondo[i]) {
            if (primo[i] > secondo[i])
                return true;
            else return false;
        }
    }
    return false;*/

}
bool stringUguale(const char* primo, const char* secondo) {
    for (int i = 0; i < wordLen; ++i) {
        if (primo[i] != secondo[i]) {
            return false;
        }
    }
    return true;
}
void deAllocaIterativo(TreeNode*nodo){
    TreeNode *attuale=nodo;
    TreeNode *precedente=NULLO;
    TreeNode * successivo;
    while (attuale!=NULLO){
        if(precedente==NULLO||precedente==attuale->father){
            if(attuale->leftSon!=NULLO)
                successivo=attuale->leftSon;
            else{
                recycleIsGood(nodo);
                if(attuale->rightSon!=NULLO)
                    successivo=attuale->rightSon;
                else successivo=attuale->father;
            }

        } else if(precedente==attuale->leftSon){
            recycleIsGood(nodo);
            if(attuale->rightSon!=NULLO)
                successivo=attuale->rightSon;
            else successivo=attuale->father;
        } else successivo=attuale->father;
        precedente=attuale;
        attuale=successivo;
    }
}
void deAllocaCompatibili(TreeNode *nodo){//rendere non ricorsivo
    if(nodo==NULLO){
        if(BUTTAINFILE)
            fprintf(ott,"errore di de-allocazione");
        printf("errore di de-allocazione");
        return;
    }
    if(nodo->leftSon!=NULLO)
         deAllocaCompatibili(nodo->leftSon);
    if(nodo->rightSon!=NULLO)
        deAllocaCompatibili(nodo->rightSon);
    if(nodo==nullHead || nodo==testaComp)
        return;
    recycleIsGood(nodo);
}
void innestaSopra(TreeNode* innesto, TreeNode* nodoOriginale, TreeNode* radice){
    if(nodoOriginale != radice){
        if(nodoOriginale == nodoOriginale->father->leftSon)
            nodoOriginale->father->leftSon=innesto;
        else nodoOriginale->father->rightSon=innesto;
    }else if(radice==testaComp){
        testaComp=innesto;
    } else treeHead=innesto;
    if(innesto!=NULLO)
        innesto->father=nodoOriginale->father;
}
void ruotaSX(TreeNode *nodo, TreeNode* testa){
    TreeNode *exFiglio=nodo->rightSon;
    nodo->rightSon=exFiglio->leftSon;
    if(exFiglio->leftSon != NULLO)
        exFiglio->leftSon->father=nodo;
    exFiglio->father=nodo->father;

    if(nodo->father==NULLO) {//aggiorna testa dell'albero???
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

void sistemaInserimento(TreeNode* nuNodo, TreeNode *testa) {
    while (!nuNodo->father->isBlack) {
        TreeNode *nonno = nuNodo->father->father;
        if (nonno->leftSon == nuNodo->father) {

            bool zioNero=nonno->rightSon->isBlack;
            if (!zioNero) {// caso 1
                nonno->rightSon->isBlack = true;
                nonno->leftSon->isBlack = true;
                nonno->isBlack = false;
                nuNodo = nonno;
                //if(padre->isBlack)
                  //  break;
            } else {
                if (nuNodo == nuNodo->father->rightSon) {//caso 2
                    nuNodo = nuNodo->father;
                    ruotaSX(nuNodo,testa);
                   // if(nuNodo->father->isBlack)//???
                     //   break;
                }
                nuNodo->father->father->isBlack=false;//nonno->isBlack = false;//caso 3?//??
                nuNodo->father->isBlack=true;//nuNodo->father->isBlack = true;
                ruotaDX(nuNodo->father->father,testa);
            }
        } else{
            bool nonnoNero=true;
            //if(nonno->leftSon!=NULLO)
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
                    if(nuNodo->father->isBlack)//???
                        break;
                }
                nuNodo->father->father->isBlack = false;
                nuNodo->father->isBlack = true;
                ruotaSX(nuNodo->father->father,testa);
            }
        }

    }
    testa->isBlack=true;


}
TreeNode * trovaNodoDaComp(char* nParola){//keyInt chiave
    TreeNode *nodo=testaComp;
    while (nodo!=NULLO){
        if(stringMaggiore(nParola, nodo->nParola))//chiave>nodo->chiave)
            nodo=nodo->rightSon;
        else if(stringMaggiore(nodo->nParola,nParola))
            nodo=nodo->leftSon;
        else return nodo;
    }
    printf("error finding node");
    return NULL;
}
TreeNode * trovaNodoDaMEM(char* nParola){//keyInt chiave
    TreeNode *nodo=treeHead;
    while (nodo!=NULLO){
        if(stringMaggiore(nParola, nodo->nParola))//chiave>nodo->chiave)
            nodo=nodo->rightSon;
        else if(stringMaggiore(nodo->nParola,nParola))
            nodo=nodo->leftSon;
        else return nodo;
    }
    printf("error finding node");
    return NULLO;
}
int aggiuntiNodi;
int aggiunteStringhe;
TreeNode *bloccoDiMalloc;
char* bloccoDiStringNU;
TreeNode * recycleEnd;
int riciclati;
TreeNode *garbodor;

TreeNode *mutGarbodor(){
    riciclati--;
    if(recycleEnd!=garbodor) {
        recycleEnd = recycleEnd->father;
        return recycleEnd->rightSon;
    }
    return recycleEnd;
}
bool isFromNuInserisci;
TreeNode *nodoFresco;

void aggiungiNodoAMem(char* nuParola){
    TreeNode *nuNodo;
    if(riciclati>0){
        //ottiene blocco di memoria riciclato
        nuNodo=mutGarbodor();
        //allocazione stringa nodo da blocco di allocazione
        nuNodo->nParola=&bloccoDiStringNU[aggiunteStringhe * wordLen];
        aggiunteStringhe++;
        if(aggiunteStringhe== ADDEDBLOCK){
            bloccoDiStringNU= malloc(sizeof(char)*wordLen*ADDEDBLOCK);//necessario, i blocchi riciclati condividono memoria con blocchi nel tree principale
            aggiunteStringhe=0;
        }
    }else {
        //allocazione nodo da blocco di allocazione
        nuNodo = &bloccoDiMalloc[aggiuntiNodi];
        aggiuntiNodi++;
        if (aggiuntiNodi == ADDEDBLOCK) {
            bloccoDiMalloc = malloc(sizeof(TreeNode) * ADDEDBLOCK);
            aggiuntiNodi = 0;
        }
        //allocazione stringa nodo da blocco di allocazione
        nuNodo->nParola=&bloccoDiStringNU[aggiunteStringhe * wordLen];
        aggiunteStringhe++;
        if(aggiunteStringhe== ADDEDBLOCK){
            bloccoDiStringNU= malloc(sizeof(char)*wordLen*ADDEDBLOCK);
            aggiunteStringhe=0;
        }

    }

    //scrittura della parola nel nodo
    copiaParola(nuNodo->nParola,nuParola);
    nuNodo->isBlack=false;
    nuNodo->father = NULLO;
    nuNodo->leftSon = NULLO;
    nuNodo->rightSon = NULLO;

    TreeNode * discensore=treeHead;
    TreeNode *padre=NULLO;

    while(discensore!=NULLO){
        padre=discensore;
        if(stringMaggiore(nuNodo->nParola,discensore->nParola))//nuNodo->chiave>discensore->chiave
            discensore= discensore->rightSon;
        else discensore=discensore->leftSon;
    }
    nuNodo->father=padre;
    if(padre==nullHead)
    {
        treeHead=nuNodo;
        nuNodo->father=NULLO;
        nuNodo->isBlack=true;
        return;
    }
    else if(stringMaggiore(nuNodo->nParola,padre->nParola))//nuNodo->chiave>padre->chiave
        padre->rightSon=nuNodo;
    else padre->leftSon=nuNodo;

    if(nuNodo->father->father==NULLO)
        return;
    if(isFromNuInserisci)//per condividere il puntatore alla memoria della stringa
        nodoFresco=nuNodo;

    sistemaInserimento(nuNodo, treeHead);
}
void aggiungiNodoPerCompatibili(char *nuParola){
    TreeNode *nuNodo;
    if(riciclati>0){
        nuNodo=mutGarbodor();
    }else {
        nuNodo = &bloccoDiMalloc[aggiuntiNodi];
        aggiuntiNodi++;
        if (aggiuntiNodi == ADDEDBLOCK) {
            bloccoDiMalloc = malloc(sizeof(TreeNode) * ADDEDBLOCK);
            //bloccoDiStringNU= malloc(sizeof(char)*wordLen*ADDEDBLOCK);
            aggiuntiNodi = 0;
        }
    }
    if(isFromNuInserisci)
        nuNodo->nParola=nodoFresco->nParola;
    else nuNodo->nParola= nuParola;//perchè deriva dal confronto
    nuNodo->isBlack=false;
    nuNodo->father = NULLO;
    nuNodo->leftSon = NULLO;
    nuNodo->rightSon = NULLO;


    TreeNode * discensore=testaComp;
    TreeNode *padre=NULLO;

    while(discensore!=NULLO){
        padre=discensore;
        if(stringMaggiore(nuNodo->nParola,discensore->nParola))//nuNodo->chiave>discensore->chiave
            discensore= discensore->rightSon;
        else discensore=discensore->leftSon;
    }
    nuNodo->father=padre;
    if(padre==nullHead)
    {
        testaComp=nuNodo;
        nuNodo->father=NULLO;
        nuNodo->isBlack=true;
        return;
    }
    else if(stringMaggiore(nuNodo->nParola,padre->nParola))//nuNodo->chiave>padre->chiave
        padre->rightSon=nuNodo;
    else padre->leftSon=nuNodo;

    if(nuNodo->father->father==NULLO)
        return;

    sistemaInserimento(nuNodo, testaComp);
}

TreeNode * minOfSubTree(TreeNode* nodo) {
    while (nodo->leftSon!=NULLO){
        nodo=nodo->leftSon;
    }
    return nodo;
}

void sistemaCancellazione(TreeNode* doppioBlack);

TreeNode *cancellaNodoV2(TreeNode* nodo){
    TreeNode *y;
    TreeNode *x;
    if(nodo->leftSon==NULLO ||nodo->rightSon==NULLO)
        y=nodo;
    else y= minOfSubTree(nodo->rightSon);
    if(y->rightSon!=NULLO)
        x=y->rightSon;
    else x=y->leftSon;
    x->father=y->father;
    if(y->father==NULLO)
        testaComp=x;
    else if(y==y->father->leftSon) { y->father->leftSon = x; }
    else y->father->rightSon=x;

    if(y!=nodo){
        nodo->nParola=y->nParola;
        //copiaParola(nodo->nParola,y->nParola);
    }

    //if(y->isBlack)
      //  sistemaCancellazione(x);
    return y;
}
//solo per i compatibili
void sistemaCancellazione(TreeNode* doppioBlack) {
    TreeNode *fratello;
    while (doppioBlack != testaComp && doppioBlack->isBlack) {
        printf("loopin");
        if (doppioBlack == doppioBlack->father->leftSon) {
           fratello = doppioBlack->father->rightSon;
            if (!fratello->isBlack) {
                fratello->isBlack = true;
                doppioBlack->father->isBlack = false;
                ruotaSX(doppioBlack->father, testaComp);// qui contronto eliminazione 2_z91
                fratello = doppioBlack->father->rightSon;
            }
            if (fratello->leftSon->isBlack && fratello->rightSon->isBlack ) {
                fratello->isBlack = false;
                doppioBlack = doppioBlack->father;
                doppioBlack->isBlack=true;
                //doppioBlack->father=doppioBlack->father;
            } else {
                if (fratello->rightSon->isBlack) {
                    fratello->leftSon->isBlack = true;
                    fratello->isBlack = false;
                    ruotaDX(fratello, testaComp);
                    fratello = doppioBlack->father->rightSon;
                }
                fratello->isBlack = doppioBlack->father->isBlack;
                doppioBlack->father->isBlack = true;
                doppioBlack->rightSon->isBlack = true;
                ruotaSX(doppioBlack->father, testaComp);
                
                doppioBlack=testaComp;

                }

        } else {
            fratello = doppioBlack->father->leftSon;
            if (!fratello->isBlack) {
                fratello->isBlack = true;
                doppioBlack->father->isBlack = false;
                ruotaDX(doppioBlack->father, testaComp);
                fratello = doppioBlack->father->leftSon;
            }
            if (fratello->rightSon->isBlack && fratello->leftSon->isBlack) {
                fratello->isBlack = false;
                doppioBlack = doppioBlack->father;
                doppioBlack->isBlack=true;
                //doppioBlack->father=doppioBlack->father;
            } else {
                if (fratello->leftSon->isBlack) {
                    fratello->rightSon->isBlack = true;
                    fratello->isBlack = false;
                    ruotaSX(fratello, testaComp);
                    fratello = doppioBlack->father->leftSon;
                }
                fratello->isBlack = doppioBlack->father->isBlack;
                doppioBlack->father->isBlack = true;
                fratello->leftSon->isBlack = true;
                ruotaDX(doppioBlack->father,testaComp);

                doppioBlack=testaComp;
            }
        }

    }
    NULLO->isBlack=true;
    doppioBlack->isBlack = true;
}
//per contrarre i compatibili
void cancellazioneNodoAlbero(TreeNode* nodo){
    TreeNode *nodoSos;
    TreeNode *min;
    //bool wasBlack=nodo->isBlack;
    if(nodo->leftSon!=NULLO&&nodo->rightSon!=NULLO){
        min = minOfSubTree(nodo->rightSon);
        //wasBlack = min->isBlack;
        nodoSos = min->rightSon;//fixme e se min non ha figli destri???
        //nodoSos->father=min;
        if (min->father == nodo) {
            nodoSos->father=min;
            //nodoSos->father=min;
        } else {
            innestaSopra(min->rightSon,min,testaComp);//ottimizzabili(poco)
            //min->rightSon->father=min->father;
            min->rightSon=nodo->rightSon;
            min->rightSon->father=min;
        }

        innestaSopra(min,nodo,testaComp);
        min->leftSon=nodo->leftSon;
        min->leftSon->father=min;
        min->isBlack = nodo->isBlack;//wasBlack;//???corretto?
        //fatherSost=nodo->father;
        if(nodo->father==NULLO)
            testaComp=min;
    }
    else if(nodo->leftSon==NULLO){
        //wasBlack=nodo->isBlack;
        nodoSos=nodo->rightSon;
        //nodoSos->father=nodo->father;
        innestaSopra(nodo->rightSon, nodo, testaComp);
        //nodoSos->father=nodo->father;
    }else if(nodo->rightSon==NULLO) {
        //wasBlack=nodo->isBlack;
        nodoSos=nodo->leftSon;

        //nodoSos->father=nodo->father;
        innestaSopra(nodo->leftSon,nodo,testaComp);
        //nodoSos->father=nodo->father;
    }
    //if(nodoSos!=NULLO)
        //fatherSost=nodoSos->father;
   // if(wasBlack)
     //    sistemaCancellazione(nodoSos);
      //NULLO->father=NULLO;
    //if(NULLO->father!=NULLO)
      //  printf("errore null  %s\n",nodo->nParola);
}


void attraversamentoIterativo(TreeNode* nodo){
    TreeNode *attuale=nodo;
    TreeNode *precedente=NULLO;
    while (attuale!=NULLO){
        TreeNode * successivo;
        if(precedente==NULLO||precedente==attuale->father){
            if(attuale->leftSon!=NULLO)
                successivo=attuale->leftSon;
            else{
                //chiaveToStringa((attuale->chiave),nuf);
               // printf("%s\n",attuale->nParola);
                customPrinter(attuale->nParola);
                if(attuale->rightSon!=NULLO)
                    successivo=attuale->rightSon;
                else successivo=attuale->father;
            }

        } else if(precedente==attuale->leftSon){
            //chiaveToStringa((attuale->chiave),nuf);
            //printf("%s\n",attuale->nParola);
            customPrinter(attuale->nParola);
            if(attuale->rightSon!=NULLO)
                successivo=attuale->rightSon;
            else successivo=attuale->father;
        } else successivo=attuale->father;
        precedente=attuale;
        attuale=successivo;
    }
}
//solo per stampa filtrate
void attraversamentoOrdinato(TreeNode* vertice){
    if(vertice==NULLO){
        return;
    }
    attraversamentoOrdinato(vertice->leftSon);
    //chiaveToStringa((vertice->chiave),nuf);
  //  if(strcmp("_fah-",nuf)==0)
    //    printf("lol");
    customPrinter(vertice->nParola);

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

bool pres(char *rParola, TreeNode* nodo){//todo rendere iterativokeyInt chiave
    /*if(nodo==NULLO)
        return false;
    else if(nodo->chiave==chiave)
        return true;
    else if(nodo->chiave<chiave)
        return pres(chiave,nodo->rightSon);
    else if(chiave<nodo->chiave)
        return pres(chiave,nodo->leftSon);
    return false;*/

    while(nodo!=NULLO){
        if(stringMaggiore(nodo->nParola,rParola))//nodo->chiave>chiave)
            nodo=nodo->leftSon;
        else if( stringMaggiore(rParola,nodo->nParola))
            nodo=nodo->rightSon;
        if(stringUguale(nodo->nParola,rParola))
            return true;
    }
    return false;
}
bool presenteTraAmmissibili(char* parola){
    if(treeHead==nullHead)
        return false;//debug
    //keyInt keyOfParola= string2chiave(parola);
    return pres(parola,treeHead);
}
/*
TreeNode* trovaNodo(keyInt val, TreeNode* nodo){
    if(nodo==NULLO)
        return NULLO;
    if(nodo->chiave<val && nodo->rightSon!=NULLO)
        return trovaNodo(val,nodo->rightSon);
    if(val<nodo->chiave && nodo->leftSon!=NULLO)
        return trovaNodo(val,nodo->leftSon);
    else return nodo;
}*/





/*
// *************DEBUG


int recContaPeso(TreeNode* nodo){
    if(nodo==NULLO)
        return 0;//non conto peso foglie null
    int adder=0;
    if(nodo->isBlack)
        adder++;
    int pesoDX= recContaPeso(nodo->rightSon);
    int pesoSX= recContaPeso(nodo->leftSon);
    if(pesoDX!=pesoSX  )
    {
        printf("right %d , left %d\n", pesoDX, pesoSX);
        if(pesoSX<1000 && pesoDX<1000) {
            printf("sotto alberi sbilanciati da nodo  %s\n", debugKEY(nodo->chiave));
        }
        return 1000;
    }
    return pesoSX+adder;
}
bool recControllaRB(TreeNode* nodo){
    if(nodo==NULLO)
        return true;
    if(!nodo->isBlack)
        if(!nodo->rightSon->isBlack || !nodo->leftSon->isBlack) {
            //printf("nodo sbarellato %s\n", debugKEY(nodo->chiave));
            return false;
        }
    return true;
}
void debugControlloAlbero(TreeNode* testa){
    printf("peso albero %d\n", recContaPeso(testa));
    if(!recControllaRB(testa))
        printf("nodo cattivo\n");
}

*/



//-------------------memoria

int leggiParolaPerConfronto(){
    char in=(char ) getc(INPUT);
    lineS++;
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

    if(presenteTraAmmissibili(wordk) ){
        confronto();
        return 2;
    }else{
        if(BUTTAINFILE)
            fprintf(ott,"not_exists\n");
        printf("not_exists\n");
        return 0;
    }

}

bool treeModified;
bool rispettaTotalMask(char* string){//todo scrivi versione che tiene conto di cosa non ricontrollare e usa solo costraint nuovi
    for(int i=0;i<wordLen;i++) {
        if (totalMask->ofPos[i].notAppear[mappaCharToInt64(string[i])] == '1')
            return false;
    }
    //bool ok=true;
    for (int i = 0; i < wordLen; ++i) {
        //countConfronto[mappaCharToInt64(string[i])]--;
        if (totalMask->ofPos[i].sureValue != string[i] && totalMask->ofPos[i].sureValue != 0)
            return false;
    }
    if(!contieneAlmenoNecessarie(string)) {
        memset(countConfronto, 0, 64);
        return false;
    }
    memset(countConfronto, 0, 64);
    return true;
    //countConfronto[mappaCharToInt64(string[i])]--;
}
bool ammissibileDaConfronto(char* damn,char* nonPresenti, int nNonPresenti){
    for (int j = 0; j < wordLen; ++j) {
        if(!accConfrontoLettera(j, wordk[j], damn[j])) {
            return false;
        }
    }
    if (contieneCaratteriNonPresenti(damn, nonPresenti, nNonPresenti))
        return  false;
    if (!contieneAlmenoNecessarie(damn))
        return  false;
    return true;

}
int nNonPr;
int removed;
TreeNode **nodeWaitingRoom;
//tree of junk
//aka feed garbodor
void recycleIsGood(TreeNode* rimosso){

    if(riciclati==0){
        garbodor=rimosso;
        garbodor->father=NULLO;
        garbodor->leftSon=NULLO;
        garbodor->rightSon=NULLO;
        recycleEnd=garbodor;
    }else {
        recycleEnd->rightSon=rimosso;
        rimosso->father=recycleEnd;
        rimosso->rightSon=NULLO;
        rimosso->leftSon=NULLO;
        recycleEnd=rimosso;
    }
    riciclati++;

}
void removalOfSelected(){
    for (int i = 0; i < removed; ++i) {
        cancellazioneNodoAlbero(nodeWaitingRoom[i]);
        recycleIsGood(nodeWaitingRoom[i]);
    }
    removed=0;
}
void aggCompDaAmmissibili(TreeNode* nodo ){
    if(nodo==NULLO)
        return;
    if(isFromNuInserisci){
        if(rispettaTotalMask(nodo->nParola)){
            isFromNuInserisci=false;
            salvaInCompatibili(nodo->nParola);
            compatibili++;
        }
    }else  if(constraintRun(nodo->nParola)){
            isFromNuInserisci=false;
            salvaInCompatibili(nodo->nParola);
            compatibili++;
        }
    aggCompDaAmmissibili(nodo->leftSon);
    aggCompDaAmmissibili(nodo->rightSon);
}
void rimuoviDaCompatibili(TreeNode* nodo ){
    if(treeModified || nodo == NULLO)
        return;
    if(!constraintRun(nodo->nParola)){// !ammissibileDaConfronto(nodo->nParola,nonPres,nNonPr)){
        nodeWaitingRoom[removed]=nodo;
        removed++;
        compatibili--;
        if(removed==REMOVEDBLOCK){
            removalOfSelected();
            treeModified=true;
            return;
        }
    }
    rimuoviDaCompatibili(nodo->leftSon);
    rimuoviDaCompatibili(nodo->rightSon);
}

void generaOscartaNuoviCon(){
    //svuota nuovi
    for (int i = 0; i < wordLen; ++i) {
        if(mascheraAt[i]!='+'){}
            //se non già presente e se non c'è costraint con già precedenza
            //crea constr e aggiungi a nuovi
        else{}//crea constr di presenza se non c'era già

        //prendere da confronto il numero di presenze o ricavare
        //se non esiste già con precedenza maggiore o uguale mettere in nuovi
    }

}
void aggiornaCompatibili( int nNonPresenti){
    nNonPr=nNonPresenti;
    generaOscartaNuoviCon();
    if(compatibili == 0) {
        aggCompDaAmmissibili(treeHead);
    }else{
        do {
            treeModified=false;
            rimuoviDaCompatibili(testaComp );
        } while (treeModified);
        removalOfSelected();
    }

    printf("%d\n", compatibili);
    if(BUTTAINFILE)
        fprintf(ott,"%d\n",compatibili);

}

void aggiornaCompatibiliNuovo(char* parolaNuova) {
    if( *confInPartita ==0)
        return;
    if (rispettaTotalMask(parolaNuova) ) {
        compatibili++;
        salvaInCompatibili(parolaNuova);
    }
}


void aggiungiAmmissibili(){
    int bo=1;

    do{
        lineS++;
        int i=0;
        char in=(char)getc(INPUT);
        if(in!='+') {
            do {
                wordk[i] = in;
                in = (char)getc(INPUT);
                i++;
            } while (in != '\n' && i < wordLen);
            wordk[wordLen]='\0';

            isFromNuInserisci=true;
            salvaAmmissibile(wordk);
            aggiornaCompatibiliNuovo(wordk);
        }
        else{
            bo=0;
        }

    } while (bo==1);
    if(nPartite==0)
        return;
    char in;
    do {
        in =(char) getc(INPUT);
    } while (in != '\n');
}


//word è un puntatore a un buffer, bisogna copiare la stringa in un nuovo blocco
void salvaAmmissibile(char* word){
    aggiungiNodoAMem(word);
    salvate++;

}
//parola è il puntatore alla string del nodo del tree principale
void salvaInCompatibili(char* parola){
    aggiungiNodoPerCompatibili(parola);
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

/**
 * inizializza la megaMask e references al primo giro
 */
void initPartita(){
    references= malloc(sizeof (*references));
    totalMask= malloc(sizeof(megaMask));
    nonPres= malloc((wordLen+1)* sizeof(char ));
    nonPres[wordLen]='\0';
    posMask* posMaschera= malloc(wordLen * sizeof (posMask));//todo malloc the space for the structures
    for (int i = 0; i < wordLen; ++i) {
        char *initAr= malloc(64);//fixme make of bits
        for (int j = 0; j < 64; ++j) {
            initAr[j]='0';
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
    references->nUniche=(cc+1);
    references->appearing=letSing;
    references->numberOfApp=nApp;
    totalMask->ofChars= malloc(sizeof (struct charInfo)*(wordLen));
    for (int i = 0; i < cc+1; ++i) {
        totalMask->ofChars[i].appears=0;
        totalMask->ofChars[i].isMax=0;
    }
    nPartite++;
}

void initSuccessivi(){
    //deAllocaCompatibili(testaComp);
    deAllocaIterativo(testaComp);
    //rinnovo compatibili
    testaComp=nullHead;
    for (int i = 0; i < wordLen; ++i) {
        for (int j = 0; j < 64; ++j) {
            totalMask->ofPos[i].notAppear[j]='0';
        }
        totalMask->ofPos[i].sureValue=0;
    }
    compatibili=0;
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
    for (int i = 0; i < cc+1; ++i) {
        totalMask->ofChars[i].appears=0;
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
    //bool ok=true;
    for (int i = 0; i < wordLen; ++i) {
        countConfronto[mappaCharToInt64(parolaDelNodo[i])]++;
    }
    for (int i = 0; i < references->nUniche; ++i) {
        if(totalMask->ofChars[i].appears > countConfronto[mappaCharToInt64(references->appearing[i])])
            return false;
    }
    for (int i = 0; i < references->nUniche; ++i) {
        if(totalMask->ofChars[i].isMax==1 && countConfronto[mappaCharToInt64(references->appearing[i])] != totalMask->ofChars[i].appears)
            return false;
    }

    return true;






    int app;
    for (int i = 0; i < references->nUniche; ++i) {
        app=0;
        for (int j = 0; j < wordLen; ++j) {
            if(references->appearing[i] == parolaDelNodo[j] )
                app++;
        }
        if(!((app>=totalMask->ofChars[i].appears && totalMask->ofChars[i].isMax==0) || (totalMask->ofChars[i].isMax==1 && app==totalMask->ofChars[i].appears)))
             return false;
    }
    return true;
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
void confronto(){//fixme, qualcosa non funziona, capire perché
    int dime=references->nUniche;
    int presenti[dime];
    for (int i = 0; i < references->nUniche; ++i) {
        presenti[i]=0;
    }
    int corrette=0;
    int nNP=0;
    for(int i=0; i < wordLen; i++){
        nonPres[i]='0';
        int pos= posOfLetterInReferences(wordk[i]);
        if(wordk[i] == riferimento[i]){
            mascheraAt[i]='+';
            if(pos>=0)
                presenti[pos]++;
            else NULLO->father=NULLO;//lmao
            corrette++;
            totalMask->ofPos[i].sureValue=wordk[i];
        }else if(pos==-1){
            nonPres[nNP]=wordk[i];
            nNP++;
            totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])]='1';
        }
    }
    nonPres[nNP]='\0';
    for(int i =0; i < wordLen; i++) {
        int prs = presenzeInRef(wordk[i]);
        if (prs > 0) {
            int pos = posOfLetterInReferences(wordk[i]);
            if (wordk[i] != riferimento[i]) {
                if(pos>=0 && pos<wordLen){//inutile se pres >o pos è valido
                    presenti[pos]++;
                    if (prs >= presenti[pos]){
                        //corrette++; nn serve
                        mascheraAt[i] = '|';
                        totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])]='1';
                    } else { mascheraAt[i] = '/';
                        //se ne sono gia apparse troppe
                            totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])]='1';
                    }
                }
                else { mascheraAt[i] = '/';
                    for (int j = 0; j < wordLen; ++j) {
                        totalMask->ofPos[j].notAppear[mappaCharToInt64(wordk[i])]='1';//okk perche non appare mai
                    }
                }
            }
        }else {
            mascheraAt[i] = '/';
            for (int j = 0; j < wordLen; ++j) {
                totalMask->ofPos[j].notAppear[mappaCharToInt64(wordk[i])]='1';
            }

        }
    }
    //creazione nuovi costraint
    // es: in pos 5 -> "J" => creo costraint pocCon ->(let "j", pos "5" , precedenza 2, orIsIt=true,
    // li creo nei stessi posti in cui aggiorno total mask e li metto nella lista dei nuovi


    //salvataggio di cosntraint accettabili confrontandoli con i vecchi di val piu alto
   // es cont: confronto O(n^2) tra i vecchi con valore più alto e i nuovi
   // se creo i posCon mentre aggiorno la maschera posso confrontarli con totalmask direttamente
   // ovvero if(totalMask->ofPos[j].appear!="0") non genero constraint per quella posizione
   // se avrei generato un constraint di notAppear prima vede se era già segnato in totalMask=>corrisponde al aver gia fatto il filtraggio su quel constr.
   // per i quant pos li confronto con quello che stava in totalMask->ofChars[i]

    // aggiornamento maschera partita
    for (int i = 0; i < references->nUniche; ++i) {
        int pres=presenti[i];
        if(totalMask->ofChars[i].appears< pres)
            totalMask->ofChars[i].appears= pres;
        if (references->numberOfApp[i] < pres) {
            totalMask->ofChars[i].isMax = 1;
            totalMask->ofChars[i].appears=references->numberOfApp[i];
        }
    }
    if(corrette == wordLen){
        printf("ok\n");
        if(BUTTAINFILE)
            fprintf(ott,"ok\n");
        *confInPartita= tentativi + 1;
        return;
    }
    printf("%s\n",mascheraAt);
    if(BUTTAINFILE)
        fprintf(ott,"%s\n",mascheraAt);
    aggiornaCompatibili(   nNP);

}

bool esattamenteNlett(const char* parola, char lett, int N){
    int app=N;
    for (int i = 0; i < wordLen; ++i) {
        if (parola[i]==lett)
            app--;
        if(app==0)
            return true;
    }
    return(app==0);
}

bool almenoNlett(const char* parola, char lett, int N){
    int app=N;
    for (int i = 0; i < wordLen; ++i) {
        if (parola[i]==lett)
            app--;
        if(app==0)
            return true;
    }
    return(app<=0);
}


bool presenteCorretto(const char*parola,char c, int pos, bool orIsIt){
    if(orIsIt)return parola[pos]==c;
    return parola[pos]!=c;
}
bool constraintRun(const char * parola){
    struct constPosList*nd=nuoviPos;
    for (int i = 0; i < dimNP; ++i) {//modifica in while succ!=null
        if(! presenteCorretto(parola,nd->nnd->lettera,nd->nnd->pos,nd->nnd->appare) )
         return false;
        nd=nd->succ;
    }
    struct constQuanList* nodo=nuoviQuant;
    for (int i = 0; i < dimNQ; ++i) {
        if(nodo->qnd->isMax ){
            if(!esattamenteNlett(parola,nodo->qnd->lettera,nodo->qnd->quant)) {
                return false; }

        }else if(!almenoNlett(parola,nodo->qnd->lettera,nodo->qnd->quant)) {
            return false; }
        nodo=nuoviQuant->succ;
    }
    return true;
}


//------------------comandi
void stampaFiltrate(){
    if(compatibili > 0){
        //attraversamentoOrdinato(testaComp);
        attraversamentoIterativo(testaComp);
    }else
        attraversamentoIterativo(treeHead);
}

bool startNuova;

void nuovaPartita() {
    //char buff[wordLen];
    compatibili = 0;
    char in = (char) getc(INPUT);
    int i = 0;
    if (in != '+') {
        do {
            riferimento[i] = in;
            in = (char) getc(INPUT);
            i++;
        } while (in != '\n');
    }
    lineS++;
    // printf("%s\n", riferimento);
    in = (char) getc(INPUT);
    i = 0;
    if (in != '+') {
        do {
            wordk[i] = in;
            in = (char) getc(INPUT);
            i++;
        } while (in != '\n');
    }
    lineS++;
    wordk[i]='\0';
    tentativi = atoi(wordk);
    char sw;
    orderedRef = strcpy(orderedRef, riferimento);
    for (int l = 0; l < wordLen; ++l) {
        for (int j = l + 1; j <
                            wordLen; ++j) {//fixme un sort con complessità migliore (non fondamentale, tanto la complessità e su wordLen, wordLen^2=25, klogk=15)
            if (orderedRef[l] > orderedRef[j]) {
                sw = orderedRef[l];
                orderedRef[l] = orderedRef[j];
                orderedRef[j] = sw;
            }
        }
    }
    if (nPartite == 0)
        initPartita();
    else initSuccessivi();
    int p;
    char buf[20];
    char been;
    *confInPartita = 0;
    while (*confInPartita < tentativi) {
        int xx = leggiParolaPerConfronto();
        if (xx == 2) {
            *confInPartita = *confInPartita+1;
        } else if (xx == 1 ) {

            been= (char)getc(INPUT);
            p=0;
            while (been!='\n'){
                buf[p]=been;
                been= (char)getc(INPUT);
                p++;
            }
            buf[p]='\0';
            if(buf[0]=='i')
                aggiungiAmmissibili();
            else if(buf[0]=='s')
                stampaFiltrate();
            else if(buf[0]=='n') {
                printf("what!? %s %d   tentivi fatti  %d  tent max %d\n", buf,nPartite, *confInPartita, tentativi);
                *confInPartita=tentativi+1;
                exit(11);
            }
        }
    }

    if (*confInPartita == tentativi) {
        printf("ko\n");
    if(BUTTAINFILE)
        fprintf(ott,"ko\n");
    }



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
    if(buf[0]=='i'){
        fnPointer=aggiungiAmmissibili;
    }
    else if(buf[0]=='s'){
        fnPointer=stampaFiltrate;
    }
    else if(buf[0]=='n'){
        fnPointer=nuovaPartita;
    }
    else return NULL;
    return fnPointer;
}

int main(){
    lineS=0;

    if(FROMFILE>0){
        if ((fp = fopen("/mnt/c/Users/User/CLionProjects/api-2022-word-checker/cmake-build-debug-wsl/api_2022_word_checker/open_testcases/test3.txt", "r")) == NULL){//C:/Users/User/CLionProjects/api-2022-word-checker/
            printf("File cannot open");
            exit(1);
        }
    }
    if(BUTTAINFILE){
        if((ott = fopen("C:/Users/User/CLionProjects/api-2022-word-checker/out/outputBro.txt","w"))==NULL) {
            printf("File cannot open");
            exit(1);
        }
    }
    riciclati=0;
    nPartite=0;

    countConfronto= malloc(64 * sizeof(char));
    for (int i = 0; i < 64; ++i) {
        countConfronto[i]=0;
    }
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
    input[4]='\0';
    wordLen= atoi(input);
    lineS++;
    nodeWaitingRoom= malloc(REMOVEDBLOCK* sizeof(TreeNode*));
    wordk= malloc((wordLen+1)* sizeof(char ));
    wordk[wordLen]='\0';

    bloccoDiMalloc= malloc(ADDEDBLOCK* sizeof (TreeNode));
    bloccoDiStringNU= malloc(ADDEDBLOCK* wordLen*sizeof(char));

    NULLO= malloc(sizeof(TreeNode));
    NULLO->father=NULLO;
    NULLO->leftSon=NULLO;
    NULLO->rightSon=NULLO;
    NULLO->isBlack=true;
    nullHead=malloc(sizeof(TreeNode));
    nullHead->father=NULLO;
    nullHead->leftSon=NULLO;
    nullHead->rightSon=NULLO;
    nullHead->nParola= calloc(wordLen, sizeof(char ));
    NULLO->nParola=nullHead->nParola;
    nullHead->nParola[0]='s';
    nullHead->nParola[1]='\0';
   // copiaParola(nullHead->nParola,"s");
    testaComp=nullHead;
    treeHead=nullHead;


    orderedRef= malloc((wordLen+1)* sizeof(char ));
    orderedRef[wordLen]='\0';
    riferimento=malloc((wordLen+1)* sizeof(char ));
    riferimento[wordLen]='\0';
    mascheraAt= malloc((wordLen+1)* sizeof(char ));
    mascheraAt[wordLen]='\0';

    aggiungiAmmissibili();
    startNuova=false;
    void (*exec)()= eseguiComando();//esegui comando nel caso avvenga roba prima di nuova partita => inizializzare roba di init prima
    exec();

    if(startNuova){
        startNuova=false;
        nuovaPartita();
    }

    char rest= (char)getc(INPUT);
    while (rest=='+'){
        lineS++;
         void (*ptr)()=eseguiComando();
         ptr();
        if(startNuova){
            startNuova=false;
            nuovaPartita();
        }
         rest= (char)getc(INPUT);
    }
    if(BUTTAINFILE)
        fclose(ott);
}

