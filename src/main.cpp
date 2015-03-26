//#include <QtGui/QApplication>

#include <QApplication>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QDesktopServices>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std;

#include "mainwindow.h"

void myMessageHandler(QtMsgType type, const char *msg)
{
	QString txt;
	switch (type) {
		case QtDebugMsg:
			fprintf(stderr, "Debug: %s\n", msg);
			break;
		case QtWarningMsg:
			fprintf(stderr, "Warning: %s\n", msg);
			break;
		case QtCriticalMsg:
			fprintf(stderr, "Critical: %s\n", msg);
			break;
		case QtFatalMsg:
			fprintf(stderr, "Fatal: %s\n", msg);
			abort();
	}
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	//QCoreApplication::setAttribute (Qt::AA_NativeWindows);
	QCoreApplication::setAttribute (Qt::AA_DontCreateNativeWidgetSiblings);
	
	//QFile outFile("pviz3.log");
	//outFile.open(QIODevice::WriteOnly | QIODevice::Truncate);

	
	std::vector<std::string> input_files;
	std::string directory_name;
    bool enableConnectToServer = false;
    bool enableTileWindows = false;
    bool enableFullscreen = false;
    int colormap = 13;
    // Using toStdString() cause an error. Instead, using toAscii()
    std::string logfile = std::string(QDesktopServices::storageLocation(QDesktopServices::DataLocation).toAscii().data()) + "/PVIZ3/pviz3.log";
    
	try 
	{
        po::options_description desc("Allowed options");
        desc.add_options()
		("help,h", "produce help message")
		("input-file,i", po::value< std::vector<std::string> >(&input_files), "input file")
		("directory-open,d", po::value< std::string >(&directory_name), "directory open")
		("connect", "connect to server")
		("tile", "tile windows")
        ("fullscreen", "fullscreen")
        ("colormap,c", po::value< int>(&colormap), "colormap")
        ("logfile", po::value< std::string >(&logfile), "logfile")
        ;

		po::positional_options_description p;
        p.add("input-file", -1);

        po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);
		
        if (vm.count("help")) 
        {
            cout << "Usage: options_description [options]\n";
            cout << desc;
            return 0;
        }
        
        if (vm.count("connect")) 
        {
            enableConnectToServer = true;
        }
        
        if (vm.count("tile")) 
        {
            enableTileWindows = true;
        }
        
        if (vm.count("fullscreen")) 
        {
            enableFullscreen = true;
        }
    }
    catch(std::exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }

    if (logfile != "-")
    {
        QFileInfo info = QFileInfo(QString::fromAscii(logfile.c_str()));
        qDebug() << "logfile ... " << info.absoluteFilePath();
        
        //freopen(info.absoluteFilePath().toStdString().c_str(), "w", stderr);
        freopen(info.absoluteFilePath().toAscii().constData(), "w", stderr);
        setvbuf(stderr, NULL, _IONBF, 0);
    }
    
	qInstallMsgHandler(myMessageHandler);
	qDebug() << QDateTime::currentDateTime().toString() << "... Started. ";
    
    MainWindow w;
    w.show();
    
    if (enableFullscreen)
        w.setWindowState(Qt::WindowFullScreen);
	
	for (std::vector<std::string>::iterator it = input_files.begin(); it < input_files.end(); it++) 
	{
		w.OpenDataFile(QString((*it).c_str()));
	}
    
    if (directory_name.length() > 0)
    {
        qDebug() << "Ready to open directory: " << QDir(QString(directory_name.c_str())).canonicalPath();
        w.OpenDirectory(QString(directory_name.c_str()));
    }
    
    if (enableConnectToServer) 
    {
        qDebug() << "Connect to server";
        w.ConnectToActiveMQServer();
    }
    
    if (enableTileWindows)
        w.TileSubWindows();
    
    w.SetColorMap(colormap);
    
    //QString location = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    //QFileInfo info = QFileInfo(location + "/PVIZ3/pviz3.ini");
    
    return a.exec();
}
