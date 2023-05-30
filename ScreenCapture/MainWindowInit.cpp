#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Icon.h"


void MainWindow::initToolMain()
{
    ui->btnRectEllipse->setFont(Icon::font);
    ui->btnRectEllipse->setText(Icon::icons[Icon::Name::rectEllipse]);
    QObject::connect(ui->btnRectEllipse,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnArrow->setFont(Icon::font);
    ui->btnArrow->setText(Icon::icons[Icon::Name::arrow]);
    connect(ui->btnArrow,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnPen->setFont(Icon::font);
    ui->btnPen->setText(Icon::icons[Icon::Name::line]);
    connect(ui->btnPen,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnMosaic->setFont(Icon::font);
    ui->btnMosaic->setText(Icon::icons[Icon::Name::mosaic]);
    QObject::connect(ui->btnMosaic,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnEraser->setFont(Icon::font);
    ui->btnEraser->setText(Icon::icons[Icon::Name::eraser]);
    QObject::connect(ui->btnEraser,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

            /*<item>
         <widget class="QPushButton" name="btnNumber">
          <property name="sizePolicy">
           <sizepolicy hsizetype="Preferred" vsizetype="Preferred">
            <horstretch>0</horstretch>
            <verstretch>0</verstretch>
           </sizepolicy>
          </property>
          <property name="minimumSize">
           <size>
            <width>34</width>
            <height>0</height>
           </size>
          </property>
          <property name="maximumSize">
           <size>
            <width>34</width>
            <height>16777215</height>
           </size>
          </property>
          <property name="font">
           <font>
            <stylestrategy>PreferAntialias</stylestrategy>
           </font>
          </property>
          <property name="cursor">
           <cursorShape>PointingHandCursor</cursorShape>
          </property>
          <property name="text">
           <string/>
          </property>
          <property name="checkable">
           <bool>true</bool>
          </property>
          <property name="autoExclusive">
           <bool>true</bool>
          </property>
         </widget>
        </item>*/
    //ui->btnNumber->setFont(Icon::font);
    //ui->btnNumber->setText(Icon::icons[Icon::Name::number]);
    //QObject::connect(ui->btnNumber,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnText->setFont(Icon::font);
    ui->btnText->setText(Icon::icons[Icon::Name::text]);
    QObject::connect(ui->btnText,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnUndo->setFont(Icon::font);
    ui->btnUndo->setText(Icon::icons[Icon::Name::undo]);
    QObject::connect(ui->btnUndo,  &QPushButton::clicked, this, &MainWindow::undo);

    ui->btnRedo->setFont(Icon::font);
    ui->btnRedo->setText(Icon::icons[Icon::Name::redo]);
    QObject::connect(ui->btnRedo,  &QPushButton::clicked, this, &MainWindow::redo);

    ui->btnOk->setFont(Icon::font);
    ui->btnOk->setText(Icon::icons[Icon::Name::ok]);
    QObject::connect(ui->btnOk,  &QPushButton::clicked, this, &MainWindow::saveToClipboard);

    ui->btnSave->setFont(Icon::font);
    ui->btnSave->setText(Icon::icons[Icon::Name::save]);
    QObject::connect(ui->btnSave,  &QPushButton::clicked, this, &MainWindow::saveToFile);

    ui->btnCopy->setFont(Icon::font);
    ui->btnCopy->setText(Icon::icons[Icon::Name::copy]);
    QObject::connect(ui->btnCopy,  &QPushButton::clicked, this, &MainWindow::saveToClipboard);

    ui->toolMain->hide();
    ui->toolMain->setStyleSheet(style.arg("toolMain"));
}

void MainWindow::initToolPen()
{
    dotPen = new ButtonDot(this);
    dotPen->setCheckable(false);
    auto box = qobject_cast<QHBoxLayout*>(ui->toolPen->layout());
    box->insertWidget(0, dotPen);
    ui->toolPen->hide();
    ui->toolPen->setStyleSheet(style.arg("toolPen"));
}

void MainWindow::initToolMosaic()
{
    auto box = qobject_cast<QHBoxLayout*>(ui->toolMosaic->layout());
    dotMosaic = new ButtonDot(this);
    dotMosaic->setCheckable(false);
    dotMosaic->setFontSize(20);
    box->insertWidget(0, dotMosaic);
    ui->toolMosaic->hide();
    ui->toolMosaic->setStyleSheet(style.arg("toolMosaic"));
}

void MainWindow::initToolEraser()
{
    auto box = qobject_cast<QHBoxLayout*>(ui->toolEraser->layout());
    dotEraser = new ButtonDot(this);
    dotEraser->setCheckable(false);
    dotEraser->setFontSize(20);
    box->insertWidget(0, dotEraser);
    ui->toolEraser->hide();
    ui->toolEraser->setStyleSheet(style.arg("toolEraser"));
}

void MainWindow::initToolRect()
{
    auto box = qobject_cast<QHBoxLayout*>(ui->btnRectEllipseFill->parentWidget()->layout());
    dotRectEllipse = new ButtonDot(this);
    box->insertWidget(0, dotRectEllipse);

    ui->btnRectEllipseFill->setFont(Icon::font);
    ui->btnRectEllipseFill->setText(Icon::icons[Icon::Name::rectFill]);
    QObject::connect(ui->btnRectEllipseFill,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnRect->setFont(Icon::font);
    ui->btnRect->setText(Icon::icons[Icon::Name::rect]);
    QObject::connect(ui->btnRect,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnEllipse->setFont(Icon::font);
    ui->btnEllipse->setText(Icon::icons[Icon::Name::ellipse]);
    QObject::connect(ui->btnEllipse,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolRectEllipse->hide();
    ui->toolRectEllipse->setStyleSheet(style.arg("toolRectEllipse"));
}

void MainWindow::initToolArrow()
{
    ui->btnArrowFill->setFont(Icon::font);
    ui->btnArrowFill->setText(Icon::icons[Icon::Name::arrowFill]);
    QObject::connect(ui->btnArrowFill,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnArrowEmpty->setFont(Icon::font);
    ui->btnArrowEmpty->setText(Icon::icons[Icon::Name::arrowEmpty]);
    QObject::connect(ui->btnArrowEmpty,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolArrow->hide();
    ui->toolArrow->setStyleSheet(style.arg("toolArrow"));
}

void MainWindow::initToolText()
{
    textInputBox = new TextInputBox(this);
    connect(textInputBox, &TextInputBox::doubleClicked, this, &MainWindow::mouseDoubleClickEvent);

    auto box = qobject_cast<QHBoxLayout*>(ui->toolText->layout());
    dotText = new ButtonDot(this);
    dotText->setFontSize(16);
    dotText->setCheckable(false);
    box->insertWidget(0, dotText);

    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(this->dotText->size + 20);
    this->textInputBox->setFont(font);

    connect(dotText,  &ButtonDot::sizeChanged, this, [this]()
    {
        this->textInputBox->setFontSizeCustom(this->dotText->size);
    });

    connect(colorSelector,  &ColorSelector::colorChanged, this, [this]()
    {
        this->textInputBox->setColorCustom(this->colorSelector->currentColor());
    });

    connect(ui->fontSelectBox, &QComboBox::currentTextChanged, this, [this]()
    {
        this->textInputBox->setFontFamilyCustom(this->ui->fontSelectBox->currentText());        
    });

    ui->btnTextBold->setFont(Icon::font);
    ui->btnTextBold->setText(Icon::icons[Icon::Name::bold]);
    connect(ui->btnTextBold,  &QPushButton::clicked, this, [this]()
    {
        QFont font = this->textInputBox->font();
        font.setBold(this->ui->btnTextBold->isChecked());
        this->textInputBox->setFont(font);
    });

    ui->btnTextItalic->setFont(Icon::font);
    ui->btnTextItalic->setText(Icon::icons[Icon::Name::italic]);
    connect(ui->btnTextItalic,  &QPushButton::clicked, this, [this]()
    {
        QFont font = this->textInputBox->font();
        font.setItalic(ui->btnTextItalic->isChecked());
        this->textInputBox->setFont(font);
    });

    ui->toolText->hide();
    ui->toolText->setStyleSheet(style.arg("toolText"));

    textInputBox->hide();
}
