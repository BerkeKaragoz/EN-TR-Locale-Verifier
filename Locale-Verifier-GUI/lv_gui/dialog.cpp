#include "dialog.h"
#include "ui_dialog.h"

#define TB_OUTPUT(EN,TR) "<br><span style=\" font-weight:600;\">(EN)</span> " EN \
    "</p><br><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">" \
    "(TR)</span> " TR "<br></p>"
#define BOLD_HTML(X) "<span style=\" font-weight:600;\">" X "</span>"

QProcess *process = nullptr;
QString qFlags = "-fl \"en\"";
bool pState = true;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setText("Apply (Uygula)");
    ui->buttonBox->button(QDialogButtonBox::Close)->setText("Close (Kapat)");
    //runCommand("chmod 777 "+QDir::currentPath()+"/locale-verifier.o ");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == 0){  
        if (ui->comboBox->currentIndex() == 1){
            qFlags = "-fl \"tr\"";
            ui->textBrowser->setHtml(TB_OUTPUT("System language will be changed to " BOLD_HTML("Turkish") ".",
                                               "Sistem dili " BOLD_HTML("Türkçe") "'ye çevrilecektir."));
        } else {
            qFlags = "-fl \"en\"";
            ui->textBrowser->setHtml(TB_OUTPUT("System language will be changed to " BOLD_HTML("English") ".",
                                               "Sistem dili " BOLD_HTML("İngilizce") "'ye çevrilecektir."));
        }
        ui->comboBox->setEnabled(true);
    } else {
        qFlags = "-fv";
        ui->textBrowser->setHtml(TB_OUTPUT("System language will be " BOLD_HTML("verified") ".",
                                           "Sistem dili " BOLD_HTML("doğrulanacaktır") "."));
        ui->comboBox->setEnabled(false);
    }
}

void Dialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button == ui->buttonBox->button(QDialogButtonBox::Apply) ){
        ui->textBrowser->setHtml(TB_OUTPUT("Making the changes...",
                                           "Değişiklikler yapılıyor..."));
        ui->buttonBox->setEnabled(false);
        runCommand("pkexec "+QDir::currentPath()+"/locale-verifier " + qFlags);
    }
}

void Dialog::runCommand(QString command){
    if (process != nullptr){
        process->kill();
    }
    process = new QProcess(nullptr);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readOutput()));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onProcessFinished()));

    process->start(command);
}

void Dialog::readOutput()
{
    QString qs = QString::fromStdString( (*process).readAllStandardOutput().toStdString() +
                                         (*process).readAllStandardError().toStdString() );
    ui->textBrowser->setHtml(qs);
}

void Dialog::onProcessFinished(){
    process->kill();
    process = nullptr;
    ui->buttonBox->setEnabled(true);
    fprintf(stderr, "Process finish.\n");

    QString tbQString = ui->textBrowser->document()->toPlainText();

    if (tbQString.startsWith("Error", Qt::CaseInsensitive)){
        ui->textBrowser->setHtml(TB_OUTPUT("There was an error.",
                                           "Bir hata oluştu.")"<span style=\"color: red;\">" + tbQString + "</span>");
        ui->stateLabel->setText("Failed (Başarısız)");
    } else {
        ui->stateLabel->setText("Success (Başarılı)");
    }

}

void Dialog::on_buttonBox_rejected()
{
    fprintf(stderr, "Closed.\n");
}

void Dialog::on_comboBox_currentIndexChanged(int index)
{
    if (index == 1){
        qFlags = "-fl \"tr\"";
        ui->textBrowser->setHtml(TB_OUTPUT("System language will be changed to " BOLD_HTML("Turkish") ".",
                                           "Sistem dili " BOLD_HTML("Türkçe") "'ye çevrilecektir."));
    } else {
        qFlags = "-fl \"en\"";
        ui->textBrowser->setHtml(TB_OUTPUT("System language will be changed to " BOLD_HTML("English") ".",
                                           "Sistem dili " BOLD_HTML("İngilizce") "'ye çevrilecektir."));
    }
}
