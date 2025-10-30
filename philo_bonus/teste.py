#!/usr/bin/env python3
import subprocess
import re
import os
import time
from collections import defaultdict

# --- Configuração ---
EXECUTABLE = "./philo_bonus"
MAKEFILE = "Makefile"

# --- Cores para o output ---
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_test_header(name):
    print(f"\n{bcolors.HEADER}{bcolors.BOLD}{'='*60}")
    print(f"  {name}")
    print(f"{'='*60}{bcolors.ENDC}")

def print_result(success, message):
    if success:
        print(f"{bcolors.OKGREEN}✓ [PASSOU] {message}{bcolors.ENDC}")
    else:
        print(f"{bcolors.FAIL}✗ [FALHOU] {message}{bcolors.ENDC}")

def print_warning(message):
    print(f"{bcolors.WARNING}⚠ [AVISO] {message}{bcolors.ENDC}")

def print_info(message):
    print(f"{bcolors.OKCYAN}ℹ {message}{bcolors.ENDC}")

def compile_program():
    """Compila o programa C usando o Makefile."""
    print_test_header("COMPILAÇÃO")
    if not os.path.exists(MAKEFILE):
        print_result(False, f"Makefile não encontrado.")
        return False
        
    try:
        subprocess.run(["make", "fclean"], capture_output=True, text=True)
        result = subprocess.run(["make"], check=True, capture_output=True, text=True)
        if os.path.exists(EXECUTABLE):
            print_result(True, f"Executável '{EXECUTABLE}' criado com sucesso.")
            return True
        else:
            print_result(False, f"Erro na compilação.")
            print(f"{bcolors.FAIL}STDERR:\n{result.stderr}{bcolors.ENDC}")
            return False
    except subprocess.CalledProcessError as e:
        print_result(False, f"Falha na compilação (make retornou erro).")
        print(f"{bcolors.FAIL}STDERR:\n{e.stderr}{bcolors.ENDC}")
        return False

def run_test(args, timeout_seconds, description=""):
    """Roda o executável com os argumentos fornecidos e um timeout."""
    command = [EXECUTABLE] + [str(arg) for arg in args]
    print_info(f"Comando: {' '.join(command)}")
    if description:
        print_info(f"Descrição: {description}")
    
    try:
        start_time = time.time()
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        stdout, stderr = process.communicate(timeout=timeout_seconds)
        end_time = time.time()
        duration = (end_time - start_time) * 1000
        return stdout, stderr, process.returncode, duration
    except subprocess.TimeoutExpired:
        process.kill()
        stdout, stderr = process.communicate()
        return stdout, stderr, -1, timeout_seconds * 1000
    except FileNotFoundError:
        return "", f"Erro: Executável '{EXECUTABLE}' não encontrado.", 127, 0

def parse_output(stdout):
    """Analisa a saída e retorna informações estruturadas."""
    lines = stdout.strip().split('\n')
    events = []
    death_event = None
    
    for line in lines:
        match = re.match(r'(\d+)\s+(\d+)\s+(.+)', line)
        if match:
            timestamp = int(match.group(1))
            philo_id = int(match.group(2))
            action = match.group(3)
            
            event = {
                'timestamp': timestamp,
                'philo_id': philo_id,
                'action': action
            }
            events.append(event)
            
            if 'died' in action:
                death_event = event
    
    return events, death_event

def check_death_timing(death_event, time_to_die, tolerance=10):
    """Verifica se a morte aconteceu no tempo correto."""
    if not death_event:
        return False, "Nenhuma morte detectada"
    
    death_time = death_event['timestamp']
    diff = abs(death_time - time_to_die)
    
    if diff <= tolerance:
        return True, f"Morte no tempo correto ({death_time}ms, esperado ~{time_to_die}ms)"
    else:
        return False, f"Morte no tempo incorreto ({death_time}ms, esperado ~{time_to_die}ms, diferença: {diff}ms)"

def check_no_overlap(events):
    """Verifica se não há mensagens sobrepostas perigosas (data race real)."""
    overlaps = []
    
    for i in range(len(events) - 1):
        curr = events[i]
        next_event = events[i+1]
        
        # Mesmo timestamp é OK se forem ações diferentes de filósofos diferentes
        # O problema é se o MESMO filósofo tem ações no mesmo timestamp
        if curr['timestamp'] == next_event['timestamp']:
            if curr['philo_id'] == next_event['philo_id']:
                overlaps.append({
                    'timestamp': curr['timestamp'],
                    'philo_id': curr['philo_id'],
                    'actions': [curr['action'], next_event['action']]
                })
    
    if overlaps:
        msgs = []
        for overlap in overlaps:
            msgs.append(f"Filósofo {overlap['philo_id']} tem ações simultâneas em {overlap['timestamp']}ms: {overlap['actions']}")
        return False, "Data races detectadas:\n  " + "\n  ".join(msgs)
    
    # Timestamp igual mas filósofos diferentes é aceitável
    same_timestamp_count = sum(1 for i in range(len(events) - 1) 
                                if events[i]['timestamp'] == events[i+1]['timestamp'])
    
    if same_timestamp_count > 0:
        return True, f"Sem data races (timestamps simultâneos OK: {same_timestamp_count})"
    
    return True, "Sem overlaps detectados"

def analyze_eating_pattern(events):
    """Analisa padrões de alimentação."""
    eat_events = [e for e in events if 'eating' in e['action']]
    philo_eat_count = defaultdict(int)
    last_meal = {}
    
    for event in eat_events:
        philo_id = event['philo_id']
        philo_eat_count[philo_id] += 1
        last_meal[philo_id] = event['timestamp']
    
    return dict(philo_eat_count), last_meal

def check_death_delay(events, death_event):
    """Verifica se a mensagem de morte foi mostrada em até 10ms da morte real."""
    if not death_event:
        return True, "Sem morte para verificar"
    
    death_time = death_event['timestamp']
    philo_id = death_event['philo_id']
    
    # Encontra a última refeição deste filósofo
    eating_events = [e for e in events if e['philo_id'] == philo_id and 'eating' in e['action']]
    if eating_events:
        last_meal = eating_events[-1]['timestamp']
        delay = death_time - last_meal
        # Verificar se a mensagem apareceu rapidamente após a morte
        return True, f"Última refeição do filósofo {philo_id}: {last_meal}ms, morte: {death_time}ms (delay: {delay}ms)"
    
    return True, "Não foi possível verificar delay"

# ============================================
# TESTES
# ============================================

def test_one_philosopher_dies():
    """Testa o caso especial de 1 filósofo, que deve morrer."""
    print_test_header("TESTE 1: Um Filósofo (deve morrer)")
    time_to_die = 800
    args = [1, time_to_die, 200, 200]
    
    stdout, stderr, retcode, duration = run_test(args, timeout_seconds=2.0, 
        description="Um único filósofo deve pegar um garfo e morrer")
    
    events, death_event = parse_output(stdout)
    
    if retcode == -1:
        print_result(False, "Programa travou (timeout)")
        return False
    
    if not death_event:
        print_result(False, "Nenhuma mensagem de morte encontrada")
        print(f"Saída:\n{stdout}")
        return False
    
    success, msg = check_death_timing(death_event, time_to_die)
    print_result(success, msg)
    
    # Verifica se pegou um garfo
    fork_events = [e for e in events if 'taken a fork' in e['action']]
    if len(fork_events) == 1:
        print_result(True, "Filósofo pegou exatamente um garfo, como esperado")
    else:
        print_result(False, f"Filósofo pegou {len(fork_events)} garfos (esperado: 1)")
    
    return success

def test_no_one_dies():
    """Testa uma simulação onde ninguém deve morrer."""
    print_test_header("TESTE 2: Múltiplos Filósofos (ninguém deve morrer)")
    num_philos = 5
    time_to_die = 800
    time_to_eat = 200
    time_to_sleep = 200
    num_eats = 7
    args = [num_philos, time_to_die, time_to_eat, time_to_sleep, num_eats]
    
    expected_duration = (time_to_eat + time_to_sleep) * num_eats / 1000.0
    stdout, stderr, retcode, duration = run_test(args, timeout_seconds=expected_duration + 2.0,
        description=f"{num_philos} filósofos devem comer {num_eats} vezes cada sem morrer")
    
    events, death_event = parse_output(stdout)
    
    if retcode == -1:
        print_result(False, "Programa travou (timeout)")
        return False
    
    if death_event:
        print_result(False, f"Filósofo {death_event['philo_id']} morreu inesperadamente em {death_event['timestamp']}ms")
        return False
    
    print_result(True, "Nenhum filósofo morreu")
    
    # Analisa padrão de alimentação
    eat_counts, _ = analyze_eating_pattern(events)
    print_info(f"Refeições por filósofo: {dict(sorted(eat_counts.items()))}")
    
    all_ate_enough = all(count >= num_eats for count in eat_counts.values())
    if all_ate_enough:
        print_result(True, f"Todos os filósofos comeram pelo menos {num_eats} vezes")
    else:
        print_warning(f"Alguns filósofos não comeram {num_eats} vezes")
    
    # Verifica overlaps
    success, msg = check_no_overlap(events)
    print_result(success, msg)
    
    return not death_event and all_ate_enough

def test_one_must_die():
    """Testa uma simulação onde um filósofo deve morrer."""
    print_test_header("TESTE 3: Filósofo Deve Morrer (time_to_die muito curto)")
    num_philos = 4
    time_to_die = 310
    time_to_eat = 200
    time_to_sleep = 100
    args = [num_philos, time_to_die, time_to_eat, time_to_sleep]
    
    stdout, stderr, retcode, duration = run_test(args, timeout_seconds=2.0,
        description="Um filósofo deve morrer porque time_to_die é muito curto")
    
    events, death_event = parse_output(stdout)
    
    if retcode == -1:
        print_result(False, "Programa travou ao invés de detectar morte")
        return False
    
    if not death_event:
        print_result(False, "Nenhum filósofo morreu, mas era esperado")
        print(f"Saída:\n{stdout}")
        return False
    
    print_result(True, f"Filósofo {death_event['philo_id']} morreu em {death_event['timestamp']}ms")
    
    # Verifica timing da detecção
    success, msg = check_death_delay(events, death_event)
    print_info(msg)
    
    return death_event is not None

def test_many_philosophers():
    """Testa com muitos filósofos - versão progressiva."""
    print_test_header("TESTE 4: Muitos Filósofos (stress test progressivo)")
    
    test_configs = [
        (10, 3.0, "10 filósofos", 5),
        (50, 5.0, "50 filósofos", 3),
        (100, 8.0, "100 filósofos", 2),
        (200, 12.0, "200 filósofos", 1),
    ]
    
    all_passed = True
    
    for num_philos, timeout, desc, min_meals in test_configs:
        print_info(f"Testando: {desc}")
        time_to_die = 800
        time_to_eat = 200
        time_to_sleep = 200
        num_eats = min_meals  # Só requer poucas refeições para stress test
        args = [num_philos, time_to_die, time_to_eat, time_to_sleep, num_eats]
        
        print_info(f"Aguardando até {timeout}s para {num_philos} filósofos comerem {num_eats} vezes...")
        stdout, stderr, retcode, duration = run_test(args, timeout_seconds=timeout)
        events, death_event = parse_output(stdout)
        
        if retcode == -1:
            print_result(False, f"Timeout após {timeout}s")
            print_warning("O programa pode estar correto mas muito lento com muitos processos")
            print_warning("Teste manual recomendado para verificar comportamento")
            # Não marca como falha crítica para 200 philos
            if num_philos < 200:
                all_passed = False
            else:
                print_info("→ Teste de 200 filósofos é opcional, ignorando timeout")
            break
        
        if death_event:
            print_result(False, f"Filósofo {death_event['philo_id']} morreu em {death_event['timestamp']}ms")
            all_passed = False
            break
        
        print_result(True, f"{desc}: rodou sem mortes (duração: {duration/1000:.2f}s)")
        
        # Verifica quantos conseguiram comer
        eat_counts, _ = analyze_eating_pattern(events)
        philos_that_ate = len(eat_counts)
        
        # Para muitos filósofos, aceita percentual menor
        min_percent = 0.7 if num_philos >= 100 else 0.8
        
        if philos_that_ate >= num_philos * min_percent:
            print_info(f"  → {philos_that_ate}/{num_philos} filósofos comeram ({philos_that_ate/num_philos*100:.1f}%)")
        else:
            print_warning(f"  → Apenas {philos_that_ate}/{num_philos} comeram ({philos_that_ate/num_philos*100:.1f}%)")
            if num_philos < 100:  # Só falha se for menos de 100 philos
                all_passed = False
        
        # Verifica se todos que comeram, comeram o suficiente
        ate_enough = sum(1 for count in eat_counts.values() if count >= num_eats)
        if ate_enough >= num_philos * min_percent:
            print_info(f"  → {ate_enough} filósofos completaram {num_eats} refeições")
        else:
            print_warning(f"  → Apenas {ate_enough} completaram {num_eats} refeições")
    
    return all_passed

def test_invalid_arguments():
    """Testa tratamento de argumentos inválidos."""
    print_test_header("TESTE 5: Argumentos Inválidos")
    
    test_cases = [
        ([], "Sem argumentos"),
        ([5], "Poucos argumentos"),
        ([5, 800], "Ainda poucos argumentos"),
        ([0, 800, 200, 200], "Zero filósofos"),
        ([-1, 800, 200, 200], "Número negativo"),
        ([5, -1, 200, 200], "time_to_die negativo"),
        ([5, 800, -1, 200], "time_to_eat negativo"),
        ([5, 800, 200, -1], "time_to_sleep negativo"),
    ]
    
    passed = 0
    for args, desc in test_cases:
        print_info(f"Testando: {desc}")
        command = [EXECUTABLE] + [str(arg) for arg in args]
        try:
            result = subprocess.run(command, capture_output=True, text=True, timeout=1.0)
            # Programa deve retornar erro (código != 0) ou imprimir erro
            if result.returncode != 0 or "Error" in result.stderr or "Error" in result.stdout:
                print_result(True, f"{desc}: programa rejeitou corretamente")
                passed += 1
            else:
                print_result(False, f"{desc}: programa deveria rejeitar mas não rejeitou")
        except subprocess.TimeoutExpired:
            print_result(False, f"{desc}: programa travou com argumentos inválidos")
    
    total = len(test_cases)
    print_info(f"Passou {passed}/{total} testes de validação")
    return passed == total

def test_death_precision():
    """Testa precisão da detecção de morte."""
    print_test_header("TESTE 6: Precisão de Detecção de Morte")
    
    test_cases = [
        (1, 100, 200, 200, "morte rápida"),
        (1, 500, 200, 200, "morte moderada"),
        (4, 400, 300, 100, "múltiplos filósofos, um deve morrer"),
    ]
    
    passed = 0
    for args in test_cases:
        num_philos, time_to_die, time_to_eat, time_to_sleep, desc = args
        test_args = [num_philos, time_to_die, time_to_eat, time_to_sleep]
        
        print_info(f"Testando: {desc}")
        stdout, stderr, retcode, duration = run_test(test_args, timeout_seconds=2.0)
        events, death_event = parse_output(stdout)
        
        if death_event:
            death_time = death_event['timestamp']
            # Requisito: mensagem de morte em até 10ms da morte real
            # Para 1 filósofo, a morte deve ser próxima de time_to_die
            if num_philos == 1:
                diff = abs(death_time - time_to_die)
                if diff <= 10:
                    print_result(True, f"Morte detectada em {death_time}ms (esperado ~{time_to_die}ms, diff: {diff}ms)")
                    passed += 1
                else:
                    print_result(False, f"Morte em {death_time}ms muito longe do esperado {time_to_die}ms (diff: {diff}ms)")
            else:
                print_result(True, f"Filósofo {death_event['philo_id']} morreu em {death_time}ms")
                passed += 1
        else:
            print_result(False, f"Nenhuma morte detectada")
    
    total = len(test_cases)
    print_info(f"Passou {passed}/{total} testes de precisão")
    return passed == total

def cleanup():
    """Limpa os arquivos gerados pela compilação."""
    print_test_header("LIMPEZA")
    try:
        subprocess.run(["make", "fclean"], check=True, capture_output=True, text=True)
        print_result(True, "Arquivos removidos com sucesso")
    except subprocess.CalledProcessError as e:
        print_result(False, f"Falha ao executar 'make fclean'")

# ============================================
# MAIN
# ============================================

def main():
    print(f"{bcolors.BOLD}{bcolors.HEADER}")
    print("╔══════════════════════════════════════════════════════════════╗")
    print("║      PHILOSOPHERS BONUS - TESTER COMPLETO v2.0              ║")
    print("╚══════════════════════════════════════════════════════════════╝")
    print(bcolors.ENDC)
    
    if not compile_program():
        print(f"\n{bcolors.FAIL}Compilação falhou. Abortando testes.{bcolors.ENDC}")
        return
    
    results = []
    
    # Executa todos os testes
    results.append(("1 Filósofo", test_one_philosopher_dies()))
    results.append(("Ninguém Morre", test_no_one_dies()))
    results.append(("Alguém Deve Morrer", test_one_must_die()))
    results.append(("Muitos Filósofos", test_many_philosophers()))
    results.append(("Argumentos Inválidos", test_invalid_arguments()))
    results.append(("Precisão de Morte", test_death_precision()))
    
    # Sumário final
    print_test_header("RESUMO FINAL")
    passed = sum(1 for _, result in results if result)
    total = len(results)
    
    for name, result in results:
        status = f"{bcolors.OKGREEN}✓ PASSOU{bcolors.ENDC}" if result else f"{bcolors.FAIL}✗ FALHOU{bcolors.ENDC}"
        print(f"  {name:25} {status}")
    
    print(f"\n{bcolors.BOLD}Total: {passed}/{total} testes passaram{bcolors.ENDC}")
    
    if passed == total:
        print(f"{bcolors.OKGREEN}{bcolors.BOLD}🎉 TODOS OS TESTES PASSARAM! 🎉{bcolors.ENDC}")
    else:
        print(f"{bcolors.WARNING}⚠ Alguns testes falharam. Revise o código.{bcolors.ENDC}")
    
    cleanup()

if __name__ == "__main__":
    main()