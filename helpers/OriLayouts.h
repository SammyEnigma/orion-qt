#ifndef ORI_LAYOUTS_H
#define ORI_LAYOUTS_H

#include <QWidget>
#include <QBoxLayout>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
class QGroupBox;
QT_END_NAMESPACE

namespace Ori {
namespace Layouts {

enum class LayoutItemMode { Layout, Widget, Stretch, Space };

class LayoutItem
{
public:
    LayoutItem(QWidget* widget) { _mode = LayoutItemMode::Widget; _widget = widget; }
    LayoutItem(QLayout* layout) { _mode = LayoutItemMode::Layout; _layout = layout; }
    LayoutItem(const QString& label);
    LayoutItem(const char* label);

protected:
    LayoutItem() {}

    LayoutItemMode _mode;
    union
    {
        QLayout* _layout = nullptr;
        QWidget* _widget;
        int _space;
    };
    
    friend class LayoutItemPlacer;
};


class Stretch : public LayoutItem
{
public:
    Stretch() { _mode = LayoutItemMode::Stretch; }
};


/// Gets space as hardcoded value
class Space : public LayoutItem
{
public:
    Space(int size) { _mode = LayoutItemMode::Space; _space = size; }
};


/// Gets space as factor of default layout horizontal spacing
class SpaceH : public LayoutItem
{
public:
    SpaceH(qreal factor = 1);
};


/// Gets space as factor of default layout vertical spacing
class SpaceV : public LayoutItem
{
public:
    SpaceV(qreal factor = 1);
};


typedef std::initializer_list<LayoutItem> LayoutItems;


class LayoutBase : public LayoutItem
{
public:
    QWidget* makeWidget() const;
    QSharedPointer<QWidget> makeWidgetAuto() const;
    QGroupBox* makeGroupBox(const QString& title) const;
    
protected:
    LayoutBase(QLayout* layout) : LayoutItem(layout) {}
    
    void _setDefMargins();
    void _setDefSpacing(qreal factor);
    void _setMargin(int value) { _layout->setContentsMargins(value, value, value, value); }
    void _setMargins(int left, int top, int right, int bottom) { _layout->setContentsMargins(left, top, right, bottom); }
    void _setSpacing(int value) { _layout->setSpacing(value); }
    void _useFor(QWidget* parent) { parent->setLayout(_layout); }
};


class LayoutBox : public LayoutBase
{
public:
    LayoutBox& setDefMargins() { _setDefMargins(); return *this; }
    LayoutBox& setDefSpacing(qreal factor = 1) { _setDefSpacing(factor); return *this; }
    LayoutBox& setMargin(int value) { _setMargin(value); return *this; }
    LayoutBox& setMargins(int left, int top, int right, int bottom) { _setMargins(left, top, right, bottom); return *this; }
    LayoutBox& setSpacing(int value) { _setSpacing(value); return *this; }
    LayoutBox& useFor(QWidget* parent) { _useFor(parent); return *this; }
    LayoutBox& setStretchFactor(QWidget* w, int s) { boxLayout()->setStretchFactor(w, s); return *this; }

    QBoxLayout* boxLayout() const { return qobject_cast<QBoxLayout*>(_layout); }

    void add(const LayoutItem& item);
    void add(const LayoutItems& items);

protected:
    LayoutBox(QBoxLayout* layout, const LayoutItems& items) : LayoutBase(layout)
    {
        add(items);
    }
};


class LayoutH : public LayoutBox
{
public:
    LayoutH(const LayoutItems& items) : LayoutBox(new QHBoxLayout, items) {}
};


class LayoutV : public LayoutBox
{
public:
    LayoutV(const LayoutItems& items) : LayoutBox(new QVBoxLayout, items) {}
};


class Grid : public LayoutBase
{
public:
    Grid(const std::initializer_list<LayoutItems>& items);

    Grid& setDefMargins() { _setDefMargins(); return *this; }
    Grid& setDefSpacing(qreal factor = 1) { _setDefSpacing(factor); return *this; }
    Grid& setMargin(int value) { _setMargin(value); return *this; }
    Grid& setMargins(int left, int top, int right, int bottom) { _setMargins(left, top, right, bottom); return *this; }
    Grid& setSpacing(int value) { _setSpacing(value); return *this; }
    Grid& useFor(QWidget* parent) { _useFor(parent); return *this; }

    QGridLayout* gridLayout() const { return qobject_cast<QGridLayout*>(_layout); }
};

} // namespace Ori
} // namespace Layouts

#endif // ORI_LAYOUTS_H
