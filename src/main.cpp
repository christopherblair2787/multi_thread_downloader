#include <QApplication>
#include "mainwindow.h"
#include "smallwindow.h"

int main(int argc, char ** argv)
{
    QApplication app( argc, argv );
    app.setOrganizationName("NetFleet");
    app.setOrganizationDomain("netfleet.googlecode.com");
    app.setApplicationName("NetFleet");

    QSettings settings;
    settings.beginGroup("MainWindow");
    QString language = settings.value("language").toString();
    settings.endGroup();
    QTranslator translator;
    if (language == "")
        translator.load(":/translations/netFleet_" + QLocale::system().name());
    else
        translator.load(":/translations/netFleet_" + language);
    app.installTranslator(&translator);
    QTranslator translatorQt;
    translatorQt.load( ":/translations/Qt_"+language+".qm" );
    if (translatorQt.isEmpty())
        translatorQt.load( QLibraryInfo::location( QLibraryInfo::TranslationsPath) + "/qt_"+QLocale::system().name()+".qm" );
    if (!translatorQt.isEmpty())
        app.installTranslator( &translatorQt );

    MainWindow win;
    win.show();
    win.threadsDetailTableWidget->hide();

    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
    return app.exec();
}
