#ifndef BALANCEBAR_H
#define BALANCEBAR_H

#include <QWidget>
#include <QToolBar>
#include <QList>

class BalanceBar : public QWidget
{
    Q_OBJECT
public:
    explicit BalanceBar(const QList<QAction*>& actionsList, QWidget *parent = nullptr);

//signals:

//public slots:
};

#endif // BALANCEBAR_H
