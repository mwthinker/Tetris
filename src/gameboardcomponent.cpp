#include "gameboardcomponent.h"
#include "tetrisdata.h"

void GameboardComponent::draw(const gui::Graphic& graphic, double deltaTime) {

}

GameboardComponent::GameboardComponent() {

}

GameboardComponent::~GameboardComponent() {

}

void GameboardComponent::addDrawRowAtTheTop(const TetrisBoard& tetrisBoard, int nbr) {
	int highestRow = tetrisBoard.getBoardVector().size() / tetrisBoard.getColumns();
	for (int i = 0; i < nbr; ++i) {
		auto drawRow = freeRows_.front();
		freeRows_.pop_front();
		drawRow->init(highestRow - i - 1, tetrisBoard);
		rows_.push_back(drawRow);
	}
}


void GameboardComponent::initStaticBackground(const LightningShader& lightningShader, const BoardShader& boardShader, Player& player) {
	const TetrisBoard& tetrisBoard = player.getTetrisBoard();
	const Color c1 = TetrisData::getInstance().getOuterSquareColor();
	const Color c2 = TetrisData::getInstance().getInnerSquareColor();
	const Color c3 = TetrisData::getInstance().getStartAreaColor();
	const Color c4 = TetrisData::getInstance().getPlayerAreaColor();	
		
	const float borderSize = TetrisData::getInstance().getTetrisBorderSize();

	const int columns = tetrisBoard.getColumns();
	const int rows = tetrisBoard.getRows();

	const float middleDistance = 5;
	const float rightDistance = 5;

	gui::Dimension dim = getSize();
	const float squareSize = dim.width_ / columns;

	mw::Buffer staticBuffer(mw::Buffer::STATIC);
	staticVertexData_ = std::make_shared<BoardVertexData>(boardShader);
	staticBuffer.addVertexData(staticVertexData_);
	staticVertexData_->begin();

	float x = 0;
	float y = 0;

	// Draw the outer square.
	x = 0;
	y = 0;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		dim.width_, dim.height_,
		c1);

	// Draw the inner squares.
	for (int row = 0; row < rows - 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			x = borderSize + squareSize * column + squareSize * 0.1f;
			y = borderSize + squareSize * row + squareSize * 0.1f;
			staticVertexData_->addSquareTRIANGLES(
				x, y,
				squareSize * 0.8f, squareSize * 0.8f,
				c2);
		}
	}

	// Draw the block start area.
	x = borderSize;
	y = borderSize + squareSize * (rows - 4);
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		squareSize * columns, squareSize * 2,
		c3);

	staticVertexData_->end();
	staticBuffer.uploadToGraphicCard();

	rows_.clear();

	mw::Buffer dynamicBuffer(mw::Buffer::DYNAMIC);

	currentBlockPtr_ = std::make_shared<DrawBlock>(boardShader, tetrisBoard.getBlock(), tetrisBoard.getRows(), squareSize, borderSize, borderSize, false);
	dynamicBuffer.addVertexData(currentBlockPtr_);

	for (int row = 0; row < rows; ++row) {
		auto drawRow = std::make_shared<DrawRow>(boardShader, row, tetrisBoard, squareSize, borderSize, borderSize);
		dynamicBuffer.addVertexData(drawRow);
		rows_.push_back(drawRow);
	}

	dynamicBuffer.uploadToGraphicCard();

	/*
	std::vector<Vec2> points_;
	for (int i = 0; i < 11; ++i) {
		points_.push_back(Vec2(lowX + (i + 1) * squareSize, 20 * squareSize));
	}

	mw::Sprite halfCircle = TetrisData::getInstance().getHalfCircleSprite();
	mw::Sprite lineSegment = TetrisData::getInstance().getLineSegmentSprite();
	*/

	//lightningBoltCluster_ = LightningBoltCluster(lightningShader, halfCircle, lineSegment, points_, 50, 100, 100);
	//lightningBoltCluster_.restart(points_, 1);

	/*
	std::vector<Vec2> points;
	for (int column = 0; column < tetrisBoard.getColumns(); ++column) {
	BlockType type = tetrisBoard.getBlockType(row->getRow() - 1, column);
	points.push_back(Vec2(lowX_ + (column + 1) * squareSize_, lowY_ + (row->getRow() + 1) * squareSize_));
	}
	lightningBoltCluster_.restart(points, 5);
	*/
}
