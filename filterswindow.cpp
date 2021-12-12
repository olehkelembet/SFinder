#include "filterswindow.h"
#include "ui_filterswindow.h"



FiltersWindow::FiltersWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FiltersWindow)
{
    ui->setupUi(this);
    ui->filtersWidget->setHidden(true);

    connect(ui->ascii, SIGNAL(toggled(bool)), this, SIGNAL(filtersChanged()));
    connect(ui->unicode, SIGNAL(toggled(bool)), this, SIGNAL(filtersChanged()));
    connect(ui->editable, SIGNAL(toggled(bool)), this, SIGNAL(filtersChanged()));
    connect(ui->filtersButton, SIGNAL(toggled(bool)), this, SLOT(filtersToggled(bool)));
    connect(ui->search_string_ledit, SIGNAL(returnPressed()), this, SIGNAL(filtersChanged()));

    setStringSearchFieldAttributes();
}

FiltersWindow::~FiltersWindow()
{
    delete ui;
}

bool FiltersWindow::getAsciiChecked()
{
    return ui->ascii->isChecked();
}

bool FiltersWindow::getUnicodeChecked()
{
    return ui->unicode->isChecked();
}

void FiltersWindow::setAsciiChecked(bool state)
{
    ui->ascii->setChecked(state);
}

void FiltersWindow::setUnicodeChecked(bool state)
{
    ui->unicode->setChecked(state);
}

int FiltersWindow::getStringSearchLen()
{
    if(ui->search_string_ledit->text().length())
    {
        return ui->search_string_ledit->text().toInt();
    }
    else
    {
        return 4;
    }
}

void FiltersWindow::setStringSearchLen(int len)
{
    ui->search_string_ledit->setText(std::to_string(len).c_str());
}

void FiltersWindow::setStringSearchFieldAttributes()
{
    ui->search_string_ledit->setValidator(new QIntValidator(1, 999, this));
}

void FiltersWindow::filtersToggled(bool show)
{
    ui->filtersWidget->setHidden(!show);
    ui->filtersButton->setArrowType(show ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
}

void FiltersWindow::setEditableChecked(bool state)
{
    ui->editable->setChecked(state);
}

bool FiltersWindow::getEditableChecked()
{
    return ui->editable->isChecked();
}


bool FiltersWindow::getStringSearchLenModified()
{
    return ui->search_string_ledit->isModified();
}
