#include "init_hora.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

// Variáveis públicas para leitura
int hora_atual = 0;
int minuto_atual = 0;
int segundo_atual = 0;

// Variáveis privadas de controle (o usuário muda a hora inicial aqui)
static int start_hora = 15;
static int start_minuto = 0;
static int start_segundo = 0;
static int64_t uptime_inicial = 0;

void relogio_init(void) {
    // Captura os milissegundos exatos em que o relógio foi inicializado
    uptime_inicial = k_uptime_get();
    
    hora_atual = start_hora;
    minuto_atual = start_minuto;
    segundo_atual = start_segundo;
}

void relogio_atualizar(void) {
    // Pega o tempo de hardware atual
    int64_t uptime_agora = k_uptime_get();
    
    // Calcula o "delta" (quanto tempo se passou desde o init)
    int64_t delta_ms = uptime_agora - uptime_inicial;
    int delta_s = (int)(delta_ms / 1000);
    
    // Converte a hora inicial toda para segundos e soma com o delta
    int total_s_inicial = start_hora * 3600 + start_minuto * 60 + start_segundo;
    int total_s_agora = total_s_inicial + delta_s;
    
    // Atualiza as variáveis globais que o usuário pode ler
    hora_atual = (total_s_agora / 3600) % 24;
    minuto_atual = (total_s_agora / 60) % 60;
    segundo_atual = total_s_agora % 60;
    
    // Você pode descomentar a linha abaixo se quiser imprimir a hora a cada rodada!
    // printk("Relogio atualizado: %02d:%02d:%02d\n", hora_atual, minuto_atual, segundo_atual);
}
