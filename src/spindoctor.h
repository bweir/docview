#pragma once

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QList>
#include <QFont>

class SpinDoctor : public QObject 
{
    Q_OBJECT

private:
    QRegularExpression re_tokens;

    struct TwoPartRule
    {
        QRegularExpression start;
        QRegularExpression end;
        bool newline;
    };

    QList<TwoPartRule> twopartrules;

public:
    SpinDoctor(QObject *parent = 0);

    void addRules(QStringList rules);

    void rebuildRules();

public slots:
    QString process(QString text);
};
