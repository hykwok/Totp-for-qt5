#include "cqrcode.h"

#include "qrcodegen.hpp"

#include <QtDebug>

#define DEFAULT_MIN_QR_CODE_VERSION             5
#define DEFAULT_MAX_QR_CODE_VERSION             40
#define DEFAULT_QR_CODE_MASK                    3

CQRCode::CQRCode(QObject *parent) : QObject(parent)
{

}

QImage CQRCode::getQRCodeImage(QString message)
{
    std::vector<qrcodegen::QrSegment> segs = qrcodegen::QrSegment::makeSegments(message.toUtf8());

    qrcodegen::QrCode qr_code = qrcodegen::QrCode::encodeSegments(segs, qrcodegen::QrCode::Ecc::HIGH, DEFAULT_MIN_QR_CODE_VERSION, DEFAULT_MAX_QR_CODE_VERSION, DEFAULT_QR_CODE_MASK, false);

    int qr_code_size = qr_code.getSize();

    QImage qr_image = QImage(qr_code_size, qr_code_size, QImage::Format_RGB888);

    for(int y=0; y<qr_code_size; y++) {
        for(int x=0; x<qr_code_size; x++) {
            if(qr_code.getModule(x, y)) {
                qr_image.setPixel(x, y, qRgb(0, 0, 0));
            } else {
                qr_image.setPixel(x, y, qRgb(255, 255, 255));
            }
        } // x loop
    } // y loop

    return qr_image;
}
