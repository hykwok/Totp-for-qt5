#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtWidgets>

#include "ctotp.h"
#include "cqrcode.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    CTOTP               m_totp;
    CQRCode             m_qr_code;

    QLineEdit           *edit_input_key;
    QLineEdit           *edit_output_key;
    QLineEdit           *edit_output_totp;

    QLineEdit           *edit_issuer;
    QLineEdit           *edit_account;
    QLineEdit           *edit_key_url;
    QLabel              *label_qr_code;

    QString             m_key;

    QTimer              m_timer_totp;

    void setupGUI();

    QGroupBox *createBase32Test();
    QGroupBox *createQrCodeTest();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:

private slots:
    void clicked_test(bool);

    void timeout_totp();

};
#endif // MAINWINDOW_H
