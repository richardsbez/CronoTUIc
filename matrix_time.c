#include <ctype.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define FRAME_DELAY 50000 // Aumentado para 0.05s para maior fluidez
#define MIN_TAIL 5
#define MAX_TAIL 15
#define STATUS_HEIGHT 3

typedef struct {
  int x;
  int y;
  int speed;
  int tail_len;
  int counter;
} RainColumn;

// Configuração global para facilitar acesso em funções de desenho
int max_y = 0, max_x = 0;

void inicializar_ncurses(void) {
  initscr();
  noecho();
  curs_set(FALSE);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE); // Não bloqueante por padrão
  start_color();
  use_default_colors();

  // Cores fiéis ao Matrix
  init_pair(1, COLOR_GREEN, -1); // Cauda
  init_pair(2, COLOR_WHITE, -1); // Cabeça brilhante
  init_pair(3, COLOR_CYAN, -1);  // UI
  init_pair(4, COLOR_BLACK, -1); // Fundo (opcional)
}

void reiniciar_coluna(RainColumn *col, int x_pos) {
  col->x = x_pos;
  col->y = -(rand() % 20); // Começa acima da tela
  col->speed = 1 + (rand() % 2);
  col->tail_len = MIN_TAIL + rand() % (MAX_TAIL - MIN_TAIL + 1);
  col->counter = 0;
}

char char_aleatorio() {
  // Retorna caracteres que lembram o Matrix
  const char chars[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$+-*/=%\"#&_(),.;:?!\\|{}<>[]";
  return chars[rand() % (sizeof(chars) - 1)];
}

int ler_tempo_segundos() {
  int h, m, s;
  printf("Configurar Timer (HH:MM:SS): ");
  if (scanf("%d:%d:%d", &h, &m, &s) != 3) {
    printf("Erro: Formato inválido.\n");
    return -1;
  }
  return (h * 3600) + (m * 60) + s;
}

void desenhar_ui(int segundos_restantes, int total) {
  char buffer[32];
  int h = segundos_restantes / 3600;
  int m = (segundos_restantes % 3600) / 60;
  int s = segundos_restantes % 60;

  snprintf(buffer, sizeof(buffer), " %02d:%02d:%02d ", h, m, s);

  // Barra de progresso
  attron(COLOR_PAIR(1) | A_BOLD);
  mvprintw(0, (max_x - (int)strlen(buffer)) / 2, "%s", buffer);

  float progresso = (float)segundos_restantes / total;
  int largura_barra = max_x - 10;
  int preenchimento = (int)(largura_barra * (1.0 - progresso));

  mvprintw(max_y - 1, 5, "[");
  for (int i = 0; i < largura_barra; i++) {
    addch(i < preenchimento ? '#' : '.');
  }
  addch(']');
  attroff(COLOR_PAIR(1) | A_BOLD);
}

int main() {
  int segundos_totais = ler_tempo_segundos();
  if (segundos_totais <= 0)
    return 1;

  inicializar_ncurses();
  srand(time(NULL));

  getmaxyx(stdscr, max_y, max_x);

  // Aloca colunas dinamicamente baseado na largura da tela (espaçamento de 2)
  int num_colunas = max_x / 2;
  RainColumn *chuva = malloc(sizeof(RainColumn) * num_colunas);

  for (int i = 0; i < num_colunas; i++) {
    reiniciar_coluna(&chuva[i], i * 2);
    chuva[i].y = rand() % max_y; // Espalhar inicialmente
  }

  time_t start_time = time(NULL);
  int segundos_decorridos = 0;

  while (segundos_decorridos < segundos_totais) {
    time_t current_time = time(NULL);
    segundos_decorridos = (int)difftime(current_time, start_time);
    int segundos_restantes = segundos_totais - segundos_decorridos;

    // Detectar redimensionamento
    int new_y, new_x;
    getmaxyx(stdscr, new_y, new_x);
    if (new_y != max_y || new_x != max_x) {
      max_y = new_y;
      max_x = new_x;
      erase(); // Limpa rastro ao redimensionar
    }

    erase(); // Limpa o buffer de desenho

    // Atualizar e desenhar chuva
    for (int i = 0; i < num_colunas; i++) {
      chuva[i].counter++;
      if (chuva[i].counter >= chuva[i].speed) {
        chuva[i].y++;
        chuva[i].counter = 0;
      }

      if (chuva[i].y - chuva[i].tail_len > max_y) {
        reiniciar_coluna(&chuva[i], i * 2);
      }

      for (int j = 0; j < chuva[i].tail_len; j++) {
        int draw_y = chuva[i].y - j;
        if (draw_y >= 1 && draw_y < max_y - 1) {
          if (j == 0) {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvaddch(draw_y, chuva[i].x, char_aleatorio());
            attroff(COLOR_PAIR(2) | A_BOLD);
          } else {
            // Efeito de desvanecimento de cor (opcional: usar tons de verde se
            // suportado)
            attron(COLOR_PAIR(1));
            mvaddch(draw_y, chuva[i].x, char_aleatorio());
            attroff(COLOR_PAIR(1));
          }
        }
      }
    }

    desenhar_ui(segundos_restantes, segundos_totais);

    // Input do usuário
    int ch = getch();
    if (ch == 'q' || ch == 'Q')
      break;

    refresh();
    usleep(FRAME_DELAY);
  }

  // Mensagem Final
  erase();
  attron(COLOR_PAIR(2) | A_BOLD | A_BLINK);
  mvprintw(max_y / 2, (max_x - 16) / 2, "TEMPO ESGOTADO!");
  refresh();
  nodelay(stdscr, FALSE); // Bloqueia para esperar tecla final
  getch();

  free(chuva);
  endwin();
  return 0;
}
