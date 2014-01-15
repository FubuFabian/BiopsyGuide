#include "Scene3DWidget.h"
#include "ui_Scene3DWidget.h"
#include "Scene3D.h"
#include "PolarisConfigurationWidget.h"

#include <QString>
#include <QMainWindow>
#include <QtGui>
#include <QtTest/QTest>

Scene3DWidget::Scene3DWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Scene3DWidget)
{
    ui->setupUi(this);

    this->View1 = igstk::View3D::New();
    this->qtDisplay1 = new igstk::QTWidget();
    this->qtDisplay1->RequestSetView( this->View1 );

    layout = new QGridLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(this->qtDisplay1,0,0);

    ui->Display->setLayout(layout);

    ui->initLoggetBt->setEnabled(false);
    ui->startTrackingBt->setEnabled(false);
    ui->upBtn->setEnabled(false);
    ui->downBtn->setEnabled(false);
    ui->leftBtn->setEnabled(false);
    ui->rightBtn->setEnabled(false);
    ui->inBtn->setEnabled(false);
    ui->outBtn->setEnabled(false);
    ui->fixBtn->setEnabled(false);   
    
    ui->targetStep->setEnabled(false);
    ui->targetStep->setText("1");
    
    this->quit =  false;
    this->fourViews = false;
}

Scene3DWidget::~Scene3DWidget()
{
    delete ui;
}

void Scene3DWidget::Show()
{
    this->quit = false;
    this->show();
    this->View1->RequestStart();

	if (fourViews){
		this->View2->RequestStart();
		this->View3->RequestStart();
		this->View4->RequestStart();
	}
}

void Scene3DWidget::Quit()
{
    this->View1->RequestStop();

	if (fourViews){
		this->View2->RequestStop();
		this->View3->RequestStop();
		this->View4->RequestStop();
	}

	this->m_Tracker->RequestStopTracking();
    this->quit = true;
    this->hide();
}

void Scene3DWidget::startTracking()
{
    this->m_Tracker->RequestStartTracking();
	scene3D->startTracking();
}

void Scene3DWidget::configTracker()
{
	PolarisConfigurationWidget *polarisConfiguration = new PolarisConfigurationWidget();
	polarisConfiguration->show();

	while(!polarisConfiguration->hasQuitted())
	{
		QTest::qWait(0.001);
	}

	std::vector<QString> files = polarisConfiguration->getFiles();

    std::string referenceToolFilename = std::string(files[0].toAscii().data());
    std::string ultrasoundProbeFilename = std::string(files[1].toAscii().data());
    std::string needleFilename = std::string(files[2].toAscii().data());
	std::string pointerFilename = std::string(files[3].toAscii().data());

	int port = polarisConfiguration->getPort();

	scene3D->configTracker(referenceToolFilename, ultrasoundProbeFilename, needleFilename, 
							pointerFilename, files[4], files[5], files[6], port);

	ui->initLoggetBt->setEnabled(true);
	ui->startTrackingBt->setEnabled(true);
}

void Scene3DWidget::openVolume()
{
	QString qtVolumeFilename = QFileDialog::getOpenFileName(this, tr("Open Volume"),
        QDir::currentPath(),tr("MHD Files (*.mhd)"));
	std::string volumeFilename = std::string(qtVolumeFilename.toAscii().data());

	scene3D->addVolumeToScene(volumeFilename);

}

void Scene3DWidget::showAligment(bool on)
{

}

void Scene3DWidget::seeFourViews(bool on)
{
	if(on){		

            std::cout<<"Displaying Four Views"<<std::endl;
            
		this->View2 = igstk::View3D::New();
		this->qtDisplay2 = new igstk::QTWidget();
		this->qtDisplay2->RequestSetView( this->View2 );

		this->View3 = igstk::View3D::New();
		this->qtDisplay3 = new igstk::QTWidget();
		this->qtDisplay3->RequestSetView( this->View3 );

		this->View4 = igstk::View3D::New();
		this->qtDisplay4 = new igstk::QTWidget();
		this->qtDisplay4->RequestSetView( this->View4 );

		this->View2->RequestStart();
		this->View3->RequestStart();
		this->View4->RequestStart();
		
		layout->addWidget(this->qtDisplay2,0,1);
		layout->addWidget(this->qtDisplay3,1,0);
		layout->addWidget(this->qtDisplay4,1,1);
		
		scene3D->addFourViews();
	
		this->fourViews = true;

	}else{
		
                scene3D->removeFourViews();
                
		this->View2->RequestStop();
		this->View3->RequestStop();
		this->View4->RequestStop();

		layout->removeWidget(this->qtDisplay2);
		layout->removeWidget(this->qtDisplay3);
		layout->removeWidget(this->qtDisplay4);
		
		View2 = NULL;
		View3 = NULL;
		View4 = NULL;
		
		delete qtDisplay2;
		delete qtDisplay3;
		delete qtDisplay4;

		this->fourViews = false;
		
	}
		
}

void Scene3DWidget::initLogger()
{
	scene3D->initLogger();
}

bool Scene3DWidget::HasQuitted()
{
    return this->quit;
}

void Scene3DWidget::setCoords(std::vector<double> coords)
{
	QString str0 = QString::number(coords[0]);
	ui->usProbeX->setText(str0);

	QString str1 = QString::number(coords[1]);
	ui->usProbeY->setText(str1);

	QString str2 = QString::number(coords[2]);
	ui->usProbeZ->setText(str2);

	QString str3 = QString::number(coords[3]);
	ui->needleX->setText(str3);

	QString str4 = QString::number(coords[4]);
	ui->needleY->setText(str4);

	QString str5 = QString::number(coords[5]);
	ui->needleZ->setText(str5);
}

void Scene3DWidget::SetTracker( TrackerType * tracker)
{
	this->m_Tracker = tracker;
}

void Scene3DWidget::setScene3D(Scene3D* scene3D)
{
    this->scene3D = scene3D;
}

void Scene3DWidget::needleTrajectory(bool on)
{
    if(on)
        this->scene3D->putNeedleTrajectory();
    else
        this->scene3D->removeNeedleTrajectory();
}

void Scene3DWidget::putTarget(bool on)
{
    if(on){
        
        this->scene3D->putTarget();
        
        this->ui->upBtn->setEnabled(true);
        this->ui->downBtn->setEnabled(true);
        this->ui->leftBtn->setEnabled(true);
        this->ui->rightBtn->setEnabled(true);
        this->ui->inBtn->setEnabled(true);
        this->ui->outBtn->setEnabled(true);
        this->ui->fixBtn->setEnabled(true);             
        
        this->ui->targetStep->setEnabled(true);
        this->ui->targetStep->setText("1");
        
    }else{
        
        this->scene3D->removeTarget();
        
        this->ui->upBtn->setEnabled(false);
        this->ui->downBtn->setEnabled(false);
        this->ui->leftBtn->setEnabled(false);
        this->ui->rightBtn->setEnabled(false);
        this->ui->inBtn->setEnabled(false);
        this->ui->outBtn->setEnabled(false);
        this->ui->fixBtn->setEnabled(false);   
        this->ui->targetStep->setEnabled(false);
    }
}

void Scene3DWidget::moveTargetUp()
{
    float step = this->ui->targetStep->text().toFloat();
    this->scene3D->moveTargetUp(step);
}

void Scene3DWidget::moveTargetDown()
{
    float step = this->ui->targetStep->text().toFloat();
    this->scene3D->moveTargetDown(step);
}

void Scene3DWidget::moveTargetLeft()
{
    float step = this->ui->targetStep->text().toFloat();
    this->scene3D->moveTargetLeft(step);
}

void Scene3DWidget::moveTargetRight()
{
    float step = this->ui->targetStep->text().toFloat();
    this->scene3D->moveTargetRight(step);
}

void Scene3DWidget::moveTargetIn()
{
    float step = this->ui->targetStep->text().toFloat();
    this->scene3D->moveTargetIn(step);
}

void Scene3DWidget::moveTargetOut()
{
    float step = this->ui->targetStep->text().toFloat();
    this->scene3D->moveTargetOut(step);
}

void Scene3DWidget::fixTarget(bool on)
{
    if(!on){
        
        this->ui->upBtn->setEnabled(true);
        this->ui->downBtn->setEnabled(true);
        this->ui->leftBtn->setEnabled(true);
        this->ui->rightBtn->setEnabled(true);
        this->ui->inBtn->setEnabled(true);
        this->ui->outBtn->setEnabled(true);          
        this->ui->targetStep->setEnabled(true);

    }else{
        
        this->ui->upBtn->setEnabled(false);
        this->ui->downBtn->setEnabled(false);
        this->ui->leftBtn->setEnabled(false);
        this->ui->rightBtn->setEnabled(false);
        this->ui->inBtn->setEnabled(false);
        this->ui->outBtn->setEnabled(false); 
        this->ui->targetStep->setEnabled(false);
        
    }
        
}

void Scene3DWidget::getTargetCoordinates()
{
    this->scene3D->getTargetCoordinatesFromThreeViews();
}