# Parser

Reconhecedor de gramática escrito em C

## Dependências

- CMake
- GCC

## Como executar

Com o GCC e CMake instalados basta executar o arquivo **build.sh** na raiz do diretório. Após isso use o comando abaixo para testar em arquivos de gramática.

```bash
./build/Parser gramatica.txt
```

## Exemplo

Para um arquivo como abaixo:

```txt
S > aSa - S > bSb - S > c - S > $
```

A Saída é esta:

```bash
G = ({S}, {a,b,c}, P, S)
P = {S -> aSa, S -> bSb, S -> c, S -> $}
Correct
```

Para um arquivo como abaixo:

```txt
S > aS - S > bSb - S > c - S > A - S > $
```

A saída é esta:

```bash
Incorrect
```

# Equipe

Samuel do Prado Rodrigues (GU3052788)
