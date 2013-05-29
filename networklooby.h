#ifndef NETWORKLOOBY_H
#define NETWORKLOOBY_H

#include "guiitem.h"
#include "gamefont.h"

#include <mw/text.h>
#include <mw/color.h>

#include <list>
#include <string>
#include <sstream>
#include <memory>
#include <functional>

class NetworkLooby : public gui::GuiItem {
public:
	NetworkLooby();

	void draw() override;
	void addConnection(int id, int nbrOfPlayers);
	void removeConnection(int id);
	void setReady(int id, bool ready);
	void clear();

private:
	struct Connection {
		Connection();
		Connection(int intId, int intNbrOfPlayers);

		int intId_;
		int intNbrOfPlayers_;
		bool boolReady_;
		mw::Text id_, numberOfPlayers_, ready_;
	};
	
	std::list<Connection> ascList_;
	mw::Color color_;
	int nextRecord_;
};

typedef std::shared_ptr<NetworkLooby> NetworkLoobyPtr;

NetworkLoobyPtr createNetworkLooby();

#endif // NETWORKLOOBY_H
