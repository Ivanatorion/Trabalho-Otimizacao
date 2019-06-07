# Resultados automaticos com o uso dos scripts bash #

Para utilizar os scripts, instale o gnuplot com:
sudo apt-get install gnuplot-x11

O Script "trabalho.sh" gera os resultados para todas as instancias com os mesmos parametros de execucao
Os parametros do programa devem ser passados como argumento para o script, use:
./trabalho.sh SEED TEMPERATURA_INICIAL ESFRIAMENTO N_VIZINHOS_POR_TEMPERATURA

O Script "meta-trabalho.sh" faz tudo (tipo, tudo), use:
./meta-trabalho.sh
E va dormir, provavelmente vai demorar um pouco...
O resultado gerado por esse script esta disponivel no repositorio (ResultadosOtimizacao.zip)

# Compilacao #

Compile com o -O3, fica umas 3 vezes mais rapido (literalmente 3 vezes).

Ou simplesmente execute o script "compile.sh"

# Uso do programa #

O programa le a instancia de entrada a partir da "stdin". 
Para fornecer um arquivo de entrada, use: 

```bash
cat NOME_DO_ARQUIVO | ./SOLV ARQUIVO_DE_SAIDA SEED <TEMPERATURE DECAY NUMBER_OF_NEIGHBORS>
```

Onde os parâmetros em `<>` são opcionais, porém posicionais, isto é, não possível setar `NUMBER_OF_NEIGHBORS` sem setar `TEMPERATURE` e `DECAY` primeiro.

