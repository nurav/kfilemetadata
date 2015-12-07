#include "taglibwritertest.h"
#include "indexerextractortestsconfig.h"
#include "writers/taglibwriter.h"
#include "writedata.h"
#include "simpleextractionresult.h"
#include "extractors/taglibextractor.h"

#include <QDebug>
#include <QTest>
#include <QDir>
#include <QFile>

#define TEST_FILENAME "writertest.opus"

using namespace KFileMetaData;

QString TagLibWriterTest::testFilePath(const QString& fileName) const
{
    return QLatin1String(INDEXER_TESTS_SAMPLE_FILES_PATH) + QDir::separator() + fileName;
}

void TagLibWriterTest::initTestCase()
{
    QFile testFile(testFilePath(TEST_FILENAME));
}

void TagLibWriterTest::test()
{
    QScopedPointer<WriterPlugin> writerPlugin(new TagLibWriter(this));

    WriteData data(testFilePath(TEST_FILENAME), "audio/opus");
    data.add(Property::Title, "Title");
    data.add(Property::Artist, "Artist");
    writerPlugin->write(&data);

    /* Testing whether data was correctly written */
    QScopedPointer<ExtractorPlugin> extractorPlugin(new TagLibExtractor(this));

    SimpleExtractionResult result(testFilePath("test.opus"), "audio/opus");
    extractorPlugin->extract(&result);

    QCOMPARE(result.properties().value(Property::Artist), QVariant(QStringLiteral("Artist")));
    QCOMPARE(result.properties().value(Property::Title), QVariant(QStringLiteral("Title")));
}

void TagLibWriterTest::cleanupTestCase()
{
    QFile testFile(testFilePath(TEST_FILENAME));
    testFile.deleteLater();
}

QTEST_MAIN(TagLibWriterTest)
