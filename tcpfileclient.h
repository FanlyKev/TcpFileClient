#ifndef TCPFILECLIENT_H
#define TCPFILECLIENT_H

#include <QDialog>
#include <QtNetwork>
#include <QtWidgets>

class TcpFileClient : public QDialog
{
    Q_OBJECT

public:
    explicit TcpFileClient(QWidget *parent = nullptr);
    ~TcpFileClient() override;
private slots:
    void start(); //使用者按開始button
    void startTransfer(); //TCP end-point connection 連線成功
    void openFile(); //使用按開檔button
    void updateClientProgress(qint64 numberBytes); //TCP end_point 持續可傳輸事件
private:
    QProgressBar     *clientProgressBar;
    QLabel           *clientStatusLabel;
    QPushButton      *startButton;
    QPushButton      *quitButton;
    QPushButton      *openButton;
    QDialogButtonBox *buttonBox;
    QTcpSocket       tcpClient;

    qint64           totalBytes;
    qint64           bytesWritten;
    qint64           bytesToWrite;
    qint64           loadSize;
    QString          fileName;
    QFile            *localFile;
    QByteArray       outBlock;
};
#endif // TCPFILECLIENT_H
