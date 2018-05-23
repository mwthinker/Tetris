#include "flags.h"
#include "flagsexception.h"

#include <ai.h>
#include <tetrisboard.h>
#include <calc/calculatorexception.h>

#include <fstream>
#include <sstream>
#include <thread>
#include <limits>
#include <queue>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std::chrono_literals;

void printBoard(const RawTetrisBoard& board) {
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

BlockType badRandomBlockType() {
	static BlockType blockType = BlockType::Z;
	if (blockType == BlockType::Z) {
		blockType = BlockType::S;
	} else {
		blockType = BlockType::Z;
	}
	return blockType;
}

BlockType readBlockType(std::ifstream& infile) {
	int data = -1;
	if (infile.is_open()) {
		if (!(infile >> data)) {
			// Restart, read from the start again.
			infile.clear();
			infile.seekg(0, std::ios::beg);
			infile >> data;
		}
	}
	if (data >= 0 && data <= 6) {
		return (BlockType) data;
	}
	// In order for the ai to fail.
	return badRandomBlockType();
}

std::istream& operator>>(std::istream& is, std::chrono::milliseconds& time) {
	int intTime;
	is >> intTime;
	time = std::chrono::milliseconds(intTime);
	return is;
}

std::chrono::duration<double> runGame(TetrisBoard& tetrisBoard, const Flags& flags) {
	auto time = std::chrono::high_resolution_clock::now();

	Ai ai = flags.ai_;
	RawTetrisBoard playBoard = tetrisBoard;
	while (!tetrisBoard.isGameOver() && tetrisBoard.getTurns() < flags.maxNbrBlocks_) {
		Ai::State state = ai.calculateBestState(tetrisBoard, flags.depth_);

		if (flags.play_) {
			std::cout << "AI: " << ai.getValueFunction() << "\n";
			std::cout << "Turns: " << tetrisBoard.getTurns() << "\n";
			playBoard = tetrisBoard;
		}

		float value = ai.moveBlockToGroundCalculateValue(state, tetrisBoard);

		if (flags.play_) {
			std::cout << "Value: " << value << "\n";
			const auto& variables = ai.getCalculator().getVariables();
			for (std::string name : variables) {
				std::cout << name << " = " << ai.getCalculator().extractVariableValue(name) << "\t";
			}
			printBoard(playBoard);
		}
		
		tetrisBoard.update(Move::DOWN_GRAVITY);
		if (flags.delay_ > 1ms) {
			std::this_thread::sleep_for(flags.delay_);
		}
	}
	return std::chrono::high_resolution_clock::now() - time;
}

void printGameResult(const TetrisBoard& tetrisBoard, const Flags& flags, std::chrono::duration<double> gameTime,
	int lines1, int lines2, int lines3, int lines4) {

	std::queue<std::string> outputOrder = flags.outputOrder_;

	std::cout << std::setprecision(2) << std::fixed;
	if (flags.play_) {
		printBoard(tetrisBoard);
		std::cout << "AI: " << flags.ai_.getValueFunction() << "\n";
		std::cout << "Turns: " << tetrisBoard.getTurns() << "\n";
		std::cout << "Time in seconds: " << gameTime.count() << "\n";
	}

	while (!outputOrder.empty()) {
		std::string flag = outputOrder.front();
		if (flag == "-T") {
			if (flags.verbose_) {
				std::cout << "time = ";
			}
			std::cout << gameTime.count();
			if (flags.verbose_) {
				std::cout << "s";
			}
			std::cout << "\t";
		} else if (flag == "-t") {
			if (flags.verbose_) {
				std::cout << "turns = ";
			}
			std::cout << tetrisBoard.getTurns() << "\t";
		} else if (flag == "-c1") {
			if (flags.verbose_) {
				std::cout << "cleared-row-1 = ";
			}
			std::cout << lines1 << "\t";
		} else if (flag == "-c2") {
			if (flags.verbose_) {
				std::cout << "cleared-row-2 = ";
			}
			std::cout << lines2 << "\t";
		} else if (flag == "-c3") {
			if (flags.verbose_) {
				std::cout << "cleared-row-3 = ";
			}
			std::cout << lines3 << "\t";
		} else if (flag == "-c4") {
			if (flags.verbose_) {
				std::cout << "cleared-row-4 = ";
			}
			std::cout << lines4 << "\t";
		}
		outputOrder.pop();
	}

	std::cout << "\n";
}

int main(const int argc, const char* const argv[]) {
	Flags flags;
	try {
		flags = Flags(argc, argv);
	} catch (FlagsException e) {
		std::cerr << e.what();
		return 1;
	}

	if (flags.printHelp_) {
		flags.printHelpFunction();
		return 0;
	}

	BlockType start = randomBlockType();
	BlockType next = randomBlockType();

	std::ifstream infile;
	infile.exceptions(std::ifstream::badbit|std::ifstream::failbit);

	if (flags.useRandomFile_) {
		try {
			infile.open(flags.randomFilePath_);
		} catch (std::ifstream::failure e) {
			std::cerr << "Failed to open file: " + flags.randomFilePath_;
			if (flags.verbose_) {
				std::cerr << e.what() << "\n";
			}
			std::exit(1);
		}
		start = readBlockType(infile);
		next = readBlockType(infile);
	}

	TetrisBoard tetrisBoard(flags.width_, flags.height_, start, next);
	int nbrOneLine = 0;
	int nbrTwoLine = 0;
	int nbrThreeLine = 0;
	int nbrFourLine = 0;

	tetrisBoard.addGameEventListener([&](GameEvent gameEvent, const TetrisBoard&) {
		if (GameEvent::BLOCK_COLLISION == gameEvent) {
			if (flags.useRandomFile_) {
				try {
					BlockType blockType = readBlockType(infile);
					tetrisBoard.updateNextBlock(blockType);
				} catch (std::ifstream::failure e) {
					std::cerr << "Failed to read " << flags.randomFilePath_ << "\n";
					if (flags.verbose_) {
						std::cerr << e.what() << "\n";
					}
					std::exit(1);
				}
			} else {
				tetrisBoard.updateNextBlock(randomBlockType());
			}
		}
		switch (gameEvent) {
			case GameEvent::ONE_ROW_REMOVED:
				++nbrOneLine;
				break;
			case GameEvent::TWO_ROW_REMOVED:
				++nbrTwoLine;
				break;
			case GameEvent::THREE_ROW_REMOVED:
				++nbrThreeLine;
				break;
			case GameEvent::FOUR_ROW_REMOVED:
				++nbrFourLine;
				break;
		}
	});

	std::chrono::duration<double> delta = runGame(tetrisBoard, flags);
	printGameResult(tetrisBoard, flags, delta, nbrOneLine, nbrTwoLine, nbrThreeLine, nbrFourLine);
	
	return 0;
}
