# Conway's Game of Life

Implementação em CUDA do famoso Automato Celular desenvolvido pelo John Conway.
<p align="center">
  <img src="gol.gif"/>
</p>

[Google Colab](https://colab.research.google.com/drive/1qe2HrjaupIzkqhecbME6N19I_NGBGjxh)

## Regras

O jogo consiste em quatro regras:

1. Qualquer célula viva com menos de dois vizinhos vivos morre de solidão.
2. Qualquer célula viva com mais de três vizinhos vivos morre de superpopulação.
3. Qualquer célula morta com exatamente três vizinhos vivos se torna uma célula viva.
4. Qualquer célula viva com dois ou três vizinhos vivos continua no mesmo estado para a próxima geração.
