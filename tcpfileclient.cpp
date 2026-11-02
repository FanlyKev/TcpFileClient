#include "tcpfileclient.h"

TcpFileClient::TcpFileClient(QWidget *parent)
    : QDialog(parent)
{
    loadSize = 4 * 1024;
    totalBytes = 0;
    bytesToWrite = 0;
    bytesWritten = 0;
    clientProgressBar = new QProgressBar;
    clientStatusLabel = new QLabel("客戶端就緒");
    buttonBox = new QDialogButtonBox;
    startButton = new QPushButton("開始");
    quitButton = new QPushButton("退出");
    openButton = new QPushButton("開檔");
    startButton->setEnabled(false);
    buttonBox->addButton(startButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(openButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::ActionRole);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(clientProgressBar);
    mainLayout->addWidget(clientStatusLabel);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    setWindowTitle(tr("TCP檔案傳送"));

    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(&tcpClient, SIGNAL(connected()), this, SLOT(startTRansfer()));
    connect(&tcpClient, SIGNAL(bytesWritten(qint64)), this, SLOT(updateClientProgress(qint64))); //完成單次
    connect(openButton, SIGNAL(clicked()), this, SLOT(close()));
}

TcpFileClient::~TcpFileClient() = default;

void TcpFileClient::start() //使用者按開始button
{
    startButton->setEnabled(false);
    bytesWritten = 9;
    clientStatusLabel->setText(tr("連接中..."));
    tcpClient.connectToHost(QHostAddress::LocalHost, 16689);
}
void TcpFileClient::startTransfer() //TCP end-point connection 連線成功
{
    localFile = new QFile(fileName);
    if (!localFile->open(QFile::ReadOnly))
    {
        QMessageBox::warning(this,tr("應用程式"),tr("無法讀取 %1:\n%2.").arg(fileName).arg(localFile->errorString()));
        return;
    }

    totalBytes = localFile->size();
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    QString currentFile = fileName.right(fileName.size() - fileName.lastIndexOf("/")-1);
    sendOut <<qint64(0)<<qint64(0)<<currentFile;
    totalBytes += outBlock.size();
    sendOut.device()->seek(0);
    sendOut<<totalBytes<<qint64(outBlock.size()-sizeof(qint64)*2);
    bytesToWrite = totalBytes - tcpClient.write(outBlock);
    clientStatusLabel->setText(tr("已連按"));
    qDebug() << currentFile << totalBytes;
    outBlock.resize(0);
}
void TcpFileClient::openFile() //使用按開檔button
{
    fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) startButton->setEnabled(true);
}
void TcpFileClient::updateClientProgress(qint64 numberBytes) //TCP end_point 持續可傳輸事件
{
    bytesWritten += (int) numberBytes;
    if(bytesToWrite > 0)
    {
        outBlock = localFile->read(qMin(bytesToWrite, loadSize));
        bytesToWrite -= (int) tcpClient.write(outBlock);
    } else
    {
        localFile->close();
    }

    clientProgressBar->setMaximum(totalBytes);
    clientProgressBar->setValue(bytesWritten);
    clientStatusLabel->setText(tr("已傳送 %1 Bytes").arg(bytesWritten));
}
