#ifndef QINVSIMULATOR_HPP
#define QINVSIMULATOR_HPP

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QSignalMapper>

class QInvSimulator : public QObject
{
        Q_OBJECT
    public:
        explicit QInvSimulator(QObject *parent = 0);
        ~QInvSimulator();

    private:
        int id;
        QTcpServer* sock;
        QHash<int, QTcpSocket*> queue;
        QSignalMapper* querymapper;
        QSignalMapper* removemapper;

        QString checksum(QString input);
        QString responsePrepare(QString data);
        int randInt(int low, int high);

    private slots:
        void handleNewConnection();
        void sendResponse(int id);
        void removeSocket(int id);
};

#endif // QINVSIMULATOR_HPP
