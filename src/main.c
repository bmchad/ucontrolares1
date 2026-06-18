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

    #define LDR_HISTORY_SIZE 5
    int angulo = 0, correcao = 10;
    int direcao = 1;
    int i = 0;
    double valor_ldr[LDR_HISTORY_SIZE] = {0};
    int buffer_cheio = 0;

    // Posiciona o servo na origem inicial
    servo_angulo_tempo(angulo, 1);

    while(1){
        valor_ldr[i] = ldr_ler(); // Le o LDR

        i++; 
        if(i == LDR_HISTORY_SIZE) {
            i = 0;
            buffer_cheio = 1; // Temos 5 leituras no historico!
        }

        if (buffer_cheio) {
            // Indice da leitura que acabamos de fazer
            int indice_atual = (i == 0) ? (LDR_HISTORY_SIZE - 1) : (i - 1); //sintexa fact ? true : false -> fact i==0, if true indice_atual ta no fim da fila (pois i acabou de zerar)
            double leitura_atual = valor_ldr[indice_atual];
            
            int menor_que_todos = 1;
            for(int k = 0; k < LDR_HISTORY_SIZE; k++){
                if(k != indice_atual){
                    if(leitura_atual >= valor_ldr[k]){
                        menor_que_todos = 0; // Nao eh a menor leitura, entao a luminosidade nao caiu
                        break;
                    }
                }
            }

            if (menor_que_todos) { //nao eh a menor leitura
                printk("Luz diminuiu abaixo do historico! Invertendo direcao e saltando %d graus.\n", correcao);
                
                // Inverte a direcao antes de mover para "tatear" buscando a luz
                direcao = -direcao;
                angulo += (direcao * correcao);

                // Protecao dos limites do servo (0 a 180 graus)
                if (angulo > 180) {
                    angulo = 180;
                    direcao = -1; // forca a voltar se bater no limite
                } else if (angulo < 0) {
                    angulo = 0;
                    direcao = 1; // forca a ir se bater no limite
                }

                // Move e espera 1 segundo para a mecanica assentar e nao dar trancos
                servo_angulo_tempo(angulo, 2);
                
                // Reseta o historico para comecar a avaliar a nova posicao
                buffer_cheio = 0;
                i = 0;
            }
        }

        k_msleep(200); // Espera 200ms entre as leituras (forma 1 segundo de historico de 5 itens)
    }
    
    return 0;
}