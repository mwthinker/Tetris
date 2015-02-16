#ifndef NETWORKEVENT_H
#define NETWORKEVENT_H

#include <vector>

class NetworkEvent {
public:
	virtual ~NetworkEvent() {
	}

protected:
	NetworkEvent() {
	}
};

class NewConnection : public NetworkEvent {
public:
	struct Data {
		Data(int id, int nbr, bool ready) : id_(id), nbr_(nbr), ready_(ready) {
		}

		int id_;
		int nbr_;
		bool ready_;
	};

	enum Status {
		LOCAL,
		SERVER,
		CLIENT
	};

	NewConnection(Status status) : status_(status) {
	}

	void add(int id, int nbrOfPlayers, bool ready) {
		dataV_.push_back(Data(id, nbrOfPlayers, ready));
	}

	std::vector<Data>::const_iterator begin() const {
		return dataV_.cbegin();
	}

	std::vector<Data>::const_iterator end() const {
		return dataV_.cend();
	}

	const Status status_;

private:
	std::vector<Data> dataV_;
};

class GameReady : public NetworkEvent {
public:
	GameReady(int id, bool ready) : id_(id), ready_(ready) {
	}

    int id_;
	bool ready_;
};

class GameStart : public NetworkEvent {
public:
	enum Status {
		LOCAL,
		SERVER,
		CLIENT
	};

	GameStart(Status status) : status_(status) {
	}

	const Status status_;
};

class GamePause : public NetworkEvent {
public:
	GamePause(bool pause) : pause_(pause) {
	}

	bool pause_;
};

class GameOver : public NetworkEvent {
public:
    GameOver(int points) : points_(points) {
    }

    int points_;
};

#endif // NETWORKEVENT_H
