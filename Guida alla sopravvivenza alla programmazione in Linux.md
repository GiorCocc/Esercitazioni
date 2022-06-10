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

L'opzione `-o` seguita da `<nome del file eseguibile>` comunica a `gcc` che deve creare un le eseguibile chiamato `<nome del file eseguibile>`. Ad esempio, se dalla shell si digita il comando `gcc -o hello hello.c`, il file sorgente `hello.c` verrà compilato e il file eseguibile sarà chiamato `hello`. Se non si scrive `-o filename`, il compilatore `gcc` crea un eseguibile chiamato `a.out`.

L'esecuzione di un programma da shell prevede la seguente sintassi: 

```bash
./<nome eseguibile> [lista argomenti invocazione]
```

In generale, la sintassi del comando `gcc` è: `gcc <opzioni> <filename>`
Oltre a `-o`, le opzioni più comuni sono:

- `-c` per creare il file oggetto anziché l'eseguibile: `gcc -c hello.c` genera il file oggetto `hello.o`
- `-Wall` genera tutti i messaggi di warning che `gcc` può fornire
- `-pedantic` mostra tutti gli errori e i warning richiesti dallo standard ANSI C
- `-O -O1 -O2 -O3` servono per definire il livello di ottimizzazione (dal più basso al più alto)
- `-O0` per non avere nessuna ottimizzazione
- `-g` per un successivo debugging

Per creare l'eseguibile a partire dal file oggetto: `gcc hello.o -o hello`
Quest'ultima operazione prende il nome di **linking**, che in generale consiste nella risoluzione dei simboli tra programmi e l'inclusione di eventuali librerie.

### Strace

Il programma `strace` consente di visualizzare le system call (con i relativi argomenti) invocate da un processo in esecuzione. È uno strumento di debug molto istruttivo che permette di ottenere, in tempo reale su schermo o in un file di testo, numerose informazioni sull'esecuzione di un programma.

Può essere invocato in due modalità principali:

1. `strace nomeprogramma [argomenti]` (esegue `nomeprogramma`)
2. `strace -p <pid processo in esecuzione>` (visualizza le SVC invocate dal processo con il PID specificato, già in esecuzione ).

Per memorizzare su le la traccia delle system call invocate, utilizzare l'opzione `-o` : ad es. 

```bash
strace -o traccialog.txt nomeprogramma
```


Per ottenere la traccia delle system call invocate da un processo e dai suoi processi figli, utilizzare l'opzione `-f` : ad es. 

```bash
strace -f nomeprogramma
```

Si tratta di una funzionalità molto importante, difficilmente ottenibile in un normale debugger che può soltanto seguire il processo padre oppure il processo glio (cfr. il comando ` set follow-fork-mode` di gdb).

Per le altre opzioni consultare il manuale di `strace`.

---

## 1. Chiamate di sistema per I/O

Quando un programma viene messo in esecuzione, possiede almeno tre file descriptor:

```c
0 Standard input
1 Standard output
2 Standard error
```

Vediamo le principali chiamate di sistema che fanno uso dei descrittori di file. La maggior parte di queste chiamate restituisce `-1` in caso di errore e assegna alla variabile `errno `il codice di errore. I codici di errore sono documentati nelle pagine di manuale delle singole chiamate di sistema e in quella di `errno`. La funzione `perror()` può essere utilizzata per visualizzare un messaggio di errore basato sul relativo codice.

### `write`

```c
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);
```

Dove: 

- `fd`: file descriptor su cui effettuare la scrittura
- `buf`: buffer che contiene il messaggio da andare a scrivere nel file descriptor indicato
- `count`: numero di byte da andare a scrivere nel file descriptor

Con la funzione `write()`, i primi `count` byte di `buf ` vengono scritti
nel file che è stato associato al file descriptor `fd`. La chiamata restituisce il numero dei byte scritti oppure `-1` se si è verificato un errore.

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

Le chiamate di sistema `dup()` e `dup2()` creano una copia del descrittore di file `oldfd`. Il nuovo descrittore è, nel caso della `dup()`, il descrittore non utilizzato con numero d'ordine più basso; nel caso della `dup2()`, è il `newfd` specificato dall'utente.

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

---

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

### `wait ` e `waitpid`

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

La primitiva `wait()` attende la terminazione di un qualunque processo figlio. Se il processo figlio termina con una `exit()` il secondo byte meno significativo di status è pari all'argomento passato alla `exit()` e il byte meno significativo è zero.

La primitiva `waitpid()` sospende l'esecuzione del processo chiamante
finchè il processo figlio identificato da `pid` termina. Se un processo figlio è già terminato al momento dell'invocazione di `waitpid`, essa ritorna immediatamente. 

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

---

## 3. Segnali

Vi sono spesso eventi importanti da noticare ai processi:

- tasti speciali sul terminale (es. Ctrl^c)
- eccezioni hardware (ad es. accesso invalido alla memoria o divisione per 0)
- eventi inviati con primitiva/comando kill (es. kill -9 1221 )
- condizioni software (es. scadenza di un timer)

L'arrivo di tali eventi asincroni può richiedere un'immediata gestione da parte del processo. Il sistema operativo UNIX consente l'invio e la ricezione di segnali per la sincronizzazione tra processi con due principali modalità principali : una gestione tradizionale e limitata, detta inaffidabile perchè può causare la perdita di segnali inviati, e una più sosticata, detta affidabile. Si suggerisce di consultare la pagina di manuale generale dedicata ai segnali UNIX eseguendo il comando [man 7 signal](https://man7.org/linux/man-pages/man7/signal.7.html).
L'interfaccia semplicata signal è definita nello standard ANSI C e in alcune versioni di UNIX (ma non in Linux) può contribuire ad una gestione inaffidabile dei segnali, ed è quindi raccomandato l'uso della `sigaction`.

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

La funzione pause sospende il processo chiamante fino alla ricezione di un segnale.

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
Come argomenti delle primitive si utilizzano delle maschere di signali (tipo di dato `sigset_t`), ovvero un insieme di bit, ognuno associato ad un tipo di segnale, che possono essere accesi o spenti. Per accendere o spegnere questi bit si utilizzano le funzioni `sigemptyset`, `sigaddset`, `sigllset`ecc. (vederne il manuale). Con

```c
sigemptyset(&zeromask);
```

per esempio si azzera la maschera di segnali `zeromask` (di cui faremo uso più avanti) che deve essere stata precedentemente dichiarata.
Per preparare l'azione del processo all'arrivo di un determinato segnale (in questo caso `SIGUSR1`) si prepara una struttura di tipo `sigaction`. Il primo campo della struttura da sistemare è la maschera `sa_mask` (di tipo `sigset_t`), che va semplicemente vuotato se non vogliamo proteggere il processo dall'arrivo di altri segnali durante l'esecuzione dell'handler (gestore del segnale).

```c
sigemptyset(&action.sa_mask);
```

Poi si indica nel campo `sa_handler` la funzione da eseguire all'arrivo del segnale, ovvero l'handler definito nel codice del programma.

```c
action.sa_handler = catcher;
```

Il campo `sa_flags` va solitamente posto a zero. 

```c
action.sa_flags = 0;
```

Infine l'azione va "comunicata" al sistema operativo, indicando a quale segnale associare l'azione. La condizione sul valore di ritorno della `sigaction` serve a vericare che l'operazione sia andata a buon fine.

```c
if (sigaction(SIGUSR1, &action, NULL) == -1)
```

Ci sono casi in cui è necessario in certe fasi del processo bloccare l'arrivo dei segnali, per poi riattivarlo quando il processo diventa pronto alla ricezione. Per specicare quali segnali non devono essere noticati al processo (e rimanere pendenti), si procede preparando una maschera di segnali vuota...

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

Riassunmento, si eseguono i seguenti comandi per garantire una gestione affidabile dei segnali:

```c

```

---

## 4. Pipe

```c
#include <unistd.h>

int pipe (int file_descriptors[2])
```

Dove:

- `file_descriptors[2]`: file descripor destinati alla lettura (`fd[0]`) e la scrittura (`fd[1]`).

Le pipe sono canali di comunicazioni unidirezionali che costituiscono un primo strumento di comunicazione basato sullo scambio di messaggi. La chiamata alla funzione `pipe()` genere due file descriptor, uno per la lettura e uno per la scrittura, operazioni che possono essere eseguite mediante `read` e `write` rispettivamente.

Il buffer associato ad ogni pipe ha una dimensione finita (`PIPE_BUF`). Se un processo cerca di scrivere (`write()` system call) su una pipe il cui buffer è pieno il processo viene bloccato dal sistema operativo finchè il buffer non viene liberato attraverso una operazione di lettura (`read()` system call). Se il buffer è vuoto e un processo cerca di leggere da una pipe, il processo viene bloccato finchè non avviene una operazione di scrittura. 

Un processo padre può comunicare con un processo figlio attraverso una pipe in quanto il processo figlio possiede
una copia dei file descriptor del padre.

Per convenzione le pipe vengono di norma utilizzate come canali di comunicazione unidirezionali. Se due processi richiedono un canale di comunicazione bidirezionale tipicamente si creano due pipe, ovvero una per ciascuna direzione, sebbene siano anche possibili utilizzi delle pipe più 
flessibili e meno ortodossi, prevedendo anche scrittori ed eventualmente lettori
multipli sulla stessa pipe.

Per maggiori informazioni vedere [man 7 pipe](https://man7.org/linux/man-pages/man7/pipe.7.html).

---

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

---

## 6. Socket

### `socket`

```
#include <sys/types.h>
#include <sys/socket.h>
int socket(int domain, int type, int protocol)
```

Per maggiori informazioni vedere []().

### `bind`

```
int bind(int sockd,struct sockaddr* my_addr_p,int addrlen)
```

Per maggiori informazioni vedere []().

### `listen`

```
int listen(int sockfd,int backlog)
```

Per maggiori informazioni vedere []().

### `accept`

```
int accept(int sockfd,struct sockaddr* addr_p,int* len_p)
```

Per maggiori informazioni vedere []().

### `connect`

```
int connect(int sockfd,struct sockaddr* addr_p,int len)
```

Per maggiori informazioni vedere []().

### `close`

```
close(sock);
```

Per maggiori informazioni vedere []().

### `select`

```
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
int select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```

Per maggiori informazioni vedere []().

### SERVER CONCORRENTI

