# Guida alla sopravvivenza alla programmazione in Linux

[TOC]

------

## 0. Introduzione

Questa non vuole essere una guida alla programmazione in Linux. Si tratta di una guida alla sopravvivenza, con piccoli tips e un ripasso delle principali system call.

Si consiglia comunque di navigare sulla pagina `man` da terminale (oppure sul sito [man7.org](https://www.kernel.org/doc/man-pages/)) specificando la voce alla quale si è interessati, in modo da comprendere appieno tutte le caratteristiche dei comandi.

### Compilazione di un programma in C

Dallo Shell, utilizzare `gcc` nel seguente modo:

```bash
gcc -o <nome del file eseguibile> <nome file sorgente.c>
```

L'opzione `-o` seguita da `<nome del file eseguibile>` comunica a `gcc` che deve creare un file eseguibile chiamato `<nome del file eseguibile>`. Ad esempio, se dalla shell si digita il comando `gcc -o hello hello.c`, il file sorgente `hello.c` verrà compilato e il file eseguibile sarà chiamato `hello`. Se non si scrive `-o filename`, il compilatore `gcc` crea un eseguibile chiamato `a.out`.

L'esecuzione di un programma da shell prevede la seguente sintassi:

```bash
./<nome eseguibile> [lista argomenti invocazione]
```

In generale, la sintassi del comando `gcc` è: `gcc <opzioni> <filename>`
Oltre a `-o`, le opzioni più comuni sono:

- `-c` per creare il file oggetto anziché l'eseguibile: `gcc -c hello.c` genera il file oggetto `hello.o`
- `-Wall` genera tutti i messaggi di warning che `gcc` può fornire
- `-pedantic` mostra tutti gli errori e i warning richiesti dallo standard ANSI C
- `-O -O1 -O2 -O3` servono per definire il livello di ottimizzazione (dal più basso al più alto)
- `-O0` per non avere nessuna ottimizzazione
- `-g` per un successivo debugging

Per creare l'eseguibile a partire dal file oggetto: `gcc hello.o -o hello`
Quest'ultima operazione prende il nome di **linking**, che in generale consiste nella risoluzione dei simboli tra programmi e l'inclusione di eventuali librerie.

### Strace

Il programma `strace` consente di visualizzare le system call (con i relativi argomenti) invocate da un processo in esecuzione. È uno strumento di debug molto istruttivo che permette di ottenere, in tempo reale su schermo o in un file di testo, numerose informazioni sull'esecuzione di un programma.

Può essere invocato in due modalità principali:

1. `strace nomeprogramma [argomenti]` (esegue `nomeprogramma`)
2. `strace -p <pid processo in esecuzione>` (visualizza le SVC invocate dal processo con il PID specificato, già in esecuzione ).

Per memorizzare su le la traccia delle system call invocate, utilizzare l'opzione `-o` : ad es.

```bash
strace -o traccialog.txt nomeprogramma
```

Per ottenere la traccia delle system call invocate da un processo e dai suoi processi figli, utilizzare l'opzione `-f` : ad es.

```bash
strace -f nomeprogramma
```

Si tratta di una funzionalità molto importante, difficilmente ottenibile in un normale debugger che può soltanto seguire il processo padre oppure il processo glio (cfr. il comando `set follow-fork-mode` di gdb).

Per le altre opzioni consultare il manuale di `strace`.

------

## 1. Chiamate di sistema per I/O

Quando un programma viene messo in esecuzione, possiede almeno tre file descriptor:

```c
0 Standard input
1 Standard output
2 Standard error
```

Vediamo le principali chiamate di sistema che fanno uso dei descrittori di file. La maggior parte di queste chiamate restituisce `-1` in caso di errore e assegna alla variabile `errno`il codice di errore. I codici di errore sono documentati nelle pagine di manuale delle singole chiamate di sistema e in quella di `errno`. La funzione `perror()` può essere utilizzata per visualizzare un messaggio di errore basato sul relativo codice.

### `write`

```c
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);
```

Dove:

- `fd`: file descriptor su cui effettuare la scrittura
- `buf`: buffer che contiene il messaggio da andare a scrivere nel file descriptor indicato
- `count`: numero di byte da andare a scrivere nel file descriptor

Con la funzione `write()`, i primi `count` byte di `buf` vengono scritti
nel file che è stato associato al file descriptor `fd`. La chiamata restituisce il numero dei byte scritti oppure `-1` se si è verificato un errore.

Per maggiori informazioni vedere [man 2 write](https://man7.org/linux/man-pages/man2/write.2.html).

### `read`

```c
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
```

Dove:

- `fd`: file descriptor su cui effettuare la lettura
- `buf`: area di memoria in cui salvare i byte letti
- `count`: numero di byte da leggere

Con la funzione `read()`, vengono letti i primi `count` byte dal file descriptor `fd` e salvati nel `buf`. La chiamata alla funzione restituisce il numero di byte letti.

Per maggiori informazioni vedere [man 2 read](https://man7.org/linux/man-pages/man2/read.2.html).

### `open` e `close`

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

Dove:

- `pathname`: percorso del file da aprire
- `flags`: modo di accesso per il file. Si usano le voci:
  - `O_RDONLY` per aprire il file in sola lettura
  - `O_WRONLY` per aprire il file in sola scrittura
  - `O_RDWR` per aprire il file in scrittura e lettura
- `mode`: modalità di accesso per il file (indispensabile quando si usa come `flags` il valore `O_CREAT`) in formato ottale per user, group e others

Se la chiamata alla funzione ha successo, viene restituito il file descriptor del file specificato; altrimenti si ritorna `-1`.

Per maggiori informazioni vedere [man 2 open](https://man7.org/linux/man-pages/man2/open.2.html).

```c
#include <unistd.h>

int close(int fd);
```

Dove:

- `fd`: file descriptor del file da chiudere

La chiamata alla funzione chiude il file descriptor indicato come parametro e tutte le risorse ad esso associato sono liberate.

Per maggiori informazioni vedere [man 2 close](https://man7.org/linux/man-pages/man2/close.2.html).

### `sendfile`

```c
#include <sys/sendfile.h>

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
```

Dove:

- `out_fd`: file descriptor di destinazione
- `in_fd`: file descriptor di partenza
- `offset`: punto in cui iniziare la lettura del file `in_fd`
- `count`: numero di byte da copiare in `out_fd`

La funzione `sendfile()` copia i dati da un file descriptor a un altro rimanendo all'interno del kernel. Risulta essere più efficiente dell'utilizzo combinato di `read` e `write`, che invece trasferiscono dati tra spazio utente e kernel. La chiamata alla funzione `sendfile` restituisce il numero di byte scritti in `out_fd`.

Per maggiori informazioni vedere [man 2 sendfile](https://man7.org/linux/man-pages/man2/sendfile.2.html).

### `lseek`

```c
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

Dove:

- `fd`: file descriptor su cui operare
- `offset`: punto in cui posizionare il cursore all'interno del file descriptor
- `whence`: direttiva di comportamento. Può essere una tra:
  - `SEEK_SET`: il file offset diventa `offset` byte dall'inizio del file `fd`
  - `SEEK_CUR`: al file offset sono aggiunti `offset` byte dalla sua posizione corrente
  - `SEEK_END`: il file offset diventa la somma della dimensione del file più `offset` byte dalla fine del file

La chiamata alla funzione `lseek()` sposta il cursore nel file `fd` di `offset` byte.

Alcuni utilizzi:

1. Riportare il cursore a inizio file: `lseek(fd, 0, SEEK_SET)`
2. Posizionare il cursore alla fine del file: `lseek(fd,0,SEEK_END)`

Per maggiori informazioni vedere [man 2 lseek](https://man7.org/linux/man-pages/man2/lseek.2.html).

### `fstat`

```c
#include <sys/stat.h>
#include <unistd.h>

int fstat(int fd, struct stat *buf);
```

Dove:

- `fd`: file descriptor
- `buf`: destinazione delle informazioni

La chiamata alla funzione `fstat()` restituisce le informazioni del file `fd` e le salva all'interno di `buf`.

Per maggiori informazioni vedere [man 2 fstat](https://man7.org/linux/man-pages/man2/fstat.2.html).

### `dup` e `dup2`

```c
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);
```

Dove:

- `oldfd`: file descriptor di cui eseguire la copia da inserire in tabella nel primo posto libero
- `newfd`: file descriptor specificato dall'utente

Le chiamate di sistema `dup()` e `dup2()` creano una copia del descrittore di file `oldfd`. Il nuovo descrittore è, nel caso della `dup()`, il descrittore non utilizzato con numero d'ordine più basso; nel caso della `dup2()`, è il `newfd` specificato dall'utente.

Per maggiori informazioni vedere [man 2 dup](https://man7.org/linux/man-pages/man2/dup.2.html) e [man 2 dup2](https://man7.org/linux/man-pages/man2/dup2.2.html).

### `link` e `unlink`

```c
#include <unistd.h>

int link(char *original_name, char *alias_name);
int unlink(char *alias_name);
```

Dove:

- `original_name`: file descriptor
- `alias_name`: nome del link

La chiamata alla funzione `link()` crea un hard link al file `fd`. La chiamata alla funzione `unlink()`, invece, rimuove l'hard link.

Per maggiori informazioni vedere [man 2 link](https://man7.org/linux/man-pages/man2/link.2.html) e [man 2 dup2](https://man7.org/linux/man-pages/man2/unlink.2.html).

------

## 2. Controllo dei processi

### `fork`

```c
#include <unistd.h>
pid_t fork(void);
```

La funzione restituisce un valore intero che rappresenta il pid del processo figlio; in caso di errore, ritorna con `-1`. La chiamata alla funzione crea un processo figlio che esegue lo stesso codice, dispone di una copia dell'area dati e della tabella dei file aperti del padre, ma possiede un pid diverso. Tra processo padre e figlio non c'è nessuna condivisione di memoria!

Per il figlio, il pid è `0` mentre per il padre rappresenta il proprio identificativo.

Per fare eseguire operazioni diverse a padre e figlio si usa fare:

```c
if((pid = fork())==0)
{
 //PROCESSO FIGLIO
 ...
} else
{
 // PROCESSO PADRE
 ...
}
```

Per maggiori informazioni vedere [man 2 fork](https://man7.org/linux/man-pages/man2/fork.2.html).

### `getpid` e `getppid`

```c
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void);
pid_t getppid(void);
```

La chiamata alla funzione `getpid()` restituisce il pid del processo attuale. La chiamata alla funzione `getppid()` restituisce il pid del processo padre.

Per maggiori informazioni vedere [man 2 getpid](https://man7.org/linux/man-pages/man2/getpid.2.html).

### `exit`

```c
void exit(int status);
```

Dove:

- `status`: valore d'uscita del processo figlio

La chiamata alla funzione `exit()` chiude tutti i file aperti, per il processo che termina. Il valore d'uscita viene passato al processo padre, se questo attende la terminazione di un suo figlio. La terminazione del processo figlio può avvenire involontariamente quando vengono eseguite azioni non consentite, segnali da tastiera (`^C` o `^\`) oppure da segnali inviati da terminale (attraverso il comando `kill -<segnale> <pid>`)

Per maggiori informazioni vedere [man 2 exit](https://man7.org/linux/man-pages/man2/exit.2.html).

### `wait` e `waitpid`

```c
#include <sys/wait.h>
#include <sys/types.h>

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
```

Dove:

- `status`: valore d'uscita del processo figlio
- `pid`: pid del processo che si vuole attendere che termini (se `pid = -1` si attende la terminazione di un qualunque processo)
- `options`: opzioni (di default `0`)

La primitiva `wait()` attende la terminazione di un qualunque processo figlio. Se il processo figlio termina con una `exit()` il secondo byte meno significativo di status è pari all'argomento passato alla `exit()` e il byte meno significativo è zero.

La primitiva `waitpid()` sospende l'esecuzione del processo chiamante
finchè il processo figlio identificato da `pid` termina. Se un processo figlio è già terminato al momento dell'invocazione di `waitpid`, essa ritorna immediatamente.

Le primitive ritornano il pid del processo figlio terminato.

Per maggiori informazioni vedere [man 2 wait](https://man7.org/linux/man-pages/man2/wait.2.html).

### `sleep`

```c
#include <unistd.h>
unsigned int sleep(unsigned int secs);
```

Dove:

- `secs`: numero di secondi di attesa

La funzione `sleep()` sospende il processo per un periodo di tempo pari a `secs` secondi.

Per maggiori informazioni vedere [man 3 sleep](https://man7.org/linux/man-pages/man3/sleep.3.html).

### `execve`

```c
int execve(char *file_name, *argv[], *envp[]);
```

Dove:

- `file_name`: percorso del file da eseguire
- `argv`: argomenti d'invocazione da eseguire sul file specificato da `file_name`
- `envp`: eventuali variabili d'ambiente per il nuovo ambiente

La primitiva `execve` non produce nessun nuovo processo, ma cambia ambiente d'esecuzione. Non è previsto il ritorno della chiamata a meno che non vi sia un errore (-1). Esistono diverse varianti della medesima funzione:

```c
#include <unistd.h>

int execl( const char *path, const char *arg, ...);
int execlp( const char *file, const char *arg, ...);
int execle( const char *path, const char *arg , ..., char * const envp[]);
int execv( const char *path, char *const argv[]);
int execvp( const char *file, char *const argv[]);
```

Per maggiori informazioni vedere [man 2 execve](https://man7.org/linux/man-pages/man2/execve.2.html).

------

## 3. Segnali

Vi sono spesso eventi importanti da noticare ai processi:

- tasti speciali sul terminale (es. Ctrl^c)
- eccezioni hardware (ad es. accesso invalido alla memoria o divisione per 0)
- eventi inviati con primitiva/comando kill (es. kill -9 1221 )
- condizioni software (es. scadenza di un timer)

L'arrivo di tali eventi asincroni può richiedere un'immediata gestione da parte del processo. Il sistema operativo UNIX consente l'invio e la ricezione di segnali per la sincronizzazione tra processi con due principali modalità principali : una gestione tradizionale e limitata, detta inaffidabile perchè può causare la perdita di segnali inviati, e una più sosticata, detta affidabile. Si suggerisce di consultare la pagina di manuale generale dedicata ai segnali UNIX eseguendo il comando [man 7 signal](https://man7.org/linux/man-pages/man7/signal.7.html).
L'interfaccia semplicata signal è definita nello standard ANSI C e in alcune versioni di UNIX (ma non in Linux) può contribuire ad una gestione inaffidabile dei segnali, ed è quindi raccomandato l'uso della `sigaction`.

### `signal`

```c
#include <signal.h>

void (*signal (int signumber, void (*handler)(int)))
```

Dove:

- `signumber`: segnale ricevuto
- `handler`: funzione destinata alla gestione del segnale ricevuto

La funzione `signal` viene utilizzata per definire il comportamento di un processo alla ricezione di un segnale `signumber`. La funzione `handler` destinata alla gestione del segnale, può essere definita dal programmatore come funzione esterna al corpo del `main`. oppure può essere sostituita con le voci `SIG_IGN` (se si vuole ignorare il segnale ricevuto) oppure `SIG_DFL` (se si vuole la gestione di default del segnale `signumber`).

Per maggiori informazioni vedere [man 7 signal](https://man7.org/linux/man-pages/man7/signal.7.html).

### `kill`

```c
#include <signal.h>

int kill (int process_id, int signumber )
```

Dove:

- `process_id`: pid del processo da segnalare
- `signumber`: numero del segnale da inviare

La funzione `kill()` viene usata per inviare un segnale `signumber` al processo `process_id`. `signumber` è solitamente designato con un valore costante, definito all'interno del `#include <signal.h>`; si consiglia l'uso dei segnali `SIGUSR1` e `SIGUSR2` piuttosto che un valore esplicito arbitrario, al fine di ottenere un cosice portabile. I principali segnali sono racchiusi nella seguente tabella:

| NOME      | NUMERO |                        DESCRIZIONE |
| --------- | ------ | ---------------------------------: |
| `SIGINT`  | 2      |    Interruzione da tastiera (`^C`) |
| `SIGKILL` | 9      |                          Uccisione |
| `SIGALRM` | 14     |               Allarme temporizzato |
| `SIGTSTP` | 18     | Sospensione da tastiera (`Ctrl^z`) |
| `SIGUSR1` | 10     |   Segnale utilizzabile liberamente |
| `SIGUSR2` | 12     |   Segnale utilizzabile liberamente |

Per maggiori informazioni vedere [man 2 kill](https://man7.org/linux/man-pages/man2/kill.2.html).

### `alarm`

```c
#include <unistd.h>

unsigned int alarm(int seconds)
```

Dove:

- `seconds`: numero di secondi prima di lanciare il segnale `SIGALRM`

La funzione `alarm()` attende n `seconds` prima di inviare al processo il segnale `SIGALRM`

Per maggiori informazioni vedere [man 2 alarm](https://man7.org/linux/man-pages/man2/alarm.2.html).

### `pause`

```c
#include <unistd.h>

unsigned int pause(void)
```

La funzione pause sospende il processo chiamante fino alla ricezione di un segnale.

Per maggiori informazioni vedere [man 2 pause](https://man7.org/linux/man-pages/man2/pause.2.html).

### `sleep` e `nanosleep`

```c
#include <unistd.h>

unsigned int sleep(unsigned int n_seconds);
```

Dove:

- `n_seconds`: numero di secondi di sospensione

La funzione `sleep()` mette in sospensione il processo per `n_seconds` per poi riprenderne il funzionamento. Il processo si può risvegliare prima di `n_seconds` secondi se nel frattempo riceve un segnale non ignorato.

Per maggiori informazioni vedere [man 3 sleep](https://man7.org/linux/man-pages/man3/sleep.3.html).

```c
#include <time.h>

int nanosleep(const struct timespec *req, struct timespec *rem);
```

Dove:

- `req`: struttura dati del tipo

  ```c
  struct timespec
  {
   time_t tv_sec; /* seconds */
   long tv_nsec; /* nanoseconds */
  };
  ```

- `rem`: tempo rimanente di attesa

La funzione `nanosleep()` ritarda l'esecuzione di un processo di un tempo specificato da `req` e nel caso in cui, durante l'attesa, arriva un segnale, la funzione interrompe l'attesa, ritorna con `-1` e in `rem` viene inserito il tempo rimante di attesa del processo.

Per maggiori informazioni vedere [man 2 nanosleep](https://man7.org/linux/man-pages/man2/nanosleep.2.html).

### GESTIONE AFFIDABILE DEI SEGNALI

### `sigaction`

```c
#include <signal.h>

int sigaction(int signum, const struct sigaction *act,struct sigaction *oldact);
```

Dove:

- `signum`: numero rappresentativo del segnale

- `act`: struttura dati di tipo `sigaction`

  ```c
  struct sigaction {
   void     (*sa_handler)(int);
      void     (*sa_sigaction)(int, siginfo_t *, void *);
      sigset_t   sa_mask;
      int        sa_flags;
      void     (*sa_restorer)(void);
  };
  ```

- `oldact`: struttura dati di tipo `sigaction`

Per maggiori informazioni vedere [man 2 sigaction](https://man7.org/linux/man-pages/man2/sigaction.2.html).

### `sigprogmask`

```c
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```

Dove:

- `how`: può valere:
  - `SIG_BLOCK`: la nuova signal mask diventa l'OR binario di quella corrente con quella specificata da `set`
  - `SIG_UNBLOCK`:  i segnali indicati da `set` sono rimossi dalla signal mask
  - `SIG_SETMASK`:  la nuova signal mask diventa quella specificata da `set`
- `set`: maschera dei segnali in uso
- `oldset`: se è diverso da `NULL`, la nuova maschera dei segnali viene salvata in `oset`

Un processo può esaminare e/o modificare la propria signal mask che è l'insieme dei segnali che sta attualmente bloccando

La funzione ritorna `0` in caso di successo e `-1` in caso di errore.

Per maggiori informazioni vedere [man 2 sigprogcmask](https://man7.org/linux/man-pages/man2/sigprocmask.2.html).

### Esempio di gestione affidabile dei segnali

La gestione affidabile dei segnali è più complessa di quella inaffidabile, ma fortunatamente segue uno schema alquanto ricorrente.
Come argomenti delle primitive si utilizzano delle maschere di signali (tipo di dato `sigset_t`), ovvero un insieme di bit, ognuno associato ad un tipo di segnale, che possono essere accesi o spenti. Per accendere o spegnere questi bit si utilizzano le funzioni `sigemptyset`, `sigaddset`, `sigllset`ecc. (vederne il manuale). Con

```c
sigemptyset(&zeromask);
```

per esempio si azzera la maschera di segnali `zeromask` (di cui faremo uso più avanti) che deve essere stata precedentemente dichiarata.
Per preparare l'azione del processo all'arrivo di un determinato segnale (in questo caso `SIGUSR1`) si prepara una struttura di tipo `sigaction`. Il primo campo della struttura da sistemare è la maschera `sa_mask` (di tipo `sigset_t`), che va semplicemente vuotato se non vogliamo proteggere il processo dall'arrivo di altri segnali durante l'esecuzione dell'handler (gestore del segnale).

```c
sigemptyset(&action.sa_mask);
```

Poi si indica nel campo `sa_handler` la funzione da eseguire all'arrivo del segnale, ovvero l'handler definito nel codice del programma.

```c
action.sa_handler = catcher;
```

Il campo `sa_flags` va solitamente posto a zero.

```c
action.sa_flags = 0;
```

Infine l'azione va "comunicata" al sistema operativo, indicando a quale segnale associare l'azione. La condizione sul valore di ritorno della `sigaction` serve a vericare che l'operazione sia andata a buon fine.

```c
if (sigaction(SIGUSR1, &action, NULL) == -1)
```

Ci sono casi in cui è necessario in certe fasi del processo bloccare l'arrivo dei segnali, per poi riattivarlo quando il processo diventa pronto alla ricezione. Per specicare quali segnali non devono essere noticati al processo (e rimanere pendenti), si procede preparando una maschera di segnali vuota...

```c
sigemptyset(&set);
```

... si aggiunge a questa maschera il segnale (o i segnali, con più `sigaddset`) su cui si vuole imporre la condizione ...

```c
sigaddset(&set, SIGUSR1);
```

... e si chiede al sistema operativo di aggiungere ("OR" bit a bit) questa maschera all'attuale maschera dei segnali bloccati (opzione `SIG_BLOCK`):

```c
sigprocmask(SIG_BLOCK, &set, NULL);
```

Per ripristinare la ricezione del segnale è sufficiente utilizzare la stessa maschera, stavolta però con l'opzione `SIG_UNBLOCK`.

```c
sigprocmask(SIG_UNBLOCK, &set, NULL);
```

In alternativa, per sbloccare TUTTI i segnali si può utilizzare la `zeromask` creata in precedenza e l'opzione `SIG_SETMASK` (per sovrascrivere l'attuale maschera dei segnali bloccati):

```c
sigprocmask(SIG_SETMASK, &zeromask, NULL);
```

Se il blocco di un segnale è attuato in previsione di una `sigsuspend` (cioè di un punto del codice in cui il processo dovrà sospendersi in attesa dell'arrivo di un segnale) non è necessario sbloccare i segnali ma è sufficiente attuare una `sigsuspend` con maschera vuota:

```c
sigsuspend(&zeromask);
```

La maschera passata come parametro alla `sigsuspend` indica infatti quali segnali devono essere bloccati durante la `sigsuspend` (in questo caso nessuno).

> NOTA: Se sono presenti segnali pendenti, bloccati dalla precedente `sigprocmask`, la `sigsuspend(&zeromask)` li sblocca e ritorna immediatamente dopo l'esecuzione dei gestori.

Riassumendo, si eseguono i seguenti comandi per garantire una gestione affidabile dei segnali:

```c
#include <signal.h>
#include <unistd.h>
void catcher(int signo)
{
static int ntimes = 0;
printf("Processo %d: SIGUSR1 ricevuto #%d volte\n",getpid(),++ntimes);
}
int main()
{
    int pid, ppid;
    struct sigaction sig, osig;
    sigset_t sigmask, oldmask, zeromask;
    
    sig.sa_handler= catcher; //assegnamento della funzione per la gestione del segnale
    sigemptyset(&sig.sa_mask); //preparazione di una maschera vuota
    sig.sa_flags= 0; //impostazione dei flags a 0
    sigemptyset(&zeromask); //preparazione di una maschera vuota
    sigemptyset(&sigmask); //preparazione di una maschera vuota
    sigaddset(&sigmask, SIGUSR1); //aggiunta alla maschera del segnale da notificare
    sigprocmask(SIG_BLOCK, &sigmask, &oldmask); //maschera di blocco dei segnali
    sigaction(SIGUSR1, &sig, &osig); /* il figlio la ereditera' */
    
    if ((pid=fork()) < 0) {
        perror("fork error");
        exit(1);
    }
    else
     if (pid == 0) {
      /* Processo figlio */
      ppid = getppid();
      printf("figlio: mio padre e' %d\n", ppid);
      while(1) {
       sleep(1);
       kill(ppid, SIGUSR1); //invio dei segnale
       /* Sblocca il segnale SIGUSR1 e lo attende */
       sigsuspend(&zeromask); //sospensione con maschera vuota
      }
     }
     else {
      /* Processo padre */
      printf("padre: mio figlio e' %d\n", pid);
      while(1) {
       /* Sblocca il segnale SIGUSR1 e lo attende */
       sigsuspend(&zeromask); //sospensione con maschera vuota
       sleep(1);
       kill(pid, SIGUSR1); //invio del segnale
   }
  }
}
```

------

## 4. Pipe

```c
#include <unistd.h>

int pipe (int file_descriptors[2])
```

Dove:

- `file_descriptors[2]`: file descripor destinati alla lettura (`fd[0]`) e la scrittura (`fd[1]`).

Le pipe sono canali di comunicazioni unidirezionali che costituiscono un primo strumento di comunicazione basato sullo scambio di messaggi. La chiamata alla funzione `pipe()` genere due file descriptor, uno per la lettura e uno per la scrittura, operazioni che possono essere eseguite mediante `read` e `write` rispettivamente.

Il buffer associato ad ogni pipe ha una dimensione finita (`PIPE_BUF`). Se un processo cerca di scrivere (`write()` system call) su una pipe il cui buffer è pieno il processo viene bloccato dal sistema operativo finchè il buffer non viene liberato attraverso una operazione di lettura (`read()` system call). Se il buffer è vuoto e un processo cerca di leggere da una pipe, il processo viene bloccato finchè non avviene una operazione di scrittura.

Un processo padre può comunicare con un processo figlio attraverso una pipe in quanto il processo figlio possiede
una copia dei file descriptor del padre.

Per convenzione le pipe vengono di norma utilizzate come canali di comunicazione unidirezionali. Se due processi richiedono un canale di comunicazione bidirezionale tipicamente si creano due pipe, ovvero una per ciascuna direzione, sebbene siano anche possibili utilizzi delle pipe più
flessibili e meno ortodossi, prevedendo anche scrittori ed eventualmente lettori
multipli sulla stessa pipe.

### Lettura e scrittura su pipe

Le funzioni `read()` e `write()` sono solitamente da svolgere tra processi padre e figlio.

```c
// Lettura su pipe
close(pipe[1]);  //chiusura del file descriptor di scrittura della pipe
read(pipe[0], &buff, sizeof (int)); //lettura dal file descriptor 0 della pipe e inserimento del valore in buf

// Scrittura su pipe
close(pipe[0]);  //chiusura del file descriptor di lettura della pipe
write(pipe[1], &buff, sizeof (int)); //scrittura dal file descriptor 0 della pipe e inserimento del valore in buf
```

Per maggiori informazioni vedere [man 7 pipe](https://man7.org/linux/man-pages/man7/pipe.7.html).

------

## 5. FIFO

```c
#include <sys/types.h>
#include <sys/stat.h>

int mkfifo (const char *pathname, mode_t mode);
```

Dove:

- `pathname`: nome del percorso assoluto
- `mode`: modalità d'accesso al file

La chiamata alla funzione `mkfifo` crea un'entità nel file system accessibile da tutti i processi. Per essere utilizzatala fifo deve essere aperta mediante funzione `open()` dopo che è stata creata.

Per maggiori informazioni vedere [man 3 mkfifo](https://man7.org/linux/man-pages/man3/mkfifo.3.html).

------

## 6. Socket

Una socket fornisce una interfaccia di comunicazione tra processi che possono essere locali oppure trovarsi su nodi distinti di una rete. In generale ogni socket è identificata da un indirizzo, che nel caso specifico di socket create nel dominio di comunicazione AF_INET è una coppia (indirizzo IP del nodo, numero di porta).

Per l'esecuzione del server e del client (che devono essere scritti su due file `.c` differenti) eseguire il comando da terminale:

```bash
./server <numero porta> <argomenti>
./client <argomenti> localhost <numero porta>
```

con `numero porta` un valore molto grande (sopra il 10000 va bene). Se si è creato solo un file `server` si può utilizzare un client di test, invocando da tastiera:

```bash
telnet localhost <numero porta>
```

### `socket`

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol)
```

Dove:

- `domain`: dominio di funzionamento della socket. I domini principali sono:
  - `PD_UNIX` dominio per una comunicazione locale
  - `PF_INET` dominio per una comunicazione su TCP/IP (IPv4)
  - `PF_INET6` dominio per una comunicazione su TCP/IP (IPv6)
- `type`: tipologia di socket da creare. Può essere uno tra:
  - `SOCK_STREAM` per il trasferimento di stream di byte
  - `SOCK_DGRAM` per il trasferimento di datagram
- `protocol`: protocollo di funzionamento (se nel dominio è presente un solo protocollo, allora questo campo è messo a `0`)

Il valore di ritorno della funzione `socket()` è un valore intero che specifica la socket creata.

Per maggiori informazioni vedere [man 7 socket](https://man7.org/linux/man-pages/man7/socket.7.html).

### `bind`

```c
int bind(int sockd,struct sockaddr* my_addr_p,int addrlen)
```

Dove:

- `sockd`: intero che identifica la socket
- `my_addr_p`: contiene il nome (indirizzo) da assegnare alla socket. Al suo interno si trova l'indirizzo IP del nodo e il numero della porta
- `addrlen`: lunghezza dell'indirizzo specificato

Nel dominio `AF_INET` le strutture dati utilizzate da bind e dalle altre primitive sono:

```c
struct sockaddr_in {
    sa_family_t sin_family;  /* address family: AF_INET */
    u_int16_t sin_port;   /* port in network byte order */
    struct in_addr sin_addr;  /* internet address */
};
```

La funzione `bind()` assegna un nome alla socket creata.

Per maggiori informazioni vedere [man 2 bind](https://man7.org/linux/man-pages/man2/bind.2.html).

### `listen`

```c
int listen(int sockfd,int backlog)
```

Dove:

- `sockfd`: intero che identifica la socket
- `backlog`: numero massimo di connessioni pendenti

La funzione `liste()` specifica il numero massimo di connessioni pendenti per la socket, ovvero il numero massimo di richieste che può soddisfare.

Per maggiori informazioni vedere [man 2 listen](https://man7.org/linux/man-pages/man2/listen.2.html).

### `accept`

```c
int accept(int sockfd,struct sockaddr* addr_p,int* len_p)
```

Dove:

- `sokfd`: intero che indica la socket
- `addr_p`: indirizzo in cui verrà memorizzata una struttura che contiene il nome/indirizzo della socket client che effettua la connessione
- `len_p`: dimensione di `addr_p`

La chiamata alla funzione `accept()` accetta la connessione da parte di una socket client. Il valore di ritorno della funzione è il file descriptor da utilizzare nel server

Per maggiori informazioni vedere [man 2 accept](https://man7.org/linux/man-pages/man2/accept.2.html).

### `connect`

```c
int connect(int sockfd,struct sockaddr* addr_p,int len)
```

Dove:

- `sokfd`: intero che indica la socket client
- `addr_p`: indirizzo in cui verrà memorizzata una struttura che contiene il nome/indirizzo della socket server
- `len_p`: dimensione di `addr_p`

Per maggiori informazioni vedere [man 2 connect](https://man7.org/linux/man-pages/man2/connect.2.html).

### `close`

```c
close(sock);
```

Il protocollo TCP attende un tempo tra 1 e 4 minuti (nello stato `TIME_WAIT`) prima di rimuoverla effettivamente, al fine di assicurarsi che eventuali
pacchetti duplicati vaganti siano consegnati al destinatario. In alcuni casi questo ritardo può essere evitabile senza conseguenze, come nel caso del debug di un server che deve essere frequentemente terminato e rieseguito. A questo scopo si utilizza la `setsockopt` per forzare il riuso dell'indirizzo nel `bind` che quindi non fallirà anche in presenza di una socket in fase di chiusura e avente lo stesso indirizzo:

```c
int on = 1;
ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
. . .
bind(. . .);
```

Per maggiori informazioni vedere [man 2 close](https://man7.org/linux/man-pages/man2/close.2.html).

### `select`

```c
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```

Dove:

- `n`: valore del descrittore più alto nei tre insiemi
- `readfds`: insieme dei file descriptor di lettura
- `writefds`: insieme dei file descriptor di scrittura
- `exceptdfs`: insieme dei file descriptor di eccezioni
- `timeout`: limite di tempo superiore per l'attesa

La funzione `select()` permette di attendere una variazione di stato per i file descriptor contenuti nei 3 insiemi `readfds`, `writefds` e `exceptfds`. Il valore di ritorno della funzione è il numero di descrittori che sono variati di stato:

Macro utili per la manipolazioni delle variabili `fd_set`:

```c
FD_ZERO(fd_set *set)     //azzera un fd_set
FD_CLR(int fd, fd_set *set)   //rimuove un fd da un fd_set
FD_SET(int fd, fd_set *set)   //inserisce un fd in un fd_set
FD_ISSET(int fd, fd_set *set)   //predicato che verifica se un certo fd è membro di un fd_set
```

Per maggiori informazioni vedere [man 2 select](https://man7.org/linux/man-pages/man2/select.2.html).

### SERVER CONCORRENTI

Nella risoluzione degli esercizi è obbligatoria la creazione di un server concorrente, ovvero un server in cui le richieste di connessione e le operazioni da eseguire sono gestite da un processo figlio. Qui sotto uno schema con tutto l'occorrente per la realizzazione di un server di tipo concorrente:

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "ftpdefines.h"

char buf[BYTES_NR];
main()
{
    int sock,length;
    struct sockaddr_in server,client;
    char buff[512];
    int s,msgsock,rval,rval2,i;
    struct hostent *hp,*gethostbyname();
    
    /* Crea la socket STREAM */
    sock= socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    { 
     perror("opening stream socket");
     exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr= INADDR_ANY;
    server.sin_port = htons(1520);
    
    if (bind(sock,(struct sockaddr *)&server,sizeof server)<0)
    {
     perror("binding stream socket");
     exit(2);
    }
    length= sizeof server;
    
    if(getsockname(sock,(struct sockaddr *)&server,&length)<0)
    {
     perror("getting socket name");
     exit(3);
    }
    
    printf("Socket port #%d\n",ntohs(server.sin_port));
    
    /* Pronto ad accettare connessioni */
    listen(sock,2);
    
    do {
    /* Attesa di una connessione */
     msgsock= accept(sock,(struct sockaddr *)&client,(int *)&length);
     if(msgsock ==-1)
     { 
            perror("accept"); 
            exit(4);
     }
     else
     {// SERVER CONCORRENTE
      if(fork()==0) {
       printf("Serving connection from %s, port %d\n",
       inet_ntoa(client.sin_addr), ntohs(client.sin_port));
       close(sock);
       ftpserv(msgsock);
       close(msgsock);
       exit(0);
      }
      else
       close(msgsock);
     }
    } while(1);
}

ftpserv(int sock)
{
    int s,rval,nread, fd;
    struct stat fbuf;
    RICHIESTA_MSG rich_mesg;
    RISPOSTA_MSG risp_mesg;
    s = 0;
    /* Ricezione del comando GET */
    if((rval=read(sock,&rich_mesg,sizeof(RICHIESTA_MSG)))<0)
    {
        perror("reading client request");
        exit(-1); 
    }
    
    if((fd= open(rich_mesg.filename,O_RDONLY))<0)
    { 
        fprintf(stderr,"Non riesco ad aprire il file %s (%s)...uscita
   !\n",rich_mesg.filename,strerror(errno));
     risp_mesg.result = -1;
     strcpy(risp_mesg.errmsg,strerror(errno));
    }
    else {
     risp_mesg.result= 0;
     fstat(fd,&fbuf); /* Ottiene la dimensione del file */
     risp_mesg.filesize= fbuf.st_size;
    }
    /* Invio della risposta */
    if((rval = write(sock,&risp_mesg,sizeof(RISPOSTA_MSG)))<0)
     perror("writing on stream socket");
    if(risp_mesg.result != 0) return -1;
     do {
      if((nread = read(fd,buf,sizeof buf))<0)
       perror("reading from file");
      if (nread >0)
       if((rval = write(sock,buf,nread))<0)
        perror("writing on stream socket");
     } while(nread > 0);
}
```

Qui invece un esempio di client concorrente:

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include "ftpdefines.h"

struct protoent *pp;
char buf[BYTES_NR];

main(argc,argv)
int argc;char *argv[];
{
    int i,s,fd,sock,rval,rval2;
    struct sockaddr_in server;
    struct hostent *hp,*gethostbyname();
    char copiafilename[RICHMSG_MAXPATHNAME+16];
    RICHIESTA_MSG rich_mesg;
    RISPOSTA_MSG risp_mesg;
    
    if(argc != 4) {
     fprintf(stderr,"Uso: %s servername porta nomefile\n\n",argv[0]);
     exit(-1);
    }
    /* Crea una socket di tipo STREAM per il dominio TCP/IP */
    sock= socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
     perror("opening stream socket");
     exit(1);
    }
        /* Ottiene l'indirizzo del server */
    server.sin_family= AF_INET;
    hp= gethostbyname(argv[1]);
    
    if(hp==0){
     fprintf(stderr,"%s: unknown host",argv[1]);
     exit(2);
    }
    memcpy( (char *)&server.sin_addr, (char *)hp->h_addr ,hp->h_length);
    /* La porta e' sulla linea di comando */
    server.sin_port= htons(atoi(argv[2]));
    /* Tenta di realizzare la connessione */
    printf("Connecting to the server %s...\n",argv[1]);
    
    if(connect(sock,(struct sockaddr *)&server,sizeof server) <0)
    {
        perror("connecting stream socket");
        exit(3);
    }
    printf("Connected to the server.\n");
    strncpy(rich_mesg.filename,argv[3],RICHMSG_MAXPATHNAME);
    /* Invio comando RICHIESTA (GET) */
    write(sock,&rich_mesg,sizeof(RICHIESTA_MSG));
    
    /* Riceve la RISPOSTA dal server */
    if((rval = read(sock,&risp_mesg,sizeof(RISPOSTA_MSG)))<0)
     perror("reading server answer");
    if(risp_mesg.result !=0) {
     fprintf(stderr,"OOPS il server risponde %d (%s)...uscita!\n",risp_mesg.result,risp_mesg.errmsg);
     close(sock);
     exit(0);
    }
                
    strcpy(copiafilename,"copia.");
    strcat(copiafilename, rich_mesg.filename);
    if((fd= open(copiafilename,O_WRONLY|O_CREAT|O_TRUNC,0644))<0) {
     fprintf(stderr,"Non posso aprire il file copia %s ...uscita!\n",copiafilename);
     close(sock);
        exit(0);
    }
                
    s=0;
                
    do{
     if((rval = read(sock,buf,sizeof buf))<0)
      perror("reading stream message");
     if(rval >0)
     {
      write(fd,buf,rval);
      putchar('.');
      s += rval;
     }
    }while(rval !=0);
                
    printf("\nTrasferimento completato di %s completato - ricevuti %d byte (dimensione sul server %d\n",rich_mesg.filename,s, risp_mesg.filesize);
    close(sock);
    close(fd);
    exit(0);
}
```
