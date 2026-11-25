# Administração de Rebanho

Sistema de gestão de rebanho de corte, em linguagem C e executado no terminal. Trabalho da disciplina de **Algoritmos 2** (Uniftec).

## Objetivo geral

Desenvolver uma aplicação modularizada que apoie o controle de um rebanho de corte, do cadastro do animal até o registro do abate, aplicando os conteúdos estudados na disciplina.

## Objetivos específicos

- Cadastrar animais com brinco, raça, peso, idade, status e piquete.
- Acompanhar pesagens, status sanitário e baixas.
- Controlar a lotação dos piquetes e os períodos de descanso do pasto.
- Consultar o rebanho por raça, com peso médio e total de arrobas.
- Projetar quantos meses faltam para um animal atingir o peso de abate.
- Registrar os cortes da desossa, com rendimento e receita estimada.
- Manter os dados entre execuções e gerar um relatório em texto.

## Conteúdos aplicados

| Conteúdo | Aplicação no projeto |
|---|---|
| Structs | `Animal` e `RegistroCorte` |
| Comandos condicionais | `if`/`else` e `switch` nos menus e nas validações |
| Laços | `for`, `while` e `do-while` |
| Vetores | Rebanho e histórico de cortes |
| Matrizes | Mapa de piquetes 5 x 5 |
| Strings | Raça e status, com busca por parte do nome |
| Arquivos | Binário para os dados e texto para o relatório |
| Modularização | Funções separadas por responsabilidade |
| Recursividade | Projeção de engorda |

## Funcionalidades

| Opção | Descrição |
|---|---|
| 1 | Cadastrar novo animal |
| 2 | Listar rebanho ativo |
| 3 | Pesagem, status e baixa de animal |
| 4 | Consultar e analisar por raça |
| 5 | Mapa de piquetes e movimentação |
| 6 | Projeção de engorda |
| 7 | Corte e desossa de animal abatido |
| 8 | Exportar relatório geral (`.txt`) |
| 9 | Preencher com dados de teste |
| 0 | Salvar e sair |

## Como compilar e executar

É necessário ter o GCC instalado.

```bash
g++ trabalho.cpp -o trabalho
./trabalho
```

Na primeira execução, use a opção **9** para carregar os dados de exemplo.

**Atenção:** os dados são gravados somente ao sair pela opção **0**.

## Arquivos gerados

| Arquivo | Conteúdo |
|---|---|
| `fazenda.bin` | Animais, piquetes e cortes salvos |
| `relatorio_fazenda.txt` | Rebanho ativo, animais prontos para abate e histórico de cortes |

## Regras e estimativas adotadas

- Capacidade de até 200 registros de animais e 10 animais por piquete.
- Arrobas estimadas considerando 50% de rendimento sobre o peso vivo (1 arroba = 15 kg).
- Preços de referência por kg: picanha R$ 78, contrafilé/alcatra R$ 52, costela R$ 32 e dianteiro/outros R$ 28.
- No relatório, animais em quarentena não são listados como prontos para abate.
