#ifndef NETWORKEVENT_H
#define NETWORKEVENT_H

#include <memory>
#include <functional>

#include <mw/packet.h>

class NetworkEvent;
typedef std::shared_ptr<NetworkEvent> NetworkEventPtr;

class NetworkEvent {
protected:
	NetworkEvent() {
	}

public:
    virtual ~NetworkEvent() {
    }
};

class NewConnection : public NetworkEvent {
public:
	enum Status {
		LOCAL,
		SERVER,
		CLIENT
	};

	NewConnection(Status status) : status_(status) {
	}

	void add(int id, int nbrOfPlayers, bool ready) {
		dataV.push_back(Data(id, nbrOfPlayers,ready));
	}

	void iterate(std::function<void(int id, int nbrOfPlayers, bool ready)> funcIdPort) {
		for (const Data& data : dataV) {
			funcIdPort(data.id_,data.nbr_,data.ready_);
		}
	}

	const Status status_;

private:
	struct Data {
		Data(int id, int nbr, bool ready) : id_(id), nbr_(nbr), ready_(ready) {
		}

		int id_;
		int nbr_;
		bool ready_;
	};

	std::vector<Data> dataV;
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
