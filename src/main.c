/* NOTAS
pwm = ptd3
*/

#include <zephyr/kernel.h>             // Funções básicas do Zephyr (ex: k_msleep, k_thread, etc.)
#include <zephyr/device.h>             // API para obter e utilizar dispositivos do sistema
#include <zephyr/drivers/gpio.h>       // API para controle de pinos de entrada/saída (GPIO)
#include <pwm_z42.h>                // Biblioteca personalizada com funções de controle do TPM (Timer/PWM Module)
#include <ldr.h>

// Para um clock de 48 MHz e prescaler de 128 (PS_128), a frequência do timer é 375 kHz.
// Para um período de 20ms (50Hz), TPM_MODULE (MOD) deve ser 7500.
// 1ms (0 graus) = 375 contagens.
// 2ms (180 graus) = 750 contagens.
#define TPM_MODULE 7500
#define SLEEP_TIME_MS 1000
#define INPUT_PORT DT_NODELABEL(gpiod)
#define INPUT_PIN 20

//aparentemente a cada 180 que peço para esse ir ele só move 90. por isso vou dobrar o angulo no código
void servo_angulo_tempo(int ang, int t){
    while(ang>180){
        ang-=180;
    }
    while(ang<0){
        ang+=180;
    }
    // 375 é o offset para 1ms (0 graus), (750-375) = variação para até 2ms (180 graus)
    pwm_tpm_CnV(TPM0, 3, (375 + (2*ang * 375 / 180))); //aqui multiplico por 2 para tentar corrigir 
    k_msleep((int)(1000 * t));
}

int main(void){

    // Configura os motores
    pwm_tpm_Init(TPM0, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);
    pwm_tpm_Ch_Init(TPM0, 3, TPM_PWM_H, GPIOD, 3);

    //configura o ldr
    printk("Iniciando Aplicacao de Monitoramento do LDR...\n");

    // Inicializa o hardware do ADC
    if (ldr_inicializar() < 0)  //configura o ldr
    {
        printk("Falha critica na inicializacao do ADC. Parando.\n");
        return -1;
    }

    printk("ADC pronto. Entrando no loop de leitura.\n");

    
    int angulo = 0, correcao = 10;
    int numero_ldr = 4, i = 0;
    double valor_ldr[numero_ldr];
    int *veredito = 2;
    servo_angulo_tempo(angulo, 1);


    while(1){

        valor_ldr[i] = ldr_ler();//le o ldr
        i++; if(i==5) i = 0;

        //verificar se a última leitura do ldr é menor que as primeras, e se sim, mover para a esquerda
        for(int k = 0; k<numero_ldr; k++){
            if(valor_ldr[numero_ldr]<valor_ldr[k]) *veredito++;
            if(*veredito == numero_ldr) *veredito = 0; //é menor
            else *veredito = -1; //não é menor 
        }

        if (*veredito = 0) {
            /* Exibe o valor lido convertido em Double */
            printk("Tensao LDR lida no main: %.2lf mV\n", valor_ldr[i]);
        } else {
            printk("Falha ao obter dados do sensor.\n");
        }

        servo_angulo_tempo(correcao, 1);

    return 0;
}