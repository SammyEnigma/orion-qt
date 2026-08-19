#include "OriLayouts.h"

#include <QApplication>
#include <QGroupBox>
#include <QLabel>
#include <QStyle>

namespace Ori {
namespace Layouts {

static qreal defSpacing(bool vert)
{
    return qApp->style()->pixelMetric(vert ?
        QStyle::PM_LayoutVerticalSpacing : QStyle::PM_LayoutHorizontalSpacing);
}

SpaceH::SpaceH(qreal factor)
{
    _mode = LayoutItemMode::Space;
    _space = qRound(defSpacing(false) * qAbs(factor));
}

SpaceV::SpaceV(qreal factor)
{
    _mode = LayoutItemMode::Space;
    _space = qRound(defSpacing(true) * qAbs(factor));
}

//------------------------------------------------------------------------------
//                                    LayoutItem
//------------------------------------------------------------------------------

LayoutItem::LayoutItem(const QString& label)
{
    _mode = LayoutItemMode::Widget;
    _widget = !label.isEmpty() ? new QLabel(label) : nullptr;
}

LayoutItem::LayoutItem(const char* label)
{
    _mode = LayoutItemMode::Widget;
    _widget = label ? new QLabel(label) : nullptr;
}

class LayoutItemPlacer
{
public:
    static void add(const LayoutItem& item, QBoxLayout* layout)
    {
        switch (item._mode)
        {
        case LayoutItemMode::Layout:
            if (item._layout)
                layout->addLayout(item._layout);
            break;
    
        case LayoutItemMode::Widget:
            if (item._widget)
                layout->addWidget(item._widget);
            break;
    
        case LayoutItemMode::Stretch:
            layout->addStretch();
            break;
    
        case LayoutItemMode::Space:
            layout->addSpacing(item._space);
            break;
        }
    }

    static void add(const LayoutItem& item, QGridLayout* layout, int row, int col)
    {
        switch (item._mode)
        {
        case LayoutItemMode::Layout:
            if (item._layout)
                layout->addLayout(item._layout, row, col);
            break;
        case LayoutItemMode::Widget:
            if (item._widget)
                layout->addWidget(item._widget, row, col);
            break;
        case LayoutItemMode::Stretch:
        case LayoutItemMode::Space:
            break;
        }
    }
};

//------------------------------------------------------------------------------
//                                   LayoutBase
//------------------------------------------------------------------------------

void LayoutBase::_setDefMargins()
{
    auto style = qApp->style();
    _layout->setContentsMargins(
        style->pixelMetric(QStyle::PM_LayoutLeftMargin),
        style->pixelMetric(QStyle::PM_LayoutTopMargin),
        style->pixelMetric(QStyle::PM_LayoutRightMargin),
        style->pixelMetric(QStyle::PM_LayoutBottomMargin));
}

void LayoutBase::_setDefSpacing(qreal factor)
{
    bool vert = qobject_cast<QVBoxLayout*>(_layout);
    _layout->setSpacing(qRound(defSpacing(vert) * factor));
}

QWidget* LayoutBase::makeWidget() const
{
    auto w = new QWidget;
    w->setLayout(_layout);
    return w;
}

QSharedPointer<QWidget> LayoutBase::makeWidgetAuto() const
{
    auto w = new QWidget;
    w->setLayout(_layout);
    return QSharedPointer<QWidget>(w);
}

QGroupBox* LayoutBase::makeGroupBox(const QString& title) const
{
    auto gb = new QGroupBox(title);
    gb->setLayout(_layout);
    return gb;
}

//------------------------------------------------------------------------------
//                                   LayoutBox
//------------------------------------------------------------------------------

void LayoutBox::add(const LayoutItem& item)
{
    LayoutItemPlacer::add(item, boxLayout());
}

void LayoutBox::add(const LayoutItems& items)
{
    auto layout = boxLayout();
    for (const LayoutItem& item : items)
        LayoutItemPlacer::add(item, layout);
}

//------------------------------------------------------------------------------
//                                    Grid
//------------------------------------------------------------------------------

Grid::Grid(const std::initializer_list<LayoutItems>& items) : LayoutBase(new QGridLayout)
{
    auto layout = gridLayout();

    int row = 0;
    for (const auto& rowItems : items)
    {
        int col = 0;
        for (const auto& item : rowItems)
        {
            LayoutItemPlacer::add(item, layout, row, col);
            col++;
        }
        row++;
    }
}

} // namespace Ori
} // namespace Layouts
