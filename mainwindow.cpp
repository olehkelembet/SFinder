#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_menu_bar();
    setup_header();
    load_settings();
    set_min_stirng_search_len(ui->filtersWnd->getStringSearchLen());
}

MainWindow::~MainWindow()
{
    QSettings settings("SFinder", "sfinder");

    settings.setValue("Window/x-position", this->x());
    settings.setValue("Window/y-position", this->y());
    settings.setValue("Window/widht", this->width());
    settings.setValue("Window/height", this->height());

    settings.setValue("Filters/Symbols/ascii", ui->filtersWnd->getAsciiChecked());
    settings.setValue("Filters/Symbols/unicode", ui->filtersWnd->getUnicodeChecked());
    settings.setValue("Filters/Symbols/string_search_length", ui->filtersWnd->getStringSearchLen());
    settings.setValue("Filters/Symbols/editable", ui->filtersWnd->getEditableChecked());

    delete ui;
}

void MainWindow::setup_menu_bar()
{
    m_file_menu	= new QMenu("File");
    m_options_menu = new QMenu("Options");

    m_file_menu->addAction("Open file",  this, SLOT(open_file()), Qt::CTRL + Qt::Key_O);
    m_file_menu->addAction("Save file",   this, SLOT(save_file()), Qt::CTRL + Qt::Key_S);
    m_file_menu->addAction("Save strings to file",   this, SLOT(save_to_file()), Qt::CTRL + Qt::Key_F);
    m_file_menu->addAction("Exit",   this, SLOT(exit()), Qt::CTRL + Qt::Key_Q);
    //connect(ui->FltrsWidget, SIGNAL(filtersChanged()), this, SLOT(applyFilters()));
    connect(ui->filtersWnd, SIGNAL(filtersChanged()), this, SLOT(handleFilterEvent()));

    ui->menubar->addMenu(m_file_menu);
    ui->menubar->addMenu(m_options_menu);
}

void MainWindow::on_pushButton_clicked()
{
    open_file();
}

void MainWindow::open_file()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open file for search"),
                                                m_last_path.isEmpty() ? QDir::homePath() : m_last_path);
    if( !QFile::exists(file_name) ) {
        qDebug() << "Selected file" << file_name << "does not exist.";
        return;
    }
    if(file_name==m_file_path)
    {
        if(m_first_run)
        {
            m_first_run = false;
            reopen_file(file_name);
            return;
        } else
        {
            reopen_file(file_name);
            return;
        }
    } else if(m_file_path.size())
    {
        if(m_first_run)
        {
           m_first_run = false;
        }
    }
    m_last_path = QFileInfo(file_name).absolutePath();
    m_file_path = file_name;
    ui->lineEdit->setText(file_name);
    open_file(file_name);
}

void MainWindow::open_file(const QString& file)
{
    if(!m_first_run)
    {
        QAbstractItemModel* const mdl = ui->tableWidget->model();
        mdl->removeRows(0, mdl->rowCount());
    }

    read_file(file);
    search_strings();
}

void MainWindow::reopen_file(const QString& file)
{
    QMessageBox::information(this,tr("You reopening the file !"),tr("The file will be reopened."));
    open_file(file);
}

void MainWindow::save_to_file()
{
    //check is there any data(strings) already
    if(m_astrings.isEmpty() && m_ustrings.isEmpty())
    {
      QMessageBox::warning(this, tr("Can't save !"), tr("There is no strings to save."));
      return;
    }
    QString file_name = QFileDialog::getSaveFileName(this,
                                            tr("Save strings to file"), "", tr("Text files (*.txt)"));

    if (file_name.isEmpty())
    {
        return;
    }
    else
    {
        QFile file(file_name);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
            return;
        }
        QTextStream out(&file);
        foreach(QString sa, m_astrings.values())
        {
            //qDebug() << s;
          out << sa << "\n";
        }
        foreach(QString su, m_ustrings.values())
        {
            //qDebug() << s;
          out << su << "\n";
        }
        file.close();
    }
}

void MainWindow::exit()
{
    QApplication::quit();
}

bool MainWindow::file_editable()
{
  /*
  Can be used QFileDevice::permissions() considering differences in the platforms supported by Qt and NTFS file system
  */
#ifdef Q_OS_WINDOWS
  uint32_t attributes = GetFileAttributes(m_file_path.toStdStirng.c_str());
  if (attributes == INVALID_FILE_ATTRIBUTES)
  {
    return false;
  }
  if (attributes & FILE_ATTRIBUTE_READONLY)
  {
    return true;
  }
#endif

#ifdef Q_OS_LINUX
  if(access(m_file_path.toStdString().c_str(), R_OK) != -1)
  {
    return true;
  }
  else
  {
    return false;
  }
#endif
}

void MainWindow::see_only_ascii()
{

}

void MainWindow::see_only_unicode()
{

}

void MainWindow::read_file(const QString& file)
{
    if(!m_file_byte_array.isNull())
    {
        m_file_byte_array.clear();
    }

    QFile file_io(file);
    if( !file_io.open(QIODevice::ReadOnly) )
    {
        qDebug() << "Can't open file: " << file;
    }
    m_file_byte_array = file_io.readAll();
}

void MainWindow::search_strings()
{
    find_ascii_strings();
    find_unicode_strings();
}

void MainWindow::find_ascii_strings()
{
    bool b_in_string{false};
    QString current_str;
    Q_ASSERT(m_min_string_len>=1 && m_min_string_len<=999);

    for( int file_offset=0, counter=0;
         counter < m_file_byte_array.size();
         ++counter )
    {
        if( std::isprint(m_file_byte_array.at(counter)) )
        {
            if(!b_in_string)
            {
                b_in_string = true;
                file_offset = counter;
            }
            current_str += m_file_byte_array.at(counter);
            continue;
        } else if( current_str.size() >= m_min_string_len )
        {
          m_astrings.insert(file_offset, current_str);
          print_string(file_offset, STYPE_ASCII, current_str);
          b_in_string=false;
          current_str.clear();
        } else
        {
            current_str.clear();
            b_in_string=false;
        }
    }
}

void MainWindow::find_unicode_strings()
{
    bool b_in_string{false};
    QString current_str;
    Q_ASSERT(m_min_string_len>=1 && m_min_string_len<=999);

    for( int file_offset=0, counter=0;
         counter < (m_file_byte_array.size()-1);
         counter +=2 )
    {
        if( std::isprint(m_file_byte_array.at(counter))  && (m_file_byte_array.at(counter+1) == 0) )
        {
            if(!b_in_string)
            {
                b_in_string = true;
                file_offset = counter;
                current_str += m_file_byte_array.at(counter);
            }
            else
            {
                current_str += m_file_byte_array.at(counter);
                continue;
            }
        }
        else if( current_str.size() >= m_min_string_len )
        {
          m_ustrings.insert(file_offset, current_str);
          print_string(file_offset, STYPE_UNICODE, current_str);
          b_in_string=false;
          current_str.clear();
        }
        else
        {
            current_str.clear();
            b_in_string=false;
        }
    }
}

void MainWindow::print_string(int file_offset, m_string_type type, const QString& str)
{
    int row = ui->tableWidget->rowCount();

    ui->tableWidget->setRowCount(row + 1);
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem((std::to_string(file_offset)).c_str()));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem((type)?"U":"A"));
    ui->tableWidget->setItem(row,2,new QTableWidgetItem(str));

}

void MainWindow::load_settings()
{
    QSettings settings("SFinder", "sfinder");
    this->setGeometry(settings.value("Window/x-position", 100).toInt(),
                      settings.value("Window/y-position", 100).toInt(),
                      settings.value("Window/widht", 800).toInt(),
                      settings.value("Window/height", 600).toInt());
    ui->filtersWnd->setAsciiChecked(settings.value("Filters/Symbols/ascii", true).toBool());
    ui->filtersWnd->setUnicodeChecked(settings.value("Filters/Symbols/unicode", true).toBool());
    ui->filtersWnd->setEditableChecked(settings.value("Filters/Editable", false).toBool());
    ui->filtersWnd->setStringSearchLen(settings.value("Filters/Symbols/string_search_length", true).toInt());
}

void MainWindow::setup_header()
{
    QTableWidget* table_widget = ui->tableWidget;

    table_widget->setColumnCount(m_column_count);
    QTableWidgetItem* offset = new QTableWidgetItem();
    table_widget->setHorizontalHeaderItem(OFFSET_COLUMN, offset);
    QTableWidgetItem* type = new QTableWidgetItem();
    table_widget->setHorizontalHeaderItem(TYPE_COLUMN, type);
    QTableWidgetItem* string = new QTableWidgetItem();
    table_widget->setHorizontalHeaderItem(STRING_COLUMN, string);

    table_widget->horizontalHeaderItem(OFFSET_COLUMN)->setTextAlignment(Qt::AlignCenter);
    table_widget->horizontalHeaderItem(OFFSET_COLUMN)->setText("Offset");
    table_widget->horizontalHeaderItem(TYPE_COLUMN)->setTextAlignment(Qt::AlignCenter);
    table_widget->horizontalHeaderItem(TYPE_COLUMN)->setText("Type");
    table_widget->horizontalHeaderItem(STRING_COLUMN)->setTextAlignment(Qt::AlignCenter);
    table_widget->horizontalHeaderItem(STRING_COLUMN)->setText("String");
}

void MainWindow::set_min_stirng_search_len(int len)
{
    m_min_string_len = len;
}

void MainWindow::handleFilterEvent()
{
    if(ui->filtersWnd->getAsciiChecked())
    {

    }
    if(ui->filtersWnd->getUnicodeChecked())
    {

    }
    if(ui->filtersWnd->getEditableChecked())
    {

    }
    if(ui->filtersWnd->getStringSearchLenModified())
    {
        set_min_stirng_search_len(ui->filtersWnd->getStringSearchLen());
    }
}
