#include "mainwindow.h"

#define MIN_KEY_LENGTH              10

void MainWindow::setupGUI()
{
    QFrame *frame = new QFrame(this);
    this->setCentralWidget(frame);

    QVBoxLayout *layout_main = new QVBoxLayout;
    frame->setLayout(layout_main);

    QGroupBox *group_test_base32 = createBase32Test();
    QGroupBox *group_qr_code = createQrCodeTest();

    layout_main->addWidget(group_test_base32);
    layout_main->addWidget(group_qr_code);
    layout_main->addStretch();
}

QGroupBox *MainWindow::createBase32Test()
{
    QGroupBox *group = new QGroupBox(tr("TOTP Testing"), this);

    QVBoxLayout *layout_group = new QVBoxLayout;
    group->setLayout(layout_group);

    QGridLayout *layout_grp = new QGridLayout;

    layout_group->addLayout(layout_grp);
    layout_group->addStretch();

    this->edit_input_key = new QLineEdit(group);
    this->edit_output_key = new QLineEdit(group);
    this->edit_output_totp = new QLineEdit(group);

    this->edit_output_key->setReadOnly(true);
    this->edit_output_totp->setReadOnly(true);

    QPushButton *btn_test = new QPushButton(tr("Test"), group);
    connect(btn_test, SIGNAL(clicked(bool)), this, SLOT(clicked_test(bool)));

    int row = 0;

    layout_grp->addWidget(new QLabel(tr("Key:"), group), row, 0);
    layout_grp->addWidget(this->edit_input_key, row, 1);
    row++;

    layout_grp->addWidget(new QLabel(tr("Secret Key for App Input:"), group), row, 0);
    layout_grp->addWidget(this->edit_output_key, row, 1);
    row++;

    layout_grp->addWidget(new QLabel(tr("TOTP Output:"), group), row, 0);
    layout_grp->addWidget(this->edit_output_totp, row, 1);
    row++;

    layout_grp->addWidget(btn_test, row, 0, 1, 2);
    row++;

    return group;
}

QGroupBox *MainWindow::createQrCodeTest()
{
    QGroupBox *group = new QGroupBox(tr("QR Code Testing"), this);

    QVBoxLayout *layout_group = new QVBoxLayout;
    group->setLayout(layout_group);

    QGridLayout *layout_input = new QGridLayout;

    this->edit_key_url = new QLineEdit(group);
    this->label_qr_code = new QLabel(group);
    this->edit_issuer = new QLineEdit(group);
    this->edit_account = new QLineEdit(group);

    int row = 0;

    layout_input->addWidget(new QLabel("Account:"), row, 0);
    layout_input->addWidget(this->edit_account, row, 1);
    row++;

    layout_input->addWidget(new QLabel("Issuer:"), row, 0);
    layout_input->addWidget(this->edit_issuer, row, 1);
    row++;

    layout_input->addWidget(new QLabel("Key Url:"), row, 0);
    layout_input->addWidget(this->edit_key_url, row, 1);
    row++;

    this->label_qr_code->setMinimumSize(256, 256);

    layout_group->addLayout(layout_input);
    layout_group->addWidget(this->label_qr_code);

    return group;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->m_key.clear();

    setupGUI();

    connect(&this->m_timer_totp, SIGNAL(timeout()), this, SLOT(timeout_totp()), Qt::QueuedConnection);

    this->m_timer_totp.start(1000);
}

MainWindow::~MainWindow()
{
    this->m_timer_totp.stop();
}

void MainWindow::clicked_test(bool)
{
    QString str_key = this->edit_input_key->text();

    if(str_key.length() < MIN_KEY_LENGTH) {
        QMessageBox::critical(this, tr("Input Key Error"), tr("Key is too short."));
        return;
    }

    this->m_key = str_key;

    QString str_key_base32 = this->m_totp.base32Encoding(str_key);
    this->edit_output_key->setText(str_key_base32);

    QString str_totp = this->m_totp.getCurrentTotpKey(str_key);
    this->edit_output_totp->setText(str_totp);

    QString str_issuer = this->edit_issuer->text();
    QString str_acc = this->edit_account->text();

    if(str_issuer.isEmpty()) {
        str_issuer = "Unknown";
    }

    if(str_acc.isEmpty()) {
        str_acc = "Unknown";
    }

    QString key_url = this->m_totp.getKeyUrl(str_key, str_issuer, str_acc);

    this->edit_key_url->setText(key_url);

    QImage qr_image = this->m_qr_code.getQRCodeImage(key_url);

    QImage out_image = qr_image.scaled(this->label_qr_code->size(), Qt::KeepAspectRatio);

    this->label_qr_code->setPixmap(QPixmap::fromImage(out_image));
}

void MainWindow::timeout_totp()
{
    if(this->m_key.isEmpty()) return;

    QString str_totp = this->m_totp.getCurrentTotpKey(this->m_key);
    this->edit_output_totp->setText(str_totp);
}

