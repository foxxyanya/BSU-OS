#include <iostream>
#include <fstream>
#include <chrono>
#include <pthread.h>

using namespace std;

// A = M(n*n), B = M(n*n)
int n;
int num;
int** A;
int** B;
int** C;
int* boarders;

pthread_mutex_t** n_n_mut;

// generates boarders of num blocks
void generate_block_boarders() {
    int* deltas = new int[num]{};
    int poz = 0;
    for (int i = 0; i < n; ++i) {
        ++deltas[poz];
        poz = (poz + 1) % num;
    }
    boarders = new int[num + 1]{};
    for (int i = 1; i < num + 1; ++i) {
        boarders[i] = boarders[i - 1] + deltas[i - 1];
    }
}

void read_matrix(int**& M, ifstream& in) {
    M = new int*[n];
    for (int i = 0; i < n; ++i) {
        M[i] = new int[n];
        for (int j = 0; j < n; ++j) {
            in >> M[i][j];
        }
    }
}

void full_with_zeros(int**& M) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            M[i][j] = 0;
        }
    }
}

// simple matrix multiplication
// 1 thread
void* mult1() {
    for (int i = 0; i < n; ++i) { // choose row in C
        for (int j = 0; j < n; ++j) { // choose column in C
            // counting C[i][j]
            int s = 0;
            for (int l = 0; l < n; ++l) {
                s += A[i][l] * B[l][j];
            }
            C[i][j] = s;
        }
    }
}

// C-block(i1-i2, j1-j2) = A-rows(i1-i2) * B-cols(j1-j2)
void* row_column_blocks_mult(void* args) {
    int* args_ = (int*)args;
    int i1 = args_[0], i2 = args_[1];
    int j1 = args_[2], j2 = args_[3];
    // choosing C block's element:
    for (int i = i1; i < i2; ++i) {
        for (int j = j1; j < j2; ++j) {
            // counting C[i][j]
            int s = 0;
            for (int z = 0; z < n; ++z) {
                s += A[i][z] * B[z][j];
            }
            C[i][j] = s;
        }
    }
}

// multithreading row-column blocks matrix multiplication
// num ^ 2 threads
// no need in mutexes
void* mult2_1() {
    pthread_t thr[num][num];
    for (int i = 0; i < num; ++i) {
        for (int j = 0; j < num; ++j) {
            int args[4] = {boarders[i], boarders[i + 1], boarders[j], boarders[j + 1]};
            pthread_create(&thr[i][j], nullptr, &row_column_blocks_mult, args);
        }
    }
    for (int i = 0; i < num; ++i) {
        for (int j = 0; j < num; ++j) {
            pthread_join(thr[i][j], nullptr);
        }
    }
}

void* column_row_blocks_mult(void* args) {
    int* ind = (int*)args;
    int i1 = ind[0];
    int i2 = ind[1];
    // for all (i,j) C[i][j] += ...
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            // adding to it sum formed from A[i][i1--i2] and B[i1--i2][j]
            int s = 0;
            for (int k = i1; k < i2; ++k) {
                s += A[i][k] * B[k][j];
            }
            pthread_mutex_lock(&n_n_mut[i][j]);
            C[i][j] += s;
            pthread_mutex_unlock(&n_n_mut[i][j]);
        }
    }
}

// multithreading column-row blocks matrix multiplication
// num threads
void* mult2_2() {
    pthread_t thr[num];
    for (int i = 0; i < num; ++i) {
        int args[2] = {boarders[i], boarders[i + 1]};
        pthread_create(&thr[i], nullptr, &column_row_blocks_mult, args);
    }
    for (int i = 0; i < num; ++i) {
        pthread_join(thr[i], nullptr);
    }
}

// adds values to C[boarders[i_a]--boarders[i_a + 1]][boarders[j_b]--boarders[j_b + 1]]
void* form(void* args) {
    int* args_ = (int*)args;
    int i_a = args_[0], j_a = args_[1];
    int i_b = args_[2], j_b = args_[3];
    // choosing C[i][j] from block
    for (int i = boarders[i_a]; i < boarders[i_a + 1]; ++i) {
        for (int j = boarders[j_b]; j < boarders[j_b + 1]; ++j) {
            // counting C[i][j]
            int s = 0;
            for (int delta = 0; delta < boarders[j_a + 1] - boarders[j_a]; ++delta) {
                s += A[i][boarders[j_a] + delta] * B[boarders[i_b] + delta][j];
            }
            pthread_mutex_lock(&n_n_mut[i][j]);
            C[i][j] += s;
            pthread_mutex_unlock(&n_n_mut[i][j]);
        }
    }
}

// multithreading blocks matrix multiplication
// num ^ 3 threads
void* mult2_3() {
    pthread_t thr[num][num][num];
    // choosing block in A
    for (int j = 0; j < num; ++j) {
        for (int i = 0; i < num; ++i) {
            // choosing for it block in B
            for (int z = 0; z < num; ++z) {
                int args[4] = {i, j, j, z};
                pthread_create(&thr[i][j][z], nullptr, &form, args);
            }
        }
    }
    for (int j = 0; j < num; ++j) {
        for (int i = 0; i < num; ++i) {
            for (int z = 0; z < num; ++z) {
                pthread_join(thr[i][j][z], nullptr);
            }
        }
    }
}

void run_and_print_time(void* mult()) {
    auto begin = std::chrono::steady_clock::now();
    mult();
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    cout << elapsed_ms.count() << "\n";
}

void generate_data(const string& filename, int n_, int num_) {
    ofstream out(filename);
    out << n_ << " " << num_ << "\n";
    for (int z = 0; z < 2; ++z) {
        for (int i = 0; i < n_; ++i) {
            for (int j = 0; j < n_; ++j) {
                out << rand() % 100 << " ";
            }
            out << "\n";
        }
    }
    out.close();
}

void initialize_mutexes() {
    n_n_mut = new pthread_mutex_t *[n];
    for (int i = 0; i < n; ++i) {
        n_n_mut[i] = new pthread_mutex_t[n];
        for (int j = 0; j < n; ++j) {
            pthread_mutex_init(&n_n_mut[i][j], nullptr);
        }
    }
}

void read_data(const string& filename) {
    ifstream in(filename);
    in >> n >> num;
    read_matrix(A, in);
    read_matrix(B, in);
    in.close();
}

int main() {

    generate_data("input.txt", 100, 5);

    read_data("input.txt");

    generate_block_boarders();
    initialize_mutexes();
    C = new int *[n];
    for (int i = 0; i < n; ++i) {
        C[i] = new int[n]{};
    }

    cout << "Simple matrix multiplication (1 thread):\n";
    run_and_print_time(mult1);
    full_with_zeros(C);

    cout << "Multithreading column-row blocks matrix multiplication (num threads):\n";
    run_and_print_time(mult2_2);
    full_with_zeros(C);

    cout << "Multithreading row-column blocks matrix multiplication (num^2 threads, no need in mutexes):\n";
    run_and_print_time(mult2_1);
    full_with_zeros(C);

    cout << "Multithreading blocks matrix multiplication (num^3 threads, too many mutexes):\n";
    run_and_print_time(mult2_3);

}