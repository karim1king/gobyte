#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include "amount.h"
#include "transactionfilterproxy.h"
#include "walletmodel.h"

#include <QWidget>
#include <QTableWidget>
#include <memory>

class DashboardButton : public QWidget
{
    Q_OBJECT
public:
    DashboardButton (QString title, QString description, QColor color, QIcon icon);
    void paintEvent(QPaintEvent *event);
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
