```
#include "QRCode.h"
#include "BarcodeFormat.h"
#include "BitMatrix.h"
#include "MultiFormatWriter.h"

#include "ReadBarcode.h"

QRCode::QRCode(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
	//auto format = ZXing::BarcodeFormatFromString("QR_CODE");
	//auto writer = ZXing::MultiFormatWriter(format);
	//writer.setEncoding(ZXing::CharacterSet::UTF8);
	//QString text1 = "中文，abc,111222333";
	//QString text = QString(text1.toUtf8());
	//auto matrix = writer.encode(text.toStdString(), 600, 600);
	//auto bitmap = ZXing::ToMatrix<uint8_t>(matrix);
	//auto img = QImage(bitmap.data(), bitmap.width(), bitmap.height(), bitmap.width(), QImage::Format::Format_Grayscale8).copy();
	//img.save("1212.png");


	QImage image = QImage("下载.png"); //
	if (image.format() != QImage::Format_Grayscale8) {
		image = image.convertToFormat(QImage::Format_Grayscale8);
		qDebug() << "Converted to Grayscale8 format.";
	}
	auto options = ZXing::ReaderOptions()
		.setCharacterSet(ZXing::CharacterSet::UTF8)
		.setFormats(ZXing::BarcodeFormat::QRCode) // 明确指定 QR Code
		.setTryInvert(true)                      // 允许尝试反色
		.setTryRotate(true)                      // 允许尝试旋转
		.setTextMode(ZXing::TextMode::HRI)
		.setMaxNumberOfSymbols(10);
	ZXing::ImageView imgView(image.bits(), image.width(), image.height(), ZXing::ImageFormat::Lum, image.bytesPerLine(), 1);
	auto barcodes = ZXing::ReadBarcodes(imgView, options);
	for (auto& barcode : barcodes) {
		qDebug() << "Text:   " << QString::fromUtf8(barcode.text());
	}
}

QRCode::~QRCode()
{

}
```