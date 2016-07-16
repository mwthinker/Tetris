#include <iostream>
#include <thread>

#include <ai.h>
#include <tetrisboard.h>

void printBoard(const TetrisBoard& board) {
	const std::vector<BlockType>& squares = board.getBoardVector();
	int rows = board.getRows();
	int columns = board.getColumns();
	Block block = board.getBlock();
	std::cout << "\n|";
	for (int row = rows - 3; row > -1; --row) {
		for (int column = 0; column < columns; ++column) {
			if (column == 0 && row < rows - 3) {
				// Add new row.
				std::cout << "  " << row + 2 << "\n|";
			}
			
			bool newSquare = false;
			for (Square sq : block) {
				if (sq.column_ == column && sq.row_ == row) {
					std::cout << "X|";
					newSquare = true;
					break;
				}
			}
			if (newSquare) {
				continue;
			}
			
			if (BlockType::EMPTY == squares[row * columns + column]) {
				std::cout << " |";
			} else {
				std::cout << "X|";
			}
		}
	}

	std::cout << "  1\n";
	for (int i = 0; i < columns; ++i) {
		std::cout << "--";
	}
	std::cout << "-\n" << "Turns : " << board.getNbrOfUpdates() << "\n\n";
}

int main(int argc, char** argv) {
	using namespace std::chrono_literals;

	std::cout << "\nAll tests succeeded!\n";	
	Ai defaultAi_;
	TetrisBoard board(24, 10, randomBlockType(), randomBlockType());
	int nbr = 0;

	bool newBlock = true;
	board.addGameEventListener([&newBlock, &board](GameEvent gameEvent, const TetrisBoard&) {
		if (GameEvent::BLOCK_COLLISION == gameEvent) {
			newBlock = true;
			board.updateNextBlock(randomBlockType());
			//std::this_thread::sleep_for(250ms);
		}
	});

	while (!board.isGameOver()) {
		Ai::State state = defaultAi_.calculateBestState(board, 1);

		if (newBlock) {
			std::cout << "Turns : " << board.getNbrOfUpdates() << "\n";
			printBoard(board);

			for (int i = 0; i < state.rotations_; ++i) {
				board.update(Move::ROTATE_LEFT);
			}

			while (state.left_ != 0) {
				if (state.left_ < 0) {
					board.update(Move::RIGHT);
					++state.left_;
				} else if (state.left_ > 0) {
					board.update(Move::LEFT);
					--state.left_;
				}
			}
		}
		
		newBlock = false;
		board.update(Move::DOWN_GRAVITY);
	}
	printBoard(board);

	return 0;
}
