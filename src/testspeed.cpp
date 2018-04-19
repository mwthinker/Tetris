// main() provided by Catch in file testmain.cpp.
#include <catch.hpp>

#include <rawtetrisboard.h>
#include <tetrisboard.h>
#include <tetrisparameters.h>

TEST_CASE("benchmarked", "[!benchmark]") {

	BENCHMARK("Copy RawTetrisBoard") {
		RawTetrisBoard board(TETRIS_WIDTH, TETRIS_HEIGHT, BlockType::S, BlockType::J);
		for (int i = 0; i < 100; ++i) {
			RawTetrisBoard copyBoard = board;
			board.update(Move::DOWN);
		}
	}

	BENCHMARK("Copy TetrisBoard") {
		TetrisBoard board(TETRIS_WIDTH, TETRIS_HEIGHT, BlockType::S, BlockType::J);
		for (int i = 0; i < 100; ++i) {
			TetrisBoard copyBoard = board;
			board.update(Move::DOWN);
		}
	}

	BENCHMARK("Copy block") {
		BlockType blockTypes[] = {BlockType::I, BlockType::J, BlockType::L,
			BlockType::O, BlockType::S, BlockType::T, BlockType::Z};

		int size = sizeof(Block);
		int size2 = sizeof(int);
		int size3 = sizeof(RawTetrisBoard);
		int size4 = sizeof(TetrisBoard);

		for (int i = 0; i < 1000; ++i) {
			Block block(blockTypes[i % sizeof(blockTypes)], 0, 0);
			for (int j = 0; j < 1; ++j) {
				block.rotateLeft();
			}
		}
	}
}
