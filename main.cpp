#include <QCoreApplication>

#include "qinvsimulator.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QInvSimulator sim;

    return a.exec();
}
