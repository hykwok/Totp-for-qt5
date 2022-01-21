#ifndef CQRCODE_H
#define CQRCODE_H

#include <QtCore>
#include <QImage>

class CQRCode : public QObject
{
    Q_OBJECT

private:

protected:

public:
    explicit CQRCode(QObject *parent = nullptr);

    QImage getQRCodeImage(QString message);

signals:

};

#endif // CQRCODE_H
