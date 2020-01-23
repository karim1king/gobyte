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
#include <QTimer>

#include "transactiontablemodel.h"
#include "init.h"
#include "guiutil.h"
#include "dashboardpage.h"
#include "optionsmodel.h"
#include "instantx.h"
#include "clientmodel.h"
#include "transactionrecord.h"
#include "addresstablemodel.h"
#include "masternodeman.h"
#include "masternodeconfig.h"

#define NUM_ITEMS 5
#define NUM_ITEMS_ADV 7

static std::map<MasterNodesState, QString> master_nodes_types {
    {MasterNodesState::Enabled, "Enabled"},
    {MasterNodesState::NewStartReq, "New start required"},
    {MasterNodesState::WatchdogExp, "Watchdog expired"},
    {MasterNodesState::Expired, "Expired"}};

static std::map<MasterNodesState, QString> master_nodes_colors {
    {MasterNodesState::Enabled, "#1FDB8C"},
    {MasterNodesState::NewStartReq, "#2C9BFF"},
    {MasterNodesState::WatchdogExp, "#BB62FF"},
    {MasterNodesState::Expired, "#FFB82E"},
    {MasterNodesState::NONE, "#A7BDD1"}};

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

    void updateDisplayUnit()
    {
        if (columns.size() > 2)
            columns[2] = BitcoinUnits::getAmountColumnTitle(walletModel->getOptionsModel()->getDisplayUnit());
    }

    QString formatTxDate(const TransactionRecord *wtx) const
    {
        if(wtx->time)
        {
            return GUIUtil::dateTimeStr(wtx->time);
        }
        return QString();
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
                        return rec->blockIndex;
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

CommunitySection::CommunitySection()
{
    QLabel* titleLabel = new QLabel("Community");
    titleLabel->setObjectName("titleLabel");
    QLabel* descriptionLabel = new QLabel("Want to meet other GoByte supporters? Check out our social media links");
    descriptionLabel->setObjectName("descriptionLabel");
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setFixedWidth(249);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setSpacing(10);
    vLayout->addWidget(titleLabel);
    vLayout->addWidget(descriptionLabel);

    QHBoxLayout* hLayoutButtons = new QHBoxLayout();
    hLayoutButtons->setSpacing(24);

    QStringList icons {"instagram", "facebook", "m_community", "discord_community", "youtube", "telegram", "reddit", "twitter"};
    for (int i = 0; i < icons.size(); ++i)
    {
        QPushButton* button = new QPushButton();
        button->setIcon(QIcon(":/icons/" + icons[i]));
        button->setIconSize(QSize(22, 22));
        button->setFixedSize(48, 48);
        hLayoutButtons->addWidget(button);
    }

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addLayout(vLayout);
    hLayout->addLayout(hLayoutButtons);
    hLayout->setSpacing(50);
    hLayout->setContentsMargins(32,25,32,25);
    hLayout->addStretch();

    setLayout(hLayout);
}

DashboardButton::DashboardButton (QString title, QString description, QColor color, QIcon icon, bool withLinkButton)
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

    QLabel* iconLabel = new QLabel();
    iconLabel->setPixmap(icon.pixmap(QSize(20, 20)));
    iconLabel->setMaximumSize(20, 20);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setSpacing(12);
    hLayout->addWidget(iconLabel);
    hLayout->addWidget(titleLabel);

    layout->addLayout(hLayout);
    layout->addWidget(descriptionLabel);

    if (withLinkButton)
    {
        QPushButton* linkButton = new QPushButton(QString::fromUtf8("Learn more \u2192"));
        linkButton->setFixedSize(115, 32);
        linkButton->setStyleSheet(linkButton->styleSheet() + "background: " + color.name(QColor::HexArgb) + "; color:" + color.name() + ";");

        layout->addWidget(linkButton);
    }
    else
    {
        QHBoxLayout* hBtnLayout = new QHBoxLayout();
        hBtnLayout->setSpacing(8);

        QList<QString> icons = {"apple_store", "google_play"};
        for (auto& iconName : icons)
        {
            QPushButton* button = new QPushButton();
            button->setIcon(QIcon(":/icons/" + iconName));
            button->setIconSize(QSize(32, 32));
            hBtnLayout->addWidget(button);

            button->setFixedSize(33, 33);
            button->setStyleSheet(button->styleSheet() + "border-radius: 16px;background: #00000000;" + "color:" + color.name() + ";");
        }

        hBtnLayout->setAlignment(Qt::AlignLeft);
        layout->addLayout(hBtnLayout);
    }

    setLayout(layout);
}

void DashboardButton::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

MasternodesChart::MasternodesChart()
{
    setFixedSize(150, 150);

    numLabelStr = QString("<span style='font-size: 18px;color: #FFFFFF;'>%1</span><br>Masternodes");
    numLabel = new QLabel(numLabelStr.arg(0));
    numLabel->setAlignment(Qt::AlignCenter);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(numLabel);
    setLayout (layout);
}

void MasternodesChart::updateValues(std::map<MasterNodesState, int> values)
{
    qreal sum = 0;
    for (auto iter : values)
        sum += iter.second;

    if (sum != 0)
    {
        for (auto iter : values)
            masterNodes[iter.first] = iter.second / sum;
    }

    numLabel->setText(numLabelStr.arg(sum));

    update();
}

void MasternodesChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.translate(4, 4);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal spacing = 2;
    qreal startAngle = 90;

    std::map<MasterNodesState, qreal> values = masterNodes;
    if (values.size() == 0)
        values[MasterNodesState::NONE] = 1;

    for (auto iter = values.rbegin(); iter != values.rend(); ++iter)
    {
        int spanAngle = iter->second * 360 - spacing;

        QPen pen;
        pen.setCapStyle(Qt::FlatCap);
        pen.setColor(QColor(master_nodes_colors[iter->first]));
        pen.setWidth(8);
        p.setPen(pen);
        p.drawArc(QRectF(0, 0, 140, 140), startAngle * 16, spanAngle * 16);

        startAngle += (spanAngle + spacing);
    }
}

class TypeCircle : public QWidget
{
    QColor circleColor;

public:
    TypeCircle(QString color)
    {
        circleColor = color;
        setFixedSize(10, 10);
    }

    void paintEvent(QPaintEvent *event)
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QBrush(circleColor));
        p.drawEllipse(0, 0, 8, 8);
    }
};

DashboardChart::DashboardChart()
{
    QLabel* titleLabel = new QLabel("My masternodes");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setFixedHeight(27);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->addWidget(titleLabel, 0, 0, 1, 2);
    gridLayout->setSpacing(11);

    int rowIndex = 1;
    for (auto iter : master_nodes_types)
    {
        QLabel* label = new QLabel(iter.second);
        label->setObjectName("descriptionLabel");
        gridLayout->addWidget(new TypeCircle(master_nodes_colors[iter.first]), rowIndex, 0);
        gridLayout->addWidget(label, rowIndex, 1);
        statesLabels[iter.first] = label;

        ++rowIndex;
    }

    QHBoxLayout* hLayout = new QHBoxLayout();
    chartView = new MasternodesChart();
    hLayout->addWidget(chartView);
    hLayout->addLayout(gridLayout);

    hLayout->setContentsMargins(32,28,32,28);
    hLayout->setSpacing(40);

    setLayout(hLayout);
}

void DashboardChart::updateValues(const std::map<MasterNodesState, int>& values)
{
    for (auto iter : values)
        if (auto label = statesLabels[iter.first])
            label->setText((master_nodes_types[iter.first] + "<span style='color: #FFFFFF;'> %1</span>").arg(iter.second));

    chartView->updateValues(values);
}

void DashboardChart::paintEvent(QPaintEvent *event)
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
    layout->setContentsMargins(32,43,32,15);

    QString theme = GUIUtil::getThemeName();
    QIcon icon (":/icons/" + theme + "/export");

    QLabel* dashboardLabel = new QLabel("Dashboard");
    dashboardLabel->setObjectName("pageTitleLabel");
    dashboardLabel->setFixedHeight(42);
    layout->addWidget(dashboardLabel, 0, 0);

    QHBoxLayout* hLayout = new QHBoxLayout();
    chart = new DashboardChart();
    chart->setFixedHeight(196);
    hLayout->addWidget(chart);
    DashboardButton* btn = new DashboardButton("Passive income", "Receive passive income by using your GBX coins to form masternodes.", "#1AB551FD", QIcon(":/icons/passive_income"));
    btn->setFixedHeight(196);
    hLayout->addWidget(btn);

    layout->addLayout (hLayout, 1, 0, 1, 3);

    QVBoxLayout* tableLayout = new QVBoxLayout();
    QLabel* titleLabel = new QLabel("My latest transactions");
    titleLabel->setObjectName("titleLabel");
    tableLayout->addWidget(titleLabel);

    listTransactions = new QTableView();
    listTransactions->setShowGrid(false);
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

    btn = new DashboardButton("Mobile application", "To have instant access to your GBX coins use our mobile application", "#1A1FDB8C", QIcon(":/icons/mobile_application"), false);
    btn->setFixedHeight(200);
    layout->addWidget(btn, 3, 2);

    CommunitySection* community = new CommunitySection();
    community->setFixedHeight(123);
    layout->addWidget(community, 4, 0, 1, 3);

    setLayout(layout);

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMyNodeList()));
    timer->start(1000);
}

void CommunitySection::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void DashboardPage::setClientModel(ClientModel* model)
{
    this->clientModel = model;
}

void DashboardPage::updateMyNodeList()
{
    std::map<MasterNodesState, int> numOfMasterNodesByState ={
                            {MasterNodesState::Enabled, 0},
                            {MasterNodesState::NewStartReq, 0},
                            {MasterNodesState::WatchdogExp, 0},
                            {MasterNodesState::Expired, 0}};

    BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries())
    {
        int32_t nOutputIndex = 0;
        if(!ParseInt32(mne.getOutputIndex(), &nOutputIndex))
            continue;

        CMasternode mn;
        bool fFound = mnodeman.Get(COutPoint(uint256S(mne.getTxHash()), nOutputIndex), mn);
        if (!fFound)
            continue;

        MasterNodesState state;
        if (mn.IsEnabled())
            state = MasterNodesState::Enabled;
        else if (mn.IsNewStartRequired())
            state = MasterNodesState::NewStartReq;
        else if (mn.IsWatchdogExpired())
            state = MasterNodesState::WatchdogExp;
        else if (mn.IsExpired())
            state = MasterNodesState::Expired;
        else
            continue;

        numOfMasterNodesByState[state]++;
    }

    chart->updateValues(numOfMasterNodesByState);
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
    if(filter && walletModel && walletModel->getOptionsModel())
    {
/*        nDisplayUnit = walletModel->getOptionsModel()->getDisplayUnit();
        if(currentBalance != -1)
            setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance, currentAnonymizedBalance,
                       currentWatchOnlyBalance, currentWatchUnconfBalance, currentWatchImmatureBalance);

        // Update txdelegate->unit with the current unit
        txdelegate->unit = nDisplayUnit;*/

        ((LatestTransactionFilterProxy*)filter.get())->updateDisplayUnit();
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