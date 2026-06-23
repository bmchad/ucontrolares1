#ifndef INIT_HORA_H
#define INIT_HORA_H

// Variáveis globais contendo a hora atualizada
extern int hora_atual;
extern int minuto_atual;
extern int segundo_atual;

// Inicializa o relógio usando os valores definidos no topo do init_hora.c
void relogio_init(void);

// Deve ser chamada continuamente (ex: no while) para calcular e atualizar o relógio
void relogio_atualizar(void);

#endif