#ifndef INPUT_H
#define INPUT_H

struct Input {
	Input() : rotate_(false), down_(false),	left_(false),
		right_(false), downGround_(false) {
	}

	bool rotate_;
	bool down_;
	bool downGround_;
	bool left_;
	bool right_;
};

#endif // INPUT_H
