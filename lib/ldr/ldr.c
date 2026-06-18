#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>

//ADC é o conversor analogico digital

#define ADC_NODE DT_PATH(zephyr_user)

static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(ADC_NODE);
static int16_t sample_buffer[1];

// configuração do ADC e das variaveis utilizadas

/* Função para inicializar o ADC apenas uma vez */
int ldr_inicializar(void)
{
    int err;
    
    if (!device_is_ready(adc_channel.dev)) {
        printk("Erro: Dispositivo ADC nao esta pronto.\n");
        return -ENODEV;
    }

    err = adc_channel_setup_dt(&adc_channel);
    if (err < 0) {
        printk("Erro ao configurar o canal ADC (%d)\n", err);
        return err;
    }

//verifica se os componentes estao preparados
    return 0;
}

/* Função focada apenas em ler e retornar o valor em millivolts */
double ldr_ler(void)
{
    int err;
    int32_t millivolts = 0;

    struct adc_sequence sequence = {
        .buffer = sample_buffer,
        .buffer_size = sizeof(sample_buffer),
    };
    //cria um local para armazenar os dados

    err = adc_sequence_init_dt(&adc_channel, &sequence);
    if (err < 0) {
        printk("Erro ao inicializar a sequencia do ADC (%d)\n", err);
        return -1.0;
    }
    //formata o armanenamento

    err = adc_read(adc_channel.dev, &sequence);
    if (err < 0) {
        printk("Erro ao ler o ADC (%d)\n", err);
        return -1.0;
    }
    //le os dados

    int32_t raw_value = sample_buffer[0];
    millivolts = raw_value;
    
    
    printk("Valor bruto: %d | Tensao: %.2f mV\n", raw_value, (double)millivolts);
    millivolts = millivolts * 0.889; //0.889 : valor arbitrario
    return (double)millivolts; 
}