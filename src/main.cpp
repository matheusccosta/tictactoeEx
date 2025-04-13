#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <cstdlib>
#include <ctime>

// Classe TicTacToe
class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
    std::mutex board_mutex; // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv; // Variável de condição para alternância de turnos
    char current_player; // Jogador atual ('X' ou 'O')
    bool game_over; // Estado do jogo
    char winner; // Vencedor do jogo

public:
    TicTacToe() {

        // Inicializar o tabuleiro e as variáveis do jogo
        current_player = 'X'; // definicao que escolhi, com x comecando, poderia ser O ou um sorteio, tanto faz
        winner = ' '; // sem vencedor ainda
        game_over = 0; // jogo n acabou

        for (auto& row : board) {
            row.fill(' '); // define o labirinto como vazio a principio
        }
       
    }

    void display_board() {
        // Exibir o tabuleiro no console
        std::cout<<"\n\n\n\n\n\n\n\n"; //para separar o tabuleiro atual do anterior no print
        for (int i = 0; i < 3; i++) {          
            for (int j = 0; j < 3; j++) {   
                std::cout << " " << board[i][j] << " ";
                if (j < 2) {
                    std::cout << "|";  // separa as infos, igual ao gif
                }
            }
            std::cout << std::endl;  // pula linha
            
            if (i < 2) {
                std::cout << "-----------" << std::endl;  // separa linha
            }
        }   
                char info;
                if (current_player=='X'){
                    info='O';
                }else{
                    info='X';
                        }
                std::cout<<"Current player: " << info << std::endl;
    }


    bool make_move(char player, int row, int col) {
        std::unique_lock<std::mutex> lock(board_mutex);
    
        // Espera até ser a vez do jogador ou jogo acabar
        turn_cv.wait(lock, [this, player]() {
            return current_player == player || game_over;
        });
    
        if (game_over || row < 0 || row > 2 || col < 0 || col > 2 || board[row][col] != ' ') {
            return false;
        }
    
        // Faz a jogada
        board[row][col] = player;
        display_board();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // Verifica se o jogo terminou
        if (check_win(player)) {
            game_over = true;
            winner = player;
            turn_cv.notify_all();
            return true;
        }
    
        if (check_draw()) {
            game_over = true;
            winner = 'D';
            turn_cv.notify_all();
            return true;
        }
    
        // Alterna o jogador
        if (current_player=='X'){
            current_player='O';
        }else{
            current_player='X';
                }
        turn_cv.notify_all();
        return true;
    }


    bool check_win(char player) {
        // Verifica linhas
        for (int i = 0; i < 3; i++) {
            if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
                return true;
        }
        // Verifica colunas
        for (int j = 0; j < 3; j++) {
            if (board[0][j] == player && board[1][j] == player && board[2][j] == player)
                return true;
        }
        // Verifica diagonais
        if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
            (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
            return true;
        }
        return false;
    }

    bool check_draw() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == ' ') {
                    return false;
                }
            }
        }
        return !check_win('X') && !check_win('O');
    }

    bool is_game_over() {
        // Retornar se o jogo terminou
        return game_over;
    }

    char get_winner() {
        // Retornar o vencedor do jogo ('X', 'O', ou 'D' para empate)
        if(check_draw())
            return 'D';

        if(check_win('X'))
            return 'X';

        if(check_win('O'))
            return 'O';
    
        return '0';
    }

    char get_current_player() const {
        return current_player;
    }
};

// Classe Player
class Player {
private:
    TicTacToe& game; // Referência para o jogo
    char symbol; // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador

public:
    Player(TicTacToe& g, char s, std::string strat) 
        : game(g), symbol(s), strategy(strat) {}

        void play() {
            while (!game.is_game_over()) {
                if (game.get_current_player() == symbol) {
                    if (strategy == "sequential") {
                        play_sequential();
                    } else if (strategy == "random") {
                        play_random();
                    }
                }

            }
            
        }

private:
void play_sequential() {
    for (int linha = 0; linha < 3; linha++) {          // Linhas primeiro
        for (int coluna = 0; coluna < 3; coluna++) {   // Depois colunas
            if (game.make_move(symbol, linha, coluna)) {
                return;  // Sai após jogar
            }
        }
    }
}

    void play_random() {
        // Implementar a estratégia aleatória de jogadas
        int coordx=0;
        int coordy=0;
        std::srand(std::time(0));
        do{
        coordx = std::rand()%3;
        coordy = std::rand()%3;
        }
        while (!game.make_move(symbol, coordx, coordy));

    }
};

// Função principal
int main() {
    // Inicializar o jogo e os jogadores
    TicTacToe match;
    //match.display_board();

    // Criar as threads para os jogadores
    Player player1(match, 'X', "sequential");
    Player player2(match, 'O', "random");
    std::thread thread1(&Player::play, &player1);
    std::thread thread2(&Player::play, &player2);


    // Aguardar o término das threads
    thread1.join();
    thread2.join();

    // Exibir o resultado final do jogo
    char win = match.get_winner();
    if(win !='D'){
    std::cout << "Player "<< win << " wins"<<std::endl;
    }else{
    std::cout << "Draw!"<<std::endl;   
    }
    return 0;
}
