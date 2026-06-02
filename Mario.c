/* Nomes:
-Afonso Guimarães 
-Pedro Henrique Toniolo
-Fabricio Milano
-Lucas Busch
*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
 
#define MAX_NOME  50
#define MAX_FASES 100

typedef enum {
    FASE_NORMAL = 0,
    FASE_FORTALEZA = 1,
    FASE_CASTELO = 2,
    FASE_SECRETA = 3,
    FASE_BONUS = 4
} TipoFase;
 
typedef enum {
    STATUS_NAO_CONCLUIDA = 0,
    STATUS_CONCLUIDA  = 1
} StatusFase;
 
typedef struct No {
    int id;
    char nome[MAX_NOME];
    TipoFase tipo;
    StatusFase status;
 
    int pesoEntrada;
    int pesoRecompensa;
 
    int idEsquerda;
    int pesoEsquerda;
 
    int idDireita;
    int pesoDireita;
 
    int idVolta;
    int pesoVolta;
} No;
 
typedef struct Player {
    int id;
    char nome[MAX_NOME];
 
    int pesoAtual;
    int faseAtualId;
    int vidas;
    bool gameOver;
 
    int historico[MAX_FASES];
    int tamanhoHistorico;
} Player;
 
typedef struct {
    No fases[MAX_FASES];
    int totalFases;
} Grafo;
 
No *buscarFasePorId(Grafo *g, int id) {
    for (int i = 0; i < g->totalFases; i++) {
        if (g->fases[i].id == id) return &g->fases[i];
    }
    return NULL;
}

int buscarIndicePorId(const Grafo *g, int id) {
    for (int i = 0; i < g->totalFases; i++) {
        if (g->fases[i].id == id) return i;
    }
    return -1;
}
 
const char *nomeTipo(TipoFase t) {
    switch (t) {
        case FASE_NORMAL: return "Normal";
        case FASE_FORTALEZA: return "Fortaleza";
        case FASE_CASTELO: return "Castelo";
        case FASE_SECRETA: return "Secreta";
        case FASE_BONUS: return "Bonus";
        default: return "?";
    }
}

void exibirStatus(const Player *p, const Grafo *g) {
    const No *fase = &g->fases[p->faseAtualId];
 
    printf("\n╔══════════════════════════════════════╗\n");
    printf("  Fase atual : %s\n", fase->nome);
    printf("  Tipo       : %s\n", nomeTipo(fase->tipo));
    printf("  Peso atual : %d\n", p->pesoAtual);
    printf("  Vidas      : %d\n", p->vidas);
    printf("╠══════════════════════════════════════╣\n");
 
    if (fase->idEsquerda != -1) {
        const No *vizinho = buscarFasePorId((Grafo *)g, fase->idEsquerda);
        if (vizinho) {
            if (p->pesoAtual >= fase->pesoEsquerda)
                printf("  [E] Esquerda -> %s\n", vizinho->nome);
            else
                printf("  [E] Esquerda -> %s  (bloqueada, precisa peso %d)\n", vizinho->nome, fase->pesoEsquerda);
        }
    }
    if (fase->idDireita != -1) {
        const No *vizinho = buscarFasePorId((Grafo *)g, fase->idDireita);
        if (vizinho) {
            if (p->pesoAtual >= fase->pesoDireita)
                printf("  [D] Direita  -> %s\n", vizinho->nome);
            else
                printf("  [D] Direita  -> %s  (bloqueada, precisa peso %d)\n", vizinho->nome, fase->pesoDireita);
        }
    }
    if (fase->idVolta != -1) {
        const No *vizinho = buscarFasePorId((Grafo *)g, fase->idVolta);
        if (vizinho) {
            if (p->pesoAtual >= fase->pesoVolta)
                printf("  [V] Voltar   -> %s\n", vizinho->nome);
            else
                printf("  [V] Voltar   -> %s  (bloqueada, precisa peso %d)\n", vizinho->nome, fase->pesoVolta);
        }
    }
 
    printf("  [J] Jogar fase atual\n");
    printf("  [M] Ver matriz de adjacencia\n");
    printf("  [N] Nova fase\n");
    printf("  [C] Nova conexao entre fases\n");
    printf("  [S] Sair\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("Escolha: ");
}

void jogarFase(Player *p, Grafo *g) {
    No *fase = &g->fases[p->faseAtualId];
 
    if (fase->status == STATUS_CONCLUIDA) {
        printf("  Voce ja completou esta fase!\n");
        return;
    }
 
    printf("\n  >> Jogando '%s'...\n", fase->nome);
 
    if (p->pesoAtual >= fase->pesoEntrada) {
        fase->status = STATUS_CONCLUIDA;
        p->pesoAtual += fase->pesoRecompensa;
        printf("  >> Fase concluida! +%d de peso. Peso total: %d\n", fase->pesoRecompensa, p->pesoAtual);
 
        if (p->tamanhoHistorico < MAX_FASES)
            p->historico[p->tamanhoHistorico++] = fase->id;
    } else {
        p->vidas--;
        printf("  >> Peso insuficiente (precisa %d, tem %d). Vidas restantes: %d\n", fase->pesoEntrada, p->pesoAtual, p->vidas);
        if (p->vidas <= 0) {
            p->gameOver = true;
            printf("  >> GAME OVER!\n");
        }
    }
}

void mover(Player *p, const Grafo *g, char direcao) {
    const No *fase = &g->fases[p->faseAtualId];
    int idProx = -1;
    int pesoMin = 0;
 
    if (direcao == 'E') { 
        idProx = fase->idEsquerda; 
        pesoMin = fase->pesoEsquerda; 
    }
    else if (direcao == 'D') { 
        idProx = fase->idDireita;  
        pesoMin = fase->pesoDireita;  
    }
    else if (direcao == 'V') { 
        idProx = fase->idVolta;    
        pesoMin = fase->pesoVolta;    
    }
 
    if (idProx == -1) {
        printf("  Sem caminho nessa direcao.\n");
        return;
    }
 
    const No *destino = buscarFasePorId((Grafo *)g, idProx);
    if (!destino) {
        printf("  Fase de destino invalida.\n");
        return;
    }
 
    if (p->pesoAtual < pesoMin) {
        printf("  Caminho bloqueado! Precisa de peso %d (voce tem %d).\n", pesoMin, p->pesoAtual);
        return;
    }
 
    for (int i = 0; i < g->totalFases; i++) {
        if (g->fases[i].id == idProx) {
            p->faseAtualId = i;
            break;
        }
    }
    printf("  Movendo para '%s'.\n", destino->nome);
}

void exibirMatrizAdjacencia(const Grafo *g) {
    int n = g->totalFases;
 
    int mat[MAX_FASES][MAX_FASES];
    memset(mat, 0, sizeof(mat));
 
    for (int i = 0; i < n; i++) {
        const No *f = &g->fases[i];

        int jE = buscarIndicePorId(g, f->idEsquerda);
        int jD = buscarIndicePorId(g, f->idDireita);
        int jV = buscarIndicePorId(g, f->idVolta);

        if (f->idEsquerda != -1 && jE != -1) mat[i][jE] = 1;
        if (f->idDireita != -1 && jD != -1) mat[i][jD] = 1;
        if (f->idVolta != -1 && jV != -1) mat[i][jV] = 1;
    }
 
    printf("\n╔══ Matriz de Adjacencia (1 = com conexao; . = sem conexao) ══╗\n");
    printf("     ");
    for (int j = 0; j < n; j++)
        printf(" %3d", g->fases[j].id);
    printf("\n     ");
    for (int j = 0; j < n; j++)
        printf("----");
    printf("\n");
 
    for (int i = 0; i < n; i++) {
        printf(" %3d|", g->fases[i].id);
        for (int j = 0; j < n; j++) {
            if (mat[i][j]) printf(" %3d", mat[i][j]);
            else printf("   .");
        }
        printf("  | %s\n", g->fases[i].nome);
    }
 
    printf("     ");
    for (int j = 0; j < n; j++)
        printf("----");
    printf("\n");
 
    printf("  IDs: ");
    for (int i = 0; i < n; i++)
        printf("[%d=%s] ", g->fases[i].id, g->fases[i].nome);
    printf("\n╚══════════════════════════════════════════════════════════════╝\n");
}

void inserirFase(Grafo *g) {
    if (g->totalFases >= MAX_FASES) {
        printf("  Limite de fases atingido (%d).\n", MAX_FASES);
        return;
    }
 
    No nova;
    memset(&nova, 0, sizeof(nova));
 
    int maxId = -1;
    for (int i = 0; i < g->totalFases; i++)
        if (g->fases[i].id > maxId)
            maxId = g->fases[i].id;
    nova.id = maxId + 1;
 
    printf("\n  === Nova Fase (ID gerado: %d) ===\n", nova.id);
 
    printf("  Nome: ");
    scanf(" %49[^\n]", nova.nome);
 
    printf("  Tipo (0=Normal 1=Fortaleza 2=Castelo 3=Secreta 4=Bonus): ");
    int t; scanf("%d", &t);
    nova.tipo = (TipoFase)(t < 0 || t > 4 ? 0 : t);
 
    printf("  Peso de entrada necessario: ");
    scanf("%d", &nova.pesoEntrada);
 
    printf("  Peso de recompensa ao concluir: ");
    scanf("%d", &nova.pesoRecompensa);
 
    nova.idEsquerda = -1; nova.pesoEsquerda = 0;
    nova.idDireita = -1; nova.pesoDireita = 0;
    nova.idVolta = -1; nova.pesoVolta = 0;
    nova.status = STATUS_NAO_CONCLUIDA;
 
    g->fases[g->totalFases++] = nova;
    printf("  Fase '%s' (ID %d) criada com sucesso!\n", nova.nome, nova.id);
    printf("  Use [C] para conectar esta fase a outras.\n");
}

void inserirConexao(Grafo *g) {
    printf("\n  === Nova Conexao ===\n");
    printf("  Fases existentes:\n");
    for (int i = 0; i < g->totalFases; i++)
        printf("    ID %d : %s\n", g->fases[i].id, g->fases[i].nome);
 
    int idOrigem;
    printf("  ID da fase de ORIGEM: ");
    scanf("%d", &idOrigem);
 
    No *origem = buscarFasePorId(g, idOrigem);
    if (!origem) {
        printf("  Fase de origem nao encontrada.\n");
        return;
    }
 
    int idDestino;
    printf("  ID da fase de DESTINO: ");
    scanf("%d", &idDestino);
 
    No *destino = buscarFasePorId(g, idDestino);
    if (!destino) {
        printf("  Fase de destino nao encontrada.\n");
        return;
    }
 
    if (idOrigem == idDestino) {
        printf("  Origem e destino nao podem ser iguais.\n");
        return;
    }
 
    printf("  Direcao a partir de '%s'\n", origem->nome);
    printf("  (E=Esquerda / D=Direita / V=Volta): ");
    char dir[4];
    scanf("%3s", dir);
    char d = dir[0];
 
    if (d != 'E' && d != 'e' && d != 'D' && d != 'd' && d != 'V' && d != 'v') {
        printf("  Direcao invalida.\n");
        return;
    }
    d = (char)(d >= 'a' ? d - 32 : d);
 
    int peso;
    printf("  Peso minimo para percorrer este caminho (0 = livre): ");
    scanf("%d", &peso);
    if (peso < 0) peso = 0;
 
    if (d == 'E') { 
        origem->idEsquerda = idDestino;
        origem->pesoEsquerda = peso; 
    }
    if (d == 'D') { 
        origem->idDireita = idDestino; 
        origem->pesoDireita = peso; 
    }
    if (d == 'V') { 
        origem->idVolta = idDestino; 
        origem->pesoVolta = peso; 
    }
 
    printf("  Conexao criada: %s --%c(peso %d)--> %s\n",
           origem->nome, d, peso, destino->nome);
 
    printf("  Criar aresta reversa automatica? (s/n): ");
    char resp[4];
    scanf("%3s", resp);
    if (resp[0] == 's' || resp[0] == 'S') {
        char rev = (d == 'E') ? 'D' : (d == 'D') ? 'E' : 'V';
        int pesoRev;
        printf("  Peso minimo da aresta reversa (0 = livre): ");
        scanf("%d", &pesoRev);
        if (pesoRev < 0) pesoRev = 0;
 
        if (rev == 'E') { 
            destino->idEsquerda = idOrigem; 
            destino->pesoEsquerda = pesoRev; 
        }
        if (rev == 'D') { 
            destino->idDireita = idOrigem; 
            destino->pesoDireita = pesoRev; 
        }
        if (rev == 'V') { 
            destino->idVolta = idOrigem; 
            destino->pesoVolta = pesoRev;
        }
 
        printf("  Aresta reversa criada: %s --%c(peso %d)--> %s\n", destino->nome, rev, pesoRev, origem->nome);
    }
}

void montarMapa(Grafo *g) {
    g->totalFases = 4;
 
    g->fases[0] = (No){
        .id = 0, .nome = "Gramado_Inicio",
        .tipo = FASE_NORMAL, .status = STATUS_NAO_CONCLUIDA,
        .pesoEntrada = 0, .pesoRecompensa = 1,
        .idEsquerda = -1, .pesoEsquerda = 0,
        .idDireita  =  1, .pesoDireita  = 1,
        .idVolta    = -1, .pesoVolta    = 0
    };
 
    g->fases[1] = (No){
        .id = 1, .nome = "Gramado_Pontes",
        .tipo = FASE_NORMAL, .status = STATUS_NAO_CONCLUIDA,
        .pesoEntrada = 1, .pesoRecompensa = 2,
        .idEsquerda =  3, .pesoEsquerda = 2,
        .idDireita  =  2, .pesoDireita  = 3,
        .idVolta    =  0, .pesoVolta    = 0
    };
 
    g->fases[2] = (No){
        .id = 2, .nome = "Gramado_Bosque",
        .tipo = FASE_NORMAL, .status = STATUS_NAO_CONCLUIDA,
        .pesoEntrada = 3, .pesoRecompensa = 2,
        .idEsquerda = -1, .pesoEsquerda = 0,
        .idDireita  = -1, .pesoDireita  = 0,
        .idVolta    =  1, .pesoVolta    = 0
    };
 
    g->fases[3] = (No){
        .id = 3, .nome = "Gramado_Torre",
        .tipo = FASE_FORTALEZA, .status = STATUS_NAO_CONCLUIDA,
        .pesoEntrada = 2, .pesoRecompensa = 3,
        .idEsquerda = -1, .pesoEsquerda = 0,
        .idDireita  = -1, .pesoDireita  = 0,
        .idVolta    =  1, .pesoVolta    = 0
    };
}

int main() {
    Grafo  mapa;
    Player player = {
        .id = 1, .nome = "Mario",
        .pesoAtual = 0, .faseAtualId = 0,
        .vidas = 3, .gameOver = false,
        .tamanhoHistorico = 0
    };

    printf("Seja bem vindo ao sistema de selecao de fases estilo Super Mario!\n");
    printf("Deseja dar um nome para seu personagem? (s: sim / n: nao[nome padrao = Mario])\n");
    char escolha[4];
    scanf("%3s", escolha);
    if(escolha[0] == 's' || escolha[0] == 'S'){
        char nome[MAX_NOME];
        printf("\nDigite o nome do seu personagem: ");
        scanf(" %49[^\n]", nome);
        strcpy(player.nome, nome);
    }
    
    montarMapa(&mapa);
 
    printf("\n=== Sistema de Fases ===\n");
    printf("Bem-vindo, %s!\n", player.nome);
 
    char entrada[8];
 
    while (!player.gameOver) {
        exibirStatus(&player, &mapa);
        scanf("%7s", entrada);
        char op = entrada[0];
 
        if (op == 'E' || op == 'e') mover(&player, &mapa, 'E');
        else if (op == 'D' || op == 'd') mover(&player, &mapa, 'D');
        else if (op == 'V' || op == 'v') mover(&player, &mapa, 'V');
        else if (op == 'J' || op == 'j') jogarFase(&player, &mapa);
        else if (op == 'M' || op == 'm') exibirMatrizAdjacencia(&mapa);
        else if (op == 'N' || op == 'n') inserirFase(&mapa);
        else if (op == 'C' || op == 'c') inserirConexao(&mapa);
        else if (op == 'S' || op == 's') break;
        else printf("  Opcao invalida.\n");
    }
 
    printf("\n=== Fim de jogo ===\n");
    printf("Peso final : %d\n", player.pesoAtual);
    printf("Caminho    : ");
    for (int i = 0; i < player.tamanhoHistorico; i++){
        int indiceReal = buscarIndicePorId(&mapa, player.historico[i]);
        if (indiceReal != -1) {
            printf("[%s] ", mapa.fases[indiceReal].nome);
        } else {
            printf("[Fase Desconhecida] ");
        }
    }
    printf("\n");
}
