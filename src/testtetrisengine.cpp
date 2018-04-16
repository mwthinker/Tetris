// main() provided by Catch in file testmain.cpp.
#include <catch.hpp>

#include <rawtetrisboard.h>
#include <tetrisparameters.h>

bool blockEqual(const Block& block1, const Block& block2) {
	if (block1.getSize() != block2.getSize()) {
		return false;
	}
	for (int i = 0; i < block1.getSize(); ++i) {
		if (block1[i] != block2[i]) {
			return false;
		}
	}
	if (block1.getBlockType() != block2.getBlockType()) {
		return false;
	}
	if (block1.getNumberOfRotations() != block2.getNumberOfRotations()) {
		return false;
	}

	if (block1.getCurrentRotation() != block2.getCurrentRotation()) {
		return false;
	}

	if (block1.getStartColumn() != block2.getStartColumn()) {
		return false;
	}

	if (block1.getRotationSquare() != block2.getRotationSquare()) {
		return false;
	}
	return true;
}

TEST_CASE("Test Square", "[square]") {
	INFO("Testing square");

	const Square sq(BlockType::L, 1, 5);
	SECTION("square constuctor give the correct result") {		
		REQUIRE((sq.blockType_ == BlockType::L && sq.row_ == 1 && sq.column_ == 5));
	}

	SECTION("square copy give the same result") {
		const Square sqCopy = sq;
		REQUIRE((sq.blockType_ == sqCopy.blockType_ && sqCopy.row_ == sq.row_ && sqCopy.column_ == sq.column_));
	}

	SECTION("testing equality") {
		SECTION("changing blockType") {
			Square sqCopy = sq;
			sqCopy.blockType_ = BlockType::J;
			REQUIRE(sqCopy.blockType_ != sq.blockType_);
			REQUIRE(sqCopy != sq);
			REQUIRE(!(sqCopy == sq));
		}
		SECTION("changing row") {
			Square sqCopy = sq;
			++sqCopy.row_;
			REQUIRE(sqCopy != sq);
			REQUIRE(!(sqCopy == sq));
		}
		SECTION("changing column") {
			Square sqCopy = sq;
			++sqCopy.column_;
			REQUIRE(sqCopy != sq);
			REQUIRE(!(sqCopy == sq));
		}
	}
}

TEST_CASE("Test Block", "[block][square]") {
	INFO("Default tetrisboard");

	SECTION("Move block") {
		Block block(BlockType::J, 0, 0);

		SECTION("Move down") {
			block.moveDown();
			Block tmpBlock(BlockType::J, -1, 0);
			REQUIRE(blockEqual(tmpBlock, block));

			block.moveDown();
			block.moveDown();
			tmpBlock = Block(BlockType::J, -3, 0);
			REQUIRE(blockEqual(tmpBlock, block));
		}

		SECTION("Move up") {
			block.moveUp();
			Block tmpBlock(BlockType::J, 1, 0);
			REQUIRE(blockEqual(tmpBlock, block));

			block.moveUp();
			block.moveUp();
			tmpBlock = Block(BlockType::J, 3, 0);
			REQUIRE(blockEqual(tmpBlock, block));
		}

		SECTION("Move left") {
			block.moveLeft();
			Block tmpBlock(BlockType::J, 0, -1);
			REQUIRE(blockEqual(tmpBlock, block));

			block.moveLeft();
			block.moveLeft();
			tmpBlock = Block(BlockType::J, 0, -3);
			REQUIRE(blockEqual(tmpBlock, block));
		}

		SECTION("Move right") {
			block.moveRight();
			Block tmpBlock(BlockType::J, 0, 1);
			REQUIRE(blockEqual(tmpBlock, block));

			block.moveRight();
			block.moveRight();
			tmpBlock = Block(BlockType::J, 0, 3);
			REQUIRE(blockEqual(tmpBlock, block));
		}
	}

	SECTION("Rotating block") {
		Block block(BlockType::J, 0, 0);
		REQUIRE(block.getNumberOfRotations() == 4);
		
		SECTION("Rotating clockwise in a full circle") {
			Block tmpBlock = block;
			block.rotateLeft();
			REQUIRE(!blockEqual(block, tmpBlock));
			block.rotateLeft();
			REQUIRE(!blockEqual(block, tmpBlock));
			block.rotateLeft();
			REQUIRE(!blockEqual(block, tmpBlock));
			block.rotateLeft();
			REQUIRE(blockEqual(block, tmpBlock));
		}

		SECTION("Rotating counter clockwise in a full circle") {
			Block tmpBlock = block;
			block.rotateRight();
			REQUIRE(!blockEqual(block, tmpBlock));
			block.rotateRight();
			REQUIRE(!blockEqual(block, tmpBlock));
			block.rotateRight();
			REQUIRE(!blockEqual(block, tmpBlock));
			block.rotateRight();
			REQUIRE(blockEqual(block, tmpBlock));
		}

		SECTION("Rotating in both directions") {
			const Block copyBlock = block;
			SECTION("Rotating one/three step") {
				Block rightBlock = block;
				rightBlock.rotateRight();
				rightBlock.rotateRight();
				rightBlock.rotateRight();
				block.rotateLeft(); // One step.
				REQUIRE(!blockEqual(block, copyBlock));
				REQUIRE(!blockEqual(rightBlock, copyBlock));
				REQUIRE(blockEqual(block, rightBlock));
			}

			SECTION("Rotating two/two steps") {
				Block rightBlock = block;
				rightBlock.rotateRight();
				rightBlock.rotateRight();
				block.rotateLeft(); // One step.
				block.rotateLeft(); // Two step.
				REQUIRE(!blockEqual(block, copyBlock));
				REQUIRE(!blockEqual(rightBlock, copyBlock));
				REQUIRE(blockEqual(block, rightBlock));
			}
		}
	}
}

TEST_CASE("Test tetrisboard", "[tetrisboard]") {
	INFO("Default tetrisboard");

	const BlockType firstNextBlockType = BlockType::L;
	const BlockType firstCurrentBlockType = BlockType::S;
	RawTetrisBoard board(TETRIS_HEIGHT, TETRIS_WIDTH, firstCurrentBlockType, firstNextBlockType);

	int size = sizeof(RawTetrisBoard);

	SECTION("should not be game over") {
		REQUIRE(!board.isGameOver());
	}

	SECTION("testing the size of the board") {
		REQUIRE(board.getColumns() == TETRIS_WIDTH);
		REQUIRE(board.getRows() == TETRIS_HEIGHT);
	}

	SECTION("update the next block") {
		REQUIRE(board.getNextBlockType() == firstNextBlockType);
		REQUIRE(board.getBlockType() == firstCurrentBlockType);

		const BlockType newBlockType = BlockType::I;
		REQUIRE(newBlockType != board.getNextBlockType());
		board.updateNextBlock(newBlockType);
		REQUIRE(newBlockType == board.getNextBlockType());
	}

	SECTION("update the current block") {
		REQUIRE(board.getNextBlockType() == firstNextBlockType);
		REQUIRE(board.getBlockType() == firstCurrentBlockType);
		REQUIRE(firstNextBlockType != firstCurrentBlockType);

		SECTION("current block becomes the nextblock") {
			Block startBlock = board.getBlock();
			REQUIRE(blockEqual(startBlock, board.getBlock()));
			board.update(Move::DOWN_GROUND);
			REQUIRE(!blockEqual(startBlock, board.getBlock()));
			board.update(Move::DOWN_GRAVITY);
			REQUIRE(board.getBlockType() == firstNextBlockType);
		}
	}


	SECTION("restart the board") {
		const int NEW_WIDTH = 15;
		const int NEW_HEIGHT = 30;
		const BlockType NEW_CURRENT = BlockType::I;
		const BlockType NEW_NEXT = BlockType::Z;
		RawTetrisBoard restartBoard(NEW_HEIGHT, NEW_WIDTH, NEW_CURRENT, NEW_NEXT);

		// Different size compare to board.
		REQUIRE(board.getBlockType() != restartBoard.getBlockType());
		REQUIRE(board.getNextBlockType() != restartBoard.getNextBlockType());
		REQUIRE(board.getColumns() != restartBoard.getColumns());
		REQUIRE(board.getRows() != restartBoard.getRows());

		SECTION("restart blocktypes") {
			// Restart.
			board.updateRestart(NEW_CURRENT, NEW_NEXT);
			// New types.
			REQUIRE(board.getBlockType() == NEW_CURRENT);
			REQUIRE(board.getNextBlockType() == NEW_NEXT);
			// Same size.
			REQUIRE(board.getColumns() == TETRIS_WIDTH);
			REQUIRE(board.getRows() == TETRIS_HEIGHT);
		}
		
		SECTION("restart and resize") {
			// Restart.
			board.update(Move::LEFT);
			board.update(Move::DOWN_GROUND);
			board.updateRestart(NEW_HEIGHT, NEW_WIDTH, NEW_CURRENT, NEW_NEXT);
			// New types.
			REQUIRE(board.getBlockType() == restartBoard.getBlockType());
			REQUIRE(board.getNextBlockType() == restartBoard.getNextBlockType());
			// Same block.
			REQUIRE(blockEqual(board.getBlock(), restartBoard.getBlock()));
			// New size.
			REQUIRE(board.getColumns() == restartBoard.getColumns());
			REQUIRE(board.getRows() == restartBoard.getRows());
		}
	}
}
