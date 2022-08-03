#include <string.h>
#include <stdbool.h>
#include "stdio.h"
#include "stdlib.h"

#define FROMFILE 1
#define INPUT fp

void salvaAmmissibile(char *word);
void nuovaPartita();
void leggiParolaPerConfronto();

void aggiungiAmmissibili();
bool accConfrontoLettera(int pos,char let1,char let2);
bool contieneAlmenoNecessarie(char *parola, int* infoSt);
bool contieneCaratteriNonPresenti(char* parola, char* nonPresenti, int quanteNP);
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
int salvate;
int compatibili;//relativo alla partita
char* riferimento;
char* orderedRef;
int wordLen;
int tentativi;
int confInPartita;//confronti fatti
FILE* fp;
refInfo * references;
char* mascheraAt;
int nPartite;
//------------------input



void leggiParolaPerConfronto(){
    char word[wordLen];
    word[0]= getc(INPUT);
    if(word[0]=='+'){
        void (*esec)()= eseguiComando();//fixme esegui funzione ricevuta da comando
        esec();
        return ;
    }
    for (int i = 1; i < wordLen; i++) {
        word[i] = getc(INPUT);
    }
    getc(INPUT);
    if(presenteTraAmmissibili(word) ){
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



void ruotaSX(TreeNode *nodo, TreeNode* testa){
    TreeNode *exFiglio=nodo->rightSon;
    nodo->rightSon=exFiglio->leftSon;
    if(exFiglio->leftSon != NULL)
        exFiglio->leftSon->father=nodo;
    exFiglio->father=nodo->father;
    if(nodo->father==NULL) {//aggiorna testa dell'albero
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
    if(exFiglio->rightSon != NULL)
        exFiglio->rightSon->father=nodo;
    exFiglio->father=nodo->father;
    if(nodo->father==NULL) {//aggiorna testa dell'albero
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
            bool isNero=true;
            if(nonno->rightSon!=NULL){
                isNero=nonno->rightSon->isBlack;
            }
            if (!isNero) {
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
            bool isNero=true;
            if(nonno->leftSon!=NULL)
                isNero=nonno->leftSon->isBlack;
            if(!isNero){
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
        padreNero=true;
        if(nuNodo->father!=NULL)
            padreNero=nuNodo->father->isBlack;
        else break;
    }
    testa->isBlack=true;
}

void scendiDaRootInsert(TreeNode *nuNodo, TreeNode *nodo){
    if(nodo!=NULL) {//fixme a che serve sto controllo
        if (nuNodo->chiave > nodo->chiave) {
            if (nodo->rightSon != NULL) {
                scendiDaRootInsert(nuNodo, nodo->rightSon);
                return;
            }
            nodo->rightSon = nuNodo;
        } else {
            if (nodo->leftSon != NULL) {
                scendiDaRootInsert(nuNodo, nodo->leftSon);
                return;
            }
            nodo->leftSon = nuNodo;
        }
    }
    nuNodo->father=nodo;
    nuNodo->rightSon=NULL;
    nuNodo->leftSon= NULL;


}
void aggiungiNodo(int key,TreeNode *testaAlbero ){
    TreeNode *nuNodo= malloc(sizeof (TreeNode));
    nuNodo->chiave=key;
    nuNodo->isBlack=false;

    if(testaAlbero->chiave==-1){//possibile ottimizzazione, rimuovi questo case?
        nuNodo->father=NULL;
        nuNodo->isBlack=true;
        nuNodo->leftSon=NULL;
        nuNodo->rightSon=NULL;
        if(testaAlbero==treeHead)
            treeHead=nuNodo;
        else testaComp=nuNodo;
    } else {
        scendiDaRootInsert(nuNodo, testaAlbero);
        sistemaInserimento(nuNodo, testaAlbero);
    }
}

TreeNode * minOfSubTree(TreeNode* nodo) {
    if(nodo->leftSon==NULL)
        return nodo;
    return minOfSubTree(nodo->leftSon);
}
//solo per i compatibili
void sistemaCancellazione(TreeNode* nodoSost) {
    while (nodoSost->father == NULL || !nodoSost->isBlack) {
        if (nodoSost == nodoSost->father->leftSon) {
            TreeNode *fratello = nodoSost->father->rightSon;
            if (!fratello->isBlack) {
                fratello->isBlack = true;
                nodoSost->father->isBlack = false;
                ruotaSX(nodoSost->father,testaComp);
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
                nodoSost->father = NULL;//this is the new root
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
        if(nodoSos!=NULL){
            nodoSos->father=nodo->father;
        }
        if(nodo->father!=NULL) {
            if (nodo->father->leftSon == nodo)
                nodo->father->leftSon = nodoSos;
            else nodo->father->rightSon = nodoSos;
        }
    }else if(nodo->rightSon==NULL) {
        nodoSos=nodo->leftSon;
        if(nodoSos!=NULL){
            nodoSos->father=nodo->father;
        }
        if(nodo->father!=NULL) {
            if (nodo->father->leftSon == nodo)
                nodo->father->leftSon = nodoSos;
            else nodo->father->rightSon = nodoSos;
        }

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
    if(nodoSos==NULL){
        free(nodo);
        return;
    }
    nodoSos->isBlack=wasBlack;//???corretto?
    if(wasBlack)
        sistemaCancellazione(nodoSos);
    free(nodo);//funzia??
}
char mappaInt64ToChar(int val){//todo ri-testare
    if(val<36){
        if(val==0)
            return 45;
        if(val>11) return (char)(val+54);
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
        res=key/(esp);
        target[wordLen - 1 - i]= mappaInt64ToChar(res);
        key-=res*esp;
        if(i != wordLen - 1)
            esp=esp/64;//occhio a 1/64
    }
}
//solo per stampa filtrate
void attraversamentoOrdinato(TreeNode* vertice){
    if(vertice==NULL){
        return;
    }
    attraversamentoOrdinato(vertice->leftSon);
    char nuf[6];
    chiaveToStringa((vertice->chiave),nuf);
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
        outKey+=mappaCharToInt64(stringa[i])*esp;
        esp=esp*64;
    }
    return outKey;
}
bool pres(int chiave, TreeNode* nodo){
    if(nodo==NULL)
        return false;
    if(nodo->chiave<chiave)
        return pres(chiave,nodo->rightSon);
    if(chiave<nodo->chiave)
        return pres(chiave,nodo->leftSon);
    else return true;
}
bool presenteTraAmmissibili(char* parola){
    int keyOfParola= string2chiave(parola);
    return pres(keyOfParola,treeHead);
}
TreeNode* trovaNodo(int val, TreeNode* nodo){
    if(nodo==NULL)
        return NULL;
    if(nodo->chiave<val)
        return trovaNodo(val,nodo->rightSon);
    if(val<nodo->chiave)
        return trovaNodo(val,nodo->leftSon);
    else return nodo;
}





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















//-------------------memoria



int isFromMem;
void recurAggComp(TreeNode* nodo, char* parola, int* presentiInNumero, char* nonPresenti, int nNonPresenti ){
    if(nodo==NULL)
        return;
    recurAggComp(nodo->leftSon, parola,  presentiInNumero,  nonPresenti, nNonPresenti);

    char stringa[wordLen];
    chiaveToStringa(nodo->chiave,stringa);
    bool ammissibile=true;
    for (int j = 0; j < wordLen; ++j) {
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
    //modifica compatibili
    if(ammissibile && isFromMem){
        salvaInCompatibili(stringa);
       // compatibili++;
    } else if(!ammissibile && !isFromMem){
        TreeNode * canc= trovaNodo(string2chiave(parola),testaComp);
        cancellazioneNodoAlbero(canc);
        compatibili--;
        if(compatibili==0)
            printf("errore filtri, 0 parole compatibili");
        return;
    }

    recurAggComp(nodo->rightSon, parola,  presentiInNumero,  nonPresenti, nNonPresenti);
}


void aggiornaCompatibili( char* parola, int* presentiInNumero, char* nonPresenti, int nNonPresenti){
    //draft senza ottimizzazioni
    TreeNode *provenienza;
    if(compatibili == 0) {
        provenienza = treeHead;
        isFromMem=1;
    }else{
        provenienza= testaComp;
        isFromMem=0;
    }
    recurAggComp(provenienza ,parola,  presentiInNumero,  nonPresenti, nNonPresenti);
    printf("%d \n", compatibili);

}


void aggiungiAmmissibili(){
    char word[wordLen];
    int bo=1;

    do{

        int i=0;
        char in=getc(INPUT);
        if(in!='+') {
            do {
                word[i] = in;
                in = getc(INPUT);
                i++;
            } while (in != '\n' && i < wordLen);
            salvaAmmissibile(word);
        }
        else{
            bo=0;
        }
    } while (bo==1);//fixme possono apparire comandi diversi da inserisci_fine?????????
    void (*esec)()= eseguiComando();
    esec();
}



void salvaAmmissibile(char* word){//con lista
/*    Nodo *nuovo= malloc(sizeof (Nodo));
    char* str= malloc(wordLen*sizeof (char));
    codaSv->stringa= strcpy(str,word);
    salvate++;
    nuovo->stringa=NULL;
    codaSv->successivo=nuovo;
    codaSv=nuovo;
*/
int chiave= string2chiave(word);
    aggiungiNodo(chiave,treeHead);
    salvate++;

}

void salvaInCompatibili(char* parola){
    /*
    Nodo *nuovo= malloc(sizeof (Nodo));
    char* str= malloc(wordLen*sizeof (char));
    codaComp->stringa=strcpy(str,parola);
    compatibili++;
    codaComp->successivo=nuovo;
    nuovo->stringa=NULL;
    codaComp=nuovo;
    */
    int chiave=string2chiave(parola);
    aggiungiNodo(chiave,testaComp);
    compatibili++;

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
    references= malloc(sizeof (riferimento));
    totalMask= malloc(sizeof(megaMask));
    posMask* posMaschera= malloc(wordLen * sizeof (posMask));//todo malloc the space for the structures
    for (int i = 0; i < wordLen; ++i) {
        char initAr[64];//fixme make of bits
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
    letSing[cc+1]='\0';
    references->nUniche=cc+1;
    references->appearing=letSing;
    references->numberOfApp=nApp;
    charInfo *arrInfo= malloc(sizeof (charInfo)*(cc+1));
    for (int i = 0; i < cc; ++i) {
        arrInfo[i].appears=0;
        arrInfo[i].letter=letSing[i];
        arrInfo[i].isMax=0;
    }
    totalMask->ofChars=arrInfo;
    nPartite++;
}

void initSuccessivi(){
    posMask* posMaschera= totalMask->ofPos;
    for (int i = 0; i < wordLen; ++i) {
        char initAr[64];//fixme make of bits,  (dimensione diminuisce di fattore 8, prob ignorabile)
        posMaschera[i].notAppear=initAr;
        posMaschera[i].sureValue=0;
    }

    char *letSing= references->appearing;
    int *nApp= references->numberOfApp;

    letSing[0]=orderedRef[0];
    int cc=0;

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
    letSing[cc+1]='\0';
    references->nUniche=cc+1;
    charInfo *arrInfo=totalMask->ofChars;
    for (int i = 0; i < cc; ++i) {
        arrInfo[i].appears=0;
        arrInfo[i].letter=letSing[i];
        arrInfo[i].isMax=0;
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
bool contieneAlmenoNecessarie(char *parola, int* infoSt){

    char presenti[references->nUniche];
    for (int i = 0; i < references->nUniche; ++i) {
        presenti[i]=0;
    }

    for (int i = 0; i < wordLen; ++i) {
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
    char nonPresenti[wordLen];
    for (int i = 0; i < references->nUniche; ++i) {
        presenti[i]=0;
    }
    int corrette=0;

    int nNP=0;
    for(int i=0; i < wordLen; i++){
        nonPresenti[i]='0';
        int pos= posOfLetterInReferences(parola[i]);
        if(parola[i] == riferimento[i]){
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
    for(int i =0; i < wordLen; i++) {
        int prs = presenzeInRef(parola[i]);
        if (prs > 0) {
            int pos = posOfLetterInReferences(parola[i]);
            if (parola[i] != riferimento[i]) {
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
    if(corrette == wordLen){
        printf("ok\n");
        confInPartita= tentativi + 1;
        return;
    }
    printf("%s\n",mascheraAt);
    aggiornaCompatibili( parola, presenti, nonPresenti, nNP);

}








//------------------comandi
void nuovaPartita(){
    char buff[wordLen];
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
    printf("%s\n", riferimento);
    in=(char)getc(INPUT);
    i=0;
    if(in!='+') {
         do{
            buff[i] = in;
            in = (char)getc(INPUT);
            i++;
        }while (in != '\n' );
    }
    tentativi = atoi(buff);
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

    confInPartita=0;
    while(confInPartita < tentativi){
        leggiParolaPerConfronto();
    }
    if(confInPartita == tentativi)
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
    }else fnPointer=leggiParolaPerConfronto;

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

    char input[5];
    char in=(char)getc(INPUT);
    int i=0;
    while(in!='\n' && in!='+'){
        input[i]=in;
        in=(char)getc(INPUT);
        i++;
    }

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
    char rest= (char)getc(INPUT);
    while (rest=='+'){
         void (*ptr)()=eseguiComando();
         ptr();
         rest= (char)getc(INPUT);
    }

}

