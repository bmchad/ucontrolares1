Primeira tentativa:
A ideia central é criar um laço while em que a cada ciclo o carrinho procura uma curva (isso é, esquerda = 0 e direita = 1 ou vice versa). Se ele não acha curva ele segue reto com n% de pwm por 10ms, antes de repetir o laço. n é um contador que começa começa em 0, acelerando 1% por ciclo. Caso o carrinho ache uma curva, o contador volta para 80. Logo, o contador só fica em 0 ao ligar o carrinho, mas durante a volta, sua velocidade de reta é no mínimo de 80. Para as curvas, a relação 90 para frente e 40 para trás trouxe bons resultados.

O código funciona de forma satisfatória, mas há um problema físico no carrinho: os sensores estão muito próximos, deixando o carrinho por vezes "preso" em zig-zag.

Segunda tentativa:
Implementou-se lógica para 3 sensores aproveitando essa ideia de aceleração. Para o sensor do meio, criou-se um sistema de memória para ele lembrar qual foi a última curva feita. Ou seja, se o sensor da direita foi ativado, o sensor do meio sabe que o carrinho está em uma curva para a direita. Assim, quando o sensor do meio for ativado, ele corrigirá o carrinho levemente para a esquerda. E vice-versa

O grande problema da implementação desse código é que os motores DC tinham diferentes potências máximas. Em média, o motor da direita era 10% mais potente que o da esquerda. Assim, essas correções leves eram raramente observadas, pois a difernça do pwm no software não superava a diferença das potências de saída do hardware.

Consequentemente, o carrinho com 3 sensores aprosentou lógica muito complexa para resultados iguais ao carrinho com 2 sensores

O que eu faria diferente:
Usaria uma lógica de PID com 4 ou 6 sensores (2 ou 3 de cada lado), de fato promovendo correções proporcionais durante as curvas

EVIDÊNCIAS: 
https://drive.google.com/file/d/1Eu0Jbbld2EuSbPxGxIpqz8YSCFXWB1Ga/view?usp=drive_link
