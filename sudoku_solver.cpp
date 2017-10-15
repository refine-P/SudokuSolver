#include <cstdio>
#include <array>
#include <tuple>
#include <vector>
#include <algorithm>
#include <cassert>

using std::array;

const int BLOCK_SIZE = 3;
const int BOARD_SIZE = BLOCK_SIZE * BLOCK_SIZE;

using Matrix = array< array<int, BOARD_SIZE>, BOARD_SIZE >;

/*
	参考
	http://algorithm.main.jp/Puzzle/Sudoku/1-HowToSolve.php
	https://www.slideshare.net/atmarksharp/ss-45348313
	http://excel-ubara.com/excelvba5/EXCELVBA231_2.html
*/
class SudokuSolver {
public:
	bool input() {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				scanf("%d", &board[i][j]);
			}
		}
		return isWrongBoard();	
	}

	void print() {
		for (int i = 0; i < BOARD_SIZE; i++) {
			printf("%d", board[i][0]);
			for (int j = 1; j < BOARD_SIZE; j++) {
				printf(" %d", board[i][j]);
			}
			printf("\n");
		}	
	}

	void solve() {
		int completed_counter = 0;
		Matrix candidate;
		init(candidate, completed_counter);
		assert(dfs(board, candidate, completed_counter, board));
	}
private:
	Matrix board;
	bool isWrongBoard() {
		//縦の判定
		for (int j = 0; j < BOARD_SIZE; j++) {
			array<bool, BOARD_SIZE + 1> used{};
			for (int i = 0; i < BOARD_SIZE; i++) {
				if (!board[i][j]) continue;
				if (used[board[i][j]]) return true;
				used[board[i][j]] = true;
			}
		}
	
		//横の判定
		for (int i = 0; i < BOARD_SIZE; i++) {
			array<bool, BOARD_SIZE + 1> used{};
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (!board[i][j]) continue;
				if (used[board[i][j]]) return true;
				used[board[i][j]] = true;
			}
		}
	
		//3*3マスの判定
		for (int i = 0; i < BLOCK_SIZE; i++) {
			for (int j = 0; j < BLOCK_SIZE; j++) {
				array<bool, BOARD_SIZE + 1> used{};
				for (int dh = 0; dh < BLOCK_SIZE; dh++) {
					for (int dw = 0; dw < BLOCK_SIZE; dw++) {
						int h = i * BLOCK_SIZE + dh, w = j * BLOCK_SIZE + dw;
						if (!board[h][w]) continue;
						if (used[board[h][w]]) return true;
						used[board[h][w]] = true;
					}
				}
			}
		}
	
		return false;
	}

	void fillNumber(Matrix& current_board, Matrix& candidate, int h, int w, int val) {
		current_board[h][w] = val;
	
		//全候補をfalseに
		candidate[h][w] = 0;
	
		int mask = ~(1 << (val - 1)); //valを候補から外す
	
		//縦に関する処理
		for (int i = 0; i < BOARD_SIZE; i++) {
			candidate[i][w] &= mask;
		}
	
		//横に関する処理
		for (int j = 0; j < BOARD_SIZE; j++) {
			candidate[h][j] &= mask;
		}
	
		//3*3マスに関する処理
		int bh = h / BLOCK_SIZE * BLOCK_SIZE, bw = w / BLOCK_SIZE * BLOCK_SIZE;
		for (int dh = 0; dh < BLOCK_SIZE; dh++) {
			for (int dw = 0; dw < BLOCK_SIZE; dw++) {
				candidate[bh + dh][bw + dw] &= mask;
			}
		}
	}

	void init(Matrix& candidate, int& completed_counter) {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (board[i][j]) completed_counter++;
				candidate[i][j] = (board[i][j] ? 0 : (1 << BOARD_SIZE) - 1);
			}
		}
	
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (!board[i][j]) continue;
				fillNumber(board, candidate, i, j, board[i][j]);
			}
		}
	}

	void solve1(Matrix& current_board, Matrix& candidate, int& completed_counter) {
		while (completed_counter < 81) {
			bool updated_flag = false;
	
			for (int k = 1; k <= BOARD_SIZE; k++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					int idx = -1;
					for (int i = 0; i < BOARD_SIZE; i++) {
						if (!(candidate[i][j] & (1 << (k - 1)))) continue;
						if (idx == -1) {
							idx = i;
						} else {
							idx = -1;
							break;
						}
					}
					if (idx != -1) {
						fillNumber(current_board, candidate, idx, j, k);
						completed_counter++;
						updated_flag = true;
					}
				}
			}
	
			for (int k = 1; k <= BOARD_SIZE; k++) {
				for (int i = 0; i < BOARD_SIZE; i++) {
					int idx = -1;
					for (int j = 0; j < BOARD_SIZE; j++) {
						if (!(candidate[i][j] & (1 << (k - 1)))) continue;
						if (idx == -1) {
							idx = j;
						} else {
							idx = -1;
							break;
						}
					}
					if (idx != -1) {
						fillNumber(current_board, candidate, i, idx, k);
						completed_counter++;
						updated_flag = true;
					}
				}
			}
	
			for (int k = 1; k <= BOARD_SIZE; k++) {
				for (int bh = 0; bh < BLOCK_SIZE; bh++) {
					for (int bw = 0; bw < BLOCK_SIZE; bw++) {
						int tmp_flag = 0;
						int nh = -1, nw = -1;
						for (int dh = 0; dh < BLOCK_SIZE; dh++) {
							for (int dw = 0; dw < BLOCK_SIZE; dw++) {
								int ch = bh * BLOCK_SIZE + dh, cw = bw * BLOCK_SIZE + dw;
								if (!(candidate[ch][cw] & (1 << (k - 1)))) continue;
								if (!tmp_flag) {
									nh = ch;
									nw = cw;
									tmp_flag = 1;
								} else {
									tmp_flag = 2;
									break;
								}
							}
							if (tmp_flag == 2) break;
						}
						if (tmp_flag == 1) {
							fillNumber(current_board, candidate, nh, nw, k);
							completed_counter++;
							updated_flag = true;
						}
					}
				}
			}
	
			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					if (current_board[i][j]) continue;
	
					int answer = 0;
					bool answer_flag = false;
					for (int k = 1; k <= BOARD_SIZE; k++) {
						if (!(candidate[i][j] & (1 << (k - 1)))) continue;
						if (!answer_flag) {
							answer_flag = true;
							answer = k;
						} else {
							answer_flag = false;
							break;
						}
					}
					if (answer_flag) {
						fillNumber(current_board, candidate, i, j, answer);
						completed_counter++;
						updated_flag = true;
					}
				}
			}
			if (!updated_flag) break;
		}
	}
	
	bool dfs(Matrix current_board, Matrix& candidate, int completed_counter, Matrix& answer_board) {
		solve1(current_board, candidate, completed_counter);
	
		if (completed_counter == 81) {
			answer_board = current_board;
			return true;
		}
	
		//候補の少ない順に探索
		std::vector<std::tuple<int, int, int> > next_pos;
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (current_board[i][j]) continue;
				int cnt = 0;
				for (int k = 1; k <= BOARD_SIZE; k++) {
					cnt += ((candidate[i][j] & (1 << (k - 1))) != 0);
				}
				if (cnt == 0) return false; //矛盾発生
				next_pos.emplace_back(cnt, i, j);
			}
		}
		std::sort(next_pos.begin(), next_pos.end());
	
		for (auto&& pos : next_pos) {
			int i = std::get<1>(pos);
			int j = std::get<2>(pos);
			if (current_board[i][j]) continue;
			for (int k = 1; k <= BOARD_SIZE; k++) {
				if (!(candidate[i][j] & (1 << (k - 1)))) continue;
				Matrix next_candidate(candidate);
				fillNumber(current_board, next_candidate, i, j, k);
				if (dfs(current_board, next_candidate, completed_counter + 1, answer_board)) {
					return true;
				}
				candidate[i][j] &= ~(1 << (k - 1)); //解が無いなら候補から外せる
				current_board[i][j] = 0;
			}
		}
	
		return false;
	}
};

int main() {
	SudokuSolver solver;

	if (solver.input()) {
		printf("Error!!!");
		return 0;
	}

	solver.solve();
	solver.print();

	return 0;
}