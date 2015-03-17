#include <QtGui>
#include <QDebug>
#include <QTreeView>

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "ui_MainControlsToolbar.h"

#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"

#include <vtkRenderWindow.h>
#include <vtkTransform.h>
#ifdef _WIN32 
#include  "Windows.h"
#endif

//#include "ServiceException.h"
#ifdef _OPENMP
#include <omp.h>
#endif

void PolyDataSummary(vtkPolyData *poly)
{
	qDebug() << QString("PolyData (C:%1, P:%2, V:%3, L:%4, S:%5)")
	.arg(poly->GetNumberOfCells())
	.arg(poly->GetNumberOfPoints())
	.arg(poly->GetNumberOfVerts())
	.arg(poly->GetNumberOfLines())
	.arg(poly->GetNumberOfStrips());
}

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
m_aboutDialog(0),
m_helpDialog(0),
ui(new Ui::MainWindow),
stopThreadWork(false),
//brokerIP("156.56.104.176"),
brokerIP("129.79.244.49"),
brokerPort(61616),
destURI("topic1"),
listenURI("topic2")
{
	ui->setupUi(this);
	
	connect(ui->actionOpenDataFile, SIGNAL(triggered()), this, SLOT(OpenDataFile()));
	connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(SaveAs()));
	connect(ui->actionSaveAnimation, SIGNAL(triggered()), this, SLOT(SaveAnimation()));
	connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(ConnectToServer()));
	connect(ui->actionBrokerConnect, SIGNAL(triggered()), this, SLOT(ConnectToActiveMQServer()));
	connect(ui->actionUpdateFromServer, SIGNAL(triggered()), this, SLOT(UpdateFromServer()));
	connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(PlayAndPause()));
	connect(ui->actionSaveScreen, SIGNAL(triggered()), this, SLOT(SaveScreen()));
	connect(ui->actionShowHalfSphere, SIGNAL(triggered()), this, SLOT(ShowHalfSphere()));
	connect(ui->actionSelectionMode, SIGNAL(triggered()), this, SLOT(SetSelectionMode()));
	connect(ui->actionResetCamera, SIGNAL(triggered()), this, SLOT(ResetCamera()));
	connect(ui->actionPickCenter, SIGNAL(triggered()), this, SLOT(PickCenter()));
	connect(ui->actionCascadeSubWindows, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));
	connect(ui->actionTileSubWindows, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
	connect(ui->bbTransforms, SIGNAL(clicked(QAbstractButton*)), 
			this, SLOT(TransformWidgetButtonClicked(QAbstractButton*)));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(About()));
	connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(Help()));
	connect(ui->actionSelectNeighbor, SIGNAL(triggered()), this, SLOT(SelectNeighbor()));
	connect(ui->actionOpenDirectory, SIGNAL(triggered()), this, SLOT(OpenDirectory()));
	connect(ui->actionFullScreen, SIGNAL(triggered()), this, SLOT(FullScreen()));
    
	/*
	treeModel = new QStandardItemModel();
	treeModel->setColumnCount(2);
	treeModel->setHorizontalHeaderItem( 0, new QStandardItem( "Cluster" ) );
	treeModel->setHorizontalHeaderItem( 1, new QStandardItem( "Label" ) );
	
	ui->tvTree->setModel(treeModel);
	
	qDebug() << "treeModel->columnWidth" << ui->tvTree->columnWidth(0);
	 */
    
	connect(ui->tvTree, SIGNAL(clicked(QModelIndex)), 
            this, SLOT(treeClicked(QModelIndex)));
	connect(ui->tvTree->header(), SIGNAL(sectionClicked(int)), 
            this, SLOT(treeSectionClicked(int)));
	
	vman1 = new QtVariantPropertyManager(this);
	connect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
			this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
	
	vman2 = new QtVariantPropertyManager(this);
	connect(vman2, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
			this, SLOT(featureValueChanged(QtProperty *, const QVariant &)));
	
	QtVariantEditorFactory *vfac1 = new QtVariantEditorFactory(this);
	ui->tpProperty->setFactoryForManager(vman1, vfac1);
	
	buildFeatureTree();
	
	// Add Menu
	ui->menuWindow->addAction(ui->dwTree->toggleViewAction());
	// Don't know why but titiles like "Preference" or "Setting" don't work
	ui->menuWindow->addAction(ui->dwPreferences->toggleViewAction());
	ui->menuWindow->addAction(ui->dwTransforms->toggleViewAction());
	ui->dwTransforms->hide();
	
#ifdef PVIZRPC_CLIENT_ENABLE
	ui->actionConnect->setEnabled(true);
	ui->actionUpdateFromServer->setEnabled(true);
#else
	ui->actionConnect->setEnabled(false);
	ui->actionUpdateFromServer->setEnabled(false);
#endif

#ifdef DAGTM_ACTIVEMQ_SERVER_ON    
	ui->actionBrokerConnect->setEnabled(true);
#else
	ui->actionBrokerConnect->setEnabled(false);
#endif
    
	connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), 
			this, SLOT(subWindowActivated(QMdiSubWindow *)));
    
	buildTransformWidget();
    
    future = new QFuture<void>;
    watcher = new QFutureWatcher<void>;    
    connect(watcher, SIGNAL(finished()), 
            this, SLOT(checkDirFinished()));
    
#ifdef USE_ACTIVEMQ    
    activemq::library::ActiveMQCPP::initializeLibrary();
#endif
    
    QString location = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QFileInfo info = QFileInfo(location + "/PVIZ3/pviz3.ini");
    
    settings = new QSettings(info.absoluteFilePath(), QSettings::IniFormat);
    qDebug() << "INI file : " << QFileInfo(settings->fileName()).absoluteFilePath();
    
    ui->tpPreferences->setResizeMode(QtTreePropertyBrowser::Interactive);
    ui->tpPreferences->setSplitterPosition(120);
    
    setAcceptDrops(TRUE);
}

MainWindow::~MainWindow()
{
    delete settings;
	delete ui;
    
    if (future->isStarted())
    {
        qDebug() << "Waiting for threading ... ";
        stopThreadWork = false;
        future->waitForFinished();
    }

#ifdef USE_ACTIVEMQ    
    activemq::library::ActiveMQCPP::shutdownLibrary();
#endif
    
	qDebug() << QDateTime::currentDateTime().toString() << "... Done. ";
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    
    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        
        for (int i = 0; i < urlList.size() && i < 32; ++i)
        {
            qDebug() << urlList.at(i).toLocalFile();
            pathList.append(urlList.at(i).toLocalFile());
            OpenDataFile(urlList.at(i).toLocalFile());
        }
        
        //if(openFiles(pathList))
        //    event->acceptProposedAction();
        
        event->acceptProposedAction();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    // The event needs to be accepted here
    event->acceptProposedAction();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // Set the drop action to be the proposed action.
    event->acceptProposedAction();
}


void MainWindow::SaveAs()
{
    QString fileName = "";
    if (PvizWidget *child = activeChild())
    {
        fileName = child->GetFileName();
    }
    
	fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                            QFileInfo(QFileInfo(fileName).dir(), QFileInfo(fileName).completeBaseName()).filePath(),
                                            tr("Pviz XML file (*.pviz);;VTK file (*.vtk);;Text file (*.txt);;All files (*.*)"));
	
    if (fileName.isEmpty())
	{
		qDebug() << "No file is selected ... ";
		return;
	}
		
    settings->setValue("LastOpenDirectory", QFileInfo(fileName).path());
	QStringList tokens = fileName.split(".");
	
	QString ext = tokens.last();
	
	if (ext.toLower() == "vtk") 
	{
		if (PvizWidget *child = activeChild())
		{
			child->SaveAsVTK(fileName);
			statusMessage(tr("File saved"));
		}
	}
	else if (ext.toLower() == "pviz") 
	{
		if (PvizWidget *child = activeChild())
		{
			child->SaveAsXML(fileName);
			statusMessage(tr("File saved"));			
		}
	} 
	else if (ext.toLower() == "txt")
	{
		if (PvizWidget *child = activeChild())
		{
			child->SaveAsTXT(fileName);
			statusMessage(tr("File saved"));			
		}
	} 
	else 
	{
		QMessageBox::warning(this, tr("Under construction"), tr("Net yet supported."));
	}
}

void MainWindow::SaveAnimation()
{
    QString fileName = "";
    if (PvizWidget *child = activeChild())
    {
        fileName = child->GetFileName();
    }
    
	fileName = QFileDialog::getSaveFileName(this, tr("Save Animation"),
                                            QFileInfo(QFileInfo(fileName).dir(), QFileInfo(fileName).completeBaseName()).filePath(),
                                            tr("AVI (*.avi)"));
	
    if (fileName.isEmpty())
	{
		qDebug() << "No file is selected ... ";
		return;
	}
    
    settings->setValue("LastOpenDirectory", QFileInfo(fileName).path());

    if (PvizWidget *child = activeChild())
    {
        child->SaveAsMovie(fileName);
        statusMessage(tr("File saved"));
    }
}

PvizWidget* MainWindow::OpenDataFile(QString fileName)
{
	PvizWidget *pviz = new PvizWidget();
    connect(pviz, SIGNAL(OnCenterPicked()), this, SLOT(UnpickCenter()));
    connect(pviz, SIGNAL(OnSelected()), this, SLOT(SetSelectionMode()));
    connect(pviz, SIGNAL(OnClusterVisibleChanged(PvizWidget*, unsigned int, bool)), this, SLOT(OnClusterVisibleChanged(PvizWidget*, unsigned int, bool)));
    connect(pviz, SIGNAL(OnClusterColorChanged(PvizWidget*, unsigned int, QColor)), this, SLOT(OnClusterColorChanged(PvizWidget*, unsigned int, QColor)));
    connect(pviz, SIGNAL(OnClusterDefaultChanged(PvizWidget*, unsigned int, bool)), this, SLOT(OnClusterDefaultChanged(PvizWidget*, unsigned int, bool)));
    connect(pviz, SIGNAL(OnClosing(PvizWidget*)), this, SLOT(OnClosing(PvizWidget*)));
    connect(pviz, SIGNAL(OnModelLoaded(PvizWidget*)), this, SLOT(OnModelLoaded(PvizWidget*)));
    connect(pviz, SIGNAL(OnCameraFocusChanged(PvizWidget*, double*)), this, SLOT(OnCameraFocusChanged(PvizWidget*, double *)));

	pviz->setMinimumSize(200, 200);

	qDebug() << "PvizWidget created : " << pviz;
	
	//treeModel = new QStandardItemModel();
	//pvizToModelMap[pviz] = new QStandardItemModel();
	
    QFileInfo file(fileName);
	//QStringList tokens = fileName.split(".");
	//QString ext = tokens.last();
    //QString ext = file.completeSuffix();
    QString ext = file.suffix();
	
    try
    {
        if (ext.toLower() == "gz")
        {
            pviz->LoadCompressedDataFile(fileName);
        }
        else if (ext.toLower() == "txt")
        {
            pviz->LoadSimpleDataFile(fileName);
        } 
        else if (ext.toLower() == "pviz")
        {
            pviz->LoadXmlDataFile(fileName);
        } 
        else if (ext.toLower() == "h5")
        {
#ifdef PVIZMODEL_USE_HDF5
            pviz->LoadHDF5DataFile(fileName);
#else
            QMessageBox::critical(this, "Error", "HDF5 is disabled in this build.");
#endif
        }
        else 
        {
            QMessageBox::warning(this, "Under construction", "Net yet supported.");
        }
    }
    catch (int e)
    {
        qDebug() << "Error " << e;
        delete pviz;
        return NULL;
    }
	
	//BuildTree(pviz);
	
	QMdiSubWindow* win = ui->mdiArea->addSubWindow(pviz);
	win->resize(500, 500);
	win->setWindowTitle(file.baseName());
	//connect(win, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), 
	//		this, SLOT(windowStateChanged(Qt::WindowStates, Qt::WindowStates)));
	//pviz->show();
	pviz->showMaximized();
	
	statusMessage(QString("%1 points loadded.").arg(pviz->model->points.size()));
	
    return pviz;
}

void MainWindow::OpenDataFile()
{
    QString dir = settings->value("LastOpenDirectory", "").toString();
	const QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Files"),
                                                                dir,
                                                                tr("Pviz XML file (*.pviz);;Text file (*.txt);;HDF5 file (*.h5);;All files (*.*)"));
	
	foreach(const QString fileName, fileNames)
	{
        settings->setValue("LastOpenDirectory", QFileInfo(fileName).path());
		OpenDataFile(fileName);
	}
}

/*
void MainWindow::windowStateChanged(Qt::WindowStates oldState, Qt::WindowStates newState)
{
	qDebug() << "windowStateChanged : " << oldState << "->" << newState;
}
*/

/*
void MainWindow::LoadTree()
{
	// Convert std::map to QMap to use foreach statement
	QMap<unsigned int, PvizCluster*> clusters_(activeChild()->model->clusters);
	
	foreach(PvizCluster* c, clusters_)
	{
		QList<QStandardItem *> items;
		
		QStandardItem *item1 = new QStandardItem();
		item1->setData(QString("%1").arg(c->id), Qt::DisplayRole);
		//item1->setData(QColor(c->color.ui_color), Qt::DecorationRole);
		double rgba[4];
		ui->pvizWidget->GetClusterColor(c->id, rgba);
		QColor col;
		col.setRedF(rgba[0]);
		col.setGreenF(rgba[1]);
		col.setBlueF(rgba[2]);
		col.setAlphaF(rgba[3]);
		
		item1->setData(col, Qt::DecorationRole);
		item1->setData(Qt::Checked, Qt::CheckStateRole);
		item1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
		
		QStandardItem *item2 = new QStandardItem();
		item2->setText(QString("%1").arg(c->label.c_str()));
		item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		
		items << item1 << item2;
				
		pvizToModelMap[activeChild()]->appendRow(items);
	}
	
	ui->tvTree->resizeColumnToContents(0);
}
*/

void MainWindow::BuildTree(PvizWidget *pviz)
{
	QStandardItemModel *model = new QStandardItemModel();
	model->setColumnCount(3);
	model->setHorizontalHeaderItem(0, new QStandardItem("Cluster"));
	model->setHorizontalHeaderItem(1, new QStandardItem("Label"));
	model->setHorizontalHeaderItem(2, new QStandardItem("Size"));
	
    //QStandardItem *item = new QStandardItem(QString("ROOT"));
    //model->setItem(0, 0, item);
    //QStandardItem *parentItem = model->invisibleRootItem();
    
    CidToItemListMap_t *cidToItemList = new CidToItemListMap_t();
    
    qDebug() << "clusterList.size" << pviz->model->clusterList.size();
    
	// Convert std::map to QMap to use foreach statement
	//QMap<unsigned int, PvizCluster*> clusters_(pviz->model->clusters);
    QList<PvizCluster*> clusters_ = QList<PvizCluster*>::fromStdList(pviz->model->clusterList);
	
    int numOfNondefault = 0;
	foreach(PvizCluster* cluster, clusters_)
	{
		QList<QStandardItem *> *items = new QList<QStandardItem *>();
		
		QStandardItem *item1 = new QStandardItem();
		item1->setData(QString("%1").arg(cluster->id), Qt::DisplayRole);
        
		item1->setData(pviz->GetClusterColor(cluster->id), Qt::DecorationRole);
		item1->setData(cluster->visible? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
		item1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
		
		QStandardItem *item2 = new QStandardItem();
		item2->setText(QString("%1").arg(cluster->label.c_str()));
		item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		
		QStandardItem *item3 = new QStandardItem();
		item3->setText(QString("%1").arg(cluster->points.size()));
		item3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        
		*items << item1 << item2 << item3;
		
        if (cluster->IsDefault())
        {
            model->appendRow(*items);
        }
        else
        {
            model->insertRow(numOfNondefault, *items);
            numOfNondefault++;
        }
        //cidToItem->insert(cluster->id, item1);
        cidToItemList->insert(cluster->id, items);
	}
    qDebug() << "Tree" << model->rowCount();
    qDebug() << "numOfNondefault" << numOfNondefault;
	
    PvizToModelMap_t::const_iterator iter = pvizToModelMap.find(pviz);
    if (iter != pvizToModelMap.end())
    {
        delete pvizToModelMap[pviz];
    }
        
    PvizToItemListMap_t::const_iterator iter3 = pvizToItemListMap.find(pviz);
    if (iter3 != pvizToItemListMap.end())
    {
        delete pvizToItemListMap[pviz];
    }
    
    pvizToModelMap[pviz] = model;
    pvizToItemListMap[pviz] = cidToItemList;
	
	//ui->tvTree->resizeColumnToContents(0); 
    //ui->tvTree->header()->setSortIndicatorShown(true); // optional
    ui->tvTree->header()->setClickable(true);
}

void MainWindow::RefreshTreeColor()
{
	if (PvizWidget *child = activeChild())
	{
		QStandardItemModel *treeModel = pvizToModelMap[child];
		
		std::cerr << "count : " << treeModel->children().count() << std::endl;
		std::cerr << "count : " << treeModel->rowCount() << std::endl;
		
		for (int i = 0; i < treeModel->rowCount(); i++)
		{
			QModelIndex index = treeModel->index(i, 0);
            unsigned int cid = treeModel->data(index, Qt::DisplayRole).toUInt();
			treeModel->setData(index, child->GetClusterColor(cid), Qt::DecorationRole);
		}
	}
}

void MainWindow::featureValueChanged(QtProperty *property, const QVariant &value)
{
	if (!featureToId.contains(property))
		return;
	
	if (PvizWidget *child = activeChild())
	{
		QString id = featureToId[property];
		
		qDebug() << "Value changed ... " << id;
		
		if (id == QLatin1String("title.name"))
		{
			child->SetTitle(value.toString());
		}
		else if (id == QLatin1String("title.visible"))
		{
			child->SetTitleVisible(value.toBool());
		}
		else if (id == QLatin1String("title.color"))
		{
			child->SetTitleColor(qVariantValue<QColor>(value));
		}
		else if (id == QLatin1String("axes.visible"))
		{
			child->SetAxesVisible(value.toBool());
		}
		else if (id == QLatin1String("cubeaxes.visible"))
		{
			child->SetCubeAxesVisible(value.toBool());
		}
		else if (id == QLatin1String("cubeaxes.resizeonselection"))
		{
			child->SetCubeAxesResizeOnSelection(value.toBool());
		}
		else if (id == QLatin1String("cubeaxes.labelscale"))
		{
			child->SetCubeAxesLabelScaleFactor(value.toDouble());
		}
		else if (id == QLatin1String("cubeaxes.xaxis.min"))
		{
			child->SetCubeAxesBounds(value.toDouble(), 0);
		}
		else if (id == QLatin1String("cubeaxes.xaxis.max"))
		{
			child->SetCubeAxesBounds(value.toDouble(), 1);
		}
		else if (id == QLatin1String("cubeaxes.yaxis.min"))
		{
			child->SetCubeAxesBounds(value.toDouble(), 2);
		}
		else if (id == QLatin1String("cubeaxes.yaxis.max"))
		{
			child->SetCubeAxesBounds(value.toDouble(), 3);
		}
		else if (id == QLatin1String("cubeaxes.zaxis.min"))
		{
			child->SetCubeAxesBounds(value.toDouble(), 4);
		}
		else if (id == QLatin1String("cubeaxes.zaxis.max"))
		{
			child->SetCubeAxesBounds(value.toDouble(), 5);
		}
		else if (id == QLatin1String("cubeaxes.xaxis.gridline"))
		{
			child->SetCubeAxesDrawXGridlines(value.toBool());
		}
		else if (id == QLatin1String("cubeaxes.yaxis.gridline"))
		{
			child->SetCubeAxesDrawYGridlines(value.toBool());
		}
		else if (id == QLatin1String("cubeaxes.zaxis.gridline"))
		{
			child->SetCubeAxesDrawZGridlines(value.toBool());
		}
        /*
		else if (id == QLatin1String("scaleaxes.visible"))
		{
			child->SetScaleAxesVisible(value.toBool());
		}
		else if (id == QLatin1String("scaleaxes.xaxis.labelscale"))
		{
			child->SetScaleAxesLabelScale(0, value.toDouble());
		}
		else if (id == QLatin1String("scaleaxes.yaxis.labelscale"))
		{
			child->SetScaleAxesLabelScale(1, value.toDouble());
		}
		else if (id == QLatin1String("scaleaxes.zaxis.labelscale"))
		{
			child->SetScaleAxesLabelScale(2, value.toDouble());
		}
        */
        
		else if (id == QLatin1String("background.color"))
		{
			child->SetBackgroundColor(qVariantValue<QColor>(value));
		}
		else if (id == QLatin1String("colormap.type"))
		{
			child->SetColorMap((PvizWidget::ColorMap_t)value.toInt());
			RefreshTreeColor();
		}
		else if (id == QLatin1String("plot.visible"))
		{
			child->SetPlotVisible(value.toBool());
		}
		else if (id == QLatin1String("plot.linewidth"))
		{
			child->SetPlotLineWidth(value.toInt());
		}
		else if (id == QLatin1String("plot.pointsize"))
		{
			child->SetPlotPointSize(value.toInt());
		}
		else if (id == QLatin1String("glyph.visible"))
		{
			child->SetGlyphVisible(value.toBool());
		}
		else if (id == QLatin1String("glyph.scalefactor"))
		{
			child->SetGlyphScaleFactor(value.toDouble());
		}
		else if (id == QLatin1String("glyph.autoorientation"))
		{
			child->SetGlyphAutoOrientation(value.toBool());
		}
		else if (id == QLatin1String("legend.visible"))
		{
			child->SetLegendVisible(value.toBool());
		}
		else if (id == QLatin1String("legend.position"))
		{
			child->SetLegendPosition((PvizWidget::LegendLocation_t)value.toInt());
		}
		else if (id == QLatin1String("legend.heightfactor"))
		{
			child->SetLegendHeightFactor(value.toDouble());
		}
		else if (id == QLatin1String("legend.widthfactor"))
		{
			child->SetLegendWidthFactor(value.toDouble());
		}
		else if (id == QLatin1String("fps.visible"))
		{
			child->SetFPSVisible(value.toDouble());
		}
		else if (id == QLatin1String("camera.sync"))
		{
			qDebug() << "Property changed - camera.sync ... ";
			if (value.toBool()) 
			{
				sync.AddMember(child);
				sync.InstallCallback(child);
			}
			else 
			{
				sync.RemoveMember(child);
			}
		}
		else if (id == QLatin1String("camera.focusmode"))
		{
			child->SetFocusMode((PvizWidget::FocusMode_t)value.toInt());
		}
		else if (id == QLatin1String("camera.focus.x"))
		{
			child->SetCameraFocusX(value.toDouble());
		}
		else if (id == QLatin1String("camera.focus.y"))
		{
			child->SetCameraFocusY(value.toDouble());
		}
		else if (id == QLatin1String("camera.focus.z"))
		{
			child->SetCameraFocusZ(value.toDouble());
		}
		else if (id == QLatin1String("label.visible"))
		{
			child->SetLabelVisible(value.toDouble());
		}
        /*
		else if (id == QLatin1String("label.pickdelta"))
		{
			child->SetPickDelta(value.toInt());
		}
         */
		else if (id == QLatin1String("neighbor.num"))
		{
			child->SetNumOfNeighbors(value.toInt());
		}
		else if (id == QLatin1String("neighbor.append"))
		{
			child->SetAppendNeighbors(value.toBool());
		}
		else if (id == QLatin1String("default.color"))
		{
			child->SetDefaultColor(qVariantValue<QColor>(value));
		}
		else if (id == QLatin1String("default.scale"))
		{
			child->SetDefaultScale(value.toDouble());
		}
	}
    
    QString id = featureToId[property];
    if (id == QLatin1String("broker.brokerIP"))
    {
        brokerIP = value.toString();
    }
    else if (id == QLatin1String("broker.brokerPort"))
    {
        brokerPort = value.toInt();
    }
    else if (id == QLatin1String("broker.destURI"))
    {
        destURI = value.toString();
    }
    else if (id == QLatin1String("broker.listenURI"))
    {
        listenURI = value.toString();
    }
    
}

void MainWindow::propertyValueChanged(QtProperty *property, const QVariant &value)
{
	if (PvizWidget *child = activeChild())
	{
        qDebug() << "propertyValueChanged ... ";
        
		QStandardItemModel *treeModel = pvizToModelMap[activeChild()];
		
		if (!propertyToId.contains(property))
			return;
		
		//if (!currentHeadItem)
		//	return;
		
		QString id = propertyToId[property];
		
		qDebug() << "Value changed ... " << id;
        
        QVector<QStandardItem*> items;
        //QVector<QStandardItem*>::iterator it;
        
        //QModelIndexList selectedModelIndeces = ui->tvTree->selectionModel()->selectedRows();
        QModelIndexList selectedModelIndeces = getSelectedRows();
        foreach (QModelIndex modelindex, selectedModelIndeces)
        {
            QModelIndex siblingIndex = modelindex.sibling(modelindex.row(), 1);
            QStandardItem* col1 = treeModel->itemFromIndex(modelindex);
            QStandardItem* col2 = treeModel->itemFromIndex(siblingIndex);
            
            uint cid = col1->data(Qt::DisplayRole).toUInt();
            items.push_back(col1);
        }
        
#pragma omp parallel private(it) default(shared)
        for (QVector<QStandardItem*>::iterator it  = items.begin(); it != items.end(); it++)
        //foreach(QStandardItem* item, items)
        {
#ifdef _OPENMP
            int tid = omp_get_thread_num();
            qDebug() << "OMP Thread ID: " << tid;
#endif
            
            QStandardItem* item = *it;
            QModelIndex	modelindex = treeModel->indexFromItem(item);
            
            QModelIndex siblingIndex = modelindex.sibling(modelindex.row(), 1);
            QStandardItem* col1 = treeModel->itemFromIndex(modelindex);
            QStandardItem* col2 = treeModel->itemFromIndex(siblingIndex);
            uint cid = col1->data(Qt::DisplayRole).toUInt();

            if (id == QLatin1String("label")) 
            {
                col2->setData(QString("%1").arg(qVariantValue<QString>(value)), Qt::DisplayRole);
                child->SetClusterLabel(cid, std::string(qVariantValue<QString>(value).toAscii().data()));
            }            
            else if (id == QLatin1String("color")) 
            {
                col1->setData(qVariantValue<QColor>(value), Qt::DecorationRole);
                child->SetClusterColor(cid, qVariantValue<QColor>(value));
                
                // PvizModel Update
                //ui->pvizWidget->model->clusters[cid]->color.ui_color = qVariantValue<QColor>(value).rgb();
                
                // PolyData Update
                // Not yet
                
                // Update Pviz
                //plot->Modified();
            } 
            else if (id == QLatin1String("visible")) 
            {
                // Now using slot/signal 2011.07.09
                //currentHeadItem->setData((value.toBool()? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
                
                child->SetClusterVisible(cid, value.toBool());
            }
            else if (id == QLatin1String("isdefault")) 
            {
                // Now using slot/signal 2011.07.09
                //currentHeadItem->setData((value.toBool()? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
                
                child->SetClusterAsDefault(cid, value.toBool());
                
            }
            else if (id == QLatin1String("scale")) 
            {
                child->SetClusterScale(cid, value.toDouble());		
            }
            else if (id == QLatin1String("shape")) 
            {
                child->SetClusterShape(cid, value.toInt());		
            }
        }
        
        child->RefreshAll();
	}
}

// Seee http://www.qtcentre.org/archive/index.php/t-16933.html
// Credit to verma
QModelIndexList MainWindow::getSelectedRows()
{
    QModelIndexList lstIndex ; 
    
	QItemSelection ranges = ui->tvTree->selectionModel()->selection();
    for (int i = 0; i < ranges.count(); ++i) 
    {
        QModelIndex parent = ranges.at(i).parent();
        int right = ranges.at(i).model()->columnCount(parent) - 1;
        if (ranges.at(i).left() == 0 && ranges.at(i).right() == right)
            for (int r = ranges.at(i).top(); r <= ranges.at(i).bottom(); ++r)
                lstIndex.append(ranges.at(i).model()->index(r, 0, parent));
    }
    return lstIndex;
}

void MainWindow::treeClicked(const QModelIndex & index)
{
	if (PvizWidget *child = activeChild())
	{
		qDebug() << "treeClicked ... ";
		
		QStandardItemModel *treeModel = pvizToModelMap[child];
        QModelIndex currentHeadIndex = index.sibling(index.row(), 0);
        QStandardItem* currentItem = treeModel->itemFromIndex(currentHeadIndex);
        //qDebug() << currentItem->row() << currentItem->column();
        
        uint cid = currentItem->data(Qt::DisplayRole).toUInt();
        child->SetClusterVisible(cid, currentItem->data(Qt::CheckStateRole).toBool());
        
        child->RefreshAll();
        
		/*
        QMap<QtProperty *, QString>::ConstIterator itProp = propertyToId.constBegin();
		while (itProp != propertyToId.constEnd()) {
			delete itProp.key();
			itProp++;
		}
		propertyToId.clear();
		idToProperty.clear();
         */
		
		//QtVariantProperty *property;
        //uint cid = currentItem->data(Qt::DisplayRole).toUInt();
        //child->SetClusterVisible(cid, currentItem->data(Qt::CheckStateRole).toBool());
		
		//PvizCluster *c = child->model->clusters[cid];
		//statusMessage(QString("cluster size : %1").arg(c->points.size()));
		
        
		//property = vman1->addProperty(QVariant::Bool, tr("Visible"));
		//property->setValue(currentHeadItem->data(Qt::CheckStateRole).toBool());
		//addProperty(property, QLatin1String("visible"));
		// vman1 has a bug in emitting valugeChanged signal
		// Explicit call propertyValueChanged to set visible
		//propertyValueChanged(property, currentHeadItem->data(Qt::CheckStateRole));
		
        /*
		property = vman1->addProperty(QVariant::String, tr("Label"));
		property->setValue(treeModel->item(currentHeadItem->row(), 1)->data(Qt::DisplayRole).toString());
		addProperty(property, QLatin1String("label"));
		
		property = vman1->addProperty(QVariant::Color, tr("Color"));
		property->setValue(currentHeadItem->data(Qt::DecorationRole).value<QColor>());
		addProperty(property, QLatin1String("color"));	
		
		property = vman1->addProperty(QVariant::Double, tr("Glyph Scale"));
		property->setAttribute(QLatin1String("minimum"), 0);
		property->setAttribute(QLatin1String("maximum"), 100);
		property->setAttribute(QLatin1String("singleStep"), 0.1);
		property->setValue(c->scale);
		addProperty(property, QLatin1String("scale"));
         */
	}
}

void MainWindow::treeSectionClicked(int logicalIndex)
{
    qDebug() << "treeSectionClicked ... " << logicalIndex;
    if (PvizWidget *child = activeChild())
	{
        QString title = pvizToModelMap[child]->horizontalHeaderItem(logicalIndex)->data(Qt::DisplayRole).toString();
        //qDebug() << title;
        
        if (title == "Size") 
        {
            child->SortClusterBySize((child->model->sortStatus % 2) == 0);
        }
        else if (title == "Cluster")
        {
            child->SortClusterById((child->model->sortStatus % 2) == 0);
        }
        else
        {
            child->SortClusterByLabel((child->model->sortStatus % 2) == 0);
        }
        
        BuildTree(child);
		ui->tvTree->setModel(pvizToModelMap[child]);
        ui->tvTree->header()->setSortIndicator(logicalIndex, child->model->sortStatus % 2 ? Qt::DescendingOrder : Qt::AscendingOrder);
        ui->tvTree->header()->setSortIndicatorShown(true);
        connect(ui->tvTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                this, SLOT(treeSelectionChanged(const QItemSelection &, const QItemSelection &)));
    }
}

void MainWindow::treeSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    if (PvizWidget *child = activeChild())
	{
		//qDebug() << "treeSelectionChanged ...";
		
        //QStandardItemModel *treeModel = pvizToModelMap[child];
        
        // selectedRows has a bug. See http://bugreports.qt.nokia.com/browse/QTBUG-7884
        //QModelIndexList selectedModelIndeces = ui->tvTree->selectionModel()->selectedRows();
        QModelIndexList selectedModelIndeces = getSelectedRows();

        QMap<QtProperty *, QString>::ConstIterator itProp = propertyToId.constBegin();
        while (itProp != propertyToId.constEnd()) {
            delete itProp.key();
            itProp++;
        }
		//qDebug() << &(propertyToId);
        propertyToId.clear();
        idToProperty.clear();
        
        disconnect(ui->tvTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                this, SLOT(treeSelectionChanged(const QItemSelection &, const QItemSelection &)));
        disconnect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                   this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
        
        if (selectedModelIndeces.size() > 1)
        {
            QModelIndex firstIndex = selectedModelIndeces[0];
            uint cid = firstIndex.data(Qt::DisplayRole).toUInt();
            PvizCluster *cluster = child->model->clusters[cid];
            
            bool allvisible = firstIndex.data(Qt::CheckStateRole).toBool();
            int allshape = cluster->shape;
            bool allisdefault = cluster->IsDefault();
            float allscale = child->GetClusterScale(cid);
            QColor allcolor = firstIndex.data(Qt::DecorationRole).value<QColor>();
            
            bool allvisibleChanged = false;
            bool allshapeChanged = false;
            bool allisdefaultChanged = false;
            bool allscaleChanged = false;
            bool allcolorChanged = false;
            
            
            foreach(QModelIndex index, selectedModelIndeces)
            {
                cid = index.data(Qt::DisplayRole).toUInt();
                cluster = child->model->clusters[cid];
                
                if (allvisible != index.data(Qt::CheckStateRole).toBool())
                {
                    allvisibleChanged = true;
                    allvisible = false;
                }

                if (allshape != cluster->shape)
                {
                    allshapeChanged = true;
                    allshape = -1;
                }

                if (allisdefault != cluster->IsDefault())
                {
                    allisdefaultChanged = true;
                    allisdefault = false;
                }
                
                if (allscale != child->GetClusterScale(cid))
                {
                    allscaleChanged = true;
                    allscale = 0.0;
                }
                
                if (allcolor != index.data(Qt::DecorationRole).value<QColor>())
                {
                    allcolorChanged = true;
                    allcolor = QColor(119, 136, 153);
                }
            }
            
            QtVariantProperty *property;
            
            if (allvisibleChanged)
            {
                property = vman1->addProperty(QVariant::String, tr("Visible"));
                property->setValue("Multiple");
                addProperty(property, QLatin1String("visible.invalid"));   
                
                property = vman1->addProperty(QVariant::Bool, tr("Reset"));
                property->setValue(allvisible);
                addProperty(property, QLatin1String("visible"));            
            }
            else
            {
                property = vman1->addProperty(QVariant::Bool, tr("Visible"));
                property->setValue(allvisible);
                addProperty(property, QLatin1String("visible"));            
            }
            
            if (allshapeChanged)
            {
                property = vman1->addProperty(QtVariantPropertyManager::enumTypeId(), tr("Shape"));
                QStringList enumNames;
                enumNames 
                << "Triangle (2D)" 
                << "Rectangle (2D)" 
                << "Petagon (2D)" 
                << "Hexagon (2D)" 
                << "Tetrahedron (3D)" 
                << "Cube (3D)" 
                << "Sphere (3D)"
                << "Cylinder (3D)"
                << "Multiple";
                
                property->setAttribute(QLatin1String("enumNames"), enumNames);
                
                property->setValue(8);
                
                addProperty(property, QLatin1String("shape"));            
            }
            else
            {
                property = vman1->addProperty(QtVariantPropertyManager::enumTypeId(), tr("Shape"));
                QStringList enumNames;
                enumNames 
                << "Triangle (2D)" 
                << "Rectangle (2D)" 
                << "Petagon (2D)" 
                << "Hexagon (2D)" 
                << "Tetrahedron (3D)" 
                << "Cube (3D)" 
                << "Sphere (3D)"
                << "Cylinder (3D)";

                property->setAttribute(QLatin1String("enumNames"), enumNames);
                
                property->setValue(allshape);
                
                addProperty(property, QLatin1String("shape"));            
            }
            
            if (allisdefaultChanged)
            {
                property = vman1->addProperty(QVariant::String, tr("Default"));
                property->setValue("Multiple");
                addProperty(property, QLatin1String("isdefault.invalid"));   
                
                property = vman1->addProperty(QVariant::Bool, tr("Reset"));
                property->setValue(allisdefault);
                addProperty(property, QLatin1String("isdefault"));            
            }
            else
            {
                property = vman1->addProperty(QVariant::Bool, tr("Default"));
                property->setValue(allisdefault);
                addProperty(property, QLatin1String("isdefault"));            
            }
            
            property = vman1->addProperty(QVariant::String, tr("Label"));
            //property->setValue("");
            addProperty(property, QLatin1String("label"));
            
            if (allcolorChanged)
            {
                property = vman1->addProperty(QVariant::String, tr("Color"));
                property->setValue("Multiple");
                addProperty(property, QLatin1String("color.invalid"));	
                
                property = vman1->addProperty(QVariant::Color, tr("Reset"));
                property->setValue(allcolor);
                addProperty(property, QLatin1String("color"));	
            }
            else
            {
                property = vman1->addProperty(QVariant::Color, tr("Color"));
                property->setValue(allcolor);
                addProperty(property, QLatin1String("color"));	
            }
            
            if (allscaleChanged)
            {
                property = vman1->addProperty(QVariant::String, tr("Glyph Scale"));
                property->setValue("Multiple");
                addProperty(property, QLatin1String("scale.invalid"));
                
                property = vman1->addProperty(QVariant::Double, tr("Reset"));
                property->setAttribute(QLatin1String("minimum"), 0);
                property->setAttribute(QLatin1String("maximum"), 100);
                property->setAttribute(QLatin1String("singleStep"), 0.1);
                property->setValue(allscale);
                addProperty(property, QLatin1String("scale"));
            }
            else
            {
                property = vman1->addProperty(QVariant::Double, tr("Glyph Scale"));
                property->setAttribute(QLatin1String("minimum"), 0);
                property->setAttribute(QLatin1String("maximum"), 100);
                property->setAttribute(QLatin1String("singleStep"), 0.1);
                property->setValue(allscale);
                addProperty(property, QLatin1String("scale"));
            }            
        }
        else if (selectedModelIndeces.size() == 1)
        {
            QModelIndex currentIndex = selectedModelIndeces[0];
            
            QtVariantProperty *property;
            uint cid = currentIndex.data(Qt::DisplayRole).toUInt();
            
            PvizCluster *c = child->model->clusters[cid];
            statusMessage(QString("cluster size : %1").arg(c->points.size()));
            
            property = vman1->addProperty(QVariant::Bool, tr("Visible"));
            property->setValue(currentIndex.data(Qt::CheckStateRole).toBool());
            addProperty(property, QLatin1String("visible"));            
            // vman1 has a bug in emitting valugeChanged signal
            // Explicit call propertyValueChanged to set visible
            //propertyValueChanged(property, c->visible);
            
            property = vman1->addProperty(QtVariantPropertyManager::enumTypeId(), tr("Shape"));
            QStringList enumNames;
            enumNames 
            << "Triangle (2D)" 
            << "Rectangle (2D)" 
            << "Petagon (2D)" 
            << "Hexagon (2D)" 
            << "Tetrahedron (3D)" 
            << "Cube (3D)" 
            << "Sphere (3D)"
            << "Cylinder (3D)";            
            property->setAttribute(QLatin1String("enumNames"), enumNames);
            property->setValue(c->shape);
            addProperty(property, QLatin1String("shape"));            
            
            
            property = vman1->addProperty(QVariant::Bool, tr("Default"));
            property->setValue(c->IsDefault());
            addProperty(property, QLatin1String("isdefault"));            
            
            property = vman1->addProperty(QVariant::String, tr("Label"));
            property->setValue(QString("%1").arg(c->label.c_str()));
            addProperty(property, QLatin1String("label"));
            
            property = vman1->addProperty(QVariant::Color, tr("Color"));
            property->setValue(currentIndex.data(Qt::DecorationRole).value<QColor>());
            addProperty(property, QLatin1String("color"));	
            
            property = vman1->addProperty(QVariant::Double, tr("Glyph Scale"));
            property->setAttribute(QLatin1String("minimum"), 0);
            property->setAttribute(QLatin1String("maximum"), 100);
            property->setAttribute(QLatin1String("singleStep"), 0.1);
            property->setValue(child->GetClusterScale(cid));
            addProperty(property, QLatin1String("scale"));
        }
        
        connect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
        connect(ui->tvTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                   this, SLOT(treeSelectionChanged(const QItemSelection &, const QItemSelection &)));
		
    }
}

void MainWindow::addProperty(QtVariantProperty *property, const QString &id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
	
    ui->tpProperty->addProperty(property);
}

void MainWindow::buildFeatureTree()
{
	QtVariantEditorFactory *vfac2 = new QtVariantEditorFactory(this);
	ui->tpPreferences->setFactoryForManager(vman2, vfac2);
	
    QMap<QtProperty *, QString>::ConstIterator itProp = featureToId.constBegin();
    while (itProp != featureToId.constEnd()) {
        delete itProp.key();
        itProp++;
    }
    featureToId.clear();
    idToFeature.clear();
	
	buildFeatureTree_AddTitleFeature();
	buildFeatureTree_AddAxesFeature();
	buildFeatureTree_AddCubeAxesFeature();
    //buildFeatureTree_AddScaleAxesFeature();
	buildFeatureTree_AddBackgroundColorFeature();
	buildFeatureTree_AddColorMapFeature();
	buildFeatureTree_AddPlotFeature();
	buildFeatureTree_AddGlyphFeature();
	buildFeatureTree_AddLegendFeature();
	buildFeatureTree_AddFPSFeature();
	buildFeatureTree_AddCameraLinkFeature();
	buildFeatureTree_AddLabelFeature();
	buildFeatureTree_AddNumOfNeighbors();
	buildFeatureTree_AddDefaultFeature();
	buildFeatureTree_AddBrokerFeature();
}

void MainWindow::buildFeatureTree_AddTitleFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Title");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::String, tr("Name"));
	property->setValue("");
	addFeature(root, property, QLatin1String("title.name"));
	
	property = vman2->addProperty(QVariant::Bool, tr("Visible"));
	//property->setValue(ui->pvizWidget->GetPlotVisible());	
	property->setValue(true);	
	addFeature(root, property, QLatin1String("title.visible"));
    
	property = vman2->addProperty(QVariant::Color, tr("Color"));
	property->setValue(Qt::red);
	addFeature(root, property, QLatin1String("title.color"));	
    
    QtBrowserItem* item = ui->tpPreferences->addProperty(root);
	ui->tpPreferences->setExpanded(item->children()[2], false);	
}

void MainWindow::buildFeatureTree_AddAxesFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Axes");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Bool, tr("Visible"));
	property->setValue(true);
	addFeature(root, property, QLatin1String("axes.visible"));
	
    ui->tpPreferences->addProperty(root);
    //ui->tpPreferences->addProperty(property);
}

void MainWindow::buildFeatureTree_AddCubeAxesFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Cube Axes");
	QtProperty *subroot;
    
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Bool, tr("Visible"));
	property->setValue(false);
	addFeature(root, property, QLatin1String("cubeaxes.visible"));
        
	property = vman2->addProperty(QVariant::Bool, tr("Resize on selection"));
	property->setValue(false);
	addFeature(root, property, QLatin1String("cubeaxes.resizeonselection"));

	property = vman2->addProperty(QVariant::Double, tr("Label Scale"));
	property->setValue(1.0);
    property->setAttribute(QLatin1String("minimum"), 0.1);
    property->setAttribute(QLatin1String("maximum"), 5.0);
    property->setAttribute(QLatin1String("singleStep"), 0.1);
	addFeature(root, property, QLatin1String("cubeaxes.labelscale"));
    
	groupManager = new QtGroupPropertyManager(this);
    subroot = groupManager->addProperty("X Axis");
    root->addSubProperty(subroot);
    
	property = vman2->addProperty(QVariant::Double, tr("Min"));
    property->setValue(0.0);
	addFeature(subroot, property, QLatin1String("cubeaxes.xaxis.min"));
    
	property = vman2->addProperty(QVariant::Double, tr("Max"));
    property->setValue(0.0);
	addFeature(subroot, property, QLatin1String("cubeaxes.xaxis.max"));
    
	property = vman2->addProperty(QVariant::Bool, tr("Gridline Visible"));
	property->setValue(false);
	addFeature(subroot, property, QLatin1String("cubeaxes.xaxis.gridline"));
    
	groupManager = new QtGroupPropertyManager(this);
    subroot = groupManager->addProperty("Y Axis");
    root->addSubProperty(subroot);
    
	property = vman2->addProperty(QVariant::Double, tr("Min"));
    property->setValue(0.0);
	addFeature(subroot, property, QLatin1String("cubeaxes.yaxis.min"));
    
	property = vman2->addProperty(QVariant::Double, tr("Max"));
    property->setValue(0.0);
	addFeature(subroot, property, QLatin1String("cubeaxes.yaxis.max"));
    
	property = vman2->addProperty(QVariant::Bool, tr("Gridline Visible"));
	property->setValue(false);
	addFeature(subroot, property, QLatin1String("cubeaxes.yaxis.gridline"));
    
	groupManager = new QtGroupPropertyManager(this);
    subroot = groupManager->addProperty("Z Axis");
    root->addSubProperty(subroot);
    
	property = vman2->addProperty(QVariant::Double, tr("Min"));
    property->setValue(0.0);
	addFeature(subroot, property, QLatin1String("cubeaxes.zaxis.min"));
    
	property = vman2->addProperty(QVariant::Double, tr("Max"));
    property->setValue(0.0);
	addFeature(subroot, property, QLatin1String("cubeaxes.zaxis.max"));
    
	property = vman2->addProperty(QVariant::Bool, tr("Gridline Visible"));
	property->setValue(false);
	addFeature(subroot, property, QLatin1String("cubeaxes.zaxis.gridline"));
    
    QtBrowserItem* item = ui->tpPreferences->addProperty(root);
    for (int i = 0; i < item->children().count(); i++)
    {
        ui->tpPreferences->setExpanded(item->children()[i], false);
    }
}

/*
void MainWindow::buildFeatureTree_AddScaleAxesFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Scale Axes");
	QtProperty *subroot;
    
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Bool, tr("Visible"));
	property->setValue(false);
	addFeature(root, property, QLatin1String("scaleaxes.visible"));

	groupManager = new QtGroupPropertyManager(this);
    subroot = groupManager->addProperty("X Axis");
    root->addSubProperty(subroot);
    
	property = vman2->addProperty(QVariant::Double, tr("Label Scale"));
    property->setValue(0.0);
    property->setAttribute(QLatin1String("minimum"), 0);
	addFeature(subroot, property, QLatin1String("scaleaxes.xaxis.labelscale"));
    
	groupManager = new QtGroupPropertyManager(this);
    subroot = groupManager->addProperty("Y Axis");
    root->addSubProperty(subroot);
    
	property = vman2->addProperty(QVariant::Double, tr("Label Scale"));
    property->setValue(0.0);
    property->setAttribute(QLatin1String("minimum"), 0);
	addFeature(subroot, property, QLatin1String("scaleaxes.yaxis.labelscale"));
    
	groupManager = new QtGroupPropertyManager(this);
    subroot = groupManager->addProperty("Z Axis");
    root->addSubProperty(subroot);
    
	property = vman2->addProperty(QVariant::Double, tr("Label Scale"));
    property->setValue(0.0);
    property->setAttribute(QLatin1String("minimum"), 0);
	addFeature(subroot, property, QLatin1String("scaleaxes.zaxis.labelscale"));
    
    QtBrowserItem* item = ui->tpPreferences->addProperty(root);
}
*/

void MainWindow::buildFeatureTree_AddBackgroundColorFeature()
{
	//QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    //QtProperty *root = groupManager->addProperty("Background");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Color, tr("Background"));
	//property->setValue(ui->pvizWidget->GetBackgroundColor());
	property->setValue(Qt::black);
	
	addFeature(NULL, property, QLatin1String("background.color"));
	
    QtBrowserItem* item = ui->tpPreferences->addProperty(property);
	ui->tpPreferences->setExpanded(item, false);	
}

void MainWindow::buildFeatureTree_AddColorMapFeature()
{
	//QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    //QtProperty *root = groupManager->addProperty("Color Map");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QtVariantPropertyManager::enumTypeId(), tr("Color Map"));
	QStringList enumNames;
    enumNames 
    << "Matlab distinct (50)" << "Salsa custom (17)" 
    << "ColorBrewer Set1 (9)" << "ColorBrewer Paired (12)"
    << "Rainbow" << "Rainbow (rev)" << "Cool2warm" << "Cool2warm (rev)" << "Experimental" 
    << "Custom";
    property->setAttribute(QLatin1String("enumNames"), enumNames);
		
	//property->setValue(ui->pvizWidget->GetColorMap());
	property->setValue(0);
	
	addFeature(NULL, property, QLatin1String("colormap.type"));
	
    ui->tpPreferences->addProperty(property);
}

void MainWindow::buildFeatureTree_AddPlotFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Plot");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Bool, tr("Visible"));
	//property->setValue(ui->pvizWidget->GetPlotVisible());	
	property->setValue(true);	
	addFeature(root, property, QLatin1String("plot.visible"));
	
	property = vman2->addProperty(QVariant::Int, tr("Line Width"));
	//property->setValue(ui->pvizWidget->GetPlotLineWidth());
	property->setValue(2);
    property->setAttribute(QLatin1String("minimum"), 1);
    property->setAttribute(QLatin1String("maximum"), 10);
	addFeature(root, property, QLatin1String("plot.linewidth"));
	
	property = vman2->addProperty(QVariant::Int, tr("Point Size"));
	//property->setValue(ui->pvizWidget->GetPlotPointSize());
	property->setValue(2);
    property->setAttribute(QLatin1String("minimum"), 1);
    property->setAttribute(QLatin1String("maximum"), 50);
	addFeature(root, property, QLatin1String("plot.pointsize"));
	
    ui->tpPreferences->addProperty(root);
}

void MainWindow::buildFeatureTree_AddGlyphFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Glyph");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Bool, tr("Visible"));
	//property->setValue(ui->pvizWidget->GetGlyphVisible());	
	property->setValue(false);	
	addFeature(root, property, QLatin1String("glyph.visible"));
		
	property = vman2->addProperty(QVariant::Double, tr("Scale Factor"));
	//property->setValue(ui->pvizWidget->GetGlyphScaleFactor());
	property->setValue(1);
    property->setAttribute(QLatin1String("minimum"), 0);
    property->setAttribute(QLatin1String("maximum"), 100);
    property->setAttribute(QLatin1String("singleStep"), 1.00);
	addFeature(root, property, QLatin1String("glyph.scalefactor"));
	
	property = vman2->addProperty(QVariant::Bool, tr("Auto Orientation"));
	//property->setValue(ui->pvizWidget->GetGlyphVisible());	
	property->setValue(false);	
	addFeature(root, property, QLatin1String("glyph.autoorientation"));
    
    ui->tpPreferences->addProperty(root);
}


void MainWindow::buildFeatureTree_AddLegendFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Legend");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Bool, tr("Visible"));
	//property->setValue(ui->pvizWidget->GetLegendVisible());	
	property->setValue(false);	
	addFeature(root, property, QLatin1String("legend.visible"));
	
	property = vman2->addProperty(QtVariantPropertyManager::enumTypeId(), tr("Position"));
	QStringList enumNames;
    enumNames << "Upper-Left" << "Upper-Right" << "Lower-Left" << "Lower-Right";
    property->setAttribute(QLatin1String("enumNames"), enumNames);
	property->setValue(0);	
	addFeature(root, property, QLatin1String("legend.position"));
	
    property = vman2->addProperty(QVariant::Double, tr("Height"));
	//property->setValue(ui->pvizWidget->GetLegendHeightFactor());
	property->setValue(0.3);
    property->setAttribute(QLatin1String("minimum"), 0.0);
    property->setAttribute(QLatin1String("maximum"), 1.0);
    property->setAttribute(QLatin1String("singleStep"), 0.01);
	addFeature(root, property, QLatin1String("legend.heightfactor"));
	
	property = vman2->addProperty(QVariant::Double, tr("Width"));
	//property->setValue(ui->pvizWidget->GetLegendWidthFactor());
	property->setValue(0.3);
    property->setAttribute(QLatin1String("minimum"), 0.0);
    property->setAttribute(QLatin1String("maximum"), 1.0);
    property->setAttribute(QLatin1String("singleStep"), 0.01);
	addFeature(root, property, QLatin1String("legend.widthfactor"));
	
    ui->tpPreferences->addProperty(root);
}

void MainWindow::buildFeatureTree_AddFPSFeature()
{
	//QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    //QtProperty *root = groupManager->addProperty("FPS");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Bool, tr("FPS"));
	//property->setValue(ui->pvizWidget->GetFPSVisible());	
	property->setValue(false);	
	addFeature(NULL, property, QLatin1String("fps.visible"));
	
    //ui->tpPreferences->addProperty(root);
    ui->tpPreferences->addProperty(property);
}

void MainWindow::buildFeatureTree_AddLabelFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Label");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Bool, tr("Visible"));
	property->setValue(false);	
	addFeature(root, property, QLatin1String("label.visible"));

    /*
	property = vman2->addProperty(QVariant::Int, tr("Pick Radius"));
	property->setValue(2);	
	addFeature(root, property, QLatin1String("label.pickdelta"));
    property->setAttribute(QLatin1String("minimum"), 1);
     */
    
    ui->tpPreferences->addProperty(root);
}

void MainWindow::buildFeatureTree_AddNumOfNeighbors()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Neighbor Selection");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Int, tr("Num. Neighbors"));
	property->setValue(15);	
    property->setAttribute(QLatin1String("minimum"), 1);
	addFeature(root, property, QLatin1String("neighbor.num"));

	property = vman2->addProperty(QVariant::Bool, tr("Append"));
	//property->setValue(ui->pvizWidget->GetLegendVisible());	
	property->setValue(false);	
	addFeature(root, property, QLatin1String("neighbor.append"));
	
    ui->tpPreferences->addProperty(root);
}

void MainWindow::buildFeatureTree_AddDefaultFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Default Setting");

    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Color, tr("Color"));
	property->setValue(QColor(119, 136, 153)); // Light Slate Gray);
	addFeature(root, property, QLatin1String("default.color"));	
    
    property = vman2->addProperty(QVariant::Double, tr("Glyph Scale"));
    property->setAttribute(QLatin1String("minimum"), 0);
    property->setAttribute(QLatin1String("maximum"), 100);
    property->setAttribute(QLatin1String("singleStep"), 0.1);
    property->setValue(0.0);
    addFeature(root, property, QLatin1String("default.scale"));
    
    QtBrowserItem* item = ui->tpPreferences->addProperty(root);
	ui->tpPreferences->setExpanded(item->children()[0], false);	
}

void MainWindow::buildFeatureTree_AddBrokerFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Broker Setting");
    
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::String, tr("Broker IP"));
	property->setValue(brokerIP);
	addFeature(root, property, QLatin1String("broker.brokerIP"));	

    property = vman2->addProperty(QVariant::Int, tr("Broker Port"));
    property->setAttribute(QLatin1String("minimum"), 0);
	property->setValue(brokerPort);
	addFeature(root, property, QLatin1String("broker.brokerPort"));	

    property = vman2->addProperty(QVariant::String, tr("Dest URI"));
	property->setValue(destURI);
	addFeature(root, property, QLatin1String("broker.destURI"));	

    property = vman2->addProperty(QVariant::String, tr("Listen URI"));
	property->setValue(listenURI);
	addFeature(root, property, QLatin1String("broker.listenURI"));	

    QtBrowserItem* item = ui->tpPreferences->addProperty(root);
	ui->tpPreferences->setExpanded(item->children()[0], false);	
}

void MainWindow::buildFeatureTree_AddCameraLinkFeature()
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty("Camera");
	
    QtVariantProperty *property;
	property = vman2->addProperty(QVariant::Bool, tr("Use SyncCamera"));
	property->setValue(false);	
	addFeature(root, property, QLatin1String("camera.sync"));
	
	property = vman2->addProperty(QtVariantPropertyManager::enumTypeId(), tr("Focus Mode"));
	QStringList enumNames;
    enumNames << "Auto" << "Origin" << "Custom";
    property->setAttribute(QLatin1String("enumNames"), enumNames);
	property->setValue(0);	
	addFeature(root, property, QLatin1String("camera.focusmode"));
        
    
    QtProperty *subroot;
	groupManager = new QtGroupPropertyManager(this);
    subroot = groupManager->addProperty("Focus");
    root->addSubProperty(subroot);
    
	property = vman2->addProperty(QVariant::Double, tr("X"));
    property->setValue(0.0);
    property->setAttribute(QLatin1String("singleStep"), 0.1);
	addFeature(subroot, property, QLatin1String("camera.focus.x"));
    
	property = vman2->addProperty(QVariant::Double, tr("Y"));
    property->setValue(0.0);
    property->setAttribute(QLatin1String("singleStep"), 0.1);
	addFeature(subroot, property, QLatin1String("camera.focus.y"));
    
	property = vman2->addProperty(QVariant::Double, tr("Z"));
    property->setValue(0.0);
    property->setAttribute(QLatin1String("singleStep"), 0.1);
	addFeature(subroot, property, QLatin1String("camera.focus.z"));
    
    QtBrowserItem* item = ui->tpPreferences->addProperty(root);
	ui->tpPreferences->setExpanded(item->children()[2], false);	
}


void MainWindow::addFeature(QtProperty *root, QtVariantProperty *property, const QString &id)
{
    featureToId[property] = id;
    idToFeature[id] = property;
	
	if (root != NULL) 
	{
		root->addSubProperty(property);
		//ui->tpPreferences->addProperty(property);
	}
}

void MainWindow::ConnectToServer()
{
#ifdef PVIZRPC_CLIENT_ENABLE
	if (PvizWidget *child = activeChild())
	{
		child->ConnectToServer();
	}	
#else
		qDebug() << "PvizRPC client disabled.";
#endif
}

#ifdef PVIZRPC_CLIENT_ENABLE
void MainWindow::handle_timer()
{
	qDebug() << "handle_timer ... ";
	//boost::asio::deadline_timer t(io_service_, boost::posix_time::seconds(10));
	//t.async_wait(boost::bind(&MainWindow::handle_timer, this));
}
#endif

void MainWindow::UpdateFromServer()
{
	qDebug() << "UpdateFromServer ... ";
	
#ifdef PVIZRPC_CLIENT_ENABLE
	if (PvizWidget *child = activeChild())
	{
		child->UpdateFromServer();	
	}	
#else
		qDebug() << "PvizRPC client disabled.";
#endif
}

void MainWindow::ConnectToNBServer()
{
	PvizWidget *pviz = new PvizWidget();
    connect(pviz, SIGNAL(OnCenterPicked()), this, SLOT(UnpickCenter()));
    connect(pviz, SIGNAL(OnSelected()), this, SLOT(SetSelectionMode()));
    connect(pviz, SIGNAL(OnClusterVisibleChanged(PvizWidget*, unsigned int, bool)), this, SLOT(OnClusterVisibleChanged(PvizWidget*, unsigned int, bool)));
    connect(pviz, SIGNAL(OnClusterColorChanged(PvizWidget*, unsigned int, QColor)), this, SLOT(OnClusterColorChanged(PvizWidget*, unsigned int, QColor)));
    connect(pviz, SIGNAL(OnClusterDefaultChanged(PvizWidget*, unsigned int, bool)), this, SLOT(OnClusterDefaultChanged(PvizWidget*, unsigned int, bool)));
    connect(pviz, SIGNAL(OnClosing(PvizWidget*)), this, SLOT(OnClosing(PvizWidget*)));
    connect(pviz, SIGNAL(OnModelLoaded(PvizWidget*)), this, SLOT(OnModelLoaded(PvizWidget*)));
    connect(pviz, SIGNAL(OnCameraFocusChanged(PvizWidget*, double *pos)), this, SLOT(OnCameraFocusChanged(PvizWidget*, double *pos)));
    
	pviz->setMinimumSize(200, 200);
    
    qDebug() << "PvizWidget created : " << pviz;
    
    /*
    try
    {
        pviz->ConnectToNBServer("156.56.104.176", 55045, 10001, 55745, "/tmp/topic1");
    }
    catch (ServiceException& e)
    {
        statusMessage(QString(e.getMessage().c_str()));
        delete pviz;
        return;
    }
     */

	QMdiSubWindow* win = ui->mdiArea->addSubWindow(pviz);
	win->resize(500, 500);
	win->setWindowTitle("Naradabroker:/tmp/topic1");
	//connect(win, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), 
	//		this, SLOT(windowStateChanged(Qt::WindowStates, Qt::WindowStates)));
	//pviz->show();
	pviz->showMaximized();
}

void MainWindow::ConnectToActiveMQServer()
{
#ifdef USE_ACTIVEMQ    
	PvizWidget *pviz = new PvizWidget();
    connect(pviz, SIGNAL(OnCenterPicked()), this, SLOT(UnpickCenter()));
    connect(pviz, SIGNAL(OnSelected()), this, SLOT(SetSelectionMode()));
    connect(pviz, SIGNAL(OnClusterVisibleChanged(PvizWidget*, unsigned int, bool)), this, SLOT(OnClusterVisibleChanged(PvizWidget*, unsigned int, bool)));
    connect(pviz, SIGNAL(OnClusterColorChanged(PvizWidget*, unsigned int, QColor)), this, SLOT(OnClusterColorChanged(PvizWidget*, unsigned int, QColor)));
    connect(pviz, SIGNAL(OnClusterDefaultChanged(PvizWidget*, unsigned int, bool)), this, SLOT(OnClusterDefaultChanged(PvizWidget*, unsigned int, bool)));
    connect(pviz, SIGNAL(OnClosing(PvizWidget*)), this, SLOT(OnClosing(PvizWidget*)));
    connect(pviz, SIGNAL(OnModelLoaded(PvizWidget*)), this, SLOT(OnModelLoaded(PvizWidget*)));
    connect(pviz, SIGNAL(OnCameraFocusChanged(PvizWidget*, double *pos)), this, SLOT(OnCameraFocusChanged(PvizWidget*, double *pos)));
    
	pviz->setMinimumSize(200, 200);
	pviz->SetColorMap(PvizWidget::RAINBOW);
    
    qDebug() << "PvizWidget created : " << pviz;
    
    //std::string brokerURI,
    //std::string destURI,
    //bool useTopic,
    //bool clientAck
    
    // Set the URI to point to the IPAddress of your broker.
    // add any optional params to the url to enable things like
    // tightMarshalling or tcp logging etc.  See the CMS web site for
    // a full list of configuration options.
    //
    //  http://activemq.apache.org/cms/
    //
    // Wire Format Options:
    // =====================
    // Use either stomp or openwire, the default ports are different for each
    //
    // Examples:
    //    tcp://127.0.0.1:61616                      default to openwire
    //    tcp://127.0.0.1:61616?wireFormat=openwire  same as above
    //    tcp://127.0.0.1:61613?wireFormat=stomp     use stomp instead
    //

    QString brokerURI = QString("tcp://%1:%2?wireFormat.maxInactivityDuration=0").arg(brokerIP).arg(brokerPort);
   
    /*
    std::string brokerURI =
    "failover:(tcp://" + brokerIP +":" + brokerPort + 
    "?wireFormat.maxInactivityDuration=0"
    //        "?wireFormat=openwire"
    //        "&connection.useAsyncSend=true"
    //        "&transport.commandTracingEnabled=true"
    //        "&transport.tcpTracingEnabled=true"
    //        "&wireFormat.tightEncodingEnabled=true"
    ")";
     */
    
    pviz->ConnectToActiveMQServer(brokerURI, destURI, listenURI);
    
    //pviz->LoadXmlDataFile("/Users/jychoi/Project/pviz3/data/100043-plots/reference.pviz");
    //BuildTree(pviz);

	QMdiSubWindow* win = ui->mdiArea->addSubWindow(pviz);
	win->resize(500, 500);
	win->setWindowTitle(brokerURI);
	//connect(win, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), 
	//		this, SLOT(windowStateChanged(Qt::WindowStates, Qt::WindowStates)));
	//pviz->show();
	pviz->showMaximized();
	
	pviz->SetInteractorMode(PvizWidget::PLAY_MODE);
	ui->actionPlay->setChecked(true);
	ui->actionSelectionMode->setChecked(false);
#endif
}

/*
void MainWindow::reset()
{
	delete treeModel;
	treeModel = new QStandardItemModel();
	treeModel->setColumnCount(2);
	treeModel->setHorizontalHeaderItem( 0, new QStandardItem( "Cluster" ) );
	treeModel->setHorizontalHeaderItem( 1, new QStandardItem( "Label" ) );
	
	ui->tvTree->setModel(treeModel);
	ui->tvTree->reset();
	
	ui->pvizWidget->reset();
}
*/

void MainWindow::SaveScreen()
{
    QString fileName = "";
    if (PvizWidget *child = activeChild())
    {
        fileName = child->GetFileName();
    }
    //qDebug() << "fileName :" << fileName;
    //qDebug() << "fileName :" << QFileInfo(QFileInfo(fileName).dir(), QFileInfo(fileName).completeBaseName()).filePath();
    
	fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                            QFileInfo(QFileInfo(fileName).dir(), QFileInfo(fileName).completeBaseName()).filePath(),
                                            tr("PNG file (*.png);; All files (*.*)"));
	
    if (fileName.isEmpty())
	{
		qDebug() << "No file is selected ... ";
		return;
	}
	
    settings->setValue("LastOpenDirectory", QFileInfo(fileName).path());
	if (PvizWidget *child = activeChild())
	{
		child->SaveScreen(fileName);
		statusMessage(tr("File saved"));
	}
}

void MainWindow::statusMessage(QString msg)
{
	//ui->statusBar->showMessage(msg);
	statusBar()->showMessage(msg, 10000);
}

void MainWindow::ShowHalfSphere()
{
	if (PvizWidget *child = activeChild())
	{
		if (child->GetVisibleMode() == PvizWidget::FULL) 
		{
			child->SetVisibleMode(PvizWidget::HALF);
		}
		else 
		{
			child->SetVisibleMode(PvizWidget::FULL);
		}
	}
}

void MainWindow::SetSelectionMode()
{
	if (PvizWidget *child = activeChild())
	{
		if (child->GetInteractorMode() != PvizWidget::SELECT_MODE) 
		{
			child->SetInteractorMode(PvizWidget::SELECT_MODE);
			ui->actionPlay->setChecked(false);
			ui->actionSelectionMode->setChecked(true);
		}
		else 
		{
			child->SetInteractorMode(PvizWidget::INTERACT_MODE);
			ui->actionPlay->setChecked(false);
			ui->actionSelectionMode->setChecked(false);
		}
	}
}

void MainWindow::PlayAndPause()
{
	if (PvizWidget *child = activeChild())
	{
		if (child->GetInteractorMode() != PvizWidget::PLAY_MODE) 
		{
			child->SetInteractorMode(PvizWidget::PLAY_MODE);
			ui->actionPlay->setChecked(true);
			ui->actionSelectionMode->setChecked(false);
		}
		else 
		{
			child->SetInteractorMode(PvizWidget::INTERACT_MODE);
			ui->actionPlay->setChecked(false);
			ui->actionSelectionMode->setChecked(false);
		}	
	}
}

void MainWindow::updatePlayAndPause(PvizWidget* pviz)
{
	if (pviz->GetInteractorMode() == PvizWidget::PLAY_MODE) 
	{
		ui->actionPlay->setChecked(true);
	}
	else 
	{
		ui->actionPlay->setChecked(false);
	}	
}

void MainWindow::updateSelectionMode(PvizWidget* pviz)
{
	if (pviz->GetInteractorMode() == PvizWidget::SELECT_MODE) 
	{
		ui->actionSelectionMode->setChecked(true);
	}
	else 
	{
		ui->actionSelectionMode->setChecked(false);
	}
}

void MainWindow::updateHalfSphere(PvizWidget* pviz)
{
	if (pviz->GetVisibleMode() == PvizWidget::FULL) 
	{
		ui->actionShowHalfSphere->setChecked(false);
	}
	else 
	{
		ui->actionShowHalfSphere->setChecked(true);
	}
}

PvizWidget *MainWindow::activeChild()
{
	if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
	{
		return qobject_cast<PvizWidget *>(activeSubWindow->widget());
	}
	return 0;
}

void MainWindow::subWindowActivated(QMdiSubWindow * window)
{
	//qDebug() << "subWindowActivated ... " << window;
	if (PvizWidget *child = activeChild())
	{
		ui->tvTree->setModel(pvizToModelMap[child]);
        connect(ui->tvTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                this, SLOT(treeSelectionChanged(const QItemSelection &, const QItemSelection &)));        
		ui->tpProperty->clear();
		updateFeature(child);
		updatePlayAndPause(child);
		updateSelectionMode(child);
		updateHalfSphere(child);
		sync.InstallCallback(child);
		updateTransformWidget(child);
	}
}

void MainWindow::updateFeature(PvizWidget *pviz)
{
	disconnect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
			this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
	
	disconnect(vman2, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
			this, SLOT(featureValueChanged(QtProperty *, const QVariant &)));
	
	idToFeature["title.name"]->setValue(pviz->GetTitle());
	idToFeature["title.visible"]->setValue(pviz->GetTitleVisible());
	idToFeature["title.color"]->setValue(pviz->GetTitleColor());
	idToFeature["axes.visible"]->setValue(pviz->GetAxesVisible());
	idToFeature["cubeaxes.visible"]->setValue(pviz->GetCubeAxesVisible());
	idToFeature["cubeaxes.resizeonselection"]->setValue(pviz->GetCubeAxesResizeOnSelection());
	idToFeature["cubeaxes.labelscale"]->setValue(pviz->GetCubeAxesLabelScaleFactor());
	idToFeature["cubeaxes.xaxis.min"]->setValue(pviz->GetCubeAxesBounds(0));
	idToFeature["cubeaxes.xaxis.max"]->setValue(pviz->GetCubeAxesBounds(1));
	idToFeature["cubeaxes.yaxis.min"]->setValue(pviz->GetCubeAxesBounds(2));
	idToFeature["cubeaxes.yaxis.max"]->setValue(pviz->GetCubeAxesBounds(3));
	idToFeature["cubeaxes.zaxis.min"]->setValue(pviz->GetCubeAxesBounds(4));
	idToFeature["cubeaxes.zaxis.max"]->setValue(pviz->GetCubeAxesBounds(5));
	idToFeature["cubeaxes.xaxis.gridline"]->setValue(pviz->GetCubeAxesDrawXGridlines());
	idToFeature["cubeaxes.yaxis.gridline"]->setValue(pviz->GetCubeAxesDrawYGridlines());
	idToFeature["cubeaxes.zaxis.gridline"]->setValue(pviz->GetCubeAxesDrawZGridlines());
    //idToFeature["scaleaxes.visible"]->setValue(pviz->GetScaleAxesVisible());
	//idToFeature["scaleaxes.xaxis.labelscale"]->setValue(pviz->GetScaleAxesLabelScale(0));
	//idToFeature["scaleaxes.yaxis.labelscale"]->setValue(pviz->GetScaleAxesLabelScale(1));
	//idToFeature["scaleaxes.zaxis.labelscale"]->setValue(pviz->GetScaleAxesLabelScale(2));
	idToFeature["background.color"]->setValue(pviz->GetBackgroundColor());
	idToFeature["colormap.type"]->setValue(pviz->GetColorMap());
	idToFeature["plot.visible"]->setValue(pviz->GetPlotVisible());
	idToFeature["plot.linewidth"]->setValue(pviz->GetPlotLineWidth());
	idToFeature["plot.pointsize"]->setValue(pviz->GetPlotPointSize());
	idToFeature["glyph.visible"]->setValue(pviz->GetGlyphVisible());
	idToFeature["glyph.scalefactor"]->setValue(pviz->GetGlyphScaleFactor());
	idToFeature["glyph.autoorientation"]->setValue(pviz->GetGlyphAutoOrientation());
	idToFeature["legend.visible"]->setValue(pviz->GetLegendVisible());
	idToFeature["legend.heightfactor"]->setValue(pviz->GetLegendHeightFactor());
	idToFeature["legend.widthfactor"]->setValue(pviz->GetLegendWidthFactor());
	idToFeature["fps.visible"]->setValue(pviz->GetFPSVisible());
	idToFeature["camera.sync"]->setValue(pviz->GetUseSyncCamera());
	idToFeature["camera.focusmode"]->setValue(pviz->GetFocusMode());
	idToFeature["camera.focus.x"]->setValue(pviz->GetCameraFocusX());
	idToFeature["camera.focus.y"]->setValue(pviz->GetCameraFocusY());
	idToFeature["camera.focus.z"]->setValue(pviz->GetCameraFocusZ());
	idToFeature["label.visible"]->setValue(pviz->GetLabelVisible());
	//idToFeature["label.pickdelta"]->setValue(pviz->GetPickDelta());
	idToFeature["neighbor.num"]->setValue(pviz->GetNumOfNeighbors());
	idToFeature["neighbor.append"]->setValue(pviz->GetAppendNeighbors());
	idToFeature["default.color"]->setValue(pviz->GetDefaultColor());
	idToFeature["default.scale"]->setValue(pviz->GetDefaultScale());
	idToFeature["broker.brokerIP"]->setValue(brokerIP);
	idToFeature["broker.brokerPort"]->setValue(brokerPort);
	idToFeature["broker.destURI"]->setValue(destURI);
	idToFeature["broker.listenURI"]->setValue(listenURI);

	connect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
			   this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
	
	connect(vman2, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
			   this, SLOT(featureValueChanged(QtProperty *, const QVariant &)));
	
}

void MainWindow::buildTransformWidget()
{
	transformIdx = 0;

	QStringList enumTransform;
    enumTransform << "Rotate X" << "Rotate Y" << "Rotate Z";
    enumTransform << "Translate";
    enumTransform << "Scale";
    enumTransform << "Inverse";
	ui->cbTransformItem->addItems(enumTransform);
	
	vmanTransform = new QtVariantPropertyManager(this);
	
	QtVariantEditorFactory *vfacTransform = new QtVariantEditorFactory(this);
	ui->gpTransforms->setFactoryForManager(vmanTransform, vfacTransform);
		
	connect(ui->btAdd, SIGNAL(clicked()), this, SLOT(TransformItemAddButtonClicked()));
	
	QList<QAbstractButton *> buttons = ui->bbTransforms->buttons();
	foreach(QAbstractButton * bt, buttons)
	{
		if (bt->text() == QLatin1String("Abort"))
		{
			bt->setEnabled(false);
			break;
		}
	}
	
}

void MainWindow::TransformItemAddButtonClicked()
{
	qDebug() << "TransformItemAddButtonClicked ... " << ui->cbTransformItem->currentIndex();
	QtProperty *property;
	switch(ui->cbTransformItem->currentIndex())
	{
		case ROTATE_X:
			property = createDegreeProperty(vmanTransform, tr("Rotate X"));
			addPropertyToTransformWidget(NULL, (QtVariantProperty*)property, QString("rotate.x.%1").arg(transformIdx));
			break;
		case ROTATE_Y:
			property = createDegreeProperty(vmanTransform, tr("Rotate Y"));
			addPropertyToTransformWidget(NULL, (QtVariantProperty*)property, QString("rotate.y.%1").arg(transformIdx));
			break;
		case ROTATE_Z:
			property = createDegreeProperty(vmanTransform, tr("Rotate Z"));
			addPropertyToTransformWidget(NULL, (QtVariantProperty*)property, QString("rotate.z.%1").arg(transformIdx));
			break;
		case TRANSLATE:
			property = buildTransformWidget_XYZ(tr("Translate"), tr("trans"));
			break;
		case SCALE:
			property = buildTransformWidget_XYZ(tr("Scale"), tr("scale"));
			break;
		case INVERSE:
			property = vmanTransform->addProperty(QVariant::Bool, tr("Inverse"));
			addPropertyToTransformWidget(NULL, (QtVariantProperty*)property, QString("inverse.%1").arg(transformIdx));
			break;
		default:
			return;
			break;
	}
	ui->gpTransforms->addProperty(property);
	transformIdx++;
}

QtProperty* MainWindow::buildTransformWidget_XYZ(const QString &title, const QString &prefix)
{
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
    QtProperty *root = groupManager->addProperty(title);	
	
    QtVariantProperty *property;
	
	property = vmanTransform->addProperty(QVariant::Double, tr("X"));
    //property->setAttribute(QLatin1String("decimals"), -1);
    property->setAttribute(QLatin1String("singleStep"), 1.00);
	addPropertyToTransformWidget(root, property, QString(prefix+".x.%1").arg(transformIdx));
	
	property = vmanTransform->addProperty(QVariant::Double, tr("Y"));
    //property->setAttribute(QLatin1String("decimals"), -1);
    property->setAttribute(QLatin1String("singleStep"), 1.00);
	addPropertyToTransformWidget(root, property, QString(prefix+".y.%1").arg(transformIdx));
	
	property = vmanTransform->addProperty(QVariant::Double, tr("Z"));
    //property->setAttribute(QLatin1String("decimals"), -1);
    property->setAttribute(QLatin1String("singleStep"), 1.00);
	addPropertyToTransformWidget(root, property, QString(prefix+".z.%1").arg(transformIdx));
	
    //ui->gpTransforms->addProperty(root);
	return root;
}

QtVariantProperty* MainWindow::createDegreeProperty(QtVariantPropertyManager* man, const QString &title)
{
    QtVariantProperty *property;
	property = man->addProperty(QVariant::Double, title);
    property->setAttribute(QLatin1String("minimum"), -180.0);
    property->setAttribute(QLatin1String("maximum"), 180.0);
    property->setAttribute(QLatin1String("singleStep"), 1.00);
	
	return property;
}

void MainWindow::addPropertyToTransformWidget(QtProperty *root, QtVariantProperty *property, const QString &id)
{
    PropertyToId_Transform[property] = id;
    idToProperty_Transform[id] = property;
	
	if (root != NULL) 
	{
		root->addSubProperty(property);
	}
}

void MainWindow::TransformWidgetButtonClicked(QAbstractButton* button)
{
	QDialogButtonBox::ButtonRole role = ui->bbTransforms->buttonRole(button);
	switch (role) 
	{
		case QDialogButtonBox::AcceptRole: // 0
		case QDialogButtonBox::ActionRole: // 3
		case QDialogButtonBox::YesRole:    // 5
		case QDialogButtonBox::ApplyRole:  // 8
			TransformApply();
			break;
		case QDialogButtonBox::RejectRole:      // 1
			TransformRevert();
			break;
			
		case QDialogButtonBox::DestructiveRole: // 2
		case QDialogButtonBox::NoRole:          // 6
		case QDialogButtonBox::ResetRole:       // 7
			TransformDiscard();
			break;
			
			break;
		default:
			break;
	}
}

void MainWindow::TransformRevert()
{
	if (PvizWidget *child = activeChild())
	{
		child->PlotRevertToSaved();
		updateTransformWidget(child);
	}	
}

void MainWindow::TransformApply()
{
	if (PvizWidget *child = activeChild())
	{
		QList<QtProperty *> itemList = ui->gpTransforms->properties();
		vtkSmartPointer<vtkTransform> alltrans = vtkTransform::New();
		alltrans->PostMultiply();
		foreach (QtProperty* item, itemList)
		{
			//qDebug() << "item ... " << item->propertyName();
			
			vtkSmartPointer<vtkTransform> trans = vtkTransform::New();		
			QString id = item->propertyName();
			if (id == QLatin1String("Rotate X"))
			{
				QVariant value = ((QtVariantProperty*)item)->value();
				trans->RotateX(value.toDouble());				
			} 
			else if (id == QLatin1String("Rotate Y"))
			{
				QVariant value = ((QtVariantProperty*)item)->value();
				trans->RotateY(value.toDouble());				
			}
			else if (id == QLatin1String("Rotate Z"))
			{
				QVariant value = ((QtVariantProperty*)item)->value();
				trans->RotateZ(value.toDouble());				
			}
			else if (id == QLatin1String("Translate"))
			{
				double d[3];
				foreach (QtProperty *sub, item->subProperties())
				{
					QString subid = sub->propertyName();
					QVariant subvalue = ((QtVariantProperty*)sub)->value();
					if (subid == QLatin1String("X"))
					{
						d[0] = subvalue.toDouble();
					}
					else if (subid == QLatin1String("Y"))
					{
						d[1] = subvalue.toDouble();
					}
					else if (subid == QLatin1String("Z"))
					{
						d[2] = subvalue.toDouble();
					}
				}
				trans->Translate(d);
			}
			else if (id == QLatin1String("Scale"))
			{
				double d[3];
				foreach (QtProperty *sub, item->subProperties())
				{
					QString subid = sub->propertyName();
					QVariant subvalue = ((QtVariantProperty*)sub)->value();
					if (subid == QLatin1String("X"))
					{
						d[0] = subvalue.toDouble();
					}
					else if (subid == QLatin1String("Y"))
					{
						d[1] = subvalue.toDouble();
					}
					else if (subid == QLatin1String("Z"))
					{
						d[2] = subvalue.toDouble();
					}
				}
				trans->Scale(d);
			}
			else if (id == QLatin1String("Inverse"))
			{
				QVariant value = ((QtVariantProperty*)item)->value();
				if (value.toBool()) 
				{
					alltrans->Inverse();
				}
			}
			alltrans->Concatenate(trans);
		}
		
		child->PlotTransform(alltrans);
		
		updateTransformWidget(child);
	}
}

void MainWindow::TransformDiscard()
{
	QList<QtProperty *> itemList = ui->gpTransforms->properties();
	
	foreach (QtProperty* item, itemList)
	{
		ui->gpTransforms->removeProperty(item);
	}
	
	if (PvizWidget *child = activeChild())
	{
		updateTransformWidget(child);
	}
}

void MainWindow::updateTransformWidget(PvizWidget* pviz)
{
	QList<QAbstractButton *> buttons = ui->bbTransforms->buttons();
	foreach(QAbstractButton * bt, buttons)
	{
		if (bt->text() == QLatin1String("Abort"))
		{
			bt->setEnabled(pviz->GetSavedPointsExist());
			break;
		}
	}
}

void MainWindow::ResetCamera()
{
	if (PvizWidget *child = activeChild())
	{
		qDebug() << "ResetCamera ...";
		child->ResetCamera();
        BuildTree(child);
		ui->tvTree->setModel(pvizToModelMap[child]);
        connect(ui->tvTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                this, SLOT(treeSelectionChanged(const QItemSelection &, const QItemSelection &)));
	}
}

void MainWindow::PickCenter()
{
	if (PvizWidget *child = activeChild())
	{
		qDebug() << "PickCenter ...";
		child->PickCenter();
	}
}

void MainWindow::UnpickCenter()
{
    if (ui->actionPickCenter->isChecked())
        ui->actionPickCenter->setChecked(FALSE);
}

void MainWindow::About()
{
    if (!m_aboutDialog) {
        m_aboutDialog = new AboutDialog(this);
        //connect(m_aboutDialog, SIGNAL(finished(int)),
        //        this, SLOT(destroyVersionDialog()));
    }
    m_aboutDialog->show();
}

void MainWindow::Help()
{
    if (!m_helpDialog) {
        m_helpDialog = new HelpDialog(this);
    }
    m_helpDialog->show();
}

void MainWindow::SelectNeighbor()
{
	if (PvizWidget *child = activeChild())
	{
		qDebug() << "SelectNeighbor ...";
		child->SelectNeighbor();
        BuildTree(child);
		ui->tvTree->setModel(pvizToModelMap[child]);
        connect(ui->tvTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                this, SLOT(treeSelectionChanged(const QItemSelection &, const QItemSelection &)));
        
        return;
	}
}

void MainWindow::OnClusterVisibleChanged(PvizWidget* pviz, unsigned int cid, bool visible)
{
    //qDebug() << "OnClusterVisibleChanged ... notified:";
    if (pviz == activeChild())
    {        
        disconnect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                   this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
        
        disconnect(vman2, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                   this, SLOT(featureValueChanged(QtProperty *, const QVariant &)));
        
        QStandardItem *item = pvizToItemListMap[pviz]->value(cid)->at(0);
        item->setData((visible? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
        
        connect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
        
        connect(vman2, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(featureValueChanged(QtProperty *, const QVariant &)));
    }
}

void MainWindow::OnClusterColorChanged(PvizWidget* pviz, unsigned int cid, QColor color)
{
    //qDebug() << "OnClusterColorChanged ... notified:" << color;
    if (pviz == activeChild())
    {        
        disconnect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                   this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
        
        disconnect(vman2, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                   this, SLOT(featureValueChanged(QtProperty *, const QVariant &)));
        
        CidToItemListMap_t::const_iterator iter = pvizToItemListMap[pviz]->find(cid);
        if (iter != pvizToItemListMap[pviz]->end())
        {
            QStandardItem *item = pvizToItemListMap[pviz]->value(cid)->at(0);
            item->setData(color, Qt::DecorationRole);
        }

        connect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
        
        connect(vman2, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(featureValueChanged(QtProperty *, const QVariant &)));
    }
}

void MainWindow::OpenDirectory()
{
    dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    
    if (dirName.length() > 0)
    {     
        OpenDirectory(dirName);
    }
}

void MainWindow::OpenDirectory(QString dirname)
{
    dirName = dirname;
    fileNames.clear();
    *future = QtConcurrent::run(this, &MainWindow::checkDir);
    watcher->setFuture(*future);
}

void MainWindow::checkDir()
{
    qDebug() << "A thread starts to work ... ";
    
    QDir dir(dirName);
    
    int i = 0;
    while (true) 
    {
        qDebug() << "+++ Go to sleep";
        // Windows doesn't work
#ifdef _WIN32 
        Sleep(1000);
#else
		sleep(1);
#endif
        
        foreach (QString file, dir.entryList(QStringList(), QDir::Files, QDir::Name))
        {
            QString filepath = dir.filePath(file);
            if (!fileNames.contains(filepath))
            {
                fileNames += filepath;
                return;
            }
        }
        
        if (i++ > 30) return;
    }
}

void MainWindow::checkDirFinished()
{
    qDebug() << "Thread work done.";
    
	{
        QString filename = fileNames.last();
        QFileInfo fileinfo(filename);
        if (fileinfo.size() == 0)
        {
            QMdiSubWindow* win = qobject_cast<QMdiSubWindow *>(dirActivePviz->parent());
            win->setWindowTitle(fileinfo.fileName() + " (Done)");
            qDebug() << ">>> Done.";
            
            if (fileNames.size() > 100)
            {
                qDebug() << ">>> Deleting old ones ... ";
                //foreach(QString filename, fileNames)
                for (int i = 50; i >= 0; i--)
                {
                    QString filename = fileNames[i];
                    QFile file(filename);
                    file.remove();
                    fileNames.removeOne(filename);
                }
            }
            
            if (!stopThreadWork)
            {
                *future = QtConcurrent::run(this, &MainWindow::checkDir);
                watcher->setFuture(*future);
            }
            return;
        }
        
        if (fileNames.size() < 2)
        {
            if (!stopThreadWork)
            {
                *future = QtConcurrent::run(this, &MainWindow::checkDir);
                watcher->setFuture(*future);
            }
            
            return;
        }
	}
	
	{
        QString filename = fileNames[fileNames.size() - 1];
        QFileInfo fileinfo(filename);
        
        if (fileNames.size() == 2)
        {
            qDebug() << ">>> First file" << filename << fileinfo.size();
            dirActivePviz = OpenDataFile(filename);
            if (dirActivePviz == NULL)
            {
                QMessageBox::critical(this, "Error", "File read error. Please try again");
                return;
            }

        }
        
        if (fileNames.size() > 2)
        {
            qDebug() << ">>> Next file" << filename << fileinfo.size();
            
            if (pvizToModelMap.find(dirActivePviz) == pvizToModelMap.end()) 
            {
                qDebug() << "Closed already";
                return;
            };
            
            QStringList tokens = filename.split(".");
            
            QString ext = tokens.last();
            if (ext.toLower() == "txt") 
            {
                QMdiSubWindow* win = qobject_cast<QMdiSubWindow *>(dirActivePviz->parent());
                win->setWindowTitle(fileinfo.fileName());
                dirActivePviz->ReloadSimpleDataFile(filename);
            } 
            else 
            {
                QMessageBox::warning(this, "Under construction", "Net yet supported.");
            }
        }
     }
	 
    // Quick & Dirty solution
    if (!stopThreadWork)
    {
        *future = QtConcurrent::run(this, &MainWindow::checkDir);
        watcher->setFuture(*future);
    }
}

void MainWindow::OnClusterDefaultChanged(PvizWidget* pviz, unsigned int cid, bool isDefault)
{
    //qDebug() << "OnClusterDefaultChanged ... notified:" << cid << isDefault;
    if (pviz == activeChild())
    {   
        // Move to top
        CidToItemListMap_t* itemListMap = pvizToItemListMap[pviz];
        
        QModelIndex index = pvizToModelMap[pviz]->indexFromItem(itemListMap->value(cid)->value(0));
        int rownum = index.row();
        
        QStandardItem* item1 = itemListMap->value(cid)->value(0)->clone();
        delete itemListMap->value(cid)->value(0);
        itemListMap->value(cid)->replace(0, item1);
        
        QStandardItem* item2 = itemListMap->value(cid)->value(1)->clone();
        delete itemListMap->value(cid)->value(1);
        itemListMap->value(cid)->replace(1, item2);
        
        QStandardItem* item3 = itemListMap->value(cid)->value(2)->clone();
        delete itemListMap->value(cid)->value(2);
        itemListMap->value(cid)->replace(2, item3);
         
        //itemMap->insert(cid, item1);
        itemListMap->insert(cid, itemListMap->value(cid));
        
        QList<QStandardItem *> *items = itemListMap->value(cid);
        
        pvizToModelMap[pviz]->removeRow(rownum);
        if (isDefault)
        {
            pvizToModelMap[pviz]->appendRow(*items);
        }
        else
        {
            pvizToModelMap[pviz]->insertRow(0, *items);
        }
        
        /*
        // Set selected
        index = pvizToModelMap[pviz]->indexFromItem(itemListMap->value(cid)->value(0));
        
        QItemSelectionModel* selectionModel = ui->tvTree->selectionModel();
        selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
         */
    }
}

void MainWindow::OnClosing(PvizWidget* pviz)
{
    qDebug() << "OnClosing ... " << pviz;
    PvizToModelMap_t::iterator it = pvizToModelMap.find(pviz);
    if (it != pvizToModelMap.end())
    {
        //qDebug() << "model" << it.value();
        if (it.value() != NULL)
        {
            it.value()->clear();
            pvizToModelMap.erase(it);
        }
        //pvizToModelMap[pviz]->clear();
        //it->second->clear();
        //pvizToModelMap.erase(it);
    }
}

void MainWindow::OnModelLoaded(PvizWidget* pviz)
{
    qDebug() << "OnModelLoaded ... " << pviz;
    BuildTree(pviz);
	subWindowActivated(NULL);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "closeEvent";
    foreach(QMdiSubWindow* child, ui->mdiArea->subWindowList())
    {
        child->close();
    }
    
    event->accept();
}

void MainWindow::TileSubWindows()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event && event->key() == Qt::Key_Escape)
    {
        qDebug() << "Esc pressed ... ";
        this->setWindowState(Qt::WindowNoState);
        this->ui->actionFullScreen->setChecked(false);
    }
}

void MainWindow::FullScreen()
{
    this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
    
    bool isFullScreen = windowState() & Qt::WindowFullScreen;
    if (isFullScreen)
    {
        this->ui->mainToolBar->setVisible(false);
    }
    else
    {
        this->ui->mainToolBar->setVisible(true);
    }
}

void MainWindow::SetColorMap(int map)
{
    idToFeature["colormap.type"]->setValue(map);
}

void MainWindow::OnCameraFocusChanged(PvizWidget* pviz, double *pos)
{
    //qDebug() << "OnClusterColorChanged ... notified:" << color;
    if (pviz == activeChild())
    {        
        disconnect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                   this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
        
        disconnect(vman2, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                   this, SLOT(featureValueChanged(QtProperty *, const QVariant &)));
        
        idToFeature["camera.focus.x"]->setValue(pos[0]);
        idToFeature["camera.focus.y"]->setValue(pos[1]);
        idToFeature["camera.focus.z"]->setValue(pos[2]);

        connect(vman1, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
        
        connect(vman2, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(featureValueChanged(QtProperty *, const QVariant &)));
    }
}
