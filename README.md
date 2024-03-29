# api-2022-word-checker

[link sottoposizioni](https://dum-e.deib.polimi.it/)

## TOC

- [WordChecker](#wordchecker)
  - [Partenza programma](#partenza-programma)
  - [Nuova partita](#nuova-partita)
  - [Nuove parole compatibili](#nuove-parole-compatibili)
  - [Confronto parole](#confronto-parole)
  - [Fine partita](#fine-partita)
  - [Vincoli appresi da confronti](#vincoli-appresi-da-confronti)
  - [Stampa filtrate](#stampa-filtrate)


# WordChecker
* L'obiettivo del progetto di quest'anno è di realizzare un sistema che,
al suo cuore, controlla la corrispondenza tra le lettere di 2 parole di
ugual lunghezza.  
* Le parole sono intese come sequenze di simboli che possono essere
caratteri alfabetici minuscoli (a-z) o maiuscoli (A-Z), cifre numeriche
(0-9), oppure i simboli - (trattino) e _ ("underscore")  


*Esempio di parola di 20 simboli:*

        djHD1af9fj7g__l-ssOP
 

## Partenza programma
 Il sistema legge da standard input una sequenza di informazioni e istruzioni, e produce delle stringhe in output a seconda dei casi.  

*Più precisamente, il sistema legge:*
* un valore **wordLen**, che indica la lunghezza delle parole
* una sequenza *(di lunghezza arbitraria)* di parole, ognuna di lunghezza **wordLen**, che
costituisce l'insieme delle parole compatibili 
   
     
Si dia pure per scontato che la sequenza di parole non contenga duplicati

A quel punto, viene letta da standard input una sequenza di "partite" in cui l'inizio di ogni nuova partita è marcato dal comando (letto sempre da input) **+nuova_partita** .

-----

## Nuova partita
 Le sequenze di stringhe in input per ogni partita ( successive al comando
**+nuova_partita** ) sono fatte nel seguente modo:
* parola di riferimento (di lunghezza **wordLen** caratteri)  
(si assuma che la parola di riferimento appartenga all'insieme di parole compatibili)
* numero **tentativi** massimo di parole da confrontare con la parola di riferimento
* sequenza di parole (ognuna di **wordLen** caratteri) da confrontare con la parola di riferimento

 Ogni tanto, nella sequenza di stringhe in input, può comparire il comando
**+stampa_filtrate**, il cui effetto è spiegato in seguito   

-----

## Nuove parole compatibili 
 Inoltre, sia durante una partita, che tra una partita e l'altra, possono comparire i
comandi **+inserisci_inizio** e **+inserisci_fine** che racchiudono tra di loro una
sequenza di nuove parole da aggiungere all'insieme delle parole compatibili
* le parole aggiunte sono anch'esse di lunghezza **wordLen**, e si dà sempre per scontato che non ci
siano parole duplicate (neanche rispetto alle parole già presenti nell'insieme di quelle
compatibili)

-----

## Confronto parole
 Per ogni parola letta **(p)**, da confrontare con la parola di riferimento **(r)**, il programma scrive su standard output una sequenza di **wordLen** caratteri fatta nella seguente maniera.  
*( p[1], ... p[wordLen] caratteri della parola **p**, r[1], ... r[wordLen] caratteri di **r**, e con res[1], ... res[wordLen] caratteri della sequenza stampata )*

Per ogni 1 ≤ **i** ≤ **wordLen**, si ha che
* res[**i**] è il carattere **+** se l'i-esimo carattere di **p** è uguale all'i-esimo carattere di **r** .  
cioè se vale che p[**i**] = r[**i**], quindi p[**i**] è "in posizione corretta"
* res[**i**] è il carattere **/** se l'i-esimo carattere di **p** non compare da nessuna parte in **r** .
* res[**i**] è il carattere **|** se l'i-esimo carattere di **p** (indicato nel seguito come p[**i**]) compare in **r**, ma non in posizione i-esima;  

Tuttavia, se in r compaiono **N<sub>i</sub>** istanze di p[**i**], se **C<sub>i</sub>** è il numero di istanze del carattere p[**i**] che sono in posizione corretta (chiaramente deve valere che 
**C<sub>i</sub>** ≤ **N<sub>i</sub>** ) e se ci sono prima del carattere i-esimo in **p** almeno **N<sub>i</sub>**-**C<sub>i</sub>** i caratteri uguali a p[**i**] che sono in posizione scorretta, allora res[**i**] deve essere **/** invece che **|** .



Per esempio, se  
**r** = abcabcabcabcabc
(quindi r[1] = a, r[2] = b, ecc.) e  
**p** = bbaabccbccbcabc
(con p[1] = b, p[2] = b, ...), abbiamo che **res** risulta la seguente sequenza:

        abcabcabcabcabc
        bbaabccbccbcabc
        /+|+++|++/+++++  
  
  
Si noti che res[1] = / perché in **r** ci sono solo 5 "b", **p** ne ha 6, e tutte le "b" successive a p[1] sono nel posto corretto   
 similmente, res[10] = / perché **r** ha 5 "c", **p** ne ha 6, di cui 4 al posto giusto, e c'è già
una "c" prima di p[10] (in p[7]) che è al posto sbagliato.

*Altri esempi di confronti (dove la prima riga è la parola di riferimento r, la
seconda è p, e la terza è l'output res)*   

        djPDi939-s__e-s        djPDi939-s__e-s        
        gioSON-we2_w234        kiidsa92KFaa94-
        /|////|/|/+//|/        /|/||/|/////|/|


        djPDi939-s__e-s        DIk834k249kaoe_
        ewi-n4wp-sesr-v        48kDkkkf-saancd
        |/|/////++/|/+/        ||+||/+////+///



* Se da standard input viene letta una parola che non appartiene all'insieme
di quelle compatibili, il programma scrive su standard output la stringa **not_exists**.
* Se invece viene letta la parola **r** (cioè se **p** = **r**), allora il programma scrive **ok** *(senza stampare il risultato dettagliato del confronto)* e la partita termina.
* Se, dopo avere letto **tentativi** parole compatibili (con **tentativi**, si ricordi, numero
massimo di parole da confrontare con **r**), nessuna di queste era uguale a **r**, il programma scrive **ko** (dopo avere stampato il risultato del confronto dell'ultima parola), e la partita termina.  
* Inoltre, dopo ogni confronto, il programma deve stampare in output il numero di parole
compatibili ancora compatibili con i vincoli appresi (tranne nel caso in cui l'esito del
confronto sia **not_exists**) 


-----

## Fine partita
Dopo che la partita è finita:
* Non ci possono essere altre parole da confrontare (ma ci potrebbe essere
l'inserimento di nuove parole compatibili)
* Se in input c'è il comando **+nuova_partita**, ha inizio una nuova partita

-----

## Vincoli appresi da confronti
Ogni confronto tra **p** e **r** produce dei vincoli appresi dal confronto  
Per esempio, dal seguente confronto

        abcabcabcabcabc
        bbaabccbccbcabc
        /+|+++|++/+++++
si apprende che "b" è in posizioni 2, 5, 8, 11, 14,   
che ci sono solo 5 "b" in **r** (la sesta "b" dà luogo a */*),  
che "c è in posizioni 6, 9, 12, 15, che non è in posizioni 7 e 10, che ci sono solo 5 "c" (come prima, la sesta dà luogo a */*),  
che a è in posizioni 4 e 13, ma non è in posizione 3


In maniera analoga, dal seguente confronto

        djPDi939-s__e-s
        gioSON-we2_w234
        /|////|/|/+//|/
si apprende che in **r** non ci sono "g", né "o", né "S",   
che in **r** c'è almeno una "i" e che questa non è in posizione 2,  
 che c'è almeno un "-" e che non è in posizione 7, ecc.
  


Si noti che i vincoli appresi riguardano, per ogni simbolo:
1. se il simbolo non appartiene a **r**
2. posti in cui quel simbolo deve comparire in **r**
3. posti in cui quel simbolo non può comparire in **r**
4. numero minimo di volte che il simbolo compare in **r**
5. numero esatto di volte che il simbolo compare in **r**  
  *(si noti che il vincolo 5 è più forte del vincolo 4)*
> l'ordine dei simboli (usato per stabilire l'ordine lessicografico delle parole) è quello specificato dallo standard ASCII


-----

## Stampa filtrate
Quando, durante una partita, da input si legge il comando **+stampa_filtrate**, il
programma deve produrre in output, in ordine lessicografico, l'insieme delle parole
compatibili che sono compatibili con i vincoli appresi fino a quel momento nella partita,
scritte una per riga  .

