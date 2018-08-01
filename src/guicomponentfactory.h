#ifndef GUICOMPONENTFACTORY_H
#define GUICOMPONENTFACTORY_H

#include "tetrisdata.h"
#include "manbutton.h"

#include <string>
#include <functional>

#include <gui/panel.h>
#include <gui/label.h>
#include <gui/button.h>
#include <gui/textfield.h>
#include <gui/checkbox.h>
#include <gui/flowlayout.h>
#include <gui/borderlayout.h>
#include <gui/combobox.h>
#include <gui/progressbar.h>

class GuiComponentFactory {
public:
	virtual std::shared_ptr<gui::Panel> createBar() const;
	
	virtual std::shared_ptr<gui::Panel> createBackground(const mw::Sprite& background) const;
	
	virtual std::shared_ptr<gui::Label> createLabel(std::string text, int size) const;

	virtual std::shared_ptr<gui::TextField> createTextField(std::string text, int size) const;
	
	virtual std::shared_ptr<gui::Button> createButton(std::string text, int size) const;
	
	virtual std::shared_ptr<gui::CheckBox> createCheckBox(std::string text, int size) const;
	
	virtual std::shared_ptr<gui::CheckBox> createRadioButton(std::string text, int size) const;

	virtual std::shared_ptr<gui::Panel> createTransparentPanel(float preferredWidth, float preferredHeight) const;
	
	virtual std::shared_ptr<gui::ComboBox> createComboBox(int size) const;

	virtual std::shared_ptr<gui::ProgressBar> createProgressBar() const;

	virtual std::shared_ptr<ManButton> createManButton(unsigned int max, const mw::Sprite& man, const mw::Sprite& cross) const;

	virtual ~GuiComponentFactory() = default;
};

#endif // GUICOMPONENTFACTORY_H
