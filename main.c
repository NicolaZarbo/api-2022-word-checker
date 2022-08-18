#include <string.h>
#include <stdbool.h>
#include "stdio.h"
#include "stdlib.h"

#define ADDEDBLOCK 20

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
bool treeModified;
bool aggiustato;
char boi;

char* countConfronto;//serve per confronti su totalMask
//------------------input

char *wordk;
// ----------------
//----debug



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
TreeNode * minOfSubTree(TreeNode* nodo);
bool constraintRun(const char * parola);
void recycleIsGood(TreeNode* rimosso);
bool rispettaTotalMask(char* string);
void innestaSopra(TreeNode* innesto, TreeNode* nodoOriginale, TreeNode* radice);
void deAllocaIterativo(TreeNode*nodo);
void recreateTheTree();

//-------------------Utili
void customPrinter(const char* word){
    for (int i = 0; i < wordLen; ++i) {
        putchar_unlocked(word[i]);
    }
    putchar_unlocked('\n');

}
void copiaParola(char* dest, const char*from){
    for (int i = 0; i < wordLen; ++i) {
        dest[i]=from[i];
    }
}

bool stringUguale(const char* primo, const char* secondo) {
    for (int i = 0; i < wordLen; ++i) {
        if (primo[i] != secondo[i]) {
            return false;
        }
    }
    return true;
}
//----------------
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
                if(attuale->rightSon!=NULLO)
                    successivo=attuale->rightSon;
                else successivo=attuale->father;
            }

        } else if(precedente==attuale->leftSon){
            recycleIsGood(attuale);

            if(attuale->rightSon!=NULLO)
                successivo=attuale->rightSon;
            else successivo=attuale->father;
        } else successivo=attuale->father;
        precedente=attuale;
        attuale=successivo;
    }
}
void innestaSopra(TreeNode* innesto, TreeNode* nodoOriginale, TreeNode* radice){
    if(nodoOriginale != radice){
        if(nodoOriginale == nodoOriginale->father->leftSon)
            nodoOriginale->father->leftSon=innesto;
        else nodoOriginale->father->rightSon=innesto;
    }else if(radice==testaComp){// lo uso solo per compatibili?!
        testaComp=innesto;
    }
   // if(innesto!=NULLO)fixme eeeeeeeeeeee oppure no, è un problema dare a NULLO un padre? Non sembra
        innesto->father=nodoOriginale->father;
}
void ruotaSX(TreeNode *nodo, TreeNode* testa){
    TreeNode *exFiglio=nodo->rightSon;
    nodo->rightSon=exFiglio->leftSon;
    if(exFiglio->leftSon != NULLO)
        exFiglio->leftSon->father=nodo;
    exFiglio->father=nodo->father;

    if(nodo->father==NULLO) {
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
    if(nodo->father==NULLO) {
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

            } else {
                if (nuNodo == nuNodo->father->rightSon) {//caso 2?
                    nuNodo = nuNodo->father;
                    ruotaSX(nuNodo,testa);

                }
                nuNodo->father->father->isBlack=false;//nonno->isBlack = false;//caso 3?//??
                nuNodo->father->isBlack=true;//nuNodo->father->isBlack = true;
                ruotaDX(nuNodo->father->father,testa);
            }
        } else{
            bool nonnoNero=true;
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
//confronto str maggiore dal nome infelice
bool strPers(const char*par1, const char*par2){
    for (int i = 0; i < wordLen; ++i) {
        if(par1[i]>par2[i]) return true;
        else if(par2[i]>par1[i]) return false;
    }
    return false;
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
    }
    else {
        //allocazione nodo da blocco di allocazione
        nuNodo = &bloccoDiMalloc[aggiuntiNodi];
        aggiuntiNodi++;
        if (aggiuntiNodi == ADDEDBLOCK) {
            bloccoDiMalloc = malloc(sizeof(TreeNode) * ADDEDBLOCK);
            aggiuntiNodi = 0;
        }
    }//allocazione stringa nodo da blocco di allocazione
    nuNodo->nParola=&bloccoDiStringNU[aggiunteStringhe * wordLen];
    aggiunteStringhe++;
    if(aggiunteStringhe== ADDEDBLOCK){
        bloccoDiStringNU= malloc(sizeof(char)*wordLen*ADDEDBLOCK);
        aggiunteStringhe=0;
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
        if(strPers(nuNodo->nParola,discensore->nParola))//nuNodo->chiave>discensore->chiave
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
    else if(strPers(nuNodo->nParola,padre->nParola))
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
    }
    else {
        nuNodo = &bloccoDiMalloc[aggiuntiNodi];
        aggiuntiNodi++;
        if (aggiuntiNodi == ADDEDBLOCK) {
            bloccoDiMalloc = malloc(sizeof(TreeNode) * ADDEDBLOCK);
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
        if(strPers(nuNodo->nParola,discensore->nParola))//nuNodo->chiave>discensore->chiave
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
    else if(strPers(nuNodo->nParola,padre->nParola))//nuNodo->chiave>padre->chiave
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
/*
void sistemaDopoCancellazione(TreeNode *x){
    treeModified=true;
    TreeNode *frat;
   while(x!=testaComp && x->isBlack){
        if(x==x->father->leftSon){
            frat=x->father->rightSon;
            if(frat->isBlack!=true){
                frat->isBlack=true;
                x->father->isBlack=false;
                ruotaSX(x->father,testaComp);
                frat=x->father->rightSon;
            }
            if(frat->leftSon->isBlack && frat->rightSon->isBlack){
                frat->isBlack=false;
                x=x->father;
            }
            else{
                if(frat->rightSon->isBlack){
                    frat->leftSon->isBlack=true;
                    frat->isBlack=false;
                    ruotaDX(frat,testaComp);
                    frat=x->father->rightSon;         
                }
                frat->isBlack=x->father->isBlack;
                x->father->isBlack=true;
                frat->rightSon->isBlack=true;
                ruotaSX(x->father, testaComp);
                x=testaComp;
            }
            
        }else{
                frat=x->father->leftSon;
                if(frat->isBlack!=true){
                    frat->isBlack=true;
                    x->father->isBlack=false;
                    ruotaDX(x->father,testaComp);
                    frat=x->father->leftSon;
                }
                if(frat->rightSon->isBlack && frat->leftSon->isBlack){
                    frat->isBlack=false;
                    x=x->father;
                }
                else{
                    if(frat->leftSon->isBlack){
                        frat->rightSon->isBlack=true;
                        frat->isBlack=false;
                        ruotaSX(frat,testaComp);
                        frat=x->father->leftSon;
                    }
                    frat->isBlack=x->father->isBlack;
                    x->father->isBlack=true;
                    frat->leftSon->isBlack=true;
                    ruotaDX(x->father, testaComp);
                    x=testaComp;
                }
        }
        
   }
   x->isBlack=true;
}
 */
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
        if(nodo->father==NULLO)
            testaComp=min;
        return min;
    }
}
/*
void cancellazioneNodoAlberoV2(TreeNode* nodo){//sistema e aggiungi fixup
    TreeNode *x;
    TreeNode *y;
    treeModified=true;
    bool needToFixUp=nodo->isBlack;
    if(nodo->leftSon==NULLO){
        x=nodo->rightSon;
        innestaSopra(nodo->rightSon, nodo, testaComp);
        return;
        if(needToFixUp)
            sistemaDopoCancellazione(x);
        //return nodo->rightSon;
    }else if(nodo->rightSon==NULLO) {
        x=nodo->leftSon;
        innestaSopra(nodo->leftSon,nodo,testaComp);
        return;
        if(needToFixUp)
            sistemaDopoCancellazione(x);
        //return nodo->leftSon;
    }else {//if(nodo->leftSon!=NULLO&&nodo->rightSon!=NULLO){
        treeModified=true;
        y = minOfSubTree(nodo->rightSon);
        needToFixUp=y->isBlack;
        x = y->rightSon;
        if (y->father == nodo) {
            x->father=y;
        } else {
            innestaSopra(y->rightSon, y, testaComp);//ottimizzabili(poco)
            y->rightSon=nodo->rightSon;
            y->rightSon->father=y;
        }

        innestaSopra(y, nodo, testaComp);
        y->leftSon=nodo->leftSon;
        y->leftSon->father=y;
        y->isBlack=nodo->isBlack;
        //if(nodo->father==NULLO)
        //    testaComp=y;
        if(needToFixUp) {
            sistemaDopoCancellazione(x);

        }

    }
}
 */



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

bool pres(char *rParola, TreeNode* nodo){
    while(nodo!=NULLO){
        if(strPers(nodo->nParola,rParola))
            nodo=nodo->leftSon;
        else if( strPers(rParola,nodo->nParola))
            nodo=nodo->rightSon;
        if(stringUguale(nodo->nParola,rParola))
            return true;
    }
    return false;
}
bool presenteTraAmmissibili(char* parola){
    if(treeHead==nullHead)
        return false;//debug
    return pres(parola,treeHead);
}




//-------------------memoria

int leggiParolaPerConfronto(){
    char in=(char ) getchar();
    if(in == '+'){
        return 1;
    }
    wordk[0]=in;
    int i=1;
    in= (char)getchar_unlocked();
    while(in!='\n' ){
       if(in!='\r') {
           wordk[i] = in;
           i++;
       }
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

bool rispettaTotalMask(char* string){//todo scrivi versione che tiene conto di cosa non ricontrollare e usa solo costraint nuovi


    for(int i=0;i<wordLen;i++) {
        if (totalMask->ofPos[i].notAppear[mappaCharToInt64(string[i])] == '1')
            return false;
    }
    for (int i = 0; i < wordLen; ++i) {
        if (totalMask->ofPos[i].sureValue != string[i] && totalMask->ofPos[i].sureValue != 0)
            return false;
    }
    if(!contieneAlmenoNecessarie(string)) {
        memset(countConfronto, 0, 64);
        return false;
    }
    memset(countConfronto, 0, 64);

    return true;
}
bool ammissibileDaConfronto(char* damn,char* nonPresenti, int nNonPresenti){//obsoleto e lento
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
//tree of junk, aka feed the garbodor, garbodor is so cool, I love him
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
//versione 4, scorre il tree di memoria e decide se salvare in compatibili
void fasterIterativeAggiungiACompatibili(TreeNode* root)
{
    if (root == NULLO)
        return;
    TreeNode *attuale;
    TreeNode *precedente;
    attuale = root;
    while (attuale != NULLO) {

        if (attuale->leftSon == NULLO) {
            if(isFromNuInserisci){//non son sicuro questa roba abbia utilizzo, ma funziona e quindi non la tocco
                if(rispettaTotalMask(attuale->nParola)){
                    isFromNuInserisci=false;
                    aggiungiNodoPerCompatibili(attuale->nParola);
                    compatibili++;
                }
            }else  if(constraintRun(attuale->nParola)){
                isFromNuInserisci=false;
                aggiungiNodoPerCompatibili(attuale->nParola);
                compatibili++;
            }
            attuale = attuale->rightSon;
        }
        else {
            precedente = attuale->leftSon;
            while (precedente->rightSon != NULLO
                   && precedente->rightSon != attuale)
                precedente = precedente->rightSon;
            if (precedente->rightSon == NULLO) {
                precedente->rightSon = attuale;
                attuale = attuale->leftSon;
            }
            else {
                precedente->rightSon = NULLO;
                if(isFromNuInserisci){
                    if(rispettaTotalMask(attuale->nParola)){
                        isFromNuInserisci=false;
                        aggiungiNodoPerCompatibili(attuale->nParola);
                        compatibili++;
                    }
                }else  if(constraintRun(attuale->nParola)){
                    isFromNuInserisci=false;
                    aggiungiNodoPerCompatibili(attuale->nParola);
                    compatibili++;
                }
                attuale = attuale->rightSon;
            }
        }
    } 
}


void rimuoviDaCompatibili(TreeNode* nodo ){
    if( nodo == NULLO)
        return;
    TreeNode *nod;
    while(!constraintRun(nodo->nParola)){//dopo aver cancellato la parola continua dal suo sostituto
        nod = cancellazioneNodoAlbero(nodo);
        recycleIsGood(nodo);
        compatibili--;
        nodo=nod;
        if(nodo==NULLO)
            return;
    }
    if(nodo->leftSon!=NULLO)
        rimuoviDaCompatibili(nodo->leftSon);
    if (nodo->rightSon!=NULLO)
        rimuoviDaCompatibili(nodo->rightSon);
}
void aggiornaCompatibili( ){
    if(compatibili == 0) {
        fasterIterativeAggiungiACompatibili(treeHead);

    }else{
        do {
            treeModified=false;
            rimuoviDaCompatibili(testaComp );
        } while (treeModified);
        aggiustato=false;
    }
    printf("%d\n", compatibili);
}


int moaronna;
void recBalla(TreeNode *arr[],TreeNode*nodo ){//iterativamente inserisce in un'array ordinato O(n)<- potrebbero esserci casini per lo stack usato
    if(nodo==NULLO || moaronna>=compatibili)
        return;
    recBalla( arr,nodo->leftSon);
    arr[moaronna]=nodo;
    moaronna++;
    recBalla( arr,nodo->rightSon);

}

void attMorrisonPerArr(TreeNode *arr[]){// attraversamento di morrison (inorder senza stack) creo "l' array dell'albero "
    TreeNode *attuale;
    TreeNode *precedente;

    if (testaComp == NULLO)
        return;

    attuale = testaComp;
    while (attuale != NULLO) {
        if (attuale->leftSon == NULLO) {
            arr[moaronna]=attuale;
            moaronna++;
            attuale = attuale->rightSon;
        }
        else {
            precedente = attuale->leftSon;
            while (precedente->rightSon != NULLO
                   && precedente->rightSon != attuale)
                precedente = precedente->rightSon;

            if (precedente->rightSon == NULLO) {
                precedente->rightSon = attuale;
                attuale = attuale->leftSon;
            }
            else {
                precedente->rightSon = NULLO;
                arr[moaronna]=attuale;
                moaronna++;
                attuale = attuale->rightSon;
            }
        }
    }
}
TreeNode * reinstallaAlbero(TreeNode*arr[], int inizio, int fine){//usando l'array ricrea l'albero bilanciato
    if(inizio>fine )
        return NULLO;
    int met=(inizio+fine)/2;
    TreeNode * nodo=arr[met];
    nodo->leftSon=reinstallaAlbero(arr,inizio, met-1);
    nodo->leftSon->father=nodo;
    nodo->rightSon= reinstallaAlbero(arr, met+1,fine);
    nodo->rightSon->father=nodo;
    return nodo;
}
//approccio O(n) per ribilanciare un albero(i compatibili non sono bilanciati nella cancellazione)
//cosi eventuali nuovi inserimenti prendono solo O(log n ) per parola
void recreateTheTree(){
    TreeNode **arr=malloc(sizeof(TreeNode* )*compatibili);
    moaronna=0;
    attMorrisonPerArr( arr);
    testaComp= reinstallaAlbero(arr,0,compatibili-1);
    testaComp->father=NULLO;
}


void aggiornaCompatibiliNuovo(char* parolaNuova) {
    if( *confInPartita ==0)
        return;
    if (rispettaTotalMask(parolaNuova) ) {
        if(!aggiustato ) {
            recreateTheTree();
            aggiustato=true;
        }
        compatibili++;
        aggiungiNodoPerCompatibili(parolaNuova);

    }
}

bool startNuova;//usato unicamente per tenere conto se si è alla prima partita, potrei usare altre cose ma fa niente

void aggiungiAmmissibili(){
    int bo=1;

    do{
        int i=0;
        char in=(char)getchar_unlocked();
        if(in!='+') {
            do {
                if(in!='\r')//fixme eee
                {
                    wordk[i] = in;
                    i++;
                }
                in = (char)getchar_unlocked();

            } while (in != '\n' );
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
    if(startNuova)
       return;
    char in;
    do {
        in =(char) getchar_unlocked();
    } while (in != '\n');
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
    for (int i = 1; i < wordLen; ++i) {
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
struct maskOfPos *fastNotApp;

void initConstraintLists(){
    nuoviPos= malloc(sizeof(int)*wordLen);
    nuoviNotPos= malloc(sizeof(int)*wordLen);
    fastNotApp= malloc(sizeof(struct maskOfPos)*wordLen);
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
    fastNotApp[dimNP].notAppear=totalMask->ofPos[pos].notAppear;
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
            nNP++;
            if(!giaPresenteNotAppear(i))
                newNotAppearConstraint(i);
            totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])] = '1';
        }

    }

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
                        mascheraAt[i] = '|';//presente ma non qui
                    } else {
                        mascheraAt[i] = '/';//se ne sono gia apparse troppe
                    }
                    if(totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])]!='1'  ) {
                        totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])] = '1';
                        if(!giaPresenteNotAppear(i))
                             newNotAppearConstraint(i);
                    }
            }
        }else {
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
    aggiornaCompatibili();

}

bool esattamenteNlett(const char* parola, char lett, int N){
    int app=0;
    for (int i = 0; i < wordLen; ++i) {
        if (parola[i]==lett)
            app++;
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


    for (int i = 0; i < dimQ; ++i) {
        if(nuoviQuant[i].isMax ){
            if(!esattamenteNlett(parola,nuoviQuant[i].lettera,nuoviQuant[i].quant)) {
                return false;
            }

        }else if(!almenoNlett(parola,nuoviQuant[i].lettera,nuoviQuant[i].quant)) {
            return false;
        }
    }
    for (int i = 0; i < dimNP; ++i) {
        if(fastNotApp[i].notAppear[mappaCharToInt64(parola[nuoviNotPos[i]])]=='1')
            return false;
    }
    for (int i = 0; i < dimP; ++i) {
        if(parola[nuoviPos[i]]!=wordk[nuoviPos[i]])
            return false;
    }

    return true;
}


//------------------comandi
void stampaFiltrate(){
    if(compatibili > 0){
        attraversamentoIterativo(testaComp);

    }else
        attraversamentoIterativo(treeHead);
}

void nuovaPartita() {
    compatibili = 0;
    char in = (char) getchar_unlocked();
    int i = 0;
    if (in != '+') {
        do {
            if(in!='\r') {
                riferimento[i] = in;
                i++;
            }
            in = (char) getchar_unlocked();

        } while (in != '\n');
    }
    in = (char) getchar_unlocked();
    i = 0;
    if (in != '+') {
        do {
            wordk[i] = in;
            in = (char) getchar_unlocked();
            i++;
        } while (in != '\n');
    }
    wordk[i]='\0';
    tentativi = atoi(wordk);
    char sw;
    orderedRef = strcpy(orderedRef, riferimento);//fixme, ma anche no, viene fatto una sola volta, ci sta
    for (int l = 0; l < wordLen; ++l) {
        for (int j = l + 1; j <wordLen; ++j) {// stupidSort
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
                if(been!='\r'){//fixme controllo, \r causa problemi di input usando il generatore
                    buf[p] = been;
                    p++;
                }
                been= (char)getchar_unlocked();
            }
            buf[p]='\0';
            if(buf[0]=='i')
                aggiungiAmmissibili();
            else if(buf[0]=='s')
                stampaFiltrate();
            else if(buf[0]=='n') {//caso che accade unicamente per via di errori precedenti
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


//restituisce il puntatore alla funzione da chiamare
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
    riciclati=0;
    nPartite=0;
 //
    countConfronto= malloc(64 * sizeof(char));
    for (int i = 0; i < 64; ++i) {
        countConfronto[i]=0;
    }
    confInPartita= malloc(sizeof (int));
    *confInPartita=0;
    char input[30];
    char in=(char)getchar_unlocked();
    int i=0;
    while(in!='\n' && in!='+'){
        if(in!='\r') {
            input[i] = in;
            i++;
        }
        in=(char)getchar_unlocked();

    }
    input[i]='\0';
    wordLen= atoi(input);

    //allocazione buffer parole di lunghezza wordlen
    wordk= malloc((wordLen+1)* sizeof(char ));
    wordk[wordLen]='\0';

    //allocazione per batch
    bloccoDiMalloc= malloc(ADDEDBLOCK* sizeof (TreeNode));
    bloccoDiStringNU= malloc(ADDEDBLOCK* wordLen*sizeof(char));

    //allocazione riferimenti constraint
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

    testaComp=nullHead;
    treeHead=nullHead;

    //allocazione informazioni parola di riferimento
    orderedRef= malloc((wordLen+1)* sizeof(char ));
    orderedRef[wordLen]='\0';
    riferimento=malloc((wordLen+1)* sizeof(char ));
    riferimento[wordLen]='\0';
    mascheraAt= malloc((wordLen+1)* sizeof(char ));
    mascheraAt[wordLen]='\0';



    startNuova=true;
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
         void (*ptr)()=eseguiComando();
         ptr();
        if(startNuova){
            startNuova=false;
            nuovaPartita();
        }
         rest= (char)getchar_unlocked();
    }
}

