#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QProcess>
#include <QDir>
#include <QPushButton>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:

    void on_checkBox_stateChanged(int arg1);

    void on_buttonBox_clicked(QAbstractButton *button);

    void readOutput();

    void runCommand(QString command);

    void on_buttonBox_rejected();

    void onProcessFinished();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::Dialog *ui;

};

#endif // DIALOG_H
