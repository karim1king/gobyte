#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QStylePainter>
#include <QToolButton>

#include "balancebar.h"
#include "guiutil.h"

struct WalletInfo
{
    QString title;
    QString gbx;
    QString balance;
    QIcon icon;
};

class WalletInfoButtonWidget : public QListWidgetItem
{
public:
    WalletInfoButtonWidget(QListWidget* view) : QListWidgetItem(view)
    {

        QPushButton* widget = new QPushButton("Add new address");
        widget->setStyleSheet("background-color: transparent; border: 1px dashed #AAAAAA; border-radius: 7px;");

        view->setItemWidget(this, widget);
        setSizeHint(QSize(widget->sizeHint().width(), 95));
    }
};

class WalletInfoWidget : public QListWidgetItem
{
public:
    WalletInfoWidget(const WalletInfo info, QListWidget* view) : QListWidgetItem(view)
    {

        QWidget* widget = new QWidget;
        QGridLayout* layout = new QGridLayout();

        QString theme = GUIUtil::getThemeName();
        QIcon icon (":/icons/" + theme + "/export");

        QLabel* iconLabel = new QLabel();
        iconLabel->setPixmap(icon.pixmap(QSize(64, 64)));
        layout->addWidget(iconLabel, 0, 0);
        layout->addWidget(new QLabel(info.title), 0, 1);
        layout->addWidget(new QLabel(info.gbx + " GBX"), 1, 1);
        layout->addWidget(new QLabel(info.balance + " USD"), 2, 1);

        widget->setStyleSheet("background-color: #232F3B; border-radius: 7px;");
        widget->setLayout(layout);

        view->setItemWidget(this, widget);
        setSizeHint(QSize(widget->sizeHint().width(), 95));
    }
};

class CustomToolButton : public QToolButton
{
public:
    CustomToolButton(QAction* action){
        setDefaultAction (action);
    }
    void paintEvent (QPaintEvent*)
    {
        QStylePainter sp(this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        const QString strText = opt.text;
        const QIcon icn = opt.icon;
        //draw background
        opt.text.clear();
        opt.icon = QIcon();
        sp.drawComplexControl(QStyle::CC_ToolButton, opt);
        //draw content
        QRect rect = opt.rect;
        rect.setLeft (rect.left() + 14);
        sp.drawItemPixmap(rect, Qt::AlignVCenter, icn.pixmap(opt.iconSize));
        opt.text = strText;
        rect.setLeft (rect.left() + opt.iconSize.width() + 12);

        if (isChecked())
            opt.palette.setColor(QPalette::ButtonText, "#FFFFFF");

        sp.drawItemText(rect, Qt::AlignVCenter, opt.palette, true, opt.text, QPalette::ButtonText);
    }
};

BalanceBar::BalanceBar(const QList<QAction*>& actionsList, QWidget *parent) : QWidget(parent)
{
    QToolBar* toolBar = new QToolBar(tr("Tabs toolbar"));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->setMovable(false); // remove unused icon in upper left corner

    for (int i = 0; i < actionsList.size(); ++i){
        CustomToolButton* button = new CustomToolButton(actionsList[i]);
        toolBar->addWidget (button);
    }

    toolBar->setOrientation(Qt::Vertical);
    toolBar->setIconSize(QSize(24, 24));

    QLabel* totalBalance = new QLabel("Total Balance<br>9.49<br> Currency <span style=\"color:#1FB0D0\">USD</span>");
    totalBalance->setObjectName("totalBalance");

    totalBalance->setFixedHeight(123);
/*    QListWidget *walletsListView = new QListWidget();
    new WalletInfoButtonWidget(walletsListView);
    walletsListView->setSelectionMode(QAbstractItemView::NoSelection);
    walletsListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    walletsListView->setStyleSheet("QListWidget::item { margin-bottom: 10px;}");//QListWidget {padding: 0px;}

    QList<WalletInfo> list = QList<WalletInfo>() << WalletInfo {"Main adress", "3", "9.49 USD"} << WalletInfo{"Trezor adress", "0", "0 USD"};
    for (int i = 0; i < 6; ++i)
        list << list;
    for (int i = 0; i < list.size(); ++i)
        new WalletInfoWidget(list[i], walletsListView);

    walletsListView->setObjectName ("walletsListView");*/

    QVBoxLayout* balanceBarLayout = new QVBoxLayout();
    balanceBarLayout->setSpacing(0);
    balanceBarLayout->setContentsMargins(16,0,16,0);
    balanceBarLayout->addWidget(totalBalance);
    balanceBarLayout->addWidget(toolBar);
    balanceBarLayout->setAlignment(Qt::AlignTop);
    setLayout (balanceBarLayout);


    QPalette pal = palette();
}
