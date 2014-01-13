#ifndef GUI_FRAME_H
#define GUI_FRAME_H

#include "panel.h"

#include <mw/window.h>
#include <mw/color.h>
#include <mw/signal.h>

#include <SDL.h>

#include <string>
#include <queue>

namespace gui {

	class Frame;
	using WindowListener = mw::Signal<Frame*, const SDL_Event&>;
	using SdlEventListener = mw::Signal<Frame*, const SDL_Event&>;
	using UpdateListener = mw::Signal<Frame*, Uint32>;

	class Frame : public mw::Window {
	public:
		Frame();
		Frame(int width, int height, bool resizeable = true, std::string title = "Frame", std::string icon = "");

		// Adds default panel in the back in the internal vector.
		// The panel index is returned.
		int addPanelBack();
		
		// Adds the provided panel in the back in the internal vector.
		// The panel index is returned.
		int push_back(Panel* panel);

		// Adds component, with layoutIndex set to default.
		// Will assert if the component already added.
		void add(Component* component);

		// Adds component, with the provided layoutIndex.
		// Will assert if the component already added.
		void add(Component* component, int layoutIndex);

		// Same as add(Component* component) but added to 
		// a traversal group too.
		void addToGroup(Component* component);

		// Same as add(Component* component, int layoutIndex) but added to 
		// a traversal group too.
		void addToGroup(Component* component, int layoutIndex);

		// Sets the layouyt manager. Takes ower the ownership of the layoutManager.
		// The old layoutManager are dealloted.
		void setLayout(LayoutManager* layoutManager);

		// Gets the current layout manager. Do not deallocate the layout manager
		// the panel takes care of that!
		LayoutManager* getLayout() const;

		std::vector<Panel*>::iterator begin();
		std::vector<Panel*>::iterator end();
		std::vector<Panel*>::const_iterator cbegin() const;
		std::vector<Panel*>::const_iterator cend() const;

		mw::signals::Connection addWindowListener(const WindowListener::Callback& callback);
		mw::signals::Connection addSdlEventListener(const SdlEventListener::Callback& callback);

		// Set the default closing true or false.
		// Makes the window's close by clicking the quit button or by 
		// pressing the ESQ key if set to true. if set to false 
		// nothing happens.
		void setDefaultClosing(bool defaultClosing);

		// Returns if the default closing is active or not.
		bool isDefaultClosing() const;

		int getNbrOfPanels() const {
			return panels_.size();
		}

		int getCurrentPanelIndex() const {
			return currentPanel_;
		}

		void setCurrentPanel(int index) {
			if (currentPanel_ != index) {
				getCurrentPanel()->panelChanged(false);
				currentPanel_ = index;
				resize();
				getCurrentPanel()->panelChanged(true);
			}
		}

		inline Panel* getCurrentPanel() const {
			return panels_[currentPanel_];
		}

		inline mw::signals::Connection addKeyListener(const KeyListener::Callback& callback) {
			return getCurrentPanel()->addKeyListener(callback);
		}

		inline mw::signals::Connection addMouseListener(const MouseListener::Callback& callback) {
			return getCurrentPanel()->addMouseListener(callback);
		}

		inline mw::signals::Connection addFocusListener(const FocusListener::Callback& callback) {
			return getCurrentPanel()->addFocusListener(callback);
		}

		inline mw::signals::Connection addActionListener(const ActionListener::Callback& callback) {
			return getCurrentPanel()->addActionListener(callback);
		}

		inline mw::signals::Connection addPanelChangeListener(const PanelChangeListener::Callback& callback) {
			return getCurrentPanel()->addPanelChangeListener(callback);
		}

		inline mw::signals::Connection addUpdateListener(const UpdateListener::Callback& callback) {
			return updateListener_.connect(callback);
		}

	private:
		// Override mw::Window.
		void update(Uint32 deltaTime) override;

		// Override mw::Window.
		void eventUpdate(const SDL_Event& windowEvent) override;

		void resize();

		void init();

		std::queue<SDL_Event> eventQueue_;
		WindowListener windowListener_;
		SdlEventListener sdlEventListener_;
		UpdateListener updateListener_;

		bool defaultClosing_;
		std::vector<Panel*> panels_;
		int currentPanel_;
	};

} // Namespace gui.

#endif // GUI_FRAME_H
