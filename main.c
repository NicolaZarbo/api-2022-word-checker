#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "stdio.h"
#include "stdlib.h"

#define FROMFILE 0
#define INPUT stdin
#define NULLKEY 0
#define REMOVEDBLOCK 20
#define ADDEDBLOCK 20

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

typedef struct maskOfPos{
    char sureValue;
    char* notAppear;//fixme make this int and use a map char to int related to this alphabet
}posMask;
typedef struct charInfo{
    int appears;
    int isMax;//fixme optimise
    //char letter;
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

typedef unsigned  int keyInt;
void salvaInCompatibili(char *parola);

//----------------globali
megaMask *totalMask;
char* parolanodo;
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
//char* target;
//----debug
char *wordk;
// ----------------





//-----------------------------------------------------------------------------RB Tree

typedef struct nodoAlbero{
    bool isBlack;
    struct nodoAlbero* leftSon;
    struct nodoAlbero* rightSon;
    struct nodoAlbero* father;
    keyInt chiave;//£ parola[i]*64^i;
}TreeNode;

TreeNode  *NULLO;
TreeNode *nullHead;
TreeNode *treeHead;
TreeNode *testaComp;

//int recContaPeso(TreeNode* nodo);
//char* debugKEY(int chiave);
void attraversamentoOrdinato(TreeNode* vertice);
void recycleIsGood(TreeNode* rimosso);

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

/*void ruotaDxSx(TreeNode *nodo){
    ruotaDX(nodo);
    ruotaSX(nodo);
}
void ruotaSxDx(TreeNode *nodo){
    ruotaSX(nodo);
    ruotaDX(nodo);
}
 */
void sistemaInserimento(TreeNode* nuNodo, TreeNode *testa) {//fixme a problem is here
    //bool padreNero=nuNodo->father->isBlack;
    /*
    if(strcmp("gDJL", debugKEY(nuNodo->chiave))==0)
        printf("debug eccoci");*/
    while (!nuNodo->father->isBlack) {
       // TreeNode *padre=nuNodo->father;
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
                }//else^?????
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
TreeNode * trovaNodoDaComp(keyInt chiave){
    TreeNode *nodo=testaComp;
    while (nodo!=NULLO){
        if(chiave>nodo->chiave)
            nodo=nodo->rightSon;
        else if(chiave<nodo->chiave)
            nodo=nodo->leftSon;
        else return nodo;
    }
    printf("error finding node");
    return NULLO;
}

void scendiDaRootInsert(TreeNode *nuNodo, TreeNode *nodo) {
    if (nodo==nuNodo) {//posso tenerlo fuori da questa funzione, migliora un po il coefficiente della complessità
        nuNodo->father = NULLO;
        nuNodo->leftSon = NULLO;
        nuNodo->rightSon = NULLO;
        //
        return;
    }
    if (nuNodo->chiave > nodo->chiave) {
        if(nodo->rightSon==NULLO){
            nodo->rightSon=nuNodo;
            nuNodo->father=nodo;
            nuNodo->leftSon=NULLO;
            nuNodo->rightSon=NULLO;
           // if(!nodo->father->isBlack)
             //   nuNodo->isBlack=true;
        }else  scendiDaRootInsert(nuNodo, nodo->rightSon);

    } else{
        if(nodo->leftSon==NULLO){
            nodo->leftSon=nuNodo;
            nuNodo->father=nodo;
            nuNodo->leftSon=NULLO;
            nuNodo->rightSon=NULLO;
            //if(!nodo->father->isBlack)
              //  nuNodo->isBlack=true;
        }else  scendiDaRootInsert(nuNodo, nodo->leftSon);
    }

}
int aggiunti;
TreeNode *bloccoDiMalloc;
//TreeNode **recycleBin;
TreeNode * recycleEnd;
int riciclati;

TreeNode *mutGarbodor(){
    if(riciclati>1) {
        recycleEnd = recycleEnd->father;
        return recycleEnd->rightSon;
    }
    return recycleEnd;
}

void aggiungiNodo(keyInt key,TreeNode *testaAlbero ){
    //= malloc(sizeof (TreeNode));
    TreeNode *nuNodo;
    if(riciclati>0){
       nuNodo=mutGarbodor();
        riciclati--;
    }else {
        nuNodo = &bloccoDiMalloc[aggiunti];
        aggiunti++;
        if (aggiunti == ADDEDBLOCK) {
            bloccoDiMalloc = malloc(sizeof(TreeNode) * ADDEDBLOCK);
            aggiunti = 0;
        }
    }
    nuNodo->chiave=key;
    nuNodo->isBlack=false;
    nuNodo->father = NULLO;
    nuNodo->leftSon = NULLO;
    nuNodo->rightSon = NULLO;
    /*
    if(presenteTraAmmissibili(debugKEY(key)) && testaAlbero==treeHead)
        printf("oh no\n");*/

        TreeNode * discensore=testaAlbero;
        TreeNode *padre=NULLO;
        //scendiDaRootInsert(nuNodo, testaAlbero);
        //test controllo inserimento
        while(discensore!=NULLO){
            padre=discensore;
            if(nuNodo->chiave>discensore->chiave)
                discensore= discensore->rightSon;
            else discensore=discensore->leftSon;
        }
        nuNodo->father=padre;
        if(padre==nullHead)
        {
            if(testaAlbero==treeHead)
                treeHead=nuNodo;
            else testaComp=nuNodo;
            nuNodo->father=NULLO;
            nuNodo->isBlack=true;
            return;
        }
        else if(nuNodo->chiave>padre->chiave)
            padre->rightSon=nuNodo;
        else padre->leftSon=nuNodo;

        if(nuNodo->father->father==NULLO)
            return;



        //if(nuNodo->father->father==NULLO)
          //  return;
   //if(strcmp("7D7d", debugKEY(nuNodo->chiave))==0)
     //   printf("debug eccoci");

        sistemaInserimento(nuNodo, testaAlbero);
    /*//debug
    printf("inserito %s\n", debugKEY(nuNodo->chiave));
    printf("figli di :%s ;peso sx %d peso dx %d\n", debugKEY(nuNodo->father->chiave), recContaPeso(nuNodo->father->leftSon), recContaPeso(nuNodo->father->rightSon));
    if(recContaPeso(treeHead)>1000)
        printf("ero insert");
   // }*/

}

TreeNode * minOfSubTree(TreeNode* nodo) {
    while (nodo->leftSon!=NULLO){
        nodo=nodo->leftSon;
    }
    return nodo;

        /*
    if(nodo->leftSon==NULLO)
        return nodo;
    return minOfSubTree(nodo->leftSon);
    */
}
//solo per i compatibili
void sistemaCancellazione(TreeNode* nodoSost) {
    /*if(nodoSost->father==NULL&& nodoSost->leftSon==NULL&& nodoSost->rightSon==NULL) {
        testaComp = nodoSost;
        return;
    }*/
    while (nodoSost != testaComp && nodoSost->isBlack) {
        if (nodoSost == nodoSost->father->leftSon) {
            TreeNode *fratello = nodoSost->father->rightSon;
            if (!fratello->isBlack) {
                fratello->isBlack = true;
                nodoSost->father->isBlack = false;
                ruotaSX(nodoSost->father, testaComp);// qui contronto eliminazione 2_z91
                fratello = nodoSost->father->rightSon;
            }
            if (fratello->leftSon->isBlack && fratello->rightSon->isBlack) {
                if(fratello!=NULLO)
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
                //nodoSost->father = NULLO;//this is the new root
                //treeHead = nodoSost;
                nodoSost=testaComp;
                }

        } else {
            TreeNode *fratello = nodoSost->father->leftSon;
            if (!fratello->isBlack) {
                fratello->isBlack = true;
                nodoSost->father->isBlack = false;
                ruotaDX(nodoSost->father, testaComp);
                fratello = nodoSost->father->leftSon;
            }
            if (fratello->rightSon->isBlack && fratello->leftSon->isBlack) {
                fratello->isBlack = false;
                nodoSost = nodoSost->father;
            } else {
                if (fratello->leftSon->isBlack) {
                    fratello->rightSon->isBlack = true;
                    fratello->isBlack = false;
                    ruotaSX(fratello, testaComp);
                    fratello = nodoSost->father->leftSon;
                }
                fratello->isBlack = nodoSost->father->isBlack;
                nodoSost->father->isBlack = true;
                fratello->leftSon->isBlack = true;
                ruotaDX(nodoSost->father,testaComp);
                //nodoSost->father = NULLO;//this is the new root fixme necessaria??
                nodoSost=testaComp;
            }
        }

    }
    nodoSost->isBlack = true;
    //testaComp=nodoSost;
}
//per contrarre i compatibili
void cancellazioneNodoAlbero(TreeNode* nodo){
    if(NULLO->father!=NULLO)
        printf("errore null");
    if(nodo->rightSon==NULLO && nodo->leftSon==NULLO)
    {

        if(nodo==testaComp){
            testaComp=nullHead;
           /* testaComp->leftSon=NULLO;
            testaComp->rightSon=NULLO;
            testaComp->chiave=ROOTKEY;*/
        }

        else {
            if (nodo->father->leftSon == nodo){
                nodo->father->leftSon = NULLO;//a caso
               // ruotaSX(nodo->father,testaComp);
            }

            else {
                nodo->father->rightSon = NULLO;
             //   ruotaDX(nodo->father,testaComp);

            }
        }
        /*if(strcmp("qDJL",parolanodo)==0){
            printf("canc 2\n");
            attraversamentoOrdinato(testaComp);
        }*/
        return;
    }
    else if(nodo->leftSon==NULLO){
    //    nodoSos=nodo->rightSon;
        innestaSopra(nodo->rightSon, nodo, testaComp);

    }else if(nodo->rightSon==NULLO) {
        //nodoSos=nodo->leftSon;
        innestaSopra(nodo->leftSon,nodo,testaComp);
    }else {
        TreeNode *min = minOfSubTree(nodo->rightSon);
   //     wasBlack = min->isBlack;
        //nodoSos = min->rightSon;//fixme e se min non ha figli destri???
        if (min->father == nodo) {
           // nodoSos->father = min;
            NULLO->father=NULLO;//debug elimina serve per non cambiare struttura if
        } else {
            innestaSopra(min->rightSon,min,testaComp);//ottimizzabili(poco)

            min->rightSon=nodo->rightSon;//capire meglio
            min->rightSon->father=min;
        }

        innestaSopra(min,nodo,testaComp);
        min->leftSon=nodo->leftSon;
        min->leftSon->father=min;
        min->isBlack = nodo->isBlack;//???corretto?
        //if(nodo->father==NULLO)
          //  testaComp=min;
    }
   /* if(strcmp("qDJL",parolanodo)==0){
        printf("cancellato\n");
        attraversamentoOrdinato(testaComp);
    }*/
 //   if(testaComp->father!=NULLO)
   //     printf("errore, root strano");
    //if(wasBlack)
      //  sistemaCancellazione(nodoSos);

   // free(nodo);//funzia??todo come faccio a de-allocare, questo incasina tutto
    if(NULLO->father!=NULLO)
        printf("errore null");
}
char mappaInt64ToChar(keyInt val){//todo ri-testare
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
int powerOfKFor(){
    int out=1;
    for (int i = 0; i < wordLen - 1; ++i) {
        out=out*64;
    }
    return out;
}
void chiaveToStringa(keyInt chiave, char* buffer){
    keyInt key=chiave;
    keyInt esp= powerOfKFor();
    keyInt res;
    for (int i = 0; i < wordLen; ++i) {
        res=key/(esp);//fixme ogni tanto accadono cose strane qui e va in segfault
        buffer[i]= mappaInt64ToChar(res);
        key-=res*esp;
        if(i != wordLen - 1)
            esp=esp/64;
    }
    buffer[wordLen]='\0';
}

void attraversamentoIterativo(TreeNode* nodo){
    char nuf[wordLen+1];
    nuf[wordLen]='\0';
    TreeNode *attuale=nodo;
    TreeNode *precedente=NULLO;
    while (attuale!=NULLO){
        TreeNode * successivo;
        if(precedente==NULLO||precedente==attuale->father){
            if(attuale->leftSon!=NULLO)
                successivo=attuale->leftSon;
            else{
                chiaveToStringa((attuale->chiave),nuf);
                printf("%s\n",nuf);
                if(attuale->rightSon!=NULLO)
                    successivo=attuale->rightSon;
                else successivo=attuale->father;
            }

        } else if(precedente==attuale->leftSon){
            chiaveToStringa((attuale->chiave),nuf);
            printf("%s\n",nuf);
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
keyInt string2chiave(char* stringa){
    keyInt outKey=0;
    keyInt esp=1;
    for (int i = 0; i < wordLen; ++i) {
        outKey+=mappaCharToInt64(stringa[wordLen-1-i])*esp;
        esp=esp*64;
    }
    return outKey;
}
bool pres(keyInt chiave, TreeNode* nodo){//todo rendere iterativo
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
        if(nodo->chiave>chiave)
            nodo=nodo->leftSon;
        else if( nodo->chiave<chiave)
            nodo=nodo->rightSon;
        if(nodo->chiave==chiave)
            return true;
    }
    return false;
}
bool presenteTraAmmissibili(char* parola){
    if(treeHead==nullHead)
        return false;//debug
    keyInt keyOfParola= string2chiave(parola);
    return pres(keyOfParola,treeHead);
}
/*
bool presenteTraComp(char* parola){
    int keyOfParola= string2chiave(parola);
    return pres(keyOfParola,testaComp);
}*/
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








/*
// *************DEBUG

char* debugKEY(int chiave){
    int key=chiave;
    int esp= powerOfKFor(64);
    int res;
    for (int i = 0; i < wordLen; ++i) {
        res=key/(esp);
        target[i]= mappaInt64ToChar(res);
        key-=res*esp;
        if(i == wordLen - 1)
            break;
        esp=esp/64;
    }
    target[wordLen]='\0';
    return target;
}

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
    if(in==EOF){
        *confInPartita=tentativi+20;
        printf("wat\n");
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
        confronto();
        //*confInPartita=*confInPartita+1;
        return 2;
    }else{
        printf("not_exists\n");
        return 0;
    }

}

bool treeModified;
bool ammissibileDaConfronto(char* nonPresenti, int nNonPresenti){
    for (int j = 0; j < wordLen; ++j) {
        if(!accConfrontoLettera(j, wordk[j], parolanodo[j])) {
            return false;
        }
    }
    if (!contieneAlmenoNecessarie(parolanodo))
        return  false;
    if (contieneCaratteriNonPresenti(parolanodo, nonPresenti, nNonPresenti))
        return  false;

    return true;

}
int nNonPr;
int removed;
keyInt *arrayForRemoval;
TreeNode *garbodor;//tree of junk
//aka feed garbodor
void recycleIsGood(TreeNode* rimosso){

    if(riciclati==0){
        garbodor=rimosso;
        garbodor->father=NULLO;
        garbodor->rightSon=NULLO;
        recycleEnd=garbodor;
    }else {

        recycleEnd->rightSon=rimosso;
        rimosso->father=recycleEnd;
        rimosso->rightSon=NULLO;
        recycleEnd=rimosso;
    }
    riciclati++;

   // recycleBin[riciclati]=rimosso;

}
void removalOfSelected(){
    TreeNode *cancella;
    for (int i = 0; i < removed; ++i) {
        cancella=trovaNodoDaComp(arrayForRemoval[i]);
        cancellazioneNodoAlbero(cancella);
        //free(cancella);//invalid free??  problema : roba allocata in batch, va deallocata in batch -> non faccio le free ma riutilizzo spazio
        recycleIsGood(cancella);
    }
    removed=0;
}
void aggCompDaAmmissibili(TreeNode* nodo ){
    if(nodo==NULLO)
        return;
    chiaveToStringa(nodo->chiave, parolanodo);
    if(ammissibileDaConfronto(nonPres,nNonPr)){//todo riscrivere condizione basate su int
        salvaInCompatibili(parolanodo);
        compatibili++;
        if(NULLO->father!=NULLO)
            printf("null error");
    }
    aggCompDaAmmissibili(nodo->leftSon);
    aggCompDaAmmissibili(nodo->rightSon);
}
void rimuoviDaCompatibili(TreeNode* nodo ){
    if(treeModified || nodo == NULLO)
        return;
    chiaveToStringa(nodo->chiave,parolanodo);
    if(!ammissibileDaConfronto(nonPres,nNonPr)){
        arrayForRemoval[removed]=nodo->chiave;
        removed++;

        //cancellazioneNodoAlbero(nodo);
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

void aggiornaCompatibili( int nNonPresenti){//tenere questi parametri fuori da stack<=ricorsione pesante
    nNonPr=nNonPresenti;
    if(compatibili == 0) {
        aggCompDaAmmissibili(treeHead);
        //aggCompDaAmmIterativo(treeHead);
    }else{
        do {
            treeModified=false;
            rimuoviDaCompatibili(testaComp );
        } while (treeModified);
            //rimuoviDaaCompIterativo(testaComp);
        removalOfSelected();
    }

    printf("%d\n", compatibili);

}
bool rispettaTotalMask(char* string){
    if(!contieneAlmenoNecessarie(string))
        return false;
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
        compatibili++;
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
            //debugControlloAlbero(testaComp);
            //debugControlloAlbero(treeHead);
        }
        else{
            bo=0;
        }
    } while (bo==1);//fixme possono apparire comandi diversi da inserisci_fine?????????
    //legge inserisce_fine
    if(nPartite==0)
        return;
    char in;
    do {
        in =(char) getc(INPUT);
    } while (in != '\n');
    //void (*esec)()= eseguiComando();
    //esec();
}



void salvaAmmissibile(char* word){
    keyInt chiave= string2chiave(word);
    //bloccoDiMalloc= malloc(sizeof(TreeNode)*ADDEDBLOCK);
    //aggiunti=0;//todo conservare valore di aggiunti tra una roba e l'altra per evitare tutti i leak
    aggiungiNodo(chiave,treeHead);
    salvate++;

}

void salvaInCompatibili(char* parola){
    keyInt chiave=string2chiave(parola);
    //bloccoDiMalloc= malloc(sizeof(TreeNode)*ADDEDBLOCK);
    //aggiunti=0;
    aggiungiNodo(chiave,testaComp);
    if(NULLO->father!=NULLO)
        printf("null error");
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
    nonPres= malloc((wordLen+1)* sizeof(char ));
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
       // arrInfo[i].letter=letSing[i];
        arrInfo[i].isMax=0;
    }
    totalMask->ofChars=arrInfo;
    nPartite++;
}

void initSuccessivi(){//todo qualcosa di molto brutto accade qui da qualche parte
    deAllocaCompatibili(testaComp);
    //rinnovo compatibili
    //testaComp= malloc(sizeof (TreeNode));//si perde ogni volta la testa temporanea, si potrebbe riusare il puntatore
   /* testaComp->father=NULLO;
    testaComp->leftSon=NULLO;
    testaComp->isBlack=true;
    testaComp->rightSon=NULLO;
    testaComp->chiave=ROOTKEY;*/
    testaComp=nullHead;
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
       // totalMask->ofChars[i].letter=references->appearing[i];
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
    bool ok=true;
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
void confronto(){//fixme, qualcosa non funziona, capire perché
    int dime=references->nUniche;
    int presenti[dime];
   // nonPres[wordLen];
    for (int i = 0; i < references->nUniche; ++i) {
        presenti[i]=0;
    }
    int corrette=0;
    //todo controlla maschera agg per "-s9k0"
    //if(strcmp("Cbbbd", parola)==0)
      //  printf("debug \n");
    int nNP=0;
    for(int i=0; i < wordLen; i++){
        nonPres[i]='0';
        int pos= posOfLetterInReferences(wordk[i]);
        if(wordk[i] == riferimento[i]){
            mascheraAt[i]='+';
          //  salvaPres[corrette]=parola[i];
            if(pos>=0)
                presenti[pos]++;
            else NULLO->father=NULLO;//lmao
            corrette++;
            totalMask->ofPos[i].sureValue=wordk[i];
        }else if(pos==-1){
            nonPres[nNP]=wordk[i];
            nNP++;
            totalMask->ofPos[i].notAppear[mappaCharToInt64(wordk[i])]=1;
        }
    }
  /*  if(strcmp("AAbld",wordk)==0) {
        printf("debug +\n");
    }*/
    //salvaPres[corrette]='\0';
    nonPres[nNP]='\0';
    for(int i =0; i < wordLen; i++) {
        int prs = presenzeInRef(wordk[i]);
        if (prs > 0) {
            int pos = posOfLetterInReferences(wordk[i]);
            if (wordk[i] != riferimento[i]) {
                if(pos>=0 && pos<wordLen){
                    presenti[pos]++;
                    if (prs >= presenti[pos]){
                        //corrette++; nn serve
                        mascheraAt[i] = '|';
                    } else mascheraAt[i] = '/';
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
        //totalMask->ofChars[i].letter=references->appearing[i];
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
    aggiornaCompatibili(   nNP);

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
           // con++;
            *confInPartita = *confInPartita+1;
        } else if (xx == 1 ) {

           // char buf[20];
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
            else if(buf[0]=='n'){
                printf("what!?\n");
                return;
            }
            //esec = eseguiComando();
            //esec();
        }
    }

    if (*confInPartita == tentativi)
        printf("ko\n");


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
    if(strcmp("inserisci_inizio", buf)==0){//ridefinsici compare x questo caso
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
    //target= malloc(wordLen+1);

    //testAlbero();

    if(FROMFILE>0){
        if ((fp = fopen("C:/Users/User/CLionProjects/api-2022-word-checker/open_testcases/test2.txt", "r")) == NULL){
            printf("File cannot open");
            exit(1);
        }
    }

    riciclati=0;
    arrayForRemoval= malloc(sizeof (keyInt)*REMOVEDBLOCK);
    parolanodo= malloc((wordLen+1)* sizeof(char ));
    parolanodo[wordLen]='\0';

    wordk= malloc((wordLen+1)* sizeof(char ));
    wordk[wordLen]='\0';

    bloccoDiMalloc= malloc(ADDEDBLOCK* sizeof (TreeNode));


    NULLO= malloc(sizeof(TreeNode));
    NULLO->father=NULLO;
    NULLO->leftSon=NULLO;
    NULLO->rightSon=NULLO;
    NULLO->isBlack=true;
    NULLO->chiave=NULLKEY;//nullkey

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
    wordLen= atoi(input);
    nullHead=malloc(sizeof(TreeNode));
    testaComp=nullHead;
    treeHead=nullHead;
    nullHead->father=NULLO;
    nullHead->leftSon=NULLO;
    nullHead->rightSon=NULLO;
    /*testaComp= malloc(sizeof(TreeNode));
    testaComp->chiave=ROOTKEY;
    testaComp->father=NULLO;
    testaComp->leftSon=NULLO;
    testaComp->rightSon=NULLO;

    treeHead= malloc(sizeof (TreeNode));
    treeHead->chiave=ROOTKEY;
    treeHead->father=NULLO;
    treeHead->leftSon=NULLO;
    treeHead->rightSon=NULLO;*/

    orderedRef= malloc((wordLen+1)* sizeof(char ));
    orderedRef[wordLen]='\0';
    riferimento=malloc((wordLen+1)* sizeof(char ));
    riferimento[wordLen]='\0';
    mascheraAt= malloc((wordLen+1)* sizeof(char ));
    mascheraAt[wordLen]='\0';

    aggiungiAmmissibili();
    startNuova=false;
    //debugControlloAlbero(treeHead);
    void (*exec)()= eseguiComando();//esegui comando nel caso avvenga roba prima di nuova partita => inizializzare roba di init prima
    exec();
    if(startNuova){
        startNuova=false;
        nuovaPartita();
    }

    char rest= (char)getc(INPUT);
    while (rest=='+'){
         void (*ptr)()=eseguiComando();
         ptr();
        if(startNuova){
            startNuova=false;
            nuovaPartita();
        }
         rest= (char)getc(INPUT);
    }
    //printf("terminato");
   // exit(0);
}

