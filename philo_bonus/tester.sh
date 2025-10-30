#!/bin/bash

# Teste de stress manual para philosophers bonus
# Testa com número crescente de filósofos

EXECUTABLE="./philo_bonus"

# Cores
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}╔══════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║     PHILOSOPHERS BONUS - TESTE DE STRESS MANUAL         ║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════════════════════════╝${NC}"

if [ ! -f "$EXECUTABLE" ]; then
    echo -e "${RED}✗ Executável não encontrado. Rode 'make' primeiro.${NC}"
    exit 1
fi

test_philosophers() {
    local num=$1
    local time_to_die=$2
    local time_to_eat=$3
    local time_to_sleep=$4
    local duration=$5
    local description=$6
    
    echo ""
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BLUE}Testando: ${description}${NC}"
    echo -e "${BLUE}Config: $num philos | die:${time_to_die}ms eat:${time_to_eat}ms sleep:${time_to_sleep}ms${NC}"
    echo -e "${BLUE}Duração: ${duration}s${NC}"
    
    # Limpa semáforos antigos
    rm -f /dev/shm/sem.philo_* 2>/dev/null
    
    # Roda o programa com timeout
    timeout ${duration}s $EXECUTABLE $num $time_to_die $time_to_eat $time_to_sleep > /tmp/philo_output.txt 2>&1 &
    PID=$!
    
    # Mostra progresso
    local elapsed=0
    while kill -0 $PID 2>/dev/null && [ $elapsed -lt $duration ]; do
        sleep 1
        elapsed=$((elapsed + 1))
        printf "\r${BLUE}Executando... ${elapsed}s / ${duration}s${NC}"
    done
    echo ""
    
    # Espera terminar
    wait $PID
    EXIT_CODE=$?
    
    # Verifica resultado
    if [ $EXIT_CODE -eq 124 ]; then
        # Timeout - pode ser OK para testes longos
        echo -e "${YELLOW}⏱  Timeout após ${duration}s${NC}"
        
        # Mata processos se ainda rodando
        pkill -9 philo_bonus 2>/dev/null
        
        # Verifica se teve mortes no output
        if grep -q "died" /tmp/philo_output.txt; then
            echo -e "${RED}✗ Um filósofo MORREU durante o teste${NC}"
            grep "died" /tmp/philo_output.txt | head -1
            return 1
        else
            echo -e "${GREEN}✓ Nenhum filósofo morreu durante ${duration}s${NC}"
            
            # Conta eventos
            local forks=$(grep -c "taken a fork" /tmp/philo_output.txt)
            local eating=$(grep -c "is eating" /tmp/philo_output.txt)
            local sleeping=$(grep -c "is sleeping" /tmp/philo_output.txt)
            
            echo -e "${BLUE}Eventos: ${eating} refeições | ${sleeping} sonos | ${forks} garfos${NC}"
            return 0
        fi
    elif [ $EXIT_CODE -eq 0 ]; then
        # Terminou normalmente
        if grep -q "died" /tmp/philo_output.txt; then
            echo -e "${RED}✗ Um filósofo MORREU${NC}"
            grep "died" /tmp/philo_output.txt | head -1
            return 1
        else
            echo -e "${GREEN}✓ Teste passou! Nenhum filósofo morreu${NC}"
            
            # Mostra estatísticas
            local eating=$(grep -c "is eating" /tmp/philo_output.txt)
            echo -e "${GREEN}Total de refeições: ${eating}${NC}"
            return 0
        fi
    else
        echo -e "${RED}✗ Programa terminou com erro (código: $EXIT_CODE)${NC}"
        return 1
    fi
}

# Array para rastrear resultados
declare -a RESULTS
declare -a TESTS

# ============================================
# TESTES BÁSICOS
# ============================================

echo ""
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}         TESTES BÁSICOS (verificação rápida)               ${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"

test_philosophers 5 800 200 200 3 "5 filósofos (baseline)"
RESULTS+=($?)
TESTS+=("5 filósofos")

test_philosophers 10 800 200 200 3 "10 filósofos"
RESULTS+=($?)
TESTS+=("10 filósofos")

# ============================================
# STRESS TESTS
# ============================================

echo ""
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}         STRESS TESTS (muitos filósofos)                   ${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"

test_philosophers 50 800 200 200 5 "50 filósofos (stress médio)"
RESULTS+=($?)
TESTS+=("50 filósofos")

test_philosophers 100 800 200 200 8 "100 filósofos (stress alto)"
RESULTS+=($?)
TESTS+=("100 filósofos")

test_philosophers 200 800 200 200 10 "200 filósofos (stress extremo)"
RESULTS+=($?)
TESTS+=("200 filósofos")

# ============================================
# CASO ESPECIAL: 1 FILÓSOFO
# ============================================

echo ""
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}         CASO ESPECIAL                                     ${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"

# 1 filósofo deve morrer
echo -e "${BLUE}Testando: 1 filósofo (deve morrer)${NC}"
rm -f /dev/shm/sem.philo_* 2>/dev/null
timeout 2s $EXECUTABLE 1 800 200 200 > /tmp/philo_output.txt 2>&1

if grep -q "died" /tmp/philo_output.txt; then
    DEATH_TIME=$(grep "died" /tmp/philo_output.txt | awk '{print $1}')
    echo -e "${GREEN}✓ Filósofo morreu corretamente em ${DEATH_TIME}ms (esperado ~800ms)${NC}"
    RESULTS+=(0)
else
    echo -e "${RED}✗ Filósofo não morreu como esperado${NC}"
    RESULTS+=(1)
fi
TESTS+=("1 filósofo")

# ============================================
# RESUMO
# ============================================

echo ""
echo -e "${BLUE}╔══════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                    RESUMO FINAL                          ║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════════════════════════╝${NC}"
echo ""

PASSED=0
TOTAL=${#RESULTS[@]}

for i in "${!RESULTS[@]}"; do
    if [ ${RESULTS[$i]} -eq 0 ]; then
        echo -e "  ${GREEN}✓${NC} ${TESTS[$i]}"
        PASSED=$((PASSED + 1))
    else
        echo -e "  ${RED}✗${NC} ${TESTS[$i]}"
    fi
done

echo ""
echo -e "${BLUE}Total: ${PASSED}/${TOTAL} testes passaram${NC}"

if [ $PASSED -eq $TOTAL ]; then
    echo -e "${GREEN}${GREEN}╔══════════════════════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║          🎉 TODOS OS TESTES PASSARAM! 🎉                 ║${NC}"
    echo -e "${GREEN}╚══════════════════════════════════════════════════════════╝${NC}"
    exit 0
else
    echo -e "${YELLOW}⚠  Alguns testes falharam. Revise o código.${NC}"
    exit 1
fi

# Limpa
rm -f /tmp/philo_output.txt