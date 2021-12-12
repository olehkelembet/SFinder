#ifndef FILTERSWINDOW_H
#define FILTERSWINDOW_H

#include <QWidget>
#include <QIntValidator>
#include <QAbstractButton>

namespace Ui {
class FiltersWindow;
}

class FiltersWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FiltersWindow(QWidget *parent = nullptr);
    ~FiltersWindow();

    bool getAsciiChecked();
    bool getUnicodeChecked();
    bool getEditableChecked();
    int  getStringSearchLen();
    bool getStringSearchLenModified();
    void setAsciiChecked(bool);
    void setUnicodeChecked(bool);
    void setEditableChecked(bool);
    void setStringSearchLen(int);
    void setStringSearchFieldAttributes();

private slots:
    void filtersToggled(bool);

signals:
    void filtersChanged();

private:
    Ui::FiltersWindow *ui;
};

#endif // FILTERSWINDOW_H
