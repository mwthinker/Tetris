#ifndef GUITYPEDEFS_H
#define GUITYPEDEFS_H

#include <memory>

namespace gui {

	class Bar;
	typedef std::shared_ptr<Bar> BarPtr;

	class BarColor;
	typedef std::shared_ptr<BarColor> BarColorPtr;

	class TextItem;
	typedef std::shared_ptr<TextItem> TextItemPtr;

	class TextBox;
	typedef std::shared_ptr<TextBox> TextBoxPtr;

	class TextBoxDraw;
	typedef std::shared_ptr<TextBoxDraw> TextBoxDrawPtr;

	class Frame;
	typedef std::shared_ptr<Frame> FramePtr;

	class Widget;
	typedef std::shared_ptr<Widget> WidgetPtr;

	class Group;
	typedef std::shared_ptr<Group> GroupPtr;

	class Background;
	typedef std::shared_ptr<Background> BackgroundPtr;

	class Button;
	typedef std::shared_ptr<Button> ButtonPtr;

	class TextButton;
	typedef std::shared_ptr<TextButton> TextButtonPtr;

} // Namespace gui.

#endif // GUITYPEDEFS_H
