// main() provided by Catch in file testmain.cpp.
#include <catch.hpp>

#include <rawtetrisboard.h>
#include <tetrisboard.h>
#include <tetrisparameters.h>
#include <block2.h>
#include <square.h>
#include <rawtetrisboard2.h>
#include <tetrisdata.h>
#include <vector>
#include <ai.h>

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

		sizeof(Block);
		sizeof(int);
		sizeof(RawTetrisBoard);
		sizeof(TetrisBoard);
		sizeof(Block2);
		sizeof(RawTetrisBoard2);
		sizeof(int_fast32_t);

		for (int i = 0; i < 10000; ++i) {
			Block block(blockTypes[i % sizeof(blockTypes)], 0, 0);
			for (int j = 0; j < 4; ++j) {
				block.rotateLeft();
			}
			for (int j = 0; j < 4; ++j) {
				block.rotateRight();
			}
			for (int j = 0; j < 8; ++j) {
				block.moveDown();
			}
			for (int j = 0; j < 8; ++j) {
				block.moveUp();
			}
		}
	}

	sizeof(Block);
	sizeof(Block2);
	
	BENCHMARK("Copy block2") {
		BlockType blockTypes[] = {BlockType::I, BlockType::J, BlockType::L,
			BlockType::O, BlockType::S, BlockType::T, BlockType::Z};

		for (int i = 0; i < 10000; ++i) {
			Block2 block(blockTypes[i % sizeof(blockTypes)]);
			for (int j = 0; j < 4; ++j) {
				block.rotateLeft();
			}
			for (int j = 0; j < 4; ++j) {
				block.rotateRight();
			}
			for (int j = 0; j < 8; ++j) {
				block.moveDown();
			}
			for (int j = 0; j < 8; ++j) {
				block.moveUp();
			}
		}
	}

	std::vector<BlockType> blockTypes;
	std::string str = "TTOOZZSSEIETOOSZSEEEEEESSEEEEEEEES";
	for (char chr : str) {
		blockTypes.push_back(charToBlockType(chr));
	}
	RawTetrisBoard board(blockTypes, TETRIS_WIDTH, TETRIS_HEIGHT,
		Block(BlockType::J, 4, 18, 0), BlockType::L);
	int highestUsedRow = calculateHighestUsedRow(board);
	
	board.update(Move::DOWN_GROUND);

	BENCHMARK("Old ai functions") {
		calculateColumnHoles(board, highestUsedRow);
		calculateRowRoughness(board, highestUsedRow);
		calculateBlockMeanHeight(board.getBlock());
		calculateHighestUsedRow(board);
		calculateBlockEdges(board, board.getBlock());
	}

	BENCHMARK("calculateColumnHoles") {
		calculateColumnHoles(board, highestUsedRow);
	}
	BENCHMARK("calculateRowRoughness") {
		calculateRowRoughness(board, highestUsedRow);
	}
	BENCHMARK("calculateBlockMeanHeight") {
		calculateBlockMeanHeight(board.getBlock());
	}
	BENCHMARK("calculateHighestUsedRow") {
		calculateHighestUsedRow(board);
	}
	BENCHMARK("calculateBlockEdges") {
		calculateBlockEdges(board, board.getBlock());
	}

	BENCHMARK("New ai functions") {
		calculateLandingHeight(board.getBlock());
		calculateErodedPieces(board);
		board.update(Move::DOWN_GROUND);
		calculateRowTransitions(board);
		calculateColumnTransitions(board);
		calculateNumberOfHoles(board);
		calculateCumulativeWells(board);
		calculateHoleDepth(board);
		calculateRowHoles(board);
	}
	
	BENCHMARK("calculateLandingHeight ") {
		calculateLandingHeight(board.getBlock());
	}
	BENCHMARK("calculateErodedPieces") {
		calculateErodedPieces(board);
	}
	BENCHMARK("calculateRowTransitions") {
		calculateRowTransitions(board);
	}
	BENCHMARK("calculateColumnTransitions") {
		calculateColumnTransitions(board);
	}
	BENCHMARK("calculateNumberOfHoles") {
		calculateNumberOfHoles(board);
	}
	BENCHMARK("calculateCumulativeWells") {
		calculateCumulativeWells(board);
	}
	BENCHMARK("calculateHoleDepth") {
		calculateHoleDepth(board);
	}

	BENCHMARK("AI calculateBestState") {
		Ai ai;
		ai.calculateBestState(board, 1);
	}
	
	Board b;
}
