#include "taglibextractortest.h"
#include "simpleextractionresult.h"
#include "indexerextractortestsconfig.h"
#include "extractors/taglibextractor.h"

#include <QDebug>
#include <QTest>
#include <QDir>

using namespace KFileMetaData;

QString TagLibExtractorTest::testFilePath(const QString& fileName) const
{
    return QLatin1String(INDEXER_TESTS_SAMPLE_FILES_PATH) + QDir::separator() + fileName;
}

void TagLibExtractorTest::test()
{
    QScopedPointer<ExtractorPlugin> plugin(new TagLibExtractor(this));

    SimpleExtractionResult result(testFilePath("test.opus"), "audio/opus");
    plugin->extract(&result);

    QCOMPARE(result.types().size(), 1);
    QCOMPARE(result.types().first(), Type::Audio);

    QCOMPARE(result.properties().value(Property::Title), QVariant(QStringLiteral("Title")));
    QCOMPARE(result.properties().value(Property::Artist), QVariant(QStringLiteral("Artist")));
    QCOMPARE(result.properties().value(Property::Album), QVariant(QStringLiteral("Test")));
    QCOMPARE(result.properties().value(Property::AlbumArtist), QVariant(QStringLiteral("Album Artist")));
    QCOMPARE(result.properties().value(Property::Genre), QVariant(QStringLiteral("Genre")));
    QCOMPARE(result.properties().value(Property::Comment), QVariant(QStringLiteral("Comment")));
    QCOMPARE(result.properties().value(Property::Composer), QVariant(QStringLiteral("Composer")));
    QCOMPARE(result.properties().value(Property::TrackNumber).toInt(), 1);
    QCOMPARE(result.properties().value(Property::ReleaseYear).toInt(), 2015);
    QCOMPARE(result.properties().value(Property::Channels).toInt(), 1);
}

QTEST_MAIN(TagLibExtractorTest)
