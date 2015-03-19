#ifndef PVIZWIDGET_H
#define PVIZWIDGET_H

#include <QString>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkLookupTable.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAxesActor.h>
#include <vtkLODActor.h>
#include <vtkQuadricLODActor.h>
#include <vtkGlyph3D.h>
#include <vtkCleanPolyData.h>
#include <vtkLegendBoxActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCornerAnnotation.h>
#include <vtkCamera.h>
#include <vtkAreaPicker.h>
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>
//#include <vtkPropPicker.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkRegularPolygonSource.h>
#include <vtkColorTransferFunction.h>
#include <vtkTextActor.h>
//#include "vtkCubeAxes2Actor.h"
#include "vtkCubeAxesActor.h"
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkAxisActor.h>
#include <vtkCubeSource.h>
#include <vtkConeSource.h>
#include <vtkSphereSource.h>
#include <vtkCylinderSource.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>

#include <QVTKWidget.h>
#include "pvizmodel.h"
#include "pvizinteractorstyle.h"
//#include "pvizAxisActor.h"
//#include "pvizAxesActor.h"
//#include "pvizCubeAxesActor.h"
//#include "pvizsync.h"

//#include "ServiceClient.h"
//#include "ServiceException.h"
//#include "Profile.h"

#define CELL_ID_NAME       "Cell ID"
#define POINT_ID_NAME      "Point ID"
#define POINT_SCALE_NAME   "Point Scale"
#define POINT_LABEL_NAME   "Point Label"
//#define POINT_COLOR_NAME   "Point Color"
#define CLUSTER_ID_NAME    "Cluster ID"
#define CLUSTER_SCALE_NAME "Cluster Scale"
//#define CLUSTER_COLOR_NAME "Cluster Color"
#define SHAPE_ID_NAME      "Shape ID"

#define SUCCESS 0
#define FAILURE -1

#ifdef USE_ACTIVEMQ
#include <message.pb.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Integer.h>
#include <activemq/util/Config.h>
#include <decaf/util/Date.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>

using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

class PvizSync;

//static bool isFirstLabelMessage = true;
//static bool isFirstPositionMessage = true;
//static int modelStatus = 0;

#ifdef USE_ACTIVEMQ
class PvizWidget : public QVTKWidget, 
public ExceptionListener,
public MessageListener,
public DefaultTransportListener 
#else
class PvizWidget : public QVTKWidget
#endif
{
	Q_OBJECT
public:
	enum ColorMap_t {
        MATLAB_DISTINCT, SALSA_CUSTOM, 
        COLORBREW_SET1, COLORBREW_PAIRED,
		RAINBOW, RAINBOW_R,
        COOL2WARM, COOL2WARM_R,
        JET, JET_R,
        HSV, HOT,
        EXPERIMENTAL,
        CUSTOM
	};
    
	enum FocusMode_t {
		AUTO = 0, ORIGIN, CUSTOM_FOCUS
	};
	
	/*
	enum PlayStatus_t {
		PLAYING, PAUSED
	};
	*/
	
	enum VisibleMode_t {
		FULL, HALF
	};
	
	// Exclusive states
	enum InteractorMode_t {
		INTERACT_MODE, SELECT_MODE, PLAY_MODE, PICKCENTER_MODE
	};
	
	enum LegendLocation_t {
            UPPERLEFT = 0, UPPERRIGHT, LOWERLEFT, LOWERRIGHT
	};
    
	//! constructor
	PvizWidget(QWidget* parent = NULL, Qt::WFlags f = 0);
	~PvizWidget() throw();
		
    void SaveToModel();
	int SaveAsVTK(QString filename);
	int SaveAsXML(QString filename);
	int SaveAsTXT(QString filename);
	
	void LoadCompressedDataFile(QString filename);
	void LoadXmlDataFile(QString filename);
	void LoadSimpleDataFile(QString filename);
	void LoadHDF5DataFile(QString filename);
    
	void ReloadSimpleDataFile(QString filename);
	
	void ConnectToServer();
	void UpdateFromServer();

	//void ConnectToNBServer(string host, int port, int entityID, int templateId, string contentSynopsis);
    void ConnectToActiveMQServer(QString brokerURI,
                                 QString destURI,
                                 QString listenURI);
	
    void ApplyColorMap_Default();
	void ApplyColorMap_Rainbow(bool rev = false);
	void ApplyColorMap(ColorMap_t type);
	void reset();
	
	void UpdateLines();
	void SetClusterLabel(unsigned int cid, std::string label);
	void SetClusterVisible(unsigned int cid, bool visible);
    
	void SetClusterAsDefault(unsigned int cid, bool isdefault);
    
	QColor GetClusterColor(unsigned int cid);
	void SetClusterColor(unsigned int cid, QColor color);
	QColor GetModelClusterColor(unsigned int cid);
    
	float GetClusterScale(unsigned int cid);
	void SetClusterScale(unsigned int cid, float scale);
    
	int GetClusterShape(unsigned int cid);
	void SetClusterShape(unsigned int cid, int shape);
    
	void SetAxesVisible(bool b);
	bool GetAxesVisible();
    
	void SetCubeAxesVisible(bool b);
	bool GetCubeAxesVisible();
    void SetCubeAxesResizeOnSelection(bool b);
    bool GetCubeAxesResizeOnSelection();
	void SetCubeAxesBounds(double val, int i);
	double GetCubeAxesBounds(int i);
    
    void SetCubeAxesDrawXGridlines(bool b);
    void SetCubeAxesDrawYGridlines(bool b);
    void SetCubeAxesDrawZGridlines(bool b);
    
    bool GetCubeAxesDrawXGridlines();
    bool GetCubeAxesDrawYGridlines();
    bool GetCubeAxesDrawZGridlines();
    
    void SetCubeAxesLabelScaleFactor(double factor);
    double GetCubeAxesLabelScaleFactor();
    
	//void SetScaleAxesVisible(bool b);
	//bool GetScaleAxesVisible();
    
    //void SetScaleAxesLabelScale(int i, double scale);
    //double GetScaleAxesLabelScale(int i);

	void SetBackgroundColor(QColor color);
	QColor& GetBackgroundColor();
	void SetColorMap(ColorMap_t type);
	ColorMap_t GetColorMap();
	
    void SetPlotVisible(bool b);
    bool GetPlotVisible();
    void SetLineVisible(bool b);
    bool GetLineVisible();
	void SetPlotLineWidth(unsigned int width);
	unsigned int GetPlotLineWidth();
	void SetPlotPointSize(unsigned int size);
	unsigned int GetPlotPointSize();
    void ApplyJitter(double factor);
	void SetGlyphVisible(bool b);
	bool GetGlyphVisible();
	void SetGlyphScaleFactor(double factor);
	double GetGlyphScaleFactor();
	void SetGlyphAutoOrientation(bool b);
	bool GetGlyphAutoOrientation();
	void SetFPSVisible(bool b);
	bool GetFPSVisible();
	//vtkCamera* GetCamera();
	void SetAsActiveCamera(vtkCamera *camera);
	vtkCamera* GetActiveCamera();
	void UseLocalCamera();
	bool GetUseSyncCamera();
	
	//void SetLinkTo(PvizWidget *parent);
	//PvizWidget * GetLinkTo();
	//void Unlink();
	
	//void SyncWith(PvizWidget *child);
	//void UnsyncWith(PvizWidget *child);
	//std::vector<PvizWidget*> GetSyncWith();
	//void AddSyncWith(PvizWidget *dest);
	//void RemoveSyncWith(PvizWidget *dest);
	//PvizWidget* GetLinkWith();

	void SetSyncServer(bool b);
	void DoSync();
	
	/*
	PlayStatus_t GetPlayStatus();
	void SetPlayStatus(PlayStatus_t status);
	*/
	
	VisibleMode_t GetVisibleMode();
	void SetVisibleMode(VisibleMode_t mode);

	InteractorMode_t GetInteractorMode();
	void SetInteractorMode(InteractorMode_t mode);
	
	void BuildPlot();
	void BuildSubplot();
	void BuildLegend();
	//void BuildLegendSelected();
	void BuildGlyph();
	void BuildGrid();
	void BuildAxes();
	void BuildHoverWidget();
	void BuildLabels();
	//void BuildHoverLabels();
	void BuildCornerAnnotation();
	
	//void UpdateLegend(unsigned int cid, QColor color);
	void SetLegendPosition(LegendLocation_t loc);
	LegendLocation_t GetLegendPosition();
	void SetLegendVisible(bool b);
	bool GetLegendVisible();
	void SetLegendHeightFactor(double height);
	double GetLegendHeightFactor();
	void SetLegendWidthFactor(double width);
	double GetLegendWidthFactor();

    void SetColorbarVisible(bool b);
    bool GetColorbarVisible();
    void SetColorbarNumberOfLabels(int num);
    int GetColorbarNumberOfLabels();
    
    
	double GetCameraFocusX();
	double GetCameraFocusY();
	double GetCameraFocusZ();
	void SetCameraFocusX(double p);
	void SetCameraFocusY(double p);
	void SetCameraFocusZ(double p);

	int SaveScreen(QString filename);
	int SaveAsMovie(QString filename);
	
	PvizModel *model;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkPolyData> plot;
	vtkSmartPointer<vtkLookupTable> lut;
		
	bool outstandingRequest_;
	QColor backgroundColor_;
	//int colorMap_;
	
	static void InteractorTimerEvent(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);
	static void ResetCameraClippingRangeEventCallBack(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);
	static void AreaPickerCallBack(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);
	static void PickerCallBack(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);
	//static void HoverTimerEventCallBack(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);
	//static void HoverEndInteractionEvent(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);
	static void HoverCallBack(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);
	static void RendererEndCallBack(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);
	static void PickCenterCallBack(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);
	
	void SetSync(PvizSync* sync);
	void PlotTransform(vtkSmartPointer<vtkTransform> transform);
	void PlotRevertToSaved();
	void PlotSaveBeforeTransform();
	bool GetSavedPointsExist();
	void ResetCamera();
    void PickCenter();
    void SetLookAt(double pos[3]);
    void UpdateTransform();
    
    void SetLabelVisible(bool b);
	bool GetLabelVisible();
    
    void SelectNeighbor();    
    void SetNumOfNeighbors(int num);
    int GetNumOfNeighbors();
    
    void SetAppendNeighbors(bool b);
    bool GetAppendNeighbors();
    
    void SetDefaultColor(QColor col);
    QColor GetDefaultColor();
    
    void SetDefaultScale(double scale);
    double GetDefaultScale();
    
    //typedef QList<PvizCluster*> PvizClusterList_t;
    //PvizClusterList_t GetSortedClusterList();
    
    void SortClusterBySize(bool descending);
    void SortClusterById(bool descending);
    void SortClusterByLabel(bool descending);

#ifdef USE_ACTIVEMQ    
    virtual void onMessage( const Message* message ) throw();

    // registered as an ExceptionListener with the connection.
    virtual void onException( const CMSException& ex AMQCPP_UNUSED ) {
        printf("CMS Exception occurred.  Shutting down client.\n");
        exit(1);
    }
    
    virtual void transportInterrupted() {
        std::cout << "The Connection's Transport has been Interrupted." << std::endl;
    }
    
    virtual void transportResumed() {
        std::cout << "The Connection's Transport has been Restored." << std::endl;
    }
#endif
	void SetFocusMode(FocusMode_t mode);
	FocusMode_t GetFocusMode();
    
    QString GetFileName();
    
    void SetTitle(QString title);
    QString GetTitle();
    void SetTitleVisible(bool b);
    bool GetTitleVisible();
    void SetTitleColor(QColor col);
    QColor GetTitleColor();

	//void SetPickDelta(int delta);
	//int GetPickDelta();
	void RefreshAll();
protected:
	bool axesVisible;
    bool cubeAxesVisible;
    bool cubeAxesResizeOnSelection;
	bool plotVisible;
    bool lineVisible;
	bool glyphVisible;
	bool glyphAutoOrientation;
	bool fpsVisible;
    bool labelVisible;
	
	unsigned int plotLineWidth;
	unsigned int plotPointSize;
    double glyphScaleFactor;

    bool legendVisible;
	double legendHeightFactor;
	double legendWidthFactor;

    bool colorbarVisible;
    int colorbarNumberOfLabels;

	double cameraClippingDistance_;
	int playTimer_;
	int playTimerDuration_;
	int hoverTimer_;
	int hoverTimerDuration_;
	
	ColorMap_t colorMap_;
	//PlayStatus_t playStatus_;
	VisibleMode_t visibleMode_;
	InteractorMode_t interactorMode_;
	//vtkCamera* localCamera;
	
	bool useSyncCamera;
	//PvizWidget* linkWith;
	//std::vector<PvizWidget*> syncWith;
	//bool syncServerType;
	PvizSync* sync_;

	//vtkSmartPointer<vtkAxesActor> axesActor;
	vtkSmartPointer<vtkActor> axesActor;
	vtkSmartPointer<vtkActor> plotActor;
	vtkSmartPointer<vtkGlyph3D> glyph;
	//vtkSmartPointer<vtkActor> glyphActor;
	//vtkSmartPointer<vtkLODActor> glyphActor;
	vtkSmartPointer<vtkQuadricLODActor> glyphActor;
	vtkSmartPointer<vtkIdTypeArray> selectedCellIds;
	vtkSmartPointer<vtkIdTypeArray> selectedCellIdsForGlyph;
	vtkSmartPointer<vtkIdTypeArray> selectedPointIds;
	//vtkSmartPointer<vtkIdTypeArray> hoveredPointIds;
	vtkSmartPointer<vtkCleanPolyData> cleansubplot;
	vtkSmartPointer<vtkCleanPolyData> cleansubplotForGlyph;
	vtkSmartPointer<vtkLegendBoxActor> legendActor;
	vtkSmartPointer<pvizInteractorStyle> interactorStyle;
	//vtkSmartPointer<vtkInteractorStyleRubberBand3D> interactorStyle;
	//vtkSmartPointer<vtkInteractorStyleRubberBand2D> interactorStyle;
    
	vtkSmartPointer<vtkOrientationMarkerWidget> orientationMarker;
	vtkSmartPointer<vtkCornerAnnotation> cornerAnnotation;
	vtkSmartPointer<vtkCallbackCommand> cbFPS;
	vtkSmartPointer<vtkCallbackCommand> cbGlyph;
	vtkSmartPointer<vtkCallbackCommand> cbPickCenter;
	vtkSmartPointer<vtkCamera> localCamera;
	vtkSmartPointer<vtkPoints> savedPoints;
    vtkSmartPointer<vtkAreaPicker> areaPicker;
    vtkSmartPointer<vtkCellPicker> cellPicker;
    vtkSmartPointer<vtkPointPicker> pointPicker;
    //vtkSmartPointer<vtkPropPicker> propPicker;
    
    vtkSmartPointer<vtkRegularPolygonSource> gs00;
    vtkSmartPointer<vtkRegularPolygonSource> gs01;
    vtkSmartPointer<vtkRegularPolygonSource> gs02;
    vtkSmartPointer<vtkRegularPolygonSource> gs03;
    
    vtkSmartPointer<vtkConeSource> gs10;
    vtkSmartPointer<vtkCubeSource> gs11;
    vtkSmartPointer<vtkSphereSource> gs12;
    vtkSmartPointer<vtkCylinderSource> gs13;

    vtkSmartPointer<vtkColorTransferFunction> cf;
	bool savedPointsExist;
    
    vtkSmartPointer<vtkActor2D> labelActor;
    //vtkSmartPointer<vtkActor2D> hoverLabelActor;
    vtkSmartPointer<vtkTextActor> titleActor;
    //vtkSmartPointer<vtkCubeAxes2Actor> cubeAxesActor;
    vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor;
    vtkSmartPointer<vtkScalarBarActor> colorbarActor;
    
    //vtkSmartPointer<pvizAxisActor> xAxisActor;
    //vtkSmartPointer<pvizAxesActor> scaleAxesActor;
	
	char fps_str[80];
	
	//void loadModel();
	void updateSelectedCellIdsForGlyph(unsigned int cid, bool visible);
	void refresh();
	
	void closeEvent(QCloseEvent *event);

    int numOfNeighbors_;
    bool appendNeighbors;
	vtkSmartPointer<vtkIdList> selectedNeighbors;
    
    QColor defaultColor;
    double defaultScale;
    bool sortNondefaultFirst;
    
    LegendLocation_t legendPosition;

#ifdef USE_ACTIVEMQ    
    // NB Client
    //ServiceClient *serviceClient;
    //void onEvent(NBEvent *nbEvent);
    
    // ActiveMQ consumer
    Connection* connection;
    Session* session;
    //Destination* destination;
    Topic* sendTopic;
    Topic* listenTopic;
    MessageConsumer* consumer;
    MessageProducer* producer;
    //bool useTopic;
    //std::string brokerURI;
    //std::string destURI;
    //std::string listenURI;
    //bool clientAck;
#endif
    long long timestamp;
    
    bool isFirstLabelMessage;
    bool isFirstPositionMessage;
    int modelStatus;

    FocusMode_t focusMode_;
    QString fileName_;
    double focus[3];
    
    bool anyClusterHasGlyph2D();
    
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    
    void addToSelectedPoint(vtkIdType pidx, bool append);
    void setHoverTimerActivate(bool b);
    
public Q_SLOTS:
	void loadModel();
	void updateModel();
    
signals:
    void OnCenterPicked();
    void OnSelected();
    void OnClusterVisibleChanged(PvizWidget* pviz, unsigned int cid, bool visible);
    void OnClusterDefaultChanged(PvizWidget* pviz, unsigned int cid, bool isdefault);
    void OnClusterColorChanged(PvizWidget* pviz, unsigned int cid, QColor color);
    void OnClosing(PvizWidget* pviz);
    void OnCameraFocusChanged(PvizWidget* pviz, double *pos);
    
    void OnModelCreated();
    void OnModelUpdated();
    void OnModelLoaded(PvizWidget* pviz);
};

#endif //PVIZWIDGET_H
