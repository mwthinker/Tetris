#ifndef NETWORKLOOBY_H
#define NETWORKLOOBY_H

#include "guiitem.h"

#include <mw/text.h>
#include <mw/color.h>

#include <list>
#include <string>
#include <memory>
#include <functional>

class NetworkLooby : public gui::GuiItem {
public:
	NetworkLooby(int nbrOfLocalPlayers, int localId) {

	}

	void draw() override {

	}

private:
	struct TextElement {
		TextElement(int intPoints, const mw::Text& points, const mw::Text& name, const mw::Text& date) {
			intPoints_ = intPoints;
			points_ = points;
			name_ = name;
			date_ = date;
		}

		int intPoints_;
		mw::Text points_, name_, date_;
	};

	// Sorts the vector in descending order.
	void sort();

	std::vector<mw::Text> numbers_;
	std::list<TextElement> ascList_;
	mw::Color color_;
	int nextRecord_;
};

typedef std::shared_ptr<NetworkLooby> NetworkLoobyPtr;

NetworkLoobyPtr createNetworkLooby(int nbr, const mw::Color&);

#endif // NETWORKLOOBY_H
