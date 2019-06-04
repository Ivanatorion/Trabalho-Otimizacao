Para utilizar o script "trabalho.sh", instale o gnuplot com:
sudo apt-get install gnuplot-x11

Compile com o -O3, fica umas 3 vezes mais rapido (literalmente 3 vezes).

O programa le a instancia de entrada a partir da "stdin". 
Para fornecer um arquivo de entrada, use: 

```bash
cat NOME_DO_ARQUIVO | ./SOLV ARQUIVO_DE_SAIDA SEED <TEMPERATURE DECAY NUMBER_OF_NEIGHBORS>
```

Onde os parâmetros em `<>` são opcionais, porém posicionais, isto é, não possível setar `NUMBER_OF_NEIGHBORS` sem setar `TEMPERATURE` e `DECAY` primeiro.

