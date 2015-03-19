#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMdiArea>
#include <QAbstractButton>
#include <QDesktopServices>

#include "pvizsync.h"
#include "pvizmodel.h"
#include "pvizwidget.h"
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"

#ifdef PVIZRPC_CLIENT_ENABLE
#include <boost/asio.hpp>
#include "signalslib.hpp"
#endif

#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>
#include <QSettings>

#include "aboutdialog.h"
#include "helpdialog.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:	
	enum TransformType_t {
		ROTATE_X = 0, ROTATE_Y, ROTATE_Z,
		TRANSLATE, SCALE, INVERSE
	};
	
	
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	
    typedef QMap<PvizWidget *, QStandardItemModel *> PvizToModelMap_t;    
    
    typedef QList<QStandardItem *> ItemList_t;
    typedef QMap<int, ItemList_t *> CidToItemListMap_t;
    typedef QMap<PvizWidget *, CidToItemListMap_t *> PvizToItemListMap_t;

	PvizToModelMap_t pvizToModelMap;
	PvizToItemListMap_t pvizToItemListMap;
	
	void LoadTree();
	void BuildTree(PvizWidget *pviz);
	void RefreshTreeColor();
    
	// 
	// Variables for Property Browser
	//
	class QtVariantPropertyManager *vman1;	// Variant Manager
	void addProperty(QtVariantProperty *property, const QString &id); // 
	
	class QtVariantPropertyManager *vman2;	// Variant Manager
	void addFeature(QtProperty *root, QtVariantProperty *property, const QString &id); // 
	
	class QtVariantPropertyManager *vmanTransform;	// Variant Manager
	
	QMap<QtProperty *, QString> propertyToId;
	QMap<QString, QtVariantProperty *> idToProperty;
	
	QMap<QtProperty *, QString> featureToId;
	QMap<QString, QtVariantProperty *> idToFeature;
	
	QMap<QtProperty *, QString> PropertyToId_Transform;
	QMap<QString, QtVariantProperty *> idToProperty_Transform;
	unsigned int transformIdx;
	
	//QMap<QStandardItem *, QString> itemToId;
	
	PvizSync sync;

	//void showProperty();
	//void itemChanged(QStandardItem * item);
	//void clicked ( const QModelIndex & index );
	void buildFeatureTree();
    void buildFeatureTree_AddTitleFeature();
	void buildFeatureTree_AddAxesFeature();
	void buildFeatureTree_AddCubeAxesFeature();
    void buildFeatureTree_AddScaleAxesFeature();
	void buildFeatureTree_AddBackgroundColorFeature();
	void buildFeatureTree_AddColorMapFeature();
	void buildFeatureTree_AddPlotFeature();
	void buildFeatureTree_AddGlyphFeature();
    void buildFeatureTree_AddLegendFeature();
    void buildFeatureTree_AddColorbarFeature();
	void buildFeatureTree_AddFPSFeature();
	void buildFeatureTree_AddCameraLinkFeature();
    void buildFeatureTree_AddLabelFeature();
    void buildFeatureTree_AddNumOfNeighbors();
    void buildFeatureTree_AddDefaultFeature();
    void buildFeatureTree_AddBrokerFeature();
	
	QtVariantProperty* createDegreeProperty(QtVariantPropertyManager* man, const QString &title);
	void addPropertyToTransformWidget(QtProperty *root, QtVariantProperty *property, const QString &id);
	void buildTransformWidget();
	QtProperty* buildTransformWidget_XYZ(const QString &title, const QString &prefix);
	void TransformDiscard();
	void TransformApply();
	void TransformRevert();
	
	void reset();
	
	PvizWidget* activeChild();
	void updateFeature(PvizWidget *pviz);
	void updatePlayAndPause(PvizWidget* pviz);
	void updateSelectionMode(PvizWidget* pviz);
	void updateHalfSphere(PvizWidget* pviz);
	void updateTransformWidget(PvizWidget* pviz);
    
    AboutDialog* m_aboutDialog;
    HelpDialog* m_helpDialog;
	
    void checkDir();
    void closeEvent(QCloseEvent *event);
    
public Q_SLOTS:
	void OpenDataFile();
	PvizWidget* OpenDataFile(QString fileName);
	void SaveAs();
	void SaveAnimation();
	void ConnectToServer();
	void ConnectToNBServer();
	void ConnectToActiveMQServer();
	void UpdateFromServer();
	void PlayAndPause();
	void SaveScreen();
	void ShowHalfSphere();
	void SetSelectionMode();
	void ResetCamera();
	void PickCenter();
    void UnpickCenter();
    void About();
    void Help();
    void SelectNeighbor();
    void OnClusterVisibleChanged(PvizWidget* pviz, unsigned int cid, bool visible);
    void OnClusterColorChanged(PvizWidget* pviz, unsigned int cid, QColor color);
    void OnClusterDefaultChanged(PvizWidget* pviz, unsigned int cid, bool isDefault);
    void OnClosing(PvizWidget* pviz);
    void OnModelLoaded(PvizWidget* pviz);
    //void OnSelectionModeChanged(bool );
    void OnCameraFocusChanged(PvizWidget* pviz, double *pos);

	void propertyValueChanged(QtProperty *property, const QVariant &value);
	void featureValueChanged(QtProperty *property, const QVariant &value);
	void treeClicked(const QModelIndex & index);
	void treeSectionClicked(int);
    void treeSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
	
	void subWindowActivated(QMdiSubWindow * window);
	//void windowStateChanged(Qt::WindowStates oldState, Qt::WindowStates newState);
	
	void TransformWidgetButtonClicked(QAbstractButton* button);
	void TransformItemAddButtonClicked();
    void OpenDirectory();
    void OpenDirectory(QString fileName);
    void checkDirFinished();
    void TileSubWindows();
    void FullScreen();
    void SetColorMap(int map);
	QModelIndexList getSelectedRows();
protected:
	void statusMessage(QString msg);
    void keyPressEvent(QKeyEvent* event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    
private:
	Ui::MainWindow *ui;

#ifdef PVIZRPC_CLIENT_ENABLE
	boost::asio::io_service io_service_;
	bool outstandingRequest_;
	
	void handle_timer();
#endif
    
    bool stopThreadWork;
    QString dirName;
    QStringList fileNames;
    PvizWidget* dirActivePviz;
    QFuture<void> *future;
    QFutureWatcher<void> *watcher;
    
    QSettings *settings;
    
    QString brokerIP;
    int brokerPort;
    QString destURI;
    QString listenURI;    
};

#define POINT_ID_NAME      "Point ID"
#define POINT_SCALE_NAME   "Point Scale"
#define POINT_COLOR_NAME   "Point Color"
#define CLUSTER_ID_NAME    "Cluster ID"
#define CLUSTER_SCALE_NAME "Cluster Scale"
#define CLUSTER_COLOR_NAME "Cluster Color"

#endif // MAINWINDOW_H
