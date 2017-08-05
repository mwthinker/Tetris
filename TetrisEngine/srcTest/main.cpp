#include <iostream>
#include <thread>
#include <chrono>

#include <ai.h>
#include <tetrisboard.h>
#include <fstream>

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
	std::cout << "-\n\n";
}

BlockType readBlockType(std::ifstream& infile) {
	int data = 0;
	if (!(infile >> data)) {
		// Restart, read from the start again.
		infile.clear();
		infile.seekg(0, std::ios::beg);
		infile >> data;
	}
	return (BlockType) data;
}

int main(int argc, char** argv) {
	using namespace std::chrono_literals;

	Ai defaultAi_;
	std::ifstream infile("randomBlockTypes");	
	TetrisBoard board(24, 10, readBlockType(infile), readBlockType(infile));

	bool newBlock = true;
	board.addGameEventListener([&infile, &newBlock, &board](GameEvent gameEvent, const TetrisBoard&) {
		if (GameEvent::BLOCK_COLLISION == gameEvent) {
			newBlock = true;
			BlockType blockType = readBlockType(infile);
			board.updateNextBlock(blockType);
		}
	});

	auto time = std::chrono::high_resolution_clock::now();
	while (!board.isGameOver()) {
		Ai::State state = defaultAi_.calculateBestState(board, 1);

		if (newBlock) {
			//std::cout << "Turns : " << board.getTurns() << "\n";
			//printBoard(board);

			for (int i = 0; i < state.rotationLeft_; ++i) {
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
		//std::cout << "Current turn: " << board.getTurns() << std::endl;
	}
	printBoard(board);
	std::chrono::duration<double> delta = std::chrono::high_resolution_clock::now() - time;
	std::cout << "Turns: " << board.getTurns() << " Time in seconds: " << delta.count() << std::endl;

	return 0;
}
