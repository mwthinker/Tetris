#include "gamesprite.h"

const mw::Sprite spriteBackground("images/background.png");

const mw::Texture textureSquares("images/tetris.png");

const mw::Sprite spriteMan(textureSquares, 64, 64, 64, 64);
const mw::Sprite spriteComputer(textureSquares, 448, 0, 64, 64);
const mw::Sprite spriteCross(textureSquares, 0, 64, 64, 64);

const mw::Sprite spriteI(textureSquares, 193, 1, 62, 62);
const mw::Sprite spriteJ(textureSquares, 129, 1, 62, 62);
const mw::Sprite spriteL(textureSquares, 257, 1, 62, 62);
const mw::Sprite spriteO(textureSquares, 385, 1, 62, 62);
const mw::Sprite spriteS(textureSquares, 65, 1, 62, 62);
const mw::Sprite spriteT(textureSquares, 321, 1, 62, 62);
const mw::Sprite spriteZ(textureSquares, 1, 1, 62, 62);
