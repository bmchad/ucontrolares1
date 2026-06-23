#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>

//ADC é o conversor analogico digital

#define ADC_NODE DT_PATH(zephyr_user)

// LDR_D (Direita) no índice 0 configurado no overlay
static const struct adc_dt_spec adc_channel_dir = ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 0);
// LDR_E (Esquerda) no índice 1 configurado no overlay
static const struct adc_dt_spec adc_channel_esq = ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 1);

static int16_t sample_buffer[1];

// configuração do ADC e das variaveis utilizadas

/* Função para inicializar o ADC apenas uma vez */
int ldr_inicializar(void)
{
    int err;
    
    // Configura o canal da direita
    if (!device_is_ready(adc_channel_dir.dev)) {
        printk("Erro: Dispositivo ADC Dir nao esta pronto.\n");
        return -ENODEV;
    }
    err = adc_channel_setup_dt(&adc_channel_dir);
    if (err < 0) {
        printk("Erro ao configurar o canal ADC Dir (%d)\n", err);
        return err;
    }

    // Configura o canal da esquerda
    if (!device_is_ready(adc_channel_esq.dev)) {
        printk("Erro: Dispositivo ADC Esq nao esta pronto.\n");
        return -ENODEV;
    }
    err = adc_channel_setup_dt(&adc_channel_esq);
    if (err < 0) {
        printk("Erro ao configurar o canal ADC Esq (%d)\n", err);
        return err;
    }

//verifica se os componentes estao preparados
    return 0;
}

/* Função focada apenas em ler e retornar o valor em millivolts */
double ldr_ler(char lado)
{
    int err;
    int32_t millivolts = 0;
    
    const struct adc_dt_spec *channel;
    
    // 'd' para Direita, 'e' para Esquerda
    if (lado == 'd' || lado == 'D') {
        channel = &adc_channel_dir;
    } else if (lado == 'e' || lado == 'E') {
        channel = &adc_channel_esq;
    } else {
        printk("Lado invalido. Use 'e' ou 'd'.\n");
        return -1.0;
    }

    struct adc_sequence sequence = {
        .buffer = sample_buffer,
        .buffer_size = sizeof(sample_buffer),
    };
    //cria um local para armazenar os dados

    err = adc_sequence_init_dt(channel, &sequence);
    if (err < 0) {
        printk("Erro ao inicializar a sequencia do ADC (%d)\n", err);
        return -1.0;
    }
    //formata o armanenamento

    err = adc_read(channel->dev, &sequence);
    if (err < 0) {
        printk("Erro ao ler o ADC (%d)\n", err);
        return -1.0;
    }
    //le os dados

    int32_t raw_value = sample_buffer[0];
    millivolts = raw_value;
    
    // Removi o printk interno constante para não poluir o terminal, o main.c gerencia isso
    millivolts = millivolts * 0.889; //0.889 : valor arbitrario
    return (double)millivolts; 
}