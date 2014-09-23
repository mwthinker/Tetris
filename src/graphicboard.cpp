#include "graphicboard.h"
#include "tetrisboard.h"

#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/text.h>
#include <mw/color.h>
#include <mw/opengl.h>

#include <sstream>
#include <vector>

namespace {

	const mw::Color WHITE(1, 1, 1, 1);

	void drawFrame(double x1, double y1, double x2, double y2, double width) {
		/*
		glBegin(GL_QUADS);
		glVertex2d(x1, y1);
		glVertex2d(x2, y1);
		glVertex2d(x2, y1 + width);
		glVertex2d(x1, y1 + width);

		glVertex2d(x2 - width, y1);
		glVertex2d(x2, y1);
		glVertex2d(x2, y2);
		glVertex2d(x2 - width, y2);

		glVertex2d(x1, y2 - width);
		glVertex2d(x2, y2 - width);
		glVertex2d(x2, y2);
		glVertex2d(x1, y2);

		glVertex2d(x1, y1);
		glVertex2d(x1 + width, y1);
		glVertex2d(x1 + width, y2);
		glVertex2d(x1, y2);
		glEnd();
		*/
	}

} // Anonymous namespace.

GraphicPlayerInfo::GraphicPlayerInfo(const mw::Font& font) {
	// Define all text sizes and font usage.
	level_ = mw::Text("", font, 14);
	points_ = mw::Text("", font, 14);
	nbrOfClearedRows_ = mw::Text("", font, 14);
	showPoints_ = true;
	font_ = font;
}

void GraphicPlayerInfo::update(int rowsCleared, int points, int level) {
	std::stringstream stream;
	stream << "Rows " << rowsCleared;
	nbrOfClearedRows_.setText(stream.str());
	stream.str("");
	stream << "Points " << points;
	points_.setText(stream.str());
	stream.str("");
	stream << "Level " << level;
	level_.setText(stream.str());
}

void GraphicPlayerInfo::draw(gui::WindowMatrixPtr wp) {
	wp->useShader();
	auto old = wp->getModel();
	wp->setModel(old * mw::getTranslateMatrix44(5, 5));
	wp->setColor(WHITE);
	nbrOfClearedRows_.draw();
	wp->setModel(wp->getModel() * mw::getTranslateMatrix44(0, nbrOfClearedRows_.getCharacterSize() + 2));
	if (showPoints_) {
		points_.draw();
		wp->setModel(wp->getModel() * mw::getTranslateMatrix44(0, nbrOfClearedRows_.getCharacterSize() + 2));
	}

	level_.draw();

	wp->setModel(old);
}

float GraphicPlayerInfo::getWidth() const {
	return 150;
}

float GraphicPlayerInfo::getHeight() const {
	return 150;
}

GraphicPreviewBlock::GraphicPreviewBlock() {
}

void GraphicPreviewBlock::draw(gui::WindowMatrixPtr wp) {
	auto old = wp->getModel();

	float x = 0, y = 0;
	for (const Square& sq : block_) {
		x += sq.column_;
		y += sq.row_;
	}

	x /= block_.nbrOfSquares();
	y /= block_.nbrOfSquares();

	// Center the blocks and scale the blocks.
	// Calculate center of mass.
	// Move center to origo. -x -y	
	const mw::Sprite& sprite = getSprite(block_.blockType());
	auto matrix = old * mw::getTranslateMatrix44(width_ * 0.5f, height_ * 0.5f) * mw::getScaleMatrix44(pixlePerSquare_, pixlePerSquare_) * mw::getTranslateMatrix44(-x, -y);
	for (const Square& sq : block_) {		
		wp->setModel(matrix * mw::getTranslateMatrix44((float) sq.column_, (float) sq.row_));
		sprite.draw();
	}
	
	wp->setModel(old);
}

void GraphicPreviewBlock::update(const BlockType& blockType, float pixlePerSquare) {
	block_ = Block(blockType, 100, 100);
	pixlePerSquare_ = pixlePerSquare;
	width_ = 5 * pixlePerSquare_;
	height_ = 5 * pixlePerSquare_;
}

float GraphicPreviewBlock::getWidth() const {
	return width_;
}

float GraphicPreviewBlock::getHeight() const {
	return height_;
}

GraphicBoard::GraphicBoard(const TetrisBoard& tetrisBoard) : tetrisBoard_(tetrisBoard) {
	height_ = 800; // Default height for the board.
	frameColor_ = mw::Color(237 / 256.0, 78 / 256.0, 8 / 256.0); // Some type of orange.

	int rows = tetrisBoard_.getNbrRows();
	int columns = tetrisBoard_.getNbrColumns();

	if (vbo_.getSize() == 0) {
		std::vector<GLfloat> data;
		for (int row = 0; row < rows; ++row) {
			for (int column = 0; column < columns; ++column) {
				// First triangle.
				data.emplace_back(0.1f + column); data.emplace_back(0.1f + row);
				data.emplace_back(0.9f + column); data.emplace_back(0.1f + row);
				data.emplace_back(0.1f + column); data.emplace_back(0.9f + row);
				// Second triangle.
				data.emplace_back(0.1f + column); data.emplace_back(0.9f + row);
				data.emplace_back(0.9f + column); data.emplace_back(0.1f + row);
				data.emplace_back(0.9f + column); data.emplace_back(0.9f + row);
			}
		}
		vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);
	}
}

void GraphicBoard::update() {
	height_ = 400;
	pixlePerSquare_ = height_ / (tetrisBoard_.getNbrRows() + 2);
	width_ = tetrisBoard_.getNbrColumns() * pixlePerSquare_;
}

void GraphicBoard::draw(gui::WindowMatrixPtr wp) {
	drawBoard(wp, tetrisBoard_);
}

float GraphicBoard::getWidth() const {
	return width_;
}

float GraphicBoard::getHeight() const {
	return height_;
}

void GraphicBoard::drawBoard(gui::WindowMatrixPtr wp, const RawTetrisBoard& tetrisBoard) const {
	auto old = wp->getModel();
	wp->setModel(old * mw::getScaleMatrix44(pixlePerSquare_, pixlePerSquare_));

	mw::glEnable(GL_BLEND);
	mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int rows = tetrisBoard.getNbrRows();
	int columns = tetrisBoard.getNbrColumns();

	// Draws the outer square.
	const float red = 0.8f, green = 0.2f, blue = 0.3f;
	GLfloat aVertices[] = {
		0, 0,
		(float) columns, 0,
		0, (float) rows,
		(float) columns, (float) rows
	};
	wp->setColor(red * 0.8f, green * 0.8f, blue * 0.8f, 0.6f);
	wp->setVertexPosition(2, aVertices);
	wp->setTexture(false);
	wp->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	// Draws the inner square.
	wp->setColor(red * 0.1f, green * 0.1f, blue * 0.1f, 0.1f);
	vbo_.bindBuffer();
	wp->setVertexPosition(2, 0);
	wp->glDrawArrays(GL_TRIANGLES, 0, 6 * rows * columns);
	vbo_.unbindBuffer();
	
	mw::glDisable(GL_BLEND);
	
	drawBeginArea(wp);

	wp->setColor(WHITE);
	// Sprite is [-0.5, 0.5] in y and x direction.
	auto old2 = wp->getModel() * mw::getTranslateMatrix44(0.5f, 0.5f);

	// Draw board.
	for (int row = 0; row < rows + 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			BlockType type = tetrisBoard.getBlockType(row, column);
			const mw::Sprite& sprite = getSprite(type);
			wp->setModel(old2 * mw::getTranslateMatrix44((float) column, (float) row));
			sprite.draw();
		}
	}
	
	// Draw block.
	const mw::Sprite& sprite = getSprite(tetrisBoard.getBlockType());
	for (const Square& sq : tetrisBoard.getBlock()) {
		if (sq.row_ < tetrisBoard.getNbrRows() + 2) {
			wp->setModel(old2 * mw::getTranslateMatrix44((float) sq.column_, (float) sq.row_));
			sprite.draw();
		}
	}

	wp->setModel(old);
}

void GraphicBoard::drawBeginArea(gui::WindowMatrixPtr wp) const {
	auto old = wp->getModel();

	int rows = tetrisBoard_.getNbrRows();
	int columns = tetrisBoard_.getNbrColumns();

	wp->setModel(old *  mw::getTranslateMatrix44(0, (float) rows) * mw::getScaleMatrix44((float) columns, 2));
	float red = 0.8f, green = 0.2f, blue = 0.3f;
	wp->setColor(red*0.8f, green*0.8f, blue*0.8f, 0.5f);
	// Draws the outer square.

	GLfloat aVertices[] = {
		0, 0,
		1, 0,
		0, 1,
		1, 1
	};
	wp->setVertexPosition(2, aVertices);
	wp->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	wp->setModel(old);
}
