#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QPainter>
#include <QTableWidget>
#include <QAbstractItemDelegate>

#include "transactiontablemodel.h"
#include "init.h"
#include "guiutil.h"
#include "dashboardpage.h"
#include "optionsmodel.h"
#include "instantx.h"
#include "clientmodel.h"

#define NUM_ITEMS 5
#define NUM_ITEMS_ADV 7

DashboardButton::DashboardButton (QString title, QString description, QColor color, QIcon icon)
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(18);
    layout->setContentsMargins(32,25,32,32);
    QLabel* titleLabel = new QLabel(title);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setFixedHeight(27);
    QLabel* descriptionLabel = new QLabel(description);
    descriptionLabel->setObjectName("descriptionLabel");
    descriptionLabel->setWordWrap (true);
    descriptionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    descriptionLabel->setFixedWidth(250);
    QPushButton* linkButton = new QPushButton(QString::fromUtf8("Learn more \u2192"));
    linkButton->setFixedSize(115, 32);
    linkButton->setStyleSheet(linkButton->styleSheet() + "background: " + color.name(QColor::HexArgb) + "; color:" + color.name() + ";");

    QLabel* iconLabel = new QLabel();
    iconLabel->setPixmap(icon.pixmap(QSize(20, 20)));
    iconLabel->setMaximumSize(20, 20);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setSpacing(12);
    hLayout->addWidget(iconLabel);
    hLayout->addWidget(titleLabel);

    layout->addLayout(hLayout);
    layout->addWidget(descriptionLabel);
    layout->addWidget(linkButton);

    setLayout(layout);
}

void DashboardButton::paintEvent(QPaintEvent *event)
{
    // call the base class paint event method
    // this will draw the base class content
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.setPen(QPen(QColor("#03405569")));
    //painter.setBrush(QBrush(QColor("#03405569")));
    QPainterPath path;
    path.addRoundedRect(QRect(0, 0, width(), height()), 5, 5);
    painter.fillPath(path, QColor("#1A405569"));//#03405569
}

DashboardPage::DashboardPage(QWidget *parent) : QWidget(parent)
{
    //QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(16);
    layout->setContentsMargins(32,43,32,43);

    QString theme = GUIUtil::getThemeName();
    QIcon icon (":/icons/" + theme + "/export");

/*    QLabel* iconLabel = new QLabel();
    iconLabel->setPixmap(icon.pixmap(QSize(64, 64)));
    layout->addWidget(iconLabel, 0, 0);*/
    QLabel* dashboardLabel = new QLabel("Dashboard");
    dashboardLabel->setObjectName("dashboardLabel");
    dashboardLabel->setFixedHeight(42);
    layout->addWidget(dashboardLabel, 0, 0);

    QHBoxLayout* hLayout = new QHBoxLayout();
    DashboardButton* btn = new DashboardButton("", "", "", icon);
    btn->setFixedHeight(196);
    hLayout->addWidget(btn);


    btn = new DashboardButton("Passive income", "Receive passive income by using your GBX coins to form masternodes.", "#1AB551FD", QIcon(":/icons/passive_income"));
    btn->setFixedHeight(196);
    hLayout->addWidget(btn);

    layout->addLayout (hLayout, 1, 0, 1, 3);

    QVBoxLayout* tableLayout = new QVBoxLayout();
    QLabel* titleLabel = new QLabel("My latest transactions");
    titleLabel->setObjectName("titleLabel");
    tableLayout->addWidget(titleLabel);

    listTransactions = new QTableView();
    tableLayout->addWidget(listTransactions);
    tableLayout->setContentsMargins(24,23,24,26);
    tableLayout->setSpacing(14);

    QWidget* container = new QWidget();
    container->setLayout (tableLayout);
    container->setObjectName("tableLayout");
    layout->addWidget(container, 2, 0, 1, 3);

    btn = new DashboardButton("Hardware wallets", "Secure your GBX and more. Give yourself peace of mind by knowing that your coins are safe.", "#1AFFB82E", QIcon(":/icons/hardware_wallets"));
    btn->setFixedHeight(200);
    layout->addWidget(btn, 3, 0);

    btn = new DashboardButton("Bounty", "Additional methods of earning extra GBX coins to multiply your funds", "#1A2C9BFF", QIcon(":/icons/bounty"));
    btn->setFixedHeight(200);
    layout->addWidget(btn, 3, 1);

    btn = new DashboardButton("Mobile application", "To have instant access to your GBX coins use our mobile application", "#1A1FDB8C", QIcon(":/icons/mobile_application"));
    btn->setFixedHeight(200);
    layout->addWidget(btn, 3, 2);


    //widget->setStyleSheet("background-color: #232F3B; border-radius: 7px;");
    //widget->setLayout(layout);

    setLayout(layout);
}

void DashboardPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if(model && model->getOptionsModel())
    {
        // update the display unit, to not use the default ("GBX")
        updateDisplayUnit();
        // Keep up to date with wallet
        setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance(), model->getAnonymizedBalance(),
                   model->getWatchBalance(), model->getWatchUnconfirmedBalance(), model->getWatchImmatureBalance());
        connect(model, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)), this, SLOT(setBalance(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)));

        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
        connect(model->getOptionsModel(), SIGNAL(advancedPSUIChanged(bool)), this, SLOT(updateAdvancedPSUI(bool)));
        // explicitly update PS frame and transaction list to reflect actual settings
        updateAdvancedPSUI(model->getOptionsModel()->getShowAdvancedPSUI());
    }
}

void DashboardPage::updateDisplayUnit()
{
    if(walletModel && walletModel->getOptionsModel())
    {
/*        nDisplayUnit = walletModel->getOptionsModel()->getDisplayUnit();
        if(currentBalance != -1)
            setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance, currentAnonymizedBalance,
                       currentWatchOnlyBalance, currentWatchUnconfBalance, currentWatchImmatureBalance);

        // Update txdelegate->unit with the current unit
        txdelegate->unit = nDisplayUnit;*/

        listTransactions->update();
    }
}

void DashboardPage::SetupTransactionList(int nNumItems)
{
    //listTransactions->setMinimumHeight(nNumItems * (DECORATION_SIZE + 2));

    if(walletModel && walletModel->getOptionsModel())
    {
        // Set up transaction list
        filter.reset(new TransactionFilterProxy());
        filter->setSourceModel(walletModel->getTransactionTableModel());
        filter->setLimit(nNumItems);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->setShowInactive(false);
        filter->sort(TransactionTableModel::Status, Qt::DescendingOrder);

        listTransactions->setModel(filter.get());
        //listTransactions->setModelColumn(TransactionTableModel::ToAddress);
    }
}

void DashboardPage::setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, const CAmount& anonymizedBalance, const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance)
{
    static int cachedTxLocks = 0;

    if(cachedTxLocks != nCompleteTXLocks){
        cachedTxLocks = nCompleteTXLocks;
        listTransactions->update();
    }
}

void DashboardPage::updateAdvancedPSUI(bool fShowAdvancedPSUI)
{
    int nNumItems = (fLiteMode || !fShowAdvancedPSUI) ? NUM_ITEMS : NUM_ITEMS_ADV;
    SetupTransactionList(nNumItems);
}