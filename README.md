# Philosophers – Concorrência em C sem rede de segurança

Implementação do clássico **Problema do Jantar dos Filósofos** de Dijkstra (1965) como projeto do currículo Common Core da 42 SP.

O desafio não é entender o problema — é resolvê-lo em C puro, sem garbage collector, sem abstrações de alto nível, gerenciando **threads, mutexes e semáforos POSIX na mão**, com restrição de tempo real: um filósofo que não comer dentro do `time_to_die` morre e a simulação encerra.

Qualquer race condition, qualquer deadlock, qualquer starvation — e o programa quebra de forma não determinística. Esse é o tipo de bug que aparece uma vez a cada dez execuções.

Duas implementações completas:

- `philo` — Threads POSIX + mutexes
- `philo_bonus` — Processos independentes + semáforos POSIX

---

## O problema

N filósofos sentam em uma mesa circular. Entre cada par, há um garfo. Para comer, um filósofo precisa de dois garfos — o da esquerda e o da direita. Depois de comer, dorme. Depois de dormir, pensa. Se não comer dentro de `time_to_die` milissegundos, morre.

O problema clássico de concorrência: múltiplas entidades competindo por recursos compartilhados. Resolver ingenuamente garante deadlock — todos pegam o garfo da esquerda ao mesmo tempo e ficam esperando o da direita para sempre.

---

## Versão obrigatória — `philo` (Threads & Mutexes)

Cada filósofo é uma **thread**. Os garfos são **mutexes**.

### Compilação e uso

```bash
make
./philo <n_filosofos> <time_to_die> <time_to_eat> <time_to_sleep> [max_refeicoes]
```

```bash
./philo 5 800 200 200
```

| Argumento | Descrição |
|---|---|
| `n_filosofos` | Número de filósofos e garfos |
| `time_to_die` | Tempo máximo (ms) sem comer antes da morte |
| `time_to_eat` | Tempo (ms) comendo |
| `time_to_sleep` | Tempo (ms) dormindo |
| `max_refeicoes` | (Opcional) Encerrra quando todos atingirem esse número |

### Decisões de design

#### Por que "ordem de bloqueio fixa" para os garfos?

A solução mais comum para deadlock é hierarquia de recursos: sempre bloquear o mutex de menor endereço de memória primeiro. Isso quebra a dependência circular — nenhum filósofo pode esperar por um recurso que outro já está esperando dele.

Alternativa considerada e descartada: **garçom centralizado** (um mutex global de permissão). Funciona, mas serializa toda a tentativa de comer — filósofos que poderiam comer em paralelo ficam esperando desnecessariamente. A solução por ordem de bloqueio preserva o paralelismo.

#### Staggering para eliminar starvation

Filósofos pares aguardam microsegundos antes de iniciar o ciclo. Sem isso, com N par de filósofos, todos tentam pegar o garfo esquerdo simultaneamente — mesmo com ordem de bloqueio fixa, a contenção cria starvation em runs longos.

#### Monitoramento separado

A thread principal não participa da simulação — ela só monitora. Verifica mortes (`ft_check_death`) e saciedade (`ft_all_sats`) em loop. Ao detectar fim de simulação, seta flags atômicas e encerra. Separar responsabilidades aqui evita que o monitor interfira no timing dos filósofos.

### Estruturas principais

| Struct | Função |
|---|---|
| `t_table` | Estado global: tempos, mutexes, flags, array de filósofos |
| `t_philo` | Estado individual: id, garfos associados, contagem de refeições, timestamp da última refeição |

### Mutexes utilizados

| Mutex | Propósito |
|---|---|
| `forks[i]` | Um por garfo — acesso exclusivo ao recurso |
| `print_mutex` | Serializa prints — evita linhas intercaladas no stdout |
| `data_mutex` | Protege flags e variáveis compartilhadas de leitura/escrita concorrente |

---

## Versão bônus — `philo_bonus` (Processos & Semáforos POSIX)

Aqui a mudança é estrutural: cada filósofo é um **processo filho independente**. Processos não compartilham memória — a sincronização passa a ser via **semáforos POSIX nomeados**.

```bash
make bonus
./philo_bonus <n_filosofos> <time_to_die> <time_to_eat> <time_to_sleep> [max_refeicoes]
```

### Por que a solução muda?

Com threads, mutexes protegem regiões de memória compartilhada. Com processos, essa memória não existe — semáforos se tornam o único mecanismo de sincronização entre eles.

O semáforo `forks_sem` é inicializado com valor N (número de garfos). Cada `sem_wait` representa pegar um garfo; cada `sem_post` devolvê-lo. Não há "garfo da esquerda" ou "garfo da direita" explícito — qualquer dois garfos disponíveis servem.

Isso simplifica o problema de deadlock mas cria outro: com N filósofos e N garfos, todos podem pegar um garfo e travar. Solução: `seats_sem` inicializado com N-1 — no máximo N-1 filósofos tentam comer ao mesmo tempo, garantindo que sempre haja garfos suficientes para pelo menos um terminar.

### Semáforos

| Semáforo | Tipo | Função |
|---|---|---|
| `forks_sem` | Contador (N) | Representa os garfos disponíveis |
| `print_sem` | Binário | Exclusão mútua no stdout |
| `seats_sem` | Contador (N-1) | Limita filósofos tentando comer simultaneamente |

### Monitoramento distribuído

Diferente da versão com threads, não há uma thread central de monitoramento. Cada processo filho cria sua própria thread interna que monitora apenas aquele filósofo:

- Tempo expirou → `exit(1)` (morte)
- Refeições completas → `exit(0)` (saciedade)

O processo pai faz `waitpid()` em loop. Se qualquer filho sair com status de morte → `SIGKILL` em todos os outros → encerra. Se todos saírem com status de saciedade → encerra normalmente.

---

## Saída

Cada evento segue o formato:

```
<timestamp_ms> <id_filosofo> <mensagem>
```

```
200  3  has taken a fork
200  3  is eating
400  3  is sleeping
601  3  is thinking
802  3  has taken a fork
```

Mensagens possíveis: `has taken a fork`, `is eating`, `is sleeping`, `is thinking`, `died`.

A thread/processo de monitoramento garante que nenhuma mensagem seja impressa após `died`.

---

## Erros comuns (e o que causam)

| Sintoma | Causa provável |
|---|---|
| Todos morrem imediatamente | `time_to_die` < `time_to_eat` — matematicamente impossível sobreviver |
| Deadlock no início | Staggering ausente ou mutex não inicializado antes de criar as threads |
| Linhas de output misturadas | `print_mutex` / `print_sem` não protegendo o printf completo |
| Filósofos nunca morrem mesmo sem comer | Monitor não está rodando ou flag de morte não está sendo verificada |
| Comportamento não determinístico | Race condition em leitura/escrita de variável compartilhada sem mutex |

---

## Testes

O repositório inclui scripts Python para validação automatizada:

```bash
python3 philo_bonus/teste.py

bash philo_bonus/teste.sh
```

Cobre: morte dentro do tempo correto, encerramento por saciedade, caso do filósofo único, argumentos inválidos, comportamento com N grande.

---

## Estrutura

```
philo/
├── includes/philo.h
├── src/
│   ├── main.c
│   ├── threads.c
│   ├── monitor.c
│   └── philo_utils.c
└── Makefile

philo_bonus/
├── includes/philo_bonus.h
├── src/
│   ├── main_bonus.c
│   ├── sem_utils.c
│   └── monitor_bonus.c
└── Makefile
```

---

## Contexto

Projeto **Philosophers** — 42 SP, Common Core.  
Problema original: Dijkstra, 1965. *"Hierarchical ordering of resources"* como solução para deadlock mutual.

Os conceitos aqui — race conditions, locks, monitoramento assíncrono, sincronização entre processos — são os mesmos que aparecem em qualquer sistema backend com concorrência real. A diferença é que em Python ou Go você tem abstrações. Em C, você implementa as abstrações.
