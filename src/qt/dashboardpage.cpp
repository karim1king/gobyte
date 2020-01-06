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
#include <QDateTime>

#include "transactiontablemodel.h"
#include "init.h"
#include "guiutil.h"
#include "dashboardpage.h"
#include "optionsmodel.h"
#include "instantx.h"
#include "clientmodel.h"
#include "transactionrecord.h"
#include "addresstablemodel.h"

#define NUM_ITEMS 5
#define NUM_ITEMS_ADV 7

class LatestTransactionFilterProxy : public TransactionFilterProxy
{
private:
    QStringList columns;
    WalletModel *walletModel;

    enum ColumnIndex {
        BlockID = 0,
        TxID = 1,
        Amount = 2,
        Date = 3,
    };

public:
    LatestTransactionFilterProxy(WalletModel *model)
    {
        walletModel = model;
        columns << tr("Block") << tr("Transaction ID") << BitcoinUnits::getAmountColumnTitle(walletModel->getOptionsModel()->getDisplayUnit()) << tr("Timestamp");
    }

    QString formatTxDate(const TransactionRecord *wtx) const
    {
        if(wtx->time)
        {
            return GUIUtil::dateTimeStr(wtx->time);
        }
        return QString();
    }

    QString lookupAddress(const std::string &address, bool tooltip) const
    {
        QString label = walletModel->getAddressTableModel()->labelForAddress(QString::fromStdString(address));
        QString description;
        if(!label.isEmpty())
        {
            description += label;
        }
        if(label.isEmpty() || tooltip)
        {
            description += QString(" (") + QString::fromStdString(address) + QString(")");
        }
        return description;
    }

    QString formatTxAmount(const TransactionRecord *wtx, bool showUnconfirmed, BitcoinUnits::SeparatorStyle separators) const
    {
        QString str = BitcoinUnits::format(walletModel->getOptionsModel()->getDisplayUnit(), wtx->credit + wtx->debit, false, separators);
        if(showUnconfirmed)
        {
            if(!wtx->status.countsForBalance)
            {
                str = QString("[") + str + QString("]");
            }
        }
        return QString(str);
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();
        TransactionRecord *rec = static_cast<TransactionRecord *>(index.internalPointer());

        switch(role)
        {
            case Qt::DisplayRole:
                switch (index.column()) {
                    case BlockID:
                        return 0;
                    case Date:
                        return formatTxDate(rec);
                    case TxID:
                        return rec->getTxID();
                    case Amount:
                        return formatTxAmount(rec, true, BitcoinUnits::separatorAlways);
                }
                break;
        }

        return QVariant();
    }

    int columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return columns.length();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        if(orientation == Qt::Horizontal)
        {
            if(role == Qt::DisplayRole)
            {
                return columns[section];
            }
            else if (role == Qt::TextAlignmentRole)
            {
                return Qt::AlignLeft;
            }
        }
        return QVariant();
    }
};

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
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
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

void DashboardPage::resizeEvent(QResizeEvent *event) {
    int width = this->width() - 100;
    listTransactions->setColumnWidth(0, width * 0.06);
    listTransactions->setColumnWidth(1, width * 0.55);
    listTransactions->setColumnWidth(2, width * 0.154);
    listTransactions->setColumnWidth(3, width * 0.242);

    QWidget::resizeEvent(event);
}

void DashboardPage::SetupTransactionList(int nNumItems)
{
    //listTransactions->setMinimumHeight(nNumItems * (DECORATION_SIZE + 2));

    if(walletModel && walletModel->getOptionsModel())
    {
        // Set up transaction list
        filter.reset(new LatestTransactionFilterProxy(walletModel));
        filter->setSourceModel(walletModel->getTransactionTableModel());
        filter->setLimit(nNumItems);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->setShowInactive(false);
        filter->sort(TransactionTableModel::Status, Qt::DescendingOrder);

        listTransactions->setModel(filter.get());
        listTransactions->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listTransactions->setAlternatingRowColors(true);
        listTransactions->setSelectionBehavior(QAbstractItemView::SelectRows);
        listTransactions->setSelectionMode(QAbstractItemView::ExtendedSelection);
        listTransactions->sortByColumn(TransactionTableModel::Status, Qt::DescendingOrder);
        listTransactions->verticalHeader()->hide();
        listTransactions->horizontalHeader()->setStretchLastSection(true);
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