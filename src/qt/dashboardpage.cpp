#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QPainter>
#include <QTableWidget>

#include "guiutil.h"
#include "dashboardpage.h"

class DashboardButton : public QWidget
{
public:
    DashboardButton (QString title, QString description, QString color, QIcon icon)
    {

    }

    virtual void paintEvent(QPaintEvent *event)
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
        painter.fillPath(path, QColor("#1FDB8C"));//#03405569
    }
};

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
    DashboardButton* btn = new DashboardButton("Passive income", "Receive passive income by using your GBX coins to form masternodes.", "#BB6BD9", icon);
    btn->setFixedHeight(196);
    hLayout->addWidget(btn);


    btn = new DashboardButton("Passive income", "Receive passive income by using your GBX coins to form masternodes.", "#BB6BD9", icon);
    btn->setFixedHeight(196);
    hLayout->addWidget(btn);

    layout->addLayout (hLayout, 1, 0, 1, 3);

    QTableWidget* latestTransactionsTable = new QTableWidget();
    layout->addWidget(latestTransactionsTable, 2, 0, 1, 3);

    btn = new DashboardButton("Passive income", "Receive passive income by using your GBX coins to form masternodes.", "#BB6BD9", icon);
    btn->setFixedHeight(200);
    layout->addWidget(btn, 3, 0);

    btn = new DashboardButton("Passive income", "Receive passive income by using your GBX coins to form masternodes.", "#BB6BD9", icon);
    btn->setFixedHeight(200);
    layout->addWidget(btn, 3, 1);

    btn = new DashboardButton("Passive income", "Receive passive income by using your GBX coins to form masternodes.", "#BB6BD9", icon);
    btn->setFixedHeight(200);
    layout->addWidget(btn, 3, 2);


    //widget->setStyleSheet("background-color: #232F3B; border-radius: 7px;");
    //widget->setLayout(layout);

    setLayout(layout);
}
