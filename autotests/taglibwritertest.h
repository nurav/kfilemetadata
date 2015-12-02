#ifndef TAGLIBEXTRACTORTEST_H
#define TAGLIBEXTRACTORTEST_H

#include <QObject>

class TagLibExtractorTest : public QObject
{
    Q_OBJECT
private:
    QString testFilePath(const QString& fileName) const;

private Q_SLOTS:
    void test();
};

#endif // TAGLIBEXTRACTORTEST_H
