# 🧠 Philosophers – Projeto da 42

O projeto **Philosophers** é uma implementação do clássico problema de concorrência **“O Jantar dos Filósofos”**, cujo objetivo é gerenciar múltiplas entidades (filósofos) competindo por recursos compartilhados (garfos), evitando **deadlocks** e **starvation**.

Existem **duas versões** do projeto:

- `philo` → Threads e Mutexes (versão obrigatória)  
- `philo_bonus` → Processos e Semáforos POSIX (versão bônus)

---

## 📘 1. philo – Versão Obrigatória (Threads & Mutexes)

### 🧩 Descrição

Cada filósofo é representado por uma **thread**.  
Eles alternam entre **pensar**, **comer** e **dormir**, compartilhando os **garfos** (mutexes).  
A simulação termina quando:
- Um filósofo morre (não comeu dentro do `time_to_die`), ou  
- Todos comeram o número de vezes definido em `[max_eats]` (se fornecido).

---

### ⚙️ Compilação

```bash
make
```

### ▶️ Execução

```bash
./philo <number_of_philosophers> <time_to_die> <time_to_eat> <time_to_sleep> [max_eats]
```

#### Parâmetros

| Argumento | Descrição |
|------------|------------|
| `number_of_philosophers` | Número de filósofos e garfos. |
| `time_to_die` | Tempo máximo (ms) sem comer antes da morte. |
| `time_to_eat` | Tempo (ms) gasto comendo. |
| `time_to_sleep` | Tempo (ms) gasto dormindo. |
| `[max_eats]` *(opcional)* | Número de vezes que cada filósofo deve comer antes do término da simulação. |

#### Exemplo

```bash
./philo 5 800 200 200
```

---

### 🧠 Lógica da Implementação

#### Estruturas Principais

- **`t_table`** – Contém os dados globais da simulação (tempos, mutexes, flags e array de filósofos).  
- **`t_philo`** – Estrutura de cada filósofo (id, garfos, contagem de refeições, último horário de refeição).

#### Sincronização

- **Garfos** → Um array de `pthread_mutex_t`.  
  Cada filósofo bloqueia dois mutexes para comer.  
- **Mutex de impressão (`print_mutex`)** → Evita sobreposição de logs.  
- **Mutex de dados (`data_mutex`)** → Protege flags e variáveis compartilhadas.

#### Estratégias Anti-Deadlock

1. **Desfase Inicial (Staggering)**  
   Filósofos pares aguardam alguns microssegundos antes de iniciar o ciclo.  
2. **Ordem de Bloqueio Fixa**  
   Sempre bloqueia o garfo de endereço menor primeiro — evita dependência circular.

#### Monitoramento

A thread principal supervisiona a simulação:
- Verifica se algum filósofo morreu (`ft_check_death`).
- Verifica se todos comeram o suficiente (`ft_all_sats`).
- Ao detectar término, define flags e encerra as threads.

---

## 🧩 2. philo_bonus – Versão Bônus (Processos & Semáforos)

### 🧩 Descrição

Nesta versão, cada filósofo é um **processo filho independente**, e a sincronização é feita via **semáforos POSIX**.

---

### ⚙️ Compilação

```bash
make bonus
```

### ▶️ Execução

```bash
./philo_bonus <number_of_philosophers> <time_to_die> <time_to_eat> <time_to_sleep> [max_eats]
```

Parâmetros e exemplo são idênticos à versão obrigatória.

---

### 🧠 Lógica da Implementação

#### Estruturas

- **`t_table`** → Armazena informações globais e PIDs dos filósofos.  
- **`t_philo_state`** → Estado individual de cada filósofo.

#### Semáforos Utilizados

| Semáforo | Tipo | Função |
|-----------|------|--------|
| `forks_sem` | Contador | Representa a pilha de garfos disponíveis. |
| `print_sem` | Binário | Garante exclusão mútua na saída (`printf`). |
| `seats_sem` | Contador | Controla quantos filósofos podem tentar comer simultaneamente (N-1). |

#### Monitoramento

- **Thread interna (por processo):**  
  Cada filósofo cria uma thread que monitora sua própria morte.  
  Se o tempo de vida expirar → `exit(1)`;  
  se terminar refeições → `exit(0)`.

- **Processo pai:**  
  Aguarda `waitpid()` dos filhos.  
  Se algum morrer, envia `SIGKILL` para todos.  
  Se todos completarem as refeições, encerra normalmente.

---

## 🧩 Estrutura do Projeto

```
philo/
 ├── includes/
 │    └── philo.h
 ├── src/
 │    ├── main.c
 │    ├── philo_utils.c
 │    ├── threads.c
 │    ├── monitor.c
 │    └── ...
 ├── Makefile
 └── README.md

philo_bonus/
 ├── includes/
 │    └── philo_bonus.h
 ├── src/
 │    ├── main_bonus.c
 │    ├── sem_utils.c
 │    ├── monitor_bonus.c
 │    └── ...
 ├── Makefile
 └── README.md
```

---

## 🔍 Validação dos Argumentos

Antes de iniciar a simulação:
- Verifica se todos os argumentos são **numéricos e positivos**.  
- `number_of_philosophers` deve ser ≥ 1.  
- `time_to_die`, `time_to_eat` e `time_to_sleep` > 0.  
- Retorna erro formatado caso contrário.

---

## 🖨️ Mensagens de Saída

Cada linha segue o formato:

```
<timestamp_in_ms> <philosopher_id> <mensagem>
```

Exemplo:
```
200 3 is eating
400 3 is sleeping
601 3 died
```

Mensagens possíveis:
- `is eating`
- `is sleeping`
- `is thinking`
- `has taken a fork`
- `died`

---

## ⚠️ Erros Comuns

| Erro | Causa |
|------|--------|
| Todos os filósofos morrem imediatamente | `time_to_die` menor que `time_to_eat` |
| Deadlock no início | Falha no staggering ou mutex não inicializado |
| Mensagens misturadas | Falta de proteção no `print_mutex` / `print_sem` |
| Filósofos não morrem nunca | Monitor não está sendo executado |

---

## 📜 Créditos

Projeto realizado na **42 School** como parte do currículo de *Common Core*.  
Implementado por **[seu_login_da_42]**.  
Inspirado no problema original de Dijkstra (1965).
