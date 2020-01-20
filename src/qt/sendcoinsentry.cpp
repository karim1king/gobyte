// Copyright (c) 2011-2015 The Bitcoin Core developers
// Copyright (c) 2014-2017 The Dash Core developers
// Copyright (c) 2017-2019 The GoByte Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "sendcoinsentry.h"
#include "ui_sendcoinsentry.h"

#include "addressbookpage.h"
#include "addresstablemodel.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "platformstyle.h"
#include "walletmodel.h"

#include <QApplication>
#include <QClipboard>
#include <QPainter>

SendCoinsEntry::SendCoinsEntry(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    model(0),
    platformStyle(platformStyle)
{
//    if (platformStyle->getUseExtraSpacing())
//        ui->payToLayout->setSpacing(4);

    deleteButton = new QPushButton();
    checkboxSubtractFeeFromAmount = new QCheckBox("Substract fee from amount");
    payTo = new QValidatedLineEdit(this);
    //payTo->setMinimumHeight(40);
    payAmount = new BitcoinAmountField();
    addAsLabel = new QLineEdit();
    titleLabel = new QLabel("Recipient");
    titleLabel->setObjectName("titleLabel");

#if QT_VERSION >= 0x040700
    addAsLabel->setPlaceholderText(tr("Enter a label for this address to add it to your address book"));
#endif

    QGridLayout* layout = new QGridLayout();
    layout->setVerticalSpacing(7);
    layout->setHorizontalSpacing(16);
    layout->setContentsMargins(32,24,22,32);

    layout->addWidget(titleLabel, 0, 0);
    QSpacerItem* spacer = new QSpacerItem(1, 8, QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addItem(spacer, 1, 0);

    QLabel* title = new QLabel("Send to");
    title->setObjectName("miniTitle");
    layout->addWidget(title, 2, 0);
    title = new QLabel("Amount");
    title->setObjectName("miniTitle");
    layout->addWidget(title, 2, 1);
    title = new QLabel("Units");
    title->setObjectName("miniTitle");
    layout->addWidget(title, 2, 2);

    layout->addWidget(payTo, 3, 0);
    layout->addWidget(payAmount->getAmountSpin(), 3, 1);
    layout->addWidget(payAmount->getUnitBox(), 3, 2);
    payAmount->setFixedWidth(300);

    spacer = new QSpacerItem(1, 8, QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addItem(spacer, 4, 0);

    title = new QLabel("Label");
    title->setObjectName("miniTitle");
    layout->addWidget(title, 5, 0);
    layout->addWidget(addAsLabel, 6, 0);
    layout->addWidget(checkboxSubtractFeeFromAmount, 6, 1);
    //setFixedHeight (233);
    //setMinimumSize(233, 896);
    setLayout(layout);


    //layout->setSpacing(16);
    //layout->setContentsMargins(32,43,32,15);
    QString theme = GUIUtil::getThemeName();

    // These icons are needed on Mac also!
    //addressBookButton->setIcon(QIcon(":/icons/" + theme + "/address-book"));
    //pasteButton->setIcon(QIcon(":/icons/" + theme + "/editpaste"));
    deleteButton->setIcon(QIcon(":/icons/" + theme + "/remove"));
//    ui->deleteButton_is->setIcon(QIcon(":/icons/" + theme + "/remove"));
//    ui->deleteButton_s->setIcon(QIcon(":/icons/" + theme + "/remove"));

    // normal gobyte address field
    GUIUtil::setupAddressWidget(payTo, this);
    // just a label for displaying gobyte address(es)
    //ui->payTo_is->setFont(GUIUtil::fixedPitchFont());

    // Connect signals
    connect(payAmount, SIGNAL(valueChanged()), this, SIGNAL(payAmountChanged()));
    connect(checkboxSubtractFeeFromAmount, SIGNAL(toggled(bool)), this, SIGNAL(subtractFeeFromAmountChanged()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    //connect(ui->deleteButton_is, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    //connect(ui->deleteButton_s, SIGNAL(clicked()), this, SLOT(deleteClicked()));
}

SendCoinsEntry::~SendCoinsEntry()
{
}

void SendCoinsEntry::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SendCoinsEntry::on_pasteButton_clicked()
{
    // Paste text from clipboard into recipient field
    payTo->setText(QApplication::clipboard()->text());
}

void SendCoinsEntry::on_addressBookButton_clicked()
{
    if(!model)
        return;
    AddressBookPage dlg(platformStyle, AddressBookPage::ForSelection, AddressBookPage::SendingTab, this);
    dlg.setModel(model->getAddressTableModel());
    if(dlg.exec())
    {
        payTo->setText(dlg.getReturnValue());
        payAmount->setFocus();
    }
}

void SendCoinsEntry::on_payTo_textChanged(const QString &address)
{
    updateLabel(address);
}

void SendCoinsEntry::setModel(WalletModel *model)
{
    this->model = model;

    if (model && model->getOptionsModel())
        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

    clear();
}

void SendCoinsEntry::clear()
{
    // clear UI elements for normal payment
    payTo->clear();
    addAsLabel->clear();
    payAmount->clear();
    checkboxSubtractFeeFromAmount->setCheckState(Qt::Unchecked);
//    ui->messageTextLabel->clear();
//    ui->messageTextLabel->hide();
//    ui->messageLabel->hide();
//    // clear UI elements for unauthenticated payment request
//    ui->payTo_is->clear();
//    ui->memoTextLabel_is->clear();
//    ui->payAmount_is->clear();
//    // clear UI elements for authenticated payment request
//    ui->payTo_s->clear();
//    ui->memoTextLabel_s->clear();
//    ui->payAmount_s->clear();

    // update the display unit, to not use the default ("BTC")
    updateDisplayUnit();
}

void SendCoinsEntry::deleteClicked()
{
    Q_EMIT removeEntry(this);
}

bool SendCoinsEntry::validate()
{
    if (!model)
        return false;

    // Check input validity
    bool retval = true;

    // Skip checks for payment request
    if (recipient.paymentRequest.IsInitialized())
        return retval;

    if (!model->validateAddress(payTo->text()))
    {
        payTo->setValid(false);
        retval = false;
    }

    if (!payAmount->validate())
    {
        retval = false;
    }

    // Sending a zero amount is invalid
    if (payAmount->value(0) <= 0)
    {
        payAmount->setValid(false);
        retval = false;
    }

    // Reject dust outputs:
    if (retval && GUIUtil::isDust(payTo->text(), payAmount->value())) {
        payAmount->setValid(false);
        retval = false;
    }

    return retval;
}

SendCoinsRecipient SendCoinsEntry::getValue()
{
    // Payment request
    if (recipient.paymentRequest.IsInitialized())
        return recipient;

    // Normal payment
    recipient.address = payTo->text();
    recipient.label = addAsLabel->text();
    recipient.amount = payAmount->value();
    //recipient.message = ui->messageTextLabel->text();
    recipient.fSubtractFeeFromAmount = (checkboxSubtractFeeFromAmount->checkState() == Qt::Checked);

    return recipient;
}

QWidget *SendCoinsEntry::setupTabChain(QWidget *prev)
{
    QWidget::setTabOrder(prev, payTo);
    QWidget::setTabOrder(payTo, addAsLabel);
    QWidget *w = payAmount->setupTabChain(addAsLabel);
    QWidget::setTabOrder(w, checkboxSubtractFeeFromAmount);
//    QWidget::setTabOrder(ui->checkboxSubtractFeeFromAmount, ui->addressBookButton);
//    QWidget::setTabOrder(ui->addressBookButton, ui->pasteButton);
//    QWidget::setTabOrder(ui->pasteButton, ui->deleteButton);
    return deleteButton;
}

void SendCoinsEntry::setValue(const SendCoinsRecipient &value)
{
    recipient = value;

    if (recipient.paymentRequest.IsInitialized()) // payment request
    {
        if (recipient.authenticatedMerchant.isEmpty()) // unauthenticated
        {
//            ui->payTo_is->setText(recipient.address);
//            ui->memoTextLabel_is->setText(recipient.message);
//            ui->payAmount_is->setValue(recipient.amount);
//            ui->payAmount_is->setReadOnly(true);
//            setCurrentWidget(ui->SendCoins_UnauthenticatedPaymentRequest);
        }
        else // authenticated
        {
//            ui->payTo_s->setText(recipient.authenticatedMerchant);
//            ui->memoTextLabel_s->setText(recipient.message);
//            ui->payAmount_s->setValue(recipient.amount);
//            ui->payAmount_s->setReadOnly(true);
//            setCurrentWidget(ui->SendCoins_AuthenticatedPaymentRequest);
        }
    }
    else // normal payment
    {
        // message
        //ui->messageTextLabel->setText(recipient.message);
        //ui->messageTextLabel->setVisible(!recipient.message.isEmpty());
        //ui->messageLabel->setVisible(!recipient.message.isEmpty());

        addAsLabel->clear();
        payTo->setText(recipient.address); // this may set a label from addressbook
        if (!recipient.label.isEmpty()) // if a label had been set from the addressbook, don't overwrite with an empty label
            addAsLabel->setText(recipient.label);
        payAmount->setValue(recipient.amount);
    }
}

void SendCoinsEntry::setAddress(const QString &address)
{
    payTo->setText(address);
    payAmount->setFocus();
}

void SendCoinsEntry::setIndex(int index)
{
    titleLabel->setText("Recipient " +  QString::number(index));
}

bool SendCoinsEntry::isClear()
{
    return payTo->text().isEmpty();// && payTo_is->text().isEmpty() && payTo_s->text().isEmpty();
}

void SendCoinsEntry::setFocus()
{
    payTo->setFocus();
}

void SendCoinsEntry::updateDisplayUnit()
{
    if(model && model->getOptionsModel())
    {
        // Update payAmount with the current unit
        payAmount->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
        //ui->payAmount_is->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
        //ui->payAmount_s->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
    }
}

bool SendCoinsEntry::updateLabel(const QString &address)
{
    if(!model)
        return false;

    // Fill in label from address book, if address has an associated label
    QString associatedLabel = model->getAddressTableModel()->labelForAddress(address);
    if(!associatedLabel.isEmpty())
    {
        addAsLabel->setText(associatedLabel);
        return true;
    }

    return false;
}
