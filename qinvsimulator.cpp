#include <QTcpServer>
#include <QTcpSocket>
#include <QSignalMapper>
#include <QPair>
#include <QByteArray>
#include <QHash>
#include <QQueue>
#include <QStringList>
#include <QDebug>

#include "qinvsimulator.hpp"

QInvSimulator::QInvSimulator(QObject *parent) : QObject(parent)
{
    id = 0;
    sock = new QTcpServer(this);
    querymapper = new QSignalMapper(this);
    removemapper = new QSignalMapper(this);

    sock->listen(QHostAddress(QHostAddress::LocalHost), 12345);

    connect(sock, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
    connect(querymapper, SIGNAL(mapped(int)), this, SLOT(sendResponse(int)));
    connect(removemapper, SIGNAL(mapped(int)), this, SLOT(removeSocket(int)));
}

QInvSimulator::~QInvSimulator()
{

}

void QInvSimulator::handleNewConnection()
{
    id++;
    QTcpSocket* conn = sock->nextPendingConnection();

    querymapper->setMapping(conn, id);
    connect(conn, SIGNAL(readyRead()), querymapper, SLOT(map()));
    connect(conn, SIGNAL(disconnected()), removemapper, SLOT(map()));

    queue.insert(id, conn);

    qDebug() << "New connection" << id;
}

void QInvSimulator::removeSocket(int id)
{
    QTcpSocket* sck = queue[id];

    queue.remove(id);

    delete sck;

    qDebug() << "Delete connection" << id;
}

void QInvSimulator::sendResponse(int id)
{
    QTcpSocket* sck = queue[id];

    QByteArray raw = sck->readAll();
    QString query(raw);

    qDebug() << "[" << id << "] RECV   " << query;

    QString resp = responsePrepare(query);

    qDebug() << "[" << id << "] SEND   " << resp;

    QByteArray response = resp.toAscii();

    sck->write(response);
}

QString QInvSimulator::responsePrepare(QString query)
{
    qDebug() << "responsePrepare";

    QStringList frag = query.split("|");
    QStringList flags = frag[0].mid(1).split(";");

    QString check = frag[2].mid(0, 4);
    QString tocheck = frag[0].mid(1) + "|" + frag[1] + "|";

    qDebug() << tocheck << checksum(tocheck) << check;

    if(checksum(tocheck) != check)
        return nullptr;

    qDebug() << "        Checkusm OK";

    if(query.length() != flags[2].toInt(nullptr, 16))
        return nullptr;

    qDebug() << "        Lenght OK";

    QQueue<QString> resvalues;

    foreach(QString elem, frag[1].mid(4).split(";")) {
        int value = 0;

        if(elem == "UDC")
            value = randInt(2700, 4200);

        if(elem == "IDC")
            value = randInt(0, 1000);

        if(elem == "UL1")
            value = randInt(2200, 2450);

        if(elem == "IL1")
            value = randInt(0, 1600);

        if(elem == "PAC")
            value = randInt(0, 5500);

        if(elem == "PRL")
            value = randInt(1, 30);

        if(elem == "TKK")
            value = randInt(10, 60);

        if(elem == "TNF")
            value = randInt(4990, 5010);

        if(elem == "KDY")
            value = randInt(40, 150);

        if(elem == "KLD")
            value = randInt(40, 150);

        QString item = elem + "=" + QString::number(value, 16).toUpper();

        resvalues.enqueue(item);
    }

    QString allvalues;

    while(!resvalues.isEmpty())
        allvalues += resvalues.dequeue() + ";";

    allvalues = allvalues.left(allvalues.length() - 1)    ;

    QString len;
    len.sprintf("%02X", 13 + allvalues.length() + 6);

    QString response;

    response += flags[1] + ";";
    response += flags[0] + ";";
    response += len;

    response += "|64:";

    response += allvalues;

    response += "|";

    response = "{" + response + checksum(response) + "}";

    return response;
}

QString QInvSimulator::checksum(QString input)
{
    if(input == nullptr)
        return nullptr;

    if(input.length() == 0)
        return "";

    int sum = 0;

    foreach(QChar c, input) {
        sum += c.toAscii();
        sum %= 0xffff;
    }

    QString ret;

    return ret.sprintf("%04X", sum);
}

int QInvSimulator::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}
