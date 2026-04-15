#define SIM_SCGC5   (*((volatile unsigned int*)0x40048038))
#define PORTB_PCR19 (*((volatile unsigned int*)0x4004A04C)) //verde
#define PORTB_PCR18 (*((volatile unsigned int*)0x4004A048)) //vermelho
#define GPIOB_PDDR  (*((volatile unsigned int*)0x400FF054))
#define GPIOB_PTOR  (*((volatile unsigned int*)0x400FF04C))

void delayMs(int n);

int main(void) {
    SIM_SCGC5 |= (1 << 10); //ativar os mux
    PORTB_PCR19 = (1 << 8);
    PORTB_PCR18 = (1 << 8);  //definir os outputs
    GPIOB_PDDR |= (1 << 19 | 1 << 18);
    GPIOB_PTOR = (1 << 18); //ele já estava verde fica vermelho
    while(1) {
        //delayMs(2000);
        //GPIOB_PTOR = (1 << 18); //fica  vermelho
        //verde+vermelho = amarelo
        delayMs(1000);
        GPIOB_PTOR = (1 << 19); //desliga o verde = fica  vermelho
        //delayMs(2000);
        //GPIOB_PTOR = (1 << 18 | 1 << 19); //desliga o vermelho liga verde
    }
    return 0;
}

void delayMs(int n) {
   volatile int i;
    volatile int j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 7000; j++) {
        }
    }
}