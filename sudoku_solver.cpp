/*
	参考
	http://algorithm.main.jp/Puzzle/Sudoku/1-HowToSolve.php
	https://www.slideshare.net/atmarksharp/ss-45348313
	http://excel-ubara.com/excelvba5/EXCELVBA231_2.html
*/

#include <iostream>
#include <array>
#include <tuple>
#include <vector>
#include <algorithm>
#include <cassert>

using std::cin;
using std::cout;
using std::endl;
using std::array;

using Matrix = array< array<int, 9>, 9 >;

class SudokuSolver {
public:
	bool input() {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				cin >> board[i][j];
			}
		}
		return isWrongBoard();	
	}

	void print() {
		for (int i = 0; i < 9; i++) {
			cout << board[i][0];
			for (int j = 1; j < 9; j++) {
				cout << " " << board[i][j];
			}
			cout << endl;
		}	
	}

	void solve2() {
		int completed_counter = 0;
		Matrix candidate;
		init(candidate, completed_counter);
		assert(dfs(board, candidate, completed_counter, board));
	}
private:
	Matrix board;
	bool isWrongBoard() {
		//縦の判定
		for (int j = 0; j < 9; j++) {
			array<bool, 10> used{};
			for (int i = 0; i < 9; i++) {
				if (!board[i][j]) continue;
				if (used[board[i][j]]) return true;
				used[board[i][j]] = true;
			}
		}
	
		//横の判定
		for (int i = 0; i < 9; i++) {
			array<bool, 10> used{};
			for (int j = 0; j < 9; j++) {
				if (!board[i][j]) continue;
				if (used[board[i][j]]) return true;
				used[board[i][j]] = true;
			}
		}
	
		//3*3マスの判定
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				array<bool, 10> used{};
				for (int dh = 0; dh < 3; dh++) {
					for (int dw = 0; dw < 3; dw++) {
						int h = i * 3 + dh, w = j * 3 + dw;
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
		for (int i = 0; i < 9; i++) {
			candidate[i][w] &= mask;
		}
	
		//横に関する処理
		for (int j = 0; j < 9; j++) {
			candidate[h][j] &= mask;
		}
	
		//3*3マスに関する処理
		int bh = h / 3 * 3, bw = w / 3 * 3;
		for (int dh = 0; dh < 3; dh++) {
			for (int dw = 0; dw < 3; dw++) {
				candidate[bh + dh][bw + dw] &= mask;
			}
		}
	}

	void init(Matrix& candidate, int& completed_counter) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (board[i][j]) completed_counter++;
				candidate[i][j] = (board[i][j] ? 0 : (1 << 9) - 1);
			}
		}
	
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (!board[i][j]) continue;
				fillNumber(board, candidate, i, j, board[i][j]);
			}
		}
	}

	void solve1(Matrix& current_board, Matrix& candidate, int& completed_counter) {
		while (completed_counter < 81) {
			bool updated_flag = false;
	
			for (int k = 1; k <= 9; k++) {
				for (int j = 0; j < 9; j++) {
					int idx = -1;
					for (int i = 0; i < 9; i++) {
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
	
			for (int k = 1; k <= 9; k++) {
				for (int i = 0; i < 9; i++) {
					int idx = -1;
					for (int j = 0; j < 9; j++) {
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
	
			for (int k = 1; k <= 9; k++) {
				for (int bh = 0; bh < 3; bh++) {
					for (int bw = 0; bw < 3; bw++) {
						int tmp_flag = 0;
						int nh = -1, nw = -1;
						for (int dh = 0; dh < 3; dh++) {
							for (int dw = 0; dw < 3; dw++) {
								int ch = bh * 3 + dh, cw = bw * 3 + dw;
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
	
			for (int i = 0; i < 9; i++) {
				for (int j = 0; j < 9; j++) {
					if (current_board[i][j]) continue;
	
					int answer = 0;
					bool answer_flag = false;
					for (int k = 1; k <= 9; k++) {
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
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (current_board[i][j]) continue;
				int cnt = 0;
				for (int k = 1; k <= 9; k++) {
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
			for (int k = 1; k <= 9; k++) {
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
	cin.tie(0);
	std::ios::sync_with_stdio(false);

	SudokuSolver solver;

	if (solver.input()) {
		cout << "Error!!!" << endl;
		return 0;
	}

	//cout << "[problem]" << endl;
	//print(problem_board);
	//cout << endl;

	solver.solve2();

	//cout << "[answer]" << endl;
	solver.print();

	return 0;
}