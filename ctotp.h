#ifndef CTOTP_H
#define CTOTP_H

#include <QtCore>

class CTOTP : public QObject
{
    Q_OBJECT

public:
    enum {
        DEFAULT_INIT_HOTP_COUNTER = 1
    };

private:
    typedef union _totp_counter_ {
        qint64          raw_64;
        qint8           raw_8[8];
    } TOTP_COUNTER;

    typedef union _totp_out_value_ {
        quint32         raw_32;
        qint8           raw_8[4];
    } TOTP_OUT_VALUE;

private:
    int                         no_of_digial;

protected:
    QByteArray base32Encoding(QByteArray input_raw, bool haspad);

    QByteArray convertToBigEndian(qint64 value);

public:
    explicit CTOTP(QObject *parent = nullptr);

    QString base32Encoding(QString value, bool haspad=false);

    QString getTotpKeyUrl(QString key, QString issuer, QString accountname);
    QString getHotpKeyUrl(QString key, QString issuer, QString accountname);

    QString getCurrentTotpPassword(QString key, int offset=0);
    QString getCurrentHotpPassword(QString key, int counter);

signals:

public slots:

};

#endif // CTOTP_H
