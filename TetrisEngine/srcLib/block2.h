#ifndef BLOCK2_H
#define BLOCK2_H

#include "square.h"

#include <cstdint>

class Block2 {
public:
	Block2() = default;

	Block2(BlockType blockType)
		: Block2(blockType, 0) {
	}

	Block2(BlockType blockType, int rotations)
		: Block2(blockType, rotations, 0, 0) {
	}

	inline Block2(BlockType blockType, int rotations, int lowX, int lowY);

	inline void moveLeft();
	inline void moveRight();

	inline void moveUp();
	inline void moveDown();

	inline void rotateRight();
	inline void rotateLeft();
	inline int getCurrentRotation() const;
	inline BlockType getBlockType() const;

	inline int getLowX() const;
	inline int getLowY() const;

	uint16_t getBits() const {
		return bits_;
	}

private:
	int lowX_, lowY_;
	uint16_t bits_;

	static constexpr uint16_t extractBlockTypeI(int rotations);
	static constexpr uint16_t extractBlockTypeJ(int rotations);
	static constexpr uint16_t extractBlockTypeL(int rotations);
	static constexpr uint16_t extractBlockTypeO(int rotations);
	static constexpr uint16_t extractBlockTypeS(int rotations);
	static constexpr uint16_t extractBlockTypeT(int rotations);
	static constexpr uint16_t extractBlockTypeZ(int rotations);
	static constexpr uint16_t extractBitsFromBlockType(BlockType blockType, int rotations);
	static constexpr BlockType extractBlockTypeFromBits(uint16_t bits);
	static constexpr int extractRotationsFromBits(uint16_t bits);

	// 0 1 0 0
	// 0 1 0 0
	// 0 1 0 0
	// 0 1 0 0
	static constexpr uint16_t I_0 = 0b0100010001000100;
	// 0 0 0 0
	// 0 0 0 0
	// 1 1 1 1
	// 0 0 0 0
	static constexpr uint16_t I_1 = 0b0000000011110000;

	// 0 0 0 0
	// 0 0 1 0
	// 0 0 1 0
	// 0 1 1 0
	static constexpr uint16_t J_0 = 0b0000001000100110;
	// 0 0 0 0
	// 0 0 0 0
	// 1 1 1 0
	// 0 0 1 0
	static constexpr uint16_t J_1 = 0b0000000011100010;

	// 0 0 0 0
	// 0 1 0 0
	// 0 1 0 0
	// 0 1 1 0
	static constexpr uint16_t L_0 = 0b0000010001000110;
	// 0 0 0 0
	// 0 0 1 0
	// 1 1 1 0
	// 0 0 0 0
	static constexpr uint16_t L_1 = 0b0000001011100000;

	// 0 0 0 0
	// 0 1 1 0
	// 0 1 1 0
	// 0 0 0 0
	static constexpr uint16_t O_0 = 0b0000011001100000;

	// 0 0 0 0
	// 0 1 1 0
	// 1 1 0 0
	// 0 0 0 0
	static constexpr uint16_t S_0 = 0b0000011011000000;
	// 0 0 0 0
	// 0 1 0 0
	// 0 1 1 0
	// 0 0 1 0
	static constexpr uint16_t S_1 = 0b0000010001100010;

	// 0 0 0 0
	// 0 1 0 0
	// 1 1 1 0
	// 0 0 0 0
	static constexpr uint16_t T_0 = 0b0000111001000100;
	// 0 0 0 0
	// 0 1 0 0
	// 1 1 0 0
	// 0 1 0 0
	static constexpr uint16_t T_1 = 0b0000010011000100;
	// 0 0 0 0
	// 0 0 0 0
	// 1 1 1 0
	// 0 1 0 0
	static constexpr uint16_t T_2 = 0b0000000011100100;
	// 0 0 0 0
	// 0 1 0 0
	// 0 1 1 0
	// 0 1 0 0
	static constexpr uint16_t T_3 = 0b0000010001100100;

	// 0 0 0 0
	// 1 1 0 0
	// 0 1 1 0
	// 0 0 0 0
	static constexpr uint16_t Z_0 = 0b0000110001100000;
	// 0 0 0 0
	// 0 1 0 0
	// 1 1 0 0
	// 1 0 0 0
	static constexpr uint16_t Z_1 = 0b0000010011001000;
};

constexpr uint16_t Block2::extractBlockTypeI(int rotations) {
	switch ((rotations % 2) + 2) {
		case 0:
			return I_0;
		case 1:
			return I_1;
	}
	return 0; // Is not possible.
}

constexpr uint16_t Block2::extractBlockTypeJ(int rotations) {
	switch ((rotations % 2) + 2) {
		case 0:
			return J_0;
		case 1:
			return J_1;
	}
	return 0; // Is not possible.
}

constexpr uint16_t Block2::extractBlockTypeL(int rotations) {
	switch ((rotations % 2) + 2) {
		case 0:
			return L_0;
		case 1:
			return L_1;
	}
	return 0; // Is not possible.
}

constexpr uint16_t Block2::extractBlockTypeO(int rotations) {
	return O_0;
}

constexpr uint16_t Block2::extractBlockTypeS(int rotations) {
	switch ((rotations % 2) + 2) {
		case 0:
			return S_0;
		case 1:
			return S_1;
	}
	return 0; // Is not possible.
}

constexpr uint16_t Block2::extractBlockTypeT(int rotations) {
	switch ((rotations % 4) + 4) {
		case 0:
			return T_0;
		case 1:
			return T_1;
		case 2:
			return T_0;
		case 3:
			return T_1;
	}
	return 0; // Is not possible.
}

constexpr uint16_t Block2::extractBlockTypeZ(int rotations) {
	switch ((rotations % 2) + 2) {
		case 0:
			return Z_0;
		case 1:
			return Z_1;
	}
	return 0; // Is not possible.
}

constexpr uint16_t Block2::extractBitsFromBlockType(BlockType blockType, int rotations) {
	switch (blockType) {
		case BlockType::WALL:
			return 0b1111111111111111;
		case BlockType::I:
			return extractBlockTypeI(rotations);
		case BlockType::J:
			return extractBlockTypeJ(rotations);
		case BlockType::L:
			return extractBlockTypeL(rotations);
		case BlockType::O:
			return extractBlockTypeO(rotations);
		case BlockType::S:
			return extractBlockTypeS(rotations);
		case BlockType::T:
			return extractBlockTypeT(rotations);
		case BlockType::Z:
			return extractBlockTypeZ(rotations);
	}
	return 0;
}

constexpr BlockType Block2::extractBlockTypeFromBits(uint16_t bits) {
	switch (bits) {
		case 0:
			return BlockType::EMPTY;
		case I_0:
		case I_1:
			return BlockType::I;
		case J_0:
		case J_1:
			return BlockType::J;
		case L_0:
		case L_1:
			return BlockType::L;
		case O_0:
			return BlockType::O;
		case S_0:
		case S_1:
			return BlockType::S;
		case T_0:
		case T_1:
		case T_2:
		case T_3:
			return BlockType::T;
		case Z_0:
		case Z_1:
			return BlockType::Z;
	}
	return BlockType::WALL;
}

constexpr int Block2::extractRotationsFromBits(uint16_t bits) {
	switch (bits) {
		case 0:
		case I_0:
		case J_0:
		case L_0:
		case O_0:
		case S_0:
		case T_0:
		case Z_0:
			return 0;
		case I_1:
		case J_1:
		case L_1:
		case S_1:
		case Z_1:
		case T_1:
			return 1;
		case T_2:
			return 2;
		case T_3:
			return 3;
	}
	return 0;
}

Block2::Block2(BlockType blockType, int rotations, int lowX, int lowY)
	: bits_(extractBitsFromBlockType(blockType, 0)),
	lowX_(lowX), lowY_(lowY) {
}

void Block2::moveLeft() {
	--lowX_;
}

void Block2::moveRight() {
	++lowX_;
}

void Block2::moveUp() {
	++lowY_;
}

void Block2::moveDown() {
	--lowY_;
}

void Block2::rotateRight() {
	BlockType blockType = extractBlockTypeFromBits(bits_);
	int rotations = getCurrentRotation();
	bits_ = extractBitsFromBlockType(blockType, rotations + 1);
}

void Block2::rotateLeft() {
	BlockType blockType = getBlockType();
	int rotations = getCurrentRotation();
	bits_ = extractBitsFromBlockType(blockType, rotations - 1);
}

int Block2::getCurrentRotation() const {
	return extractRotationsFromBits(bits_);
}

BlockType Block2::getBlockType() const {
	return extractBlockTypeFromBits(bits_);
}

int Block2::getLowX() const {
	return lowX_;
}

int Block2::getLowY() const {
	return lowY_;
}

#endif	// BLOCK2_H
