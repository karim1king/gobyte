#include "balancebar.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
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


BalanceBar::BalanceBar(QWidget *parent) : QWidget(parent)
{
    QLabel* totalBalance = new QLabel("Total Balance<br>9.49<br> Currency <span style=\"color:#1FB0D0\">USD</span>");
    totalBalance->setObjectName ("totalBalance");

    totalBalance->setFixedHeight(123);
    QListWidget *walletsListView = new QListWidget();
    new WalletInfoButtonWidget(walletsListView);
    walletsListView->setSelectionMode(QAbstractItemView::NoSelection);
    walletsListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    walletsListView->setStyleSheet("QListWidget::item { margin-bottom: 10px;}");//QListWidget {padding: 0px;}

    QList<WalletInfo> list = QList<WalletInfo>() << WalletInfo {"Main adress", "3", "9.49 USD"} << WalletInfo{"Trezor adress", "0", "0 USD"};
    for (int i = 0; i < 6; ++i)
        list << list;
    for (int i = 0; i < list.size(); ++i)
        new WalletInfoWidget(list[i], walletsListView);

    walletsListView->setObjectName ("walletsListView");

    QVBoxLayout *balanceBarLayout = new QVBoxLayout();
    balanceBarLayout->setSpacing(0);
    balanceBarLayout->setContentsMargins(54,-1,54,-1);
    balanceBarLayout->addWidget(totalBalance);
    balanceBarLayout->addWidget(walletsListView);
    balanceBarLayout->setAlignment(Qt::AlignTop);
    setLayout (balanceBarLayout);


    QPalette pal = palette();
}
