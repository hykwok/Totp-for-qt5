#include "ctotp.h"

#include <QtDebug>

#define DEFAULT_NO_OF_DIGITAL           6

#define TOTP_TIME_INTERVAL              30          // in second

QByteArray CTOTP::base32Encoding(QByteArray input_raw, bool haspad)
{
    QByteArray ret_value;

    if(input_raw.isEmpty()) return ret_value;

    // alphabet set, which is defined in RFC 4648
    const QString alphabet_set = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

    int input_size = input_raw.size();

    int output_size = input_size / 5;

    if(input_size % 5) {
        output_size++;
    }

    output_size *= 8;

    quint16 left_data = 0;
    quint8 remain_bit = 0;

    for(int i=0; i<input_size; i++) {
        quint8 raw_value = input_raw.at(i);
        left_data = (left_data << 8) | raw_value;
        remain_bit += 8;

        while(remain_bit >= 5) {
            quint8 index = (left_data >> (remain_bit - 5)) & 0x1F;

            ret_value.append(alphabet_set.at(index));

            remain_bit -= 5;
        }
    } // i loop

    for(int i=ret_value.size(); i<output_size; i++) {
        if(remain_bit > 0) {
            quint8 index = (left_data << (5 - remain_bit)) & 0x1F;
            ret_value.append(alphabet_set.at(index));

            // clear data
            left_data = 0;
            remain_bit = 0;
        } else {
            if(haspad) {
                // pad character, which is defined in RFC 4648
                ret_value.append('=');
            } else {
                break;
            }
        }
    } // i loop

    return ret_value;
}

QByteArray CTOTP::convertToBigEndian(qint64 value)
{
    QByteArray out;

    TOTP_COUNTER temp;

    temp.raw_64 = value;

    for(int i=0; i<8; i++) {
        out.append(temp.raw_8[7 - i]);
    } // i loop

    return out;
}

CTOTP::CTOTP(QObject *parent) : QObject(parent)
{
    this->no_of_digial = DEFAULT_NO_OF_DIGITAL;
 }

QString CTOTP::base32Encoding(QString value, bool haspad)
{
    // convert to UTF 8
    QByteArray input_raw = value.toUtf8();

    QByteArray out_raw = base32Encoding(input_raw, haspad);

    return QString::fromUtf8(out_raw);
}

QString CTOTP::getTotpKeyUrl(QString key, QString issuer, QString accountname)
{
    // Reference: https://github.com/google/google-authenticator/wiki/Key-Uri-Format

    // for TOTP
    QString str_type = "totp";

    QString str_label;

    QString url_issuer = QUrl::toPercentEncoding(issuer);
    QString url_accountname = QUrl::toPercentEncoding(accountname);

    str_label = QString("%1:%2").arg(url_issuer).arg(url_accountname);

    QString str_key = base32Encoding(key);

    QString str_param;
    str_param = QString("secret=%1&issuer=%2").arg(str_key).arg(url_issuer);

    return QString("otpauth://%1/%2?%3").arg(str_type).arg(str_label).arg(str_param);
}

QString CTOTP::getHotpKeyUrl(QString key, QString issuer, QString accountname)
{
    // Reference: https://github.com/google/google-authenticator/wiki/Key-Uri-Format

    // for HOTP
    QString str_type = "hotp";

    QString str_label;

    QString url_issuer = QUrl::toPercentEncoding(issuer);
    QString url_accountname = QUrl::toPercentEncoding(accountname);

    str_label = QString("%1:%2").arg(url_issuer).arg(url_accountname);

    QString str_key = base32Encoding(key);

    QString str_param;
    str_param = QString("secret=%1&issuer=%2&counter=%3").arg(str_key).arg(url_issuer).arg(DEFAULT_INIT_HOTP_COUNTER);

    return QString("otpauth://%1/%2?%3").arg(str_type).arg(str_label).arg(str_param);
}

QString CTOTP::getCurrentTotpPassword(QString key, int offset)
{
    // prepare TOTP (refer to RFC 6238)
    qint64 ctime = QDateTime::currentMSecsSinceEpoch();
 
    qint64 counter = floor(ctime / (TOTP_TIME_INTERVAL * 1000));
    counter += offset;

    return getCurrentHotpPassword(key, counter);
}

QString CTOTP::getCurrentHotpPassword(QString key, int counter)
{
    // get HOTP (refer to RFC 4226)
    QByteArray raw_key = key.toUtf8();

    QByteArray raw_challenge = convertToBigEndian(counter);

    QByteArray hash_result = QMessageAuthenticationCode::hash(raw_challenge, raw_key, QCryptographicHash::Sha1);

    // get truncated hash

    // 19 = size of SHA1 hash - 1
    int truncated_hash_offset = hash_result.at(19) & 0x0F;

    TOTP_OUT_VALUE out_value;

    // convert to little endian
    for(int i=0; i<4; i++) {
        if(i) {
            out_value.raw_8[3 - i] = hash_result.at(truncated_hash_offset + i) & 0xFF;
        } else {
            out_value.raw_8[3 - i] = hash_result.at(truncated_hash_offset + i) & 0x7F;
        }
    } // i loop

    int mod = qPow(10, this->no_of_digial);

    // 6 digital
    quint32 truncatedHash = out_value.raw_32 % mod;

    QString str_ret;
    str_ret.setNum(truncatedHash);

    // pad zero
    int len = str_ret.size();

    for(int i=len; i<this->no_of_digial; i++) {
        str_ret.prepend("0");
    } // i loop

    return str_ret;
}
