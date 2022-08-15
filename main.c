#include <string.h>
#include <stdbool.h>
#include "stdio.h"
#include "stdlib.h"

#define REMOVEDBLOCK 300
#define ADDEDBLOCK 10

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

struct constQuanList{
    char lettera;
    int quant;
    bool isMax;
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
} refInfo;

void salvaInCompatibili(char *parola);

//----------------globali
megaMask *totalMask;

int *nuoviPos;
int dimP;
int *nuoviNotPos;
int dimNP;
struct constQuanList *nuoviQuant;
int dimQ;

int salvate;
int compatibili;//relativo alla partita
char* riferimento;
char* orderedRef;
int wordLen;
int tentativi;
int *confInPartita;//confronti fatti
refInfo * references;
char* mascheraAt;
int nPartite;
char* countConfronto;//serve per confronti su totalMask
//------------------input

char *wordk;
// ----------------
//----debug
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
        printf("%c",word[i]);
    }
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
                recycleIsGood(attuale);
                //ee(nodo);
                if(attuale->rightSon!=NULLO)
                    successivo=attuale->rightSon;
                else successivo=attuale->father;
            }

        } else if(precedente==attuale->leftSon){
            recycleIsGood(attuale);
            //free(nodo);

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
        if(strncmp(nParola, nodo->nParola,wordLen)>0)//chiave>nodo->chiave)
            nodo=nodo->rightSon;
        else if(strncmp(nodo->nParola,nParola,wordLen)>0)
            nodo=nodo->leftSon;
        else return nodo;
    }
    printf("error finding node");
    return NULL;
}
TreeNode * trovaNodoDaMEM(char* nParola){//keyInt chiave
    TreeNode *nodo=treeHead;
    while (nodo!=NULLO){
        if(strncmp(nParola, nodo->nParola,wordLen)>0)//chiave>nodo->chiave)
            nodo=nodo->rightSon;
        else if(strncmp(nodo->nParola,nParola,wordLen)>0)
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

void aggiungiNodoAMem(const char* nuParola){
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
        if(strncmp(nuNodo->nParola,discensore->nParola,wordLen)>0)//nuNodo->chiave>discensore->chiave
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
        if(strncmp(nuNodo->nParola,discensore->nParola,wordLen)>0)//nuNodo->chiave>discensore->chiave
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
    else if(strncmp(nuNodo->nParola,padre->nParola,wordLen)>0)//nuNodo->chiave>padre->chiave
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
//per contrarre i compatibili
TreeNode *cancellazioneNodoAlbero(TreeNode* nodo){//rendere nuovamente rb
    TreeNode *nodoSos;
    TreeNode *min;

    if(nodo->leftSon==NULLO){
        //nodoSos=nodo->rightSon;
        innestaSopra(nodo->rightSon, nodo, testaComp);
        return nodo->rightSon;
    }else if(nodo->rightSon==NULLO) {
        //nodoSos=nodo->leftSon;
        innestaSopra(nodo->leftSon,nodo,testaComp);
        return nodo->leftSon;
    }else {//if(nodo->leftSon!=NULLO&&nodo->rightSon!=NULLO){
        min = minOfSubTree(nodo->rightSon);
        nodoSos = min->rightSon;//fixme e se min non ha figli destri???
        if (min->father == nodo) {
            nodoSos->father=min;
        } else {
            innestaSopra(min->rightSon,min,testaComp);//ottimizzabili(poco)
            min->rightSon=nodo->rightSon;
            min->rightSon->father=min;
        }

        innestaSopra(min,nodo,testaComp);
        min->leftSon=nodo->leftSon;
        min->leftSon->father=min;
        min->isBlack = nodo->isBlack;//wasBlack;//???corretto?
        if(nodo->father==NULLO)
            testaComp=min;
        return min;
    }
}


void attraversamentoIterativo(TreeNode* nodo){
    TreeNode *attuale=nodo;
    TreeNode *precedente=NULLO;
    TreeNode * successivo;
    while (attuale!=NULLO){
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
        if(strncmp(nodo->nParola,rParola,wordLen)>0)//nodo->chiave>chiave)
            nodo=nodo->leftSon;
        else if( strncmp(rParola,nodo->nParola,wordLen)>0)
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
    char in=(char ) getchar();
    lineS++;
    if(in == '+'){
        return 1;
    }
    wordk[0]=in;
    int i=1;
    in= (char)getchar_unlocked();
    while(in!='\n' && i<wordLen){
        wordk[i]=in;
        i++;
        in=(char ) getchar_unlocked();
    }

    wordk[wordLen]= '\0';

    if(presenteTraAmmissibili(wordk) ){
        confronto();
        return 2;
    }else{
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
        //free(nodeWaitingRoom[i]);
    }
    removed=0;
}
void boiIterativo(TreeNode* nodo){
    TreeNode *attuale=nodo;
    TreeNode *precedente=NULLO;
    TreeNode * successivo;
    while (attuale!=NULLO){
        if(precedente==NULLO||precedente==attuale->father){
            if(attuale->leftSon!=NULLO)
                successivo=attuale->leftSon;
            else{


                if(isFromNuInserisci){
                    if(rispettaTotalMask(attuale->nParola)){
                        isFromNuInserisci=false;
                        salvaInCompatibili(attuale->nParola);
                        compatibili++;
                    }
                }else  if(constraintRun(attuale->nParola)){
                    isFromNuInserisci=false;
                    salvaInCompatibili(attuale->nParola);
                    compatibili++;
                }


                if(attuale->rightSon!=NULLO)
                    successivo=attuale->rightSon;
                else successivo=attuale->father;
            }
        } else if(precedente==attuale->leftSon){


            if(isFromNuInserisci){
                if(rispettaTotalMask(attuale->nParola)){
                    isFromNuInserisci=false;
                    salvaInCompatibili(attuale->nParola);
                    compatibili++;
                }
            }else  if(constraintRun(attuale->nParola)){
                isFromNuInserisci=false;
                salvaInCompatibili(attuale->nParola);
                compatibili++;
            }


            if(attuale->rightSon!=NULLO)
                successivo=attuale->rightSon;
            else successivo=attuale->father;
        } else successivo=attuale->father;
        precedente=attuale;
        attuale=successivo;
    }
}
void aggCompDaAmmissibili(TreeNode* nodo ){
    //if(nodo==NULLO)
      //  return;
    if(isFromNuInserisci){
        if(rispettaTotalMask(nodo->nParola)){
            isFromNuInserisci=false;
            aggiungiNodoPerCompatibili(nodo->nParola);
            compatibili++;
        }
    }else  if(constraintRun(nodo->nParola)){
            isFromNuInserisci=false;
            aggiungiNodoPerCompatibili(nodo->nParola);
            compatibili++;
        }
    if(nodo->leftSon!=NULLO)
        aggCompDaAmmissibili(nodo->leftSon);
    if(nodo->rightSon!=NULLO)
        aggCompDaAmmissibili(nodo->rightSon);
}

void rimCompIt(TreeNode* nodo){
    TreeNode *attuale=nodo;
    TreeNode *precedente=NULLO;
    TreeNode * successivo;
    while (attuale!=NULLO){
        if(precedente==NULLO||precedente==attuale->father){
            if(attuale->leftSon!=NULLO)
                successivo=attuale->leftSon;
            else{


                if(!constraintRun(attuale->nParola)){
                    nodeWaitingRoom[removed]=attuale;
                    removed++;
                    compatibili--;
                    if(removed==REMOVEDBLOCK){
                        removalOfSelected();
                        treeModified=true;
                        return;
                    }
                }


                if(attuale->rightSon!=NULLO)
                    successivo=attuale->rightSon;
                else successivo=attuale->father;
            }
        } else if(precedente==attuale->leftSon){


            if(!constraintRun(attuale->nParola)){
                nodeWaitingRoom[removed]=attuale;
                removed++;
                compatibili--;
                if(removed==REMOVEDBLOCK){
                    removalOfSelected();
                    treeModified=true;
                    return;
                }
            }


            if(attuale->rightSon!=NULLO)
                successivo=attuale->rightSon;
            else successivo=attuale->father;
        } else successivo=attuale->father;
        precedente=attuale;
        attuale=successivo;
    }
}

/*
struct stackPers{
    TreeNode *data;
    struct stackPers* succ;
};

struct stackPers* cima;
int stackDim;
struct stackPers* reserve;
int reserveDim;

void push(TreeNode* nodo){
    if(reserveDim>0){
        struct stackPers* temp=reserve->succ;
        reserve->succ=cima;
        cima=reserve;
        cima->data=nodo;
        reserve=temp;
        reserveDim--;
        stackDim++;
        return;
    }
    struct stackPers* temp= malloc(sizeof(struct stackPers));
    temp->data=nodo;
    temp->succ=cima;
    cima=temp;
    stackDim++;
}
TreeNode *pop(){
    struct stackPers* temp=cima->succ;
    cima->succ=reserve;
    cima=temp;
    stackDim--;
    reserveDim++;
    return reserve->data;
}
void resettaRiserva(){
    if(cima==NULL)
        return;
    struct stackPers* temp=cima->succ;
    while(temp!=NULL){
        cima->succ=reserve;
        reserve=cima;
        cima=temp;
        temp=cima->succ;
        reserveDim++;
    }
    stackDim=0;
}

void iterativePreorder(TreeNode* testa){
    //cima=NULL;
    //cima->data=testa;
    push(testa);
    TreeNode *nn;
    while(stackDim!=0){
        nn=pop();
        printf("%s", nn->nParola);//placeholder
        if (nn->rightSon!=NULL)
            push(nn->rightSon);
        if (nn->leftSon!=NULL)
            push(nn->leftSon);
    }


}
*/





int recStopper;
void rimuoviDaCompatibili(TreeNode* nodo ){
    if(treeModified || nodo == NULLO)
        return;
    recStopper++;/*
    if(recStopper>1000){
        treeModified=true;
        recStopper=0;
        return;
    }*/
    TreeNode *nod;
    while(!constraintRun(nodo->nParola) && nodo!=NULLO){
        //nodeWaitingRoom[removed]=nodo;
        nod = cancellazioneNodoAlbero(nodo);
        recycleIsGood(nodo);
        compatibili--;
        nodo=nod;
        //rimuoviDaCompatibili(nod);
        //while()
        //return;
        /*removed++;
        if(removed==REMOVEDBLOCK){
            removalOfSelected();
            treeModified=true;
            return;
        }*/
    }
   // if(nodo->leftSon!=NULLO)
        rimuoviDaCompatibili(nodo->leftSon);
    //if (nodo->rightSon!=NULLO)
        rimuoviDaCompatibili(nodo->rightSon);
}

void aggiornaCompatibili( ){
    //nNonPr=nNonPresenti;
    //generaOscartaNuoviCon();
    if(compatibili == 0) {
        //aggCompDaAmmissibili(treeHead);
        boiIterativo(treeHead);
    }else{
        do {
            recStopper=0;
            treeModified=false;
            rimuoviDaCompatibili(testaComp );
            //rimCompIt(testaComp);
        } while (treeModified);
        //removalOfSelected();
    }

    printf("%d\n", compatibili);
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
        char in=(char)getchar_unlocked();
        if(in!='+') {
            do {
                wordk[i] = in;
                in = (char)getchar_unlocked();
                i++;
            } while (in != '\n' && i < wordLen);
            wordk[wordLen]='\0';

            isFromNuInserisci=true;
            aggiungiNodoAMem(wordk);
            salvate++;
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
        in =(char) getchar_unlocked();
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
    //nonPres= malloc((wordLen+1)* sizeof(char ));
    //nonPres[wordLen]='\0';
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

void initConstraintLists(){
    nuoviPos= malloc(sizeof(int)*wordLen);
    nuoviNotPos= malloc(sizeof(int)*wordLen);
    nuoviQuant= malloc(sizeof(struct constQuanList)*wordLen);
    dimP=0;
    dimNP=0;
    dimQ=0;
}
void resetConstraintLists(){
    dimNP=0;
    dimP=0;
    dimQ=0;
}
void newNotAppearConstraint( int pos){
    nuoviNotPos[dimNP]=pos;
    dimNP++;
}
void newAppearConstraint( int pos){
    nuoviPos[dimP]=pos;
    dimP++;
}
void newQuantConstraint(char lettera, int quante, bool isMax){
    nuoviQuant[dimQ].lettera=lettera;
    nuoviQuant[dimQ].quant=quante;
    nuoviQuant[dimQ].isMax=isMax;
    dimQ++;
}
bool giaPresenteNotAppear(int pos){
    for (int i = 0; i < dimNP; ++i) {
        if(nuoviNotPos[i]==pos )
            return true;
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
    resetConstraintLists();
    for (int i = 0; i < references->nUniche; ++i) {
        presenti[i]=0;
    }
    int corrette=0;
    int nNP=0;
    for(int i=0; i < wordLen; i++){
        //nonPres[i]='0';
        int pos= posOfLetterInReferences(wordk[i]);
        if(wordk[i] == riferimento[i]){
            mascheraAt[i]='+';
            if(pos>=0)
                presenti[pos]++;
            else NULLO->father=NULLO;//lmao
            corrette++;
            if(totalMask->ofPos[i].sureValue!=wordk[i])
                newAppearConstraint(i);
            totalMask->ofPos[i].sureValue=wordk[i];

        }else if(pos==-1){
            //nonPres[nNP]=wordk[i];
            nNP++;
            //if(totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])]!='1') {
                totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])] = '1';
            newNotAppearConstraint(i);
            //    newNotAppearConstraint(wordk[i],i);
           // }
        }

    }


    //nonPres[nNP]='\0';
    for(int i =0; i < wordLen; i++) {
        int prs = presenzeInRef(wordk[i]);
        if (prs > 0) {
            int pos = posOfLetterInReferences(wordk[i]);
            if(pos<0 || pos>wordLen){
                exit(31);
            }
            if (wordk[i] != riferimento[i]) {
                    presenti[pos]++;
                    if (prs >= presenti[pos]){
                        mascheraAt[i] = '|';//corrette++; nn serve
                    } else {
                        mascheraAt[i] = '/';//se ne sono gia apparse troppe
                    }
                    if(totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])]!='1'  ) {
                        totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])] = '1';
                        if(!giaPresenteNotAppear(i))
                             newNotAppearConstraint(i);
                    }
            }
        }else {//ma questo else viene mai visitato????? prob no fixme
            mascheraAt[i] = '/';//non appare nemmeno una volta
            for (int j = 0; j < wordLen; ++j) {
                if(totalMask->ofPos[j].notAppear[mappaCharToInt64(wordk[i])]!='1' ) {
                    totalMask->ofPos[j].notAppear[mappaCharToInt64(wordk[i])] = '1';
                    if(!giaPresenteNotAppear(j))
                        newNotAppearConstraint(j);
                }
            }

        }
    }

    // aggiornamento maschera partita
    bool aggiornamento;
    for (int i = 0; i < references->nUniche; ++i) {
        int pres=presenti[i];
        aggiornamento=false;
        if(totalMask->ofChars[i].appears< pres) {
            totalMask->ofChars[i].appears = pres;
            aggiornamento=true;
        }
        if (references->numberOfApp[i] < pres) {
            totalMask->ofChars[i].isMax = 1;
            totalMask->ofChars[i].appears=references->numberOfApp[i];
            aggiornamento=true;
        }
        if(aggiornamento)
            newQuantConstraint(references->appearing[i], totalMask->ofChars[i].appears,totalMask->ofChars[i].isMax==1);
    }
    if(corrette == wordLen){
        printf("ok\n");
        *confInPartita= tentativi + 1;
        return;
    }
    printf("%s\n",mascheraAt);
    aggiornaCompatibili(   );

}

bool esattamenteNlett(const char* parola, char lett, int N){
    int app=0;
    for (int i = 0; i < wordLen; ++i) {
        if (parola[i]==lett)
            app++;
        //if(app==N)
            //return true;
    }
    return(app==N);
}

bool almenoNlett(const char* parola, char lett, int N){
    int app=0;
    for (int i = 0; i < wordLen; ++i) {
        if (parola[i]==lett)
            app++;
        if(app==N)
            return true;
    }
    return(app>=N);
}


bool constraintRun(const char * parola){
    for (int i = 0; i < dimP; ++i) {//modifica in while succ!=null
        if(parola[nuoviPos[i]]!=wordk[nuoviPos[i]])//presenteCorretto(parola,wordk[nuoviPos[i]],nuoviPos[i]) )
            return false;
    }
    for (int i = 0; i < dimNP; ++i) {
        if(totalMask->ofPos[nuoviNotPos[i]].notAppear[mappaCharToInt64(parola[nuoviNotPos[i]])]=='1')//nonPresenteCorretto(parola,nuoviNotPos[i]))
            return false;
    }
    for (int i = 0; i < dimQ; ++i) {
        if(nuoviQuant[i].isMax ){
            if(!esattamenteNlett(parola,nuoviQuant[i].lettera,nuoviQuant[i].quant)) {
                return false; }

        }else if(!almenoNlett(parola,nuoviQuant[i].lettera,nuoviQuant[i].quant)) {
            return false; }
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
    char in = (char) getchar_unlocked();
    int i = 0;
    if (in != '+') {
        do {
            riferimento[i] = in;
            in = (char) getchar_unlocked();
            i++;
        } while (in != '\n');
    }
    lineS++;
    // printf("%s\n", riferimento);
    in = (char) getchar_unlocked();
    i = 0;
    if (in != '+') {
        do {
            wordk[i] = in;
            in = (char) getchar_unlocked();
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

            been= (char)getchar_unlocked();
            p=0;
            while (been!='\n'){
                buf[p]=been;
                been= (char)getchar_unlocked();
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
    }



}



void* eseguiComando(){
    char buf[20];
    char in= (char)getchar_unlocked();
    int i=0;
    while (in!='\n'){
        buf[i]=in;
        in= (char)getchar_unlocked();
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
    riciclati=0;
    nPartite=0;
 //
    countConfronto= malloc(64 * sizeof(char));
    for (int i = 0; i < 64; ++i) {
        countConfronto[i]=0;
    }
    confInPartita= malloc(sizeof (int));
    *confInPartita=0;
    char input[5];
    char in=(char)getchar_unlocked();
    int i=0;
    while(in!='\n' && in!='+'){
        input[i]=in;
        in=(char)getchar_unlocked();
        i++;
    }
    input[4]='\0';
    wordLen= atoi(input);
    lineS++;

    //allocazione buffer parole di lunghezza wordlen
    wordk= malloc((wordLen+1)* sizeof(char ));
    wordk[wordLen]='\0';

    //allocazione per batch
    nodeWaitingRoom= malloc(REMOVEDBLOCK* sizeof(TreeNode*));
    bloccoDiMalloc= malloc(ADDEDBLOCK* sizeof (TreeNode));
    bloccoDiStringNU= malloc(ADDEDBLOCK* wordLen*sizeof(char));

    //allocazione riferimenti constraint
    //nuoviQuant= malloc(sizeof(struct constQuanList));
   // nuoviPos= malloc(sizeof(struct constPosList));
    //nuoviNotPos= malloc(sizeof(struct constNotPosList));
    initConstraintLists();

    //allocazione riferimenti alberi
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

    //allocazione informazioni parola di riferimento
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

    char rest= (char)getchar_unlocked();
    while (rest=='+'){
        lineS++;
         void (*ptr)()=eseguiComando();
         ptr();
        if(startNuova){
            startNuova=false;
            nuovaPartita();
        }
         rest= (char)getchar_unlocked();
    }
}

