#include <iostream>
#include <unistd.h>
#include <wait.h>

using namespace std;
typedef int (*func_ptr)();

int n = 5;
// fds[i][1] - writing, fds[i][0] - reading
int fds[5][2];

void close_unused_fds(int ind) {
    for (int i = 0; i < 5; ++i) {
        if (i == ind) {
            close(fds[i][1]);
        }else if (i == ind + 1) {
            close(fds[i][0]);
        } else {
            close(fds[i][0]);
            close(fds[i][1]);
        }
    }
}

void close_start_unused_fds() {
    close(fds[0][0]);
    for (int i = 1; i < 4; ++i) {
        close(fds[i][0]);
        close(fds[i][1]);
    }
    close(fds[4][1]);
}

void close_used_fds(int ind) {
    close(fds[ind][0]);
    close(fds[ind + 1][1]);
}

// reading array from fds[0][0], writing array to fds[1][1]
int M() {
    close_unused_fds(0);
    int nums[n];
    if (read(fds[0][0], &nums, n * sizeof (int)) < 0) {
        return 2;
    }
    for (int& num : nums) {
        num *= 7;
    }
    if (write(fds[1][1], &nums, n * sizeof(int)) < 0) {
        return 3;
    }
    close_used_fds(0);
    return 0;
}

// reading array from fds[1][0], writing array to fds[2][1]
int A() {
    close_unused_fds(1);
    int nums[n];
    if (read(fds[1][0], &nums, n * sizeof (int)) < 0) {
        return 4;
    }
    for (int& num : nums) {
        num += 14;
    }
    if (write(fds[2][1], &nums, n * sizeof (int)) < 0) {
        return 5;
    }
    close_used_fds(1);
    return 0;
}

// reading array from fds[2][0], writing array to fds[3][1]
int P() {
    close_unused_fds(2);
    int nums[n];
    if (read(fds[2][0], &nums, n * sizeof (int)) < 0) {
        return 6;
    }
    for (int& num : nums) {
        num *= num * num;
    }
    if (write(fds[3][1], &nums, n * sizeof (int)) < 0) {
        return 7;
    }
    close_used_fds(2);
    return 0;
}

// reading array from fds[3][0], writing number to fds[4][1]
int S() {
    close_unused_fds(3);
    int nums[n];
    int sum = 0;
    if (read(fds[3][0], &nums, n * sizeof(int)) < 0) {
        return 8;
    }
    for (int& num : nums) {
        sum += num;
    }
    if (write(fds[4][1], &sum, sizeof(int)) < 0) {
        return 9;
    }
    close_used_fds(3);
    return 0;
}

int main() {

    for (int i = 0; i < 5; ++i) {
        if(pipe(fds[i]) == -1) {
            return 1;
        }
    }

    int ids[4];
    func_ptr ptrs[4]{M, A, P, S};
    for (int i = 0; i < 4; ++i) {
        ids[i] = fork();
        if (ids[i] == 0) {
            return (*ptrs[i])();
        }
    }

    close_start_unused_fds();

    int nums[n];
    cout << "Enter " << n << " numbers:\n";
    for (int i = 0; i < n; ++i) {
        cin >> nums[i];
    }
    if (write(fds[0][1], &nums, n * sizeof(int)) < 0) {
        return 10;
    }

    int rez;
    if (read(fds[4][0], &rez, sizeof(int)) < 0) {
        return 11;
    }
    cout << rez;

    close(fds[0][1]);
    close(fds[4][0]);

    for (int i = 0; i < 4; ++i) {
        waitpid(ids[i], nullptr, 0);
    }

}