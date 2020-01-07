#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include "amount.h"
#include "transactionfilterproxy.h"
#include "walletmodel.h"

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <memory>

class DashboardButton : public QWidget
{
    Q_OBJECT
public:
    DashboardButton (QString title, QString description, QColor color, QIcon icon);
    void paintEvent(QPaintEvent *event);
};

class MasternodesChart : public QWidget
{
    Q_OBJECT
public:
    MasternodesChart(QList<QString> typeColors);
    void paintEvent(QPaintEvent *);

private:
    QString numLabelStr;
    QList<qreal> masterNodes;
    QList<QString> colors;
};

class DashboardChart : public QWidget
{
    Q_OBJECT
public:
    DashboardChart();
    void paintEvent(QPaintEvent *event);

private:
    void updateValues();
    const QList<QString> types {"Enabled", "New start required", "Watchdog expired", "Expired"};
    const QList<QString> colors {"#1FDB8C", "#2C9BFF", "#BB62FF", "#FFB82E"};
    QList<QLabel*> typeLabels;
};

class DashboardPage : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardPage(QWidget *parent = nullptr);
    void setWalletModel(WalletModel *model);
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void updateAdvancedPSUI(bool fShowAdvancedPSUI);
    void updateDisplayUnit();
    void setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, const CAmount& anonymizedBalance,
                    const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);

private:
    void SetupTransactionList(int nNumItems);

    WalletModel *walletModel;
    QTableView *listTransactions;
    std::unique_ptr<TransactionFilterProxy> filter;
};

#endif // DASHBOARDPAGE_H
