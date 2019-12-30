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

    layout->addWidget(titleLabel);
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


    btn = new DashboardButton("Passive income", "Receive passive income by using your GBX coins to form masternodes.", "#1AB551FD", icon);
    btn->setFixedHeight(196);
    hLayout->addWidget(btn);

    layout->addLayout (hLayout, 1, 0, 1, 3);

    QVBoxLayout* tableLayout = new QVBoxLayout();
    QLabel* titleLabel = new QLabel("My latest transactions");
    titleLabel->setObjectName("titleLabel");
    tableLayout->addWidget(titleLabel);

    QTableWidget* latestTransactionsTable = new QTableWidget();
    tableLayout->addWidget(latestTransactionsTable);
    tableLayout->setContentsMargins(24,23,24,26);
    tableLayout->setSpacing(14);

    QWidget* container = new QWidget();
    container->setLayout (tableLayout);
    container->setObjectName("tableLayout");
    layout->addWidget(container, 2, 0, 1, 3);

    btn = new DashboardButton("Hardware wallets", "Secure your GBX and more. Give yourself peace of mind by knowing that your coins are safe.", "#1AFFB82E", icon);
    btn->setFixedHeight(200);
    layout->addWidget(btn, 3, 0);

    btn = new DashboardButton("Bounty", "Additional methods of earning extra GBX coins to multiply your funds", "#1A2C9BFF", icon);
    btn->setFixedHeight(200);
    layout->addWidget(btn, 3, 1);

    btn = new DashboardButton("Mobile application", "To have instant access to your GBX coins use our mobile application", "#1A1FDB8C", icon);
    btn->setFixedHeight(200);
    layout->addWidget(btn, 3, 2);


    //widget->setStyleSheet("background-color: #232F3B; border-radius: 7px;");
    //widget->setLayout(layout);

    setLayout(layout);
}
