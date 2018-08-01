#include "guicomponentfactory.h"

#include "tetrisdata.h"

std::shared_ptr<gui::Panel> GuiComponentFactory::createBar() const {
	auto panel = std::make_shared<gui::Panel>();
	panel->setPreferredSize(TetrisData::getInstance().getWindowBarHeight(), TetrisData::getInstance().getWindowBarHeight());
	panel->setBackgroundColor(TetrisData::getInstance().getWindowBarColor());
	panel->setLayout<gui::FlowLayout>(gui::FlowLayout::LEFT, 5.f, 0.f);
	return panel;
}

std::shared_ptr<gui::Panel> GuiComponentFactory::createBackground(const mw::Sprite& background) const {
	auto panel = std::make_shared<gui::Panel>();
	panel->setBackground(background);
	panel->setLayout<gui::BorderLayout>();
	return panel;
}

std::shared_ptr<gui::Label> GuiComponentFactory::createLabel(std::string text, int size) const {
	auto& font = TetrisData::getInstance().getDefaultFont(size);
	auto label = std::make_shared<gui::Label>(text, font);
	label->setTextColor(TetrisData::getInstance().getLabelTextColor());
	label->setBackgroundColor(TetrisData::getInstance().getLabelBackgroundColor());
	return label;
}

std::shared_ptr<gui::TextField> GuiComponentFactory::createTextField(std::string text, int size) const {
	auto& font = TetrisData::getInstance().getDefaultFont(size);
	return std::make_shared<gui::TextField>(text, font);;
}

std::shared_ptr<gui::Button> GuiComponentFactory::createButton(std::string text, int size) const {
	auto& font = TetrisData::getInstance().getDefaultFont(size);
	auto button = std::make_shared<gui::Button>(text, font);
	button->setFocusColor(TetrisData::getInstance().getButtonFocusColor());
	button->setTextColor(TetrisData::getInstance().getButtonTextColor());
	button->setHoverColor(TetrisData::getInstance().getButtonHoverColor());
	button->setPushColor(TetrisData::getInstance().getButtonPushColor());
	button->setBackgroundColor(TetrisData::getInstance().getButtonBackgroundColor());
	button->setBorderColor(TetrisData::getInstance().getButtonBorderColor());
	button->setAutoSizeToFitText(true);
	return button;
}

std::shared_ptr<gui::CheckBox> GuiComponentFactory::createCheckBox(std::string text, int size) const {
	auto& font = TetrisData::getInstance().getDefaultFont(size);
	auto checkBox = std::make_shared<gui::CheckBox>(text, font, TetrisData::getInstance().getCheckboxBoxSprite(), TetrisData::getInstance().getCheckboxCheckSprite());
	checkBox->setTextColor(TetrisData::getInstance().getCheckboxTextColor());
	checkBox->setBackgroundColor(TetrisData::getInstance().getCheckboxBackgroundColor());
	checkBox->setBoxColor(TetrisData::getInstance().getCheckboxBoxColor());
	checkBox->setCheckColor(TetrisData::getInstance().getChecboxCheckColor());
	return checkBox;
}

std::shared_ptr<gui::CheckBox> GuiComponentFactory::createRadioButton(std::string text, int size) const {
	auto& font = TetrisData::getInstance().getDefaultFont(size);
	auto radioButton = std::make_shared<gui::CheckBox>(text, font, TetrisData::getInstance().getRadioButtonBoxSprite(), TetrisData::getInstance().getRadioButtonCheckSprite());
	radioButton->setTextColor(TetrisData::getInstance().getRadioButtonTextColor());
	radioButton->setBackgroundColor(TetrisData::getInstance().getRadioButtonBackgroundColor());
	radioButton->setBoxColor(TetrisData::getInstance().getRadioButtonBoxColor());
	radioButton->setCheckColor(TetrisData::getInstance().getRadioButtonCheckColor());
	return radioButton;
}

std::shared_ptr<gui::Panel> GuiComponentFactory::createTransparentPanel(float preferredWidth, float preferredHeight) const {
	auto panel = std::make_shared<gui::Panel>();
	panel->setBackgroundColor(1, 1, 1, 0);
	panel->setPreferredSize(preferredWidth, preferredHeight);
	return panel;
}

std::shared_ptr<gui::ComboBox> GuiComponentFactory::createComboBox(int size) const {
	auto& font = TetrisData::getInstance().getDefaultFont(size);
	auto comboBox = std::make_shared<gui::ComboBox>(font);
	comboBox->setFocusColor(TetrisData::getInstance().getComboBoxFocusColor());
	comboBox->setTextColor(TetrisData::getInstance().getComboBoxTextColor());
	comboBox->setSelectedBackgroundColor(TetrisData::getInstance().getComboBoxSelectedBackgroundColor());
	comboBox->setSelectedTextColor(TetrisData::getInstance().getComboBoxSelectedTextColor());
	comboBox->setShowDropDownColor(TetrisData::getInstance().getComboBoxShowDropDownColor());
	comboBox->setBackgroundColor(TetrisData::getInstance().getComboBoxBackgroundColor());
	comboBox->setBorderColor(TetrisData::getInstance().getComboBoxBorderColor());
	return comboBox;
}

std::shared_ptr<gui::ProgressBar> GuiComponentFactory::createProgressBar() const {
	return std::make_shared<gui::ProgressBar>();
}

std::shared_ptr<ManButton> GuiComponentFactory::createManButton(unsigned int max, const mw::Sprite& man, const mw::Sprite& cross) const {
	return std::make_shared<ManButton>(max, man, cross);
}
