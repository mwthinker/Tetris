#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <list>
#include <tuple>
#include <string>
#include <algorithm>

namespace gui {

	typedef std::tuple<int,std::string,std::string> HighscoreElement;

	class Highscore {
	public:
		Highscore(int nbr) {
			for (int i = 0; i < nbr; ++i) {
				descList_.push_back(HighscoreElement(0,"EMPTY","NO DATE"));
			}
		}

		bool isNewRecord(int record) const {
			for (const HighscoreElement& highscore : descList_) {
				int points = std::get<0>(highscore);
				// New record?
				if (record > points) {
					return true;
				}
			}
			return false;
		}

		void addNewRecord(int record, std::string name, std::string date) {
			descList_.push_back(HighscoreElement(record,name,date));
			sort();
			descList_.pop_back();
		}

		const std::list<HighscoreElement>& get() {
			return descList_;
		}

	private:
		// Sorts the vector in descending order.
		void sort() {			
			descList_.sort([](const HighscoreElement& a, const HighscoreElement& b) {
				// Points A less than points b?
				if (std::get<0>(a) < std::get<0>(b)) {
					return true;
					// Points A equal points b?
				} else if (std::get<0>(a) == std::get<0>(b)) {
					// A has a later date?
					return std::get<2>(a) > std::get<2>(b);
				}
				// Points B larger than points A.
				return false;
			});
		}

		std::list<HighscoreElement> descList_;
	};

} // Namespace gui.

#endif // HIGHSCORE_H