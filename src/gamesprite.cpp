#include "gamesprite.h"

const mw::Sprite spriteBackground("images/background.png");
const mw::Texture textureSquares("images/tetris.png");

// Top bar.
const mw::Sprite spriteComputer(textureSquares,   1, 1, 62, 62);
const mw::Sprite spriteCross(textureSquares,     65, 1, 62, 62);
const mw::Sprite spriteMan(textureSquares,      129, 1, 62, 62);
const mw::Sprite spriteZoom(textureSquares,     193, 1, 62, 62);

// Tetris squares.
const mw::Sprite spriteZ(textureSquares, 1, 65, 62, 62);
const mw::Sprite spriteS(textureSquares, 65, 65, 62, 62);
const mw::Sprite spriteJ(textureSquares, 129, 65, 62, 62);
const mw::Sprite spriteI(textureSquares, 193, 65, 62, 62);
const mw::Sprite spriteL(textureSquares, 257, 65, 62, 62);
const mw::Sprite spriteT(textureSquares, 321, 65, 62, 62);
const mw::Sprite spriteO(textureSquares, 385, 65, 62, 62);
