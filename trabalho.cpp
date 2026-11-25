#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ANIMAIS 200
#define LINHAS_PASTO 5
#define COLUNAS_PASTO 5
#define CAPACIDADE_PIQUETE 10
#define ARQUIVO_BIN "fazenda.bin"
#define ARQUIVO_TXT "relatorio_fazenda.txt"

typedef struct {
    int brinco;
    char raca[30];
    float peso_kg;
    int idade_meses;
    char status[20]; // "SAUDAVEL", "QUARENTENA", "ABATE", "VENDIDO", "ABATIDO"
    int piquete_lin;
    int piquete_col;
    int ativo;       // 1 = na fazenda, 0 = baixa/abatido
} Animal;

typedef struct {
    int brinco;
    char raca[30];
    float peso_vivo;
    float kg_picanha;
    float kg_contrafile;
    float kg_costela;
    float kg_dianteiro;
    float total_carne_kg;
    float rendimento_pct;
    float valor_estimado;
} RegistroCorte;

// --- Prototipos: Utilitarios e Strings ---
void limparBuffer();
void lerString(char destino[], int tamanho);
void padronizarMaiusculas(char texto[]);
int buscarIndiceAtivoPorBrinco(Animal rebanho[], int total, int brinco);
int brincoJaExiste(Animal rebanho[], int total, int brinco);
void selecionarRaca(char destino[]);

// --- Prototipos: Persistencia (Binario e Texto) ---
void inicializarPastos(int pastos[LINHAS_PASTO][COLUNAS_PASTO]);
void carregarDadosBinario(Animal rebanho[], int *total, int pastos[LINHAS_PASTO][COLUNAS_PASTO], RegistroCorte cortes[], int *total_cortes);
void salvarDadosBinario(Animal rebanho[], int total, int pastos[LINHAS_PASTO][COLUNAS_PASTO], RegistroCorte cortes[], int total_cortes);
void exportarRelatorioTxt(Animal rebanho[], int total, RegistroCorte cortes[], int total_cortes);

// --- Prototipos: Modulos do Sistema ---
void cadastrarAnimal(Animal rebanho[], int *total, int pastos[LINHAS_PASTO][COLUNAS_PASTO]);
void listarRebanho(Animal rebanho[], int total);
void atualizarPesagemEBaixa(Animal rebanho[], int total, int pastos[LINHAS_PASTO][COLUNAS_PASTO]);
void consultarPorRaca(Animal rebanho[], int total);

void exibirMapaPastos(int pastos[LINHAS_PASTO][COLUNAS_PASTO]);
void gerenciarPastos(Animal rebanho[], int total, int pastos[LINHAS_PASTO][COLUNAS_PASTO]);

int projetarMesesEngorda(float peso_atual, float peso_meta, float ganho_mensal, int meses_acumulados);
void moduloProjecaoEngorda(Animal rebanho[], int total);

void moduloCorteAnimal(Animal rebanho[], int total, int pastos[LINHAS_PASTO][COLUNAS_PASTO], RegistroCorte cortes[], int *total_cortes);
void listarCortes(RegistroCorte cortes[], int total_cortes);

void preencherDadosTeste(Animal rebanho[], int *total, int pastos[LINHAS_PASTO][COLUNAS_PASTO], RegistroCorte cortes[], int *total_cortes);

int main() {
    Animal rebanho[MAX_ANIMAIS];
    RegistroCorte cortes[MAX_ANIMAIS];
    int pastos[LINHAS_PASTO][COLUNAS_PASTO];
    int total_animais = 0;
    int total_cortes = 0;
    int opcao;

    carregarDadosBinario(rebanho, &total_animais, pastos, cortes, &total_cortes);

    do {
        printf("\n==============================================\n");
        printf("        SISTEMA DE GESTAO DE REBANHO          \n");
        printf("==============================================\n");
        printf("1. Cadastrar novo animal\n");
        printf("2. Listar rebanho ativo\n");
        printf("3. Pesagem, status e baixa de animal\n");
        printf("4. Consultar e analisar por raca\n");
        printf("5. Mapa de piquetes e movimentacao (Matriz)\n");
        printf("6. Projecao de engorda (Recursivo)\n");
        printf("7. Corte / desossa de animal abatido\n");
        printf("8. Exportar relatorio geral (.txt)\n");
        printf("9. Preencher banco com dados de teste\n");
        printf("0. Salvar e Sair\n");
        printf("----------------------------------------------\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1:
                cadastrarAnimal(rebanho, &total_animais, pastos);
                break;
            case 2:
                listarRebanho(rebanho, total_animais);
                break;
            case 3:
                atualizarPesagemEBaixa(rebanho, total_animais, pastos);
                break;
            case 4:
                consultarPorRaca(rebanho, total_animais);
                break;
            case 5:
                gerenciarPastos(rebanho, total_animais, pastos);
                break;
            case 6:
                moduloProjecaoEngorda(rebanho, total_animais);
                break;
            case 7:
                moduloCorteAnimal(rebanho, total_animais, pastos, cortes, &total_cortes);
                break;
            case 8:
                exportarRelatorioTxt(rebanho, total_animais, cortes, total_cortes);
                break;
            case 9:
                preencherDadosTeste(rebanho, &total_animais, pastos, cortes, &total_cortes);
                break;
            case 0:
                salvarDadosBinario(rebanho, total_animais, pastos, cortes, total_cortes);
                printf("\nDados salvos em '%s'. Encerrando...\n", ARQUIVO_BIN);
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}

// =========================================================
// UTILITARIOS E MANIPULACAO DE STRINGS
// =========================================================

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerString(char destino[], int tamanho) {
    fgets(destino, tamanho, stdin);
    destino[strcspn(destino, "\n")] = '\0';
}

void padronizarMaiusculas(char texto[]) {
    int i;
    for (i = 0; texto[i] != '\0'; i++) {
        texto[i] = toupper((unsigned char)texto[i]);
    }
}

int buscarIndiceAtivoPorBrinco(Animal rebanho[], int total, int brinco) {
    int i;
    for (i = 0; i < total; i++) {
        if (rebanho[i].brinco == brinco && rebanho[i].ativo == 1) {
            return i;
        }
    }
    return -1;
}

int brincoJaExiste(Animal rebanho[], int total, int brinco) {
    int i;
    for (i = 0; i < total; i++) {
        if (rebanho[i].brinco == brinco) {
            return 1;
        }
    }
    return 0;
}

void selecionarRaca(char destino[]) {
    int op;
    printf("Selecione a raca:\n");
    printf("  1. NELORE\n");
    printf("  2. ANGUS\n");
    printf("  3. BRAHMAN\n");
    printf("  4. HEREFORD\n");
    printf("  5. Outra (digitar)\n");
    printf("Opcao: ");
    scanf("%d", &op);
    limparBuffer();

    switch (op) {
        case 1: strcpy(destino, "NELORE"); break;
        case 2: strcpy(destino, "ANGUS"); break;
        case 3: strcpy(destino, "BRAHMAN"); break;
        case 4: strcpy(destino, "HEREFORD"); break;
        default:
            printf("Digite o nome da raca: ");
            lerString(destino, 30);
            padronizarMaiusculas(destino);
            break;
    }
}

// =========================================================
// PERSISTENCIA EM ARQUIVO BINARIO E TEXTO
// =========================================================

void inicializarPastos(int pastos[LINHAS_PASTO][COLUNAS_PASTO]) {
    int i, j;
    for (i = 0; i < LINHAS_PASTO; i++) {
        for (j = 0; j < COLUNAS_PASTO; j++) {
            pastos[i][j] = 0;
        }
    }
}

void carregarDadosBinario(Animal rebanho[], int *total, int pastos[LINHAS_PASTO][COLUNAS_PASTO], RegistroCorte cortes[], int *total_cortes) {
    FILE *fp = fopen(ARQUIVO_BIN, "rb");
    if (fp == NULL) {
        *total = 0;
        *total_cortes = 0;
        inicializarPastos(pastos);
        printf("[INFO] Nenhum banco encontrado. Use a opcao 9 para carregar dados de teste.\n");
        return;
    }

    fread(total, sizeof(int), 1, fp);
    fread(rebanho, sizeof(Animal), *total, fp);
    fread(pastos, sizeof(int), LINHAS_PASTO * COLUNAS_PASTO, fp);
    fread(total_cortes, sizeof(int), 1, fp);
    fread(cortes, sizeof(RegistroCorte), *total_cortes, fp);

    fclose(fp);
    printf("[INFO] %d animais e %d registros de corte carregados de '%s'.\n", *total, *total_cortes, ARQUIVO_BIN);
}

void salvarDadosBinario(Animal rebanho[], int total, int pastos[LINHAS_PASTO][COLUNAS_PASTO], RegistroCorte cortes[], int total_cortes) {
    FILE *fp = fopen(ARQUIVO_BIN, "wb");
    if (fp == NULL) {
        printf("[ERRO] Nao foi possivel salvar o arquivo binario!\n");
        return;
    }

    fwrite(&total, sizeof(int), 1, fp);
    fwrite(rebanho, sizeof(Animal), total, fp);
    fwrite(pastos, sizeof(int), LINHAS_PASTO * COLUNAS_PASTO, fp);
    fwrite(&total_cortes, sizeof(int), 1, fp);
    fwrite(cortes, sizeof(RegistroCorte), total_cortes, fp);

    fclose(fp);
}

void exportarRelatorioTxt(Animal rebanho[], int total, RegistroCorte cortes[], int total_cortes) {
    float peso_minimo;
    int i, encontrados = 0;
    float receita_total = 0.0f;

    printf("\n--- EXPORTAR RELATORIO (.TXT) ---\n");
    printf("Peso minimo para listar como pronto para abate (ex: 480): ");
    scanf("%f", &peso_minimo);
    limparBuffer();

    FILE *fp = fopen(ARQUIVO_TXT, "w");
    if (fp == NULL) {
        printf("[ERRO] Nao foi possivel criar o arquivo '%s'.\n", ARQUIVO_TXT);
        return;
    }

    fprintf(fp, "====================================================================\n");
    fprintf(fp, "                RELATORIO DE GESTAO DE REBANHO                      \n");
    fprintf(fp, "====================================================================\n\n");

    fprintf(fp, "1. REBANHO ATIVO COMPLETO\n");
    fprintf(fp, "%-8s | %-14s | %-9s | %-8s | %-12s | %-7s\n",
            "BRINCO", "RACA", "PESO(kg)", "ARROBAS", "STATUS", "PIQUETE");
    fprintf(fp, "--------------------------------------------------------------------\n");

    int ativos = 0;
    for (i = 0; i < total; i++) {
        if (rebanho[i].ativo == 1) {
            float arrobas = (rebanho[i].peso_kg * 0.50f) / 15.0f;
            fprintf(fp, "%-8d | %-14s | %-9.1f | %-8.2f | %-12s | [%d][%d]\n",
                    rebanho[i].brinco, rebanho[i].raca, rebanho[i].peso_kg,
                    arrobas, rebanho[i].status, rebanho[i].piquete_lin, rebanho[i].piquete_col);
            ativos++;
        }
    }
    if (ativos == 0) {
        fprintf(fp, "Nenhum animal ativo cadastrado.\n");
    } else {
        fprintf(fp, "Total de animais ativos: %d\n", ativos);
    }

    fprintf(fp, "\n2. ANIMAIS PRONTOS PARA ABATE (Peso >= %.1f kg, fora de quarentena)\n", peso_minimo);
    fprintf(fp, "%-8s | %-14s | %-9s | %-8s | %-12s | %-7s\n",
            "BRINCO", "RACA", "PESO(kg)", "ARROBAS", "STATUS", "PIQUETE");
    fprintf(fp, "--------------------------------------------------------------------\n");

    for (i = 0; i < total; i++) {
        if (rebanho[i].ativo == 1 && rebanho[i].peso_kg >= peso_minimo &&
            strcmp(rebanho[i].status, "QUARENTENA") != 0) {
            float arrobas = (rebanho[i].peso_kg * 0.50f) / 15.0f;
            fprintf(fp, "%-8d | %-14s | %-9.1f | %-8.2f | %-12s | [%d][%d]\n",
                    rebanho[i].brinco, rebanho[i].raca, rebanho[i].peso_kg,
                    arrobas, rebanho[i].status, rebanho[i].piquete_lin, rebanho[i].piquete_col);
            encontrados++;
        }
    }
    if (encontrados == 0) {
        fprintf(fp, "Nenhum animal apto atingiu o peso minimo informado.\n");
    }

    fprintf(fp, "\n3. HISTORICO DE ABATES E DESOSSA (CORTES)\n");
    fprintf(fp, "%-8s | %-12s | %-9s | %-9s | %-7s | %-12s\n",
            "BRINCO", "RACA", "VIVO(kg)", "CARNE(kg)", "REND(%)", "RECEITA(R$)");
    fprintf(fp, "--------------------------------------------------------------------\n");

    for (i = 0; i < total_cortes; i++) {
        fprintf(fp, "%-8d | %-12s | %-9.1f | %-9.1f | %-6.1f%% | R$ %9.2f\n",
                cortes[i].brinco, cortes[i].raca, cortes[i].peso_vivo,
                cortes[i].total_carne_kg, cortes[i].rendimento_pct, cortes[i].valor_estimado);
        receita_total += cortes[i].valor_estimado;
    }

    if (total_cortes == 0) {
        fprintf(fp, "Nenhum corte registrado ate o momento.\n");
    } else {
        fprintf(fp, "--------------------------------------------------------------------\n");
        fprintf(fp, "RECEITA TOTAL ESTIMADA DOS CORTES: R$ %.2f\n", receita_total);
    }

    fclose(fp);
    printf("[OK] Relatorio exportado com sucesso para '%s'.\n", ARQUIVO_TXT);
}

// =========================================================
// 1 e 2. CADASTRO E LISTAGEM
// =========================================================

void cadastrarAnimal(Animal rebanho[], int *total, int pastos[LINHAS_PASTO][COLUNAS_PASTO]) {
    if (*total >= MAX_ANIMAIS) {
        printf("\n[ERRO] Capacidade maxima de registros atingida.\n");
        return;
    }

    Animal novo;
    printf("\n--- CADASTRO DE ANIMAL ---\n");
    printf("Numero do brinco (ID): ");
    scanf("%d", &novo.brinco);
    limparBuffer();

    if (brincoJaExiste(rebanho, *total, novo.brinco)) {
        printf("[ERRO] Ja existe um registro com o brinco %d.\n", novo.brinco);
        return;
    }

    selecionarRaca(novo.raca);

    printf("Peso atual (kg): ");
    scanf("%f", &novo.peso_kg);
    limparBuffer();

    printf("Idade (meses): ");
    scanf("%d", &novo.idade_meses);
    limparBuffer();

    printf("Status (SAUDAVEL / QUARENTENA / ABATE): ");
    lerString(novo.status, 20);
    padronizarMaiusculas(novo.status);

    exibirMapaPastos(pastos);

    int lin, col;
    printf("Escolha o Piquete - Linha (0 a %d): ", LINHAS_PASTO - 1);
    scanf("%d", &lin);
    printf("Escolha o Piquete - Coluna (0 a %d): ", COLUNAS_PASTO - 1);
    scanf("%d", &col);
    limparBuffer();

    if (lin < 0 || lin >= LINHAS_PASTO || col < 0 || col >= COLUNAS_PASTO) {
        printf("[ERRO] Coordenada de piquete invalida.\n");
        return;
    }
    if (pastos[lin][col] == -1) {
        printf("[ERRO] Piquete [%d][%d] esta bloqueado/em descanso.\n", lin, col);
        return;
    }
    if (pastos[lin][col] >= CAPACIDADE_PIQUETE) {
        printf("[ERRO] Piquete [%d][%d] atingiu a lotacao maxima (%d).\n", lin, col, CAPACIDADE_PIQUETE);
        return;
    }

    novo.piquete_lin = lin;
    novo.piquete_col = col;
    novo.ativo = 1;

    rebanho[*total] = novo;
    (*total)++;
    pastos[lin][col]++;

    printf("[OK] Animal brinco %d (%s) cadastrado no piquete [%d][%d].\n", novo.brinco, novo.raca, lin, col);
}

void listarRebanho(Animal rebanho[], int total) {
    int i, ativos = 0;
    printf("\n--- REBANHO ATIVO ---\n");
    printf("%-8s | %-14s | %-9s | %-7s | %-12s | %-7s\n",
           "BRINCO", "RACA", "PESO(kg)", "IDADE", "STATUS", "PIQUETE");
    printf("---------------------------------------------------------------------\n");

    for (i = 0; i < total; i++) {
        if (rebanho[i].ativo == 1) {
            printf("%-8d | %-14s | %-9.1f | %-5dm  | %-12s | [%d][%d]\n",
                   rebanho[i].brinco, rebanho[i].raca, rebanho[i].peso_kg,
                   rebanho[i].idade_meses, rebanho[i].status,
                   rebanho[i].piquete_lin, rebanho[i].piquete_col);
            ativos++;
        }
    }

    if (ativos == 0) {
        printf("Nenhum animal ativo cadastrado.\n");
    } else {
        printf("Total de animais ativos: %d\n", ativos);
    }
}

// =========================================================
// 3. PESAGEM, STATUS E BAIXA
// =========================================================

void atualizarPesagemEBaixa(Animal rebanho[], int total, int pastos[LINHAS_PASTO][COLUNAS_PASTO]) {
    int brinco, idx, sub;
    printf("\n--- PESAGEM, STATUS E BAIXA ---\n");
    printf("Digite o brinco do animal: ");
    scanf("%d", &brinco);
    limparBuffer();

    idx = buscarIndiceAtivoPorBrinco(rebanho, total, brinco);
    if (idx == -1) {
        printf("[ERRO] Animal ativo com brinco %d nao encontrado.\n", brinco);
        return;
    }

    printf("\nAnimal: Brinco %d | Raca: %s | Peso: %.1f kg | Status: %s | Piquete: [%d][%d]\n",
           rebanho[idx].brinco, rebanho[idx].raca, rebanho[idx].peso_kg,
           rebanho[idx].status, rebanho[idx].piquete_lin, rebanho[idx].piquete_col);

    printf("1. Registrar nova pesagem\n");
    printf("2. Alterar status sanitario\n");
    printf("3. Dar baixa (venda vivo / obito)\n");
    printf("0. Voltar\n");
    printf("Opcao: ");
    scanf("%d", &sub);
    limparBuffer();

    if (sub == 1) {
        float novo_peso, diferenca;
        printf("Novo peso (kg): ");
        scanf("%f", &novo_peso);
        limparBuffer();

        if (novo_peso <= 0) {
            printf("[ERRO] Peso invalido.\n");
            return;
        }
        diferenca = novo_peso - rebanho[idx].peso_kg;
        rebanho[idx].peso_kg = novo_peso;
        printf("[OK] Peso atualizado para %.1f kg (Variacao: %+.1f kg).\n", novo_peso, diferenca);

    } else if (sub == 2) {
        printf("Novo status (SAUDAVEL / QUARENTENA / ABATE): ");
        lerString(rebanho[idx].status, 20);
        padronizarMaiusculas(rebanho[idx].status);
        printf("[OK] Status atualizado para %s.\n", rebanho[idx].status);

    } else if (sub == 3) {
        int l = rebanho[idx].piquete_lin;
        int c = rebanho[idx].piquete_col;
        rebanho[idx].ativo = 0;
        strcpy(rebanho[idx].status, "VENDIDO");
        if (pastos[l][c] > 0) {
            pastos[l][c]--;
        }
        printf("[OK] Baixa registrada para o brinco %d. Vaga liberada no piquete [%d][%d].\n", brinco, l, c);
    }
}

// =========================================================
// 4. CONSULTA E ANALISE POR RACA (STRINGS)
// =========================================================

void consultarPorRaca(Animal rebanho[], int total) {
    char busca[30];
    int i, qtd = 0;
    float soma_peso = 0.0f;

    printf("\n--- CONSULTA POR RACA ---\n");
    printf("Digite a raca (ou parte do nome, ex: NEL): ");
    lerString(busca, 30);
    padronizarMaiusculas(busca);

    printf("\n%-8s | %-14s | %-9s | %-8s | %-12s\n",
           "BRINCO", "RACA", "PESO(kg)", "ARROBAS", "STATUS");
    printf("-------------------------------------------------------------\n");

    for (i = 0; i < total; i++) {
        if (rebanho[i].ativo == 1 && strstr(rebanho[i].raca, busca) != NULL) {
            float arrobas = (rebanho[i].peso_kg * 0.50f) / 15.0f;
            printf("%-8d | %-14s | %-9.1f | %-8.2f | %-12s\n",
                   rebanho[i].brinco, rebanho[i].raca, rebanho[i].peso_kg,
                   arrobas, rebanho[i].status);
            soma_peso += rebanho[i].peso_kg;
            qtd++;
        }
    }

    if (qtd == 0) {
        printf("Nenhum animal ativo encontrado para '%s'.\n", busca);
    } else {
        float media = soma_peso / qtd;
        float total_arrobas = (soma_peso * 0.50f) / 15.0f;
        printf("-------------------------------------------------------------\n");
        printf("Resumo '%s': %d cabecas | Peso Medio: %.1f kg | Total Estimado: %.2f @\n",
               busca, qtd, media, total_arrobas);
    }
}

// =========================================================
// 5. MAPA DE PIQUETES E MOVIMENTACAO (MATRIZ)
// =========================================================

void exibirMapaPastos(int pastos[LINHAS_PASTO][COLUNAS_PASTO]) {
    int i, j;
    printf("\n--- MAPA DE PIQUETES (Lotacao max: %d | [ X ] = Bloqueado) ---\n", CAPACIDADE_PIQUETE);
    printf("     ");
    for (j = 0; j < COLUNAS_PASTO; j++) {
        printf("  C%d  ", j);
    }
    printf("\n");

    for (i = 0; i < LINHAS_PASTO; i++) {
        printf(" L%d  ", i);
        for (j = 0; j < COLUNAS_PASTO; j++) {
            if (pastos[i][j] == -1) {
                printf("[ X ] ");
            } else {
                printf("[%2d ] ", pastos[i][j]);
            }
        }
        printf("\n");
    }
}

void gerenciarPastos(Animal rebanho[], int total, int pastos[LINHAS_PASTO][COLUNAS_PASTO]) {
    int sub;
    exibirMapaPastos(pastos);

    printf("\n1. Mover animal de piquete\n");
    printf("2. Bloquear / Desbloquear piquete (descanso)\n");
    printf("0. Voltar\n");
    printf("Opcao: ");
    scanf("%d", &sub);
    limparBuffer();

    if (sub == 1) {
        int brinco, idx, nl, nc;
        printf("Brinco do animal a mover: ");
        scanf("%d", &brinco);
        limparBuffer();

        idx = buscarIndiceAtivoPorBrinco(rebanho, total, brinco);
        if (idx == -1) {
            printf("[ERRO] Animal ativo nao encontrado.\n");
            return;
        }

        printf("Posicao atual: [%d][%d]\n", rebanho[idx].piquete_lin, rebanho[idx].piquete_col);
        printf("Novo piquete - Linha (0 a %d): ", LINHAS_PASTO - 1);
        scanf("%d", &nl);
        printf("Novo piquete - Coluna (0 a %d): ", COLUNAS_PASTO - 1);
        scanf("%d", &nc);
        limparBuffer();

        if (nl < 0 || nl >= LINHAS_PASTO || nc < 0 || nc >= COLUNAS_PASTO) {
            printf("[ERRO] Coordenada invalida.\n");
            return;
        }
        if (pastos[nl][nc] == -1) {
            printf("[ERRO] Piquete de destino esta bloqueado.\n");
            return;
        }
        if (pastos[nl][nc] >= CAPACIDADE_PIQUETE) {
            printf("[ERRO] Piquete de destino esta lotado.\n");
            return;
        }

        pastos[rebanho[idx].piquete_lin][rebanho[idx].piquete_col]--;
        pastos[nl][nc]++;
        rebanho[idx].piquete_lin = nl;
        rebanho[idx].piquete_col = nc;

        printf("[OK] Animal %d movido para o piquete [%d][%d].\n", brinco, nl, nc);

    } else if (sub == 2) {
        int l, c;
        printf("Piquete - Linha (0 a %d): ", LINHAS_PASTO - 1);
        scanf("%d", &l);
        printf("Piquete - Coluna (0 a %d): ", COLUNAS_PASTO - 1);
        scanf("%d", &c);
        limparBuffer();

        if (l < 0 || l >= LINHAS_PASTO || c < 0 || c >= COLUNAS_PASTO) {
            printf("[ERRO] Coordenada invalida.\n");
            return;
        }

        if (pastos[l][c] == -1) {
            pastos[l][c] = 0;
            printf("[OK] Piquete [%d][%d] liberado para uso.\n", l, c);
        } else if (pastos[l][c] == 0) {
            pastos[l][c] = -1;
            printf("[OK] Piquete [%d][%d] bloqueado para descanso.\n", l, c);
        } else {
            printf("[ERRO] Nao e possivel bloquear: ha %d animais nesse piquete. Mova-os primeiro.\n", pastos[l][c]);
        }
    }
}

// =========================================================
// 6. PROJECAO DE ENGORDA (RECURSIVIDADE)
// =========================================================

int projetarMesesEngorda(float peso_atual, float peso_meta, float ganho_mensal, int meses_acumulados) {
    if (peso_atual >= peso_meta) {
        return 0; // Caso base: meta atingida
    }
    if (ganho_mensal <= 0 || meses_acumulados >= 120) {
        return -1; // Trava de seguranca contra recursao infinita
    }
    return 1 + projetarMesesEngorda(peso_atual + ganho_mensal, peso_meta, ganho_mensal, meses_acumulados + 1);
}

void moduloProjecaoEngorda(Animal rebanho[], int total) {
    int brinco, idx, meses;
    float peso_meta, ganho_mensal;

    printf("\n--- PROJECAO DE ENGORDA (RECURSIVA) ---\n");
    printf("Digite o brinco do animal: ");
    scanf("%d", &brinco);
    limparBuffer();

    idx = buscarIndiceAtivoPorBrinco(rebanho, total, brinco);
    if (idx == -1) {
        printf("[ERRO] Animal ativo nao encontrado.\n");
        return;
    }

    printf("Animal: %d (%s) | Peso atual: %.1f kg\n", rebanho[idx].brinco, rebanho[idx].raca, rebanho[idx].peso_kg);
    printf("Peso-alvo de abate (ex: 520): ");
    scanf("%f", &peso_meta);
    printf("Ganho medio mensal estimado em kg (ex: 25): ");
    scanf("%f", &ganho_mensal);
    limparBuffer();

    meses = projetarMesesEngorda(rebanho[idx].peso_kg, peso_meta, ganho_mensal, 0);

    if (meses == -1) {
        printf("[ERRO] Nao foi possivel projetar (verifique o ganho mensal informado).\n");
    } else if (meses == 0) {
        printf("[INFO] O animal ja atingiu o peso-alvo! Estimativa atual: %.2f arrobas.\n",
               (rebanho[idx].peso_kg * 0.50f) / 15.0f);
    } else {
        float peso_final = rebanho[idx].peso_kg + (meses * ganho_mensal);
        printf("\n--- RESULTADO DA PROJECAO ---\n");
        printf("Tempo estimado ate o abate : %d mes(es)\n", meses);
        printf("Peso final projetado       : %.1f kg\n", peso_final);
        printf("Rendimento estimado        : %.2f arrobas\n", (peso_final * 0.50f) / 15.0f);
    }
}

// =========================================================
// 7. CORTE / DESOSSA DE ANIMAL ABATIDO
// =========================================================

void moduloCorteAnimal(Animal rebanho[], int total, int pastos[LINHAS_PASTO][COLUNAS_PASTO], RegistroCorte cortes[], int *total_cortes) {
    int sub;
    printf("\n--- CORTE E DESOSSA DE ANIMAL ---\n");
    printf("1. Registrar abate e cortes de um animal ativo\n");
    printf("2. Listar historico de cortes e faturamento\n");
    printf("0. Voltar\n");
    printf("Opcao: ");
    scanf("%d", &sub);
    limparBuffer();

    if (sub == 2) {
        listarCortes(cortes, *total_cortes);
        return;
    }
    if (sub != 1) return;

    if (*total_cortes >= MAX_ANIMAIS) {
        printf("[ERRO] Limite de registros de corte atingido.\n");
        return;
    }

    int brinco, idx;
    printf("Brinco do animal abatido: ");
    scanf("%d", &brinco);
    limparBuffer();

    idx = buscarIndiceAtivoPorBrinco(rebanho, total, brinco);
    if (idx == -1) {
        printf("[ERRO] Animal ativo nao encontrado.\n");
        return;
    }

    RegistroCorte rc;
    rc.brinco = rebanho[idx].brinco;
    strcpy(rc.raca, rebanho[idx].raca);
    rc.peso_vivo = rebanho[idx].peso_kg;

    printf("\nRegistrando desossa - Brinco %d (%s | Peso vivo: %.1f kg)\n",
           rc.brinco, rc.raca, rc.peso_vivo);
    printf("Informe o peso obtido de cada grupo de cortes (em kg):\n");

    printf("  Picanha (kg)              : ");
    scanf("%f", &rc.kg_picanha);
    printf("  Contrafile / Alcatra (kg) : ");
    scanf("%f", &rc.kg_contrafile);
    printf("  Costela (kg)              : ");
    scanf("%f", &rc.kg_costela);
    printf("  Dianteiro / Outros (kg)   : ");
    scanf("%f", &rc.kg_dianteiro);
    limparBuffer();

    rc.total_carne_kg = rc.kg_picanha + rc.kg_contrafile + rc.kg_costela + rc.kg_dianteiro;

    if (rc.total_carne_kg <= 0 || rc.total_carne_kg > rc.peso_vivo) {
        printf("[ERRO] Soma dos cortes (%.1f kg) invalida para o peso vivo (%.1f kg).\n",
               rc.total_carne_kg, rc.peso_vivo);
        return;
    }

    // Precos de referencia por kg (R$)
    float p_picanha = 78.0f, p_contra = 52.0f, p_costela = 32.0f, p_diant = 28.0f;
    rc.rendimento_pct = (rc.total_carne_kg / rc.peso_vivo) * 100.0f;
    rc.valor_estimado = (rc.kg_picanha * p_picanha) +
                        (rc.kg_contrafile * p_contra) +
                        (rc.kg_costela * p_costela) +
                        (rc.kg_dianteiro * p_diant);

    // Salva corte e da baixa automatica no animal e piquete
    cortes[*total_cortes] = rc;
    (*total_cortes)++;

    int l = rebanho[idx].piquete_lin;
    int c = rebanho[idx].piquete_col;
    rebanho[idx].ativo = 0;
    strcpy(rebanho[idx].status, "ABATIDO");
    if (pastos[l][c] > 0) {
        pastos[l][c]--;
    }

    printf("\n--- RESUMO DA DESOSSA (Brinco %d) ---\n", rc.brinco);
    printf("Carne limpa aproveitada : %.1f kg (Rendimento: %.1f%%)\n", rc.total_carne_kg, rc.rendimento_pct);
    printf("Quebra (ossos/aparas)   : %.1f kg\n", rc.peso_vivo - rc.total_carne_kg);
    printf("Faturamento estimado    : R$ %.2f\n", rc.valor_estimado);
    printf("[OK] Animal baixado e vaga liberada no piquete [%d][%d].\n", l, c);
}

void listarCortes(RegistroCorte cortes[], int total_cortes) {
    int i;
    float soma_receita = 0.0f;
    printf("\n--- HISTORICO DE CORTES / DESOSSA ---\n");
    printf("%-8s | %-11s | %-8s | %-9s | %-7s | %-12s\n",
           "BRINCO", "RACA", "VIVO(kg)", "CARNE(kg)", "REND(%)", "RECEITA(R$)");
    printf("--------------------------------------------------------------------\n");

    for (i = 0; i < total_cortes; i++) {
        printf("%-8d | %-11s | %-8.1f | %-9.1f | %-6.1f%% | R$ %9.2f\n",
               cortes[i].brinco, cortes[i].raca, cortes[i].peso_vivo,
               cortes[i].total_carne_kg, cortes[i].rendimento_pct, cortes[i].valor_estimado);
        soma_receita += cortes[i].valor_estimado;
    }

    if (total_cortes == 0) {
        printf("Nenhum corte registrado.\n");
    } else {
        printf("--------------------------------------------------------------------\n");
        printf("Faturamento acumulado: R$ %.2f\n", soma_receita);
    }
}

// =========================================================
// 9. PREENCHER BANCO COM DADOS DE TESTE
// =========================================================

void preencherDadosTeste(Animal rebanho[], int *total, int pastos[LINHAS_PASTO][COLUNAS_PASTO], RegistroCorte cortes[], int *total_cortes) {
    char conf;
    if (*total > 0) {
        printf("\n[AVISO] Isso substituira os dados atuais em memoria. Continuar? (S/N): ");
        scanf("%c", &conf);
        limparBuffer();
        if (toupper((unsigned char)conf) != 'S') {
            printf("Operacao cancelada.\n");
            return;
        }
    }

    inicializarPastos(pastos);
    *total = 0;
    *total_cortes = 0;

    // Piquete [2][2] bloqueado para descanso
    pastos[2][2] = -1;

    Animal exemplos[8] = {
        {101, "NELORE",   510.0f, 30, "ABATE",      0, 0, 1},
        {102, "NELORE",   420.5f, 22, "SAUDAVEL",   0, 0, 1},
        {103, "ANGUS",    545.0f, 26, "ABATE",      0, 1, 1},
        {104, "ANGUS",    380.0f, 18, "SAUDAVEL",   0, 1, 1},
        {105, "BRAHMAN",  495.0f, 28, "SAUDAVEL",   1, 0, 1},
        {106, "HEREFORD", 310.0f, 14, "QUARENTENA", 4, 4, 1},
        {107, "NELORE",   460.0f, 24, "SAUDAVEL",   1, 2, 1},
        {108, "BRAHMAN",  530.0f, 32, "ABATIDO",    0, 0, 0} // Ja abatido para o historico
    };

    int i;
    for (i = 0; i < 8; i++) {
        rebanho[i] = exemplos[i];
        if (exemplos[i].ativo == 1) {
            pastos[exemplos[i].piquete_lin][exemplos[i].piquete_col]++;
        }
    }
    *total = 8;

    // 1 registro de corte ja preenchido (referente ao brinco 108)
    RegistroCorte c1 = {108, "BRAHMAN", 530.0f, 4.5f, 48.0f, 52.0f, 145.5f, 250.0f, 47.17f, 8585.0f};
    cortes[0] = c1;
    *total_cortes = 1;

    printf("[OK] Banco populado com 7 animais ativos, 1 piquete bloqueado [2][2] e 1 abate registrado.\n");
}