#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include "amount.h"
#include "transactionfilterproxy.h"
#include "walletmodel.h"
#include "clientmodel.h"
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <memory>
#include <map>

enum class MasterNodesState {Enabled, NewStartReq, WatchdogExp, Expired};

class DashboardButton : public QWidget
{
    Q_OBJECT
public:
    DashboardButton(QString title, QString description, QColor color, QIcon icon);
    void paintEvent(QPaintEvent *event);
};

class MasternodesChart : public QWidget
{
    Q_OBJECT
public:
    MasternodesChart();
    void paintEvent(QPaintEvent*);
    void updateValues(std::map<MasterNodesState, int> values);

private:
    QString numLabelStr;
    QLabel* numLabel;
    std::map<MasterNodesState, qreal> masterNodes;
};

class DashboardChart : public QWidget
{
    Q_OBJECT
public:
    DashboardChart();
    void paintEvent(QPaintEvent *event);
    void updateValues(const std::map<MasterNodesState, int>& values);

private:
    std::map<MasterNodesState, QLabel*> statesLabels;
    MasternodesChart* chartView;
};

class DashboardPage : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardPage(QWidget *parent = nullptr);
    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *model);
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void updateNodeList();
    void updateAdvancedPSUI(bool fShowAdvancedPSUI);
    void updateDisplayUnit();
    void setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, const CAmount& anonymizedBalance,
                    const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);

private:
    void SetupTransactionList(int nNumItems);

    WalletModel* walletModel;
    ClientModel* clientModel;
    QTableView* listTransactions;
    DashboardChart* chart;
    std::unique_ptr<TransactionFilterProxy> filter;
};

#endif // DASHBOARDPAGE_H
