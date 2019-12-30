#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>

class DashboardPage : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardPage(QWidget *parent = nullptr);

//signals:

//public slots:
};

class DashboardButton : public QWidget
{
    Q_OBJECT
public:
    DashboardButton (QString title, QString description, QColor color, QIcon icon);
    void paintEvent(QPaintEvent *event);
};

#endif // DASHBOARDPAGE_H
