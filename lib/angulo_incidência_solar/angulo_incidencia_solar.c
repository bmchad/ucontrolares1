#include "angulo_incidencia_solar.h"
#include <stdio.h>
#include <math.h>

// Definição de Pi para uso na biblioteca math.h
#define PI 3.14159265358979323846

// =========================================================
// PREENCHA ESTAS VARIÁVEIS ANTES DE RODAR O SEU TRACKER!
// =========================================================
static int dia = 23;            // Formato DD, ex: 06
static int mes = 06;            // Formato MM, ex: 07
static int ano = 2026;         // Formato AAAA, ex: 2026
static double phi = -23.5;     // Latitude local (graus, sul é negativo. Ex SP: -23.5)
static double L = 46.6;        // Longitude local (graus, oeste é positivo. Ex SP: 46.6)
static double L0 = 45.0;       // Longitude do meridiano legal (graus, ex: 45 para Brasil)
static double beta = 30.0;     // Inclinação da placa em relação à horizontal (graus)
// =========================================================

// Funções auxiliares para converter graus e radianos
// (A biblioteca math.h do C exige radianos)
double deg2rad(double deg) {
    return deg * PI / 180.0;
}

double rad2deg(double rad) {
    return rad * 180.0 / PI;
}

double arredondar_para_multiplo_de_5(double theta) {
    return round(theta / 5.0) * 5.0;
}

double calcular_theta(double HL, double gama_p) {
    // Variáveis de Entrada o usuário digital aqui

    /*

    int dia = ; //formato DD, ex: 06
    int mes = ; //formato MM, ex: 07
    int ano = ; //formato AAAA, ex: 2026
    //double HL = //pega a hora do relogio e converte;       // Hora Legal (ex: 10.5 para 10h30)
    double phi = ;      // Latitude local (graus, sul é negativo)
    double L = ;        // Longitude local (grausm oeste é positvo)
    double L0 = ;       // Longitude da hora legal (graus, ex: 45 para fuso de SP)
    double beta = ;     // Inclinação da placa em relação à horizontal (graus)
    double gama_p = //angulo do main;   // Azimute da placa (graus)

    printf("=== TRACKER SOLAR: CALCULO DE INCIDENCIA ===\n\n");
    
    // Coleta das informações do usuário
    
    printf("Digite a data (DD MM AAAA): ");
    scanf("%d %d %d", &dia, &mes, &ano);
    
    printf("Digite o Horario Legal (em decimal, ex: 10.0 para 10h00): ");
    scanf("%lf", &HL);
    
    printf("Digite a Latitude (graus, negativo para Sul): ");
    scanf("%lf", &phi);
    
    printf("Digite a Longitude local (graus, positivo p/ Oeste): ");
    scanf("%lf", &L);
    
    printf("Digite a Longitude do meridiano legal (L0) (ex: 45 para Brasil): ");
    scanf("%lf", &L0);
    
    printf("Digite a Inclinacao da placa (beta, em graus): ");
    scanf("%lf", &beta);
    
    printf("Digite o Azimute da placa (gama, em graus, Sul=0, Norte=180/-180): ");
    scanf("%lf", &gama_p);

    printf("\n--- RESULTADOS ---\n");

*/

    // 1. Encontrar 'n' (dia do ano)
    int cor = 0;
    if (mes <= 2) {
        cor = mes / 2; // Int truncado
    } else if (mes > 2 && mes <= 8) {
        cor = (mes / 2) - 2;
    } else if (mes > 8) {
        cor = ((mes + 1) / 2) - 2; 
    }
    int n = dia + (mes - 1) * 30 + cor;
    //printf("Dia do ano (n): %d\n", n);

    // 2. Calcular a declinacao solar (delta)
    double angulo_delta = (360.0 / 365.0) * (284.0 + n);
    double delta = 23.45 * sin(deg2rad(angulo_delta));
    //printf("Declinacao Solar (delta): %.2lf graus\n", delta);

    // 3. Calcular a Hora Solar (HS) e Equacao do Tempo (E)
    double B = (360.0 / 364.0) * (n - 81.0);
    double E = 9.87 * sin(deg2rad(2 * B)) - 7.53 * cos(deg2rad(B)) - 1.5 * sin(deg2rad(B));
    
    double cor_hora = (4.0 * (L0 - L) + E) / 60.0;
    double HS = HL + cor_hora;
    //printf("Hora Solar (HS): %.2lf h\n", HS);

    // 4. Calcular o Angulo Horario (omega)
    double omega = (HS - 12.0) * 15.0;
    //printf("Angulo Horario (omega): %.2lf graus\n", omega);

    // 5 e 6. Por do Sol e Duracao da Insolacao (N)
    //cos(omega_s) = -tan(phi)*tan(delta)
    double cos_ws = -tan(deg2rad(phi)) * tan(deg2rad(delta));
    double omega_s = rad2deg(acos(cos_ws));
    double N = (2.0 / 15.0) * omega_s;
    //printf("Duracao da insolacao no dia (N): %.2lf h\n", N);

    // 7. Calcular o Angulo de Incidencia (theta) sobre a placa
    double sen_delta = sin(deg2rad(delta));
    double cos_delta = cos(deg2rad(delta));
    double sen_phi = sin(deg2rad(phi));
    double cos_phi = cos(deg2rad(phi));
    double sen_beta = sin(deg2rad(beta));
    double cos_beta = cos(deg2rad(beta));
    double sen_gama = sin(deg2rad(gama_p));
    double cos_gama = cos(deg2rad(gama_p));
    double sen_omega = sin(deg2rad(omega));
    double cos_omega = cos(deg2rad(omega));

    // Equacao 9.15 do material:
    double termo1 = sen_delta * sen_phi * cos_beta;
    double termo2 = - (sen_delta * cos_phi * sen_beta * cos_gama);
    double termo3 = cos_delta * cos_phi * cos_beta * cos_omega;
    double termo4 = cos_delta * sen_phi * sen_beta * cos_gama * cos_omega;
    double termo5 = cos_delta * sen_beta * sen_gama * sen_omega;

    double cos_theta = termo1 + termo2 + termo3 + termo4 + termo5;
    double theta = rad2deg(acos(cos_theta));

    //printf("\n>>> ANGULO DE INCIDENCIA S/ PLACA (theta): %.2lf graus <<<\n\n", theta);

    return theta;
}