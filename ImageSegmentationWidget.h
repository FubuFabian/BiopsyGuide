#ifndef IMAGESEGMENTATIONWIDGET_H
#define IMAGESEGMENTATIONWIDGET_H

#include "mainwindow.h"
#include "QVTKImageWidget.h"
#include "ImageSegmentation.h"

#include <QWidget>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include <itkImage.h>

namespace Ui {
    class ImageSegmentationWidget;
}

//!User interface for image segmentation
/*!
 *This class is the user interface for segmenting a breast tumor from
 *2D ultrasound image using the class ImageSegmentation.h
*/
class ImageSegmentationWidget : public QWidget
{
    Q_OBJECT
    
public:
    
    typedef itk::Image<unsigned char, 2> ImageType; ///< itk 2D image type

    /**
     * \brief class contructor
     * @param parent
     */
    explicit ImageSegmentationWidget(QWidget *parent = 0);
    ~ImageSegmentationWidget();

    /**
     * \brief set the ultrasoun image
     * @param a vtk image
     */
    void setImage(vtkSmartPointer<vtkImageData> image);
    
    /**
     * \brief Set the calling main window
     * @param mainWindow
     */
    void setMainWindow(MainWindow * mainWindow);
	   
    
private:
    
    Ui::ImageSegmentationWidget *ui; ///< the user interface

    ImageSegmentation * imageSegmentation; ///< has all the functions for image segmentation
    
    MainWindow * mainWindow; ///< the calling main window
    
    vtkSmartPointer<vtkImageData> vtkProbabilityImage; ///< the probability image
    
    vtkSmartPointer<vtkImageData> vtkRegionGrowingImage; ///< the region growing image
    
    vtkSmartPointer<vtkImageData> vtkContourImage; ///< the tumor contour image
    
    std::vector<ImageType::IndexType> contourPixels; ///< the tumor contour pixels coordinates
    
    QVTKImageWidget *displayWidget; ///< the display widget
    
    std::vector<float> intensityProbs; ///< the intensity probability function
    
    std::vector<float> textureProbs; ///< the texture probability function
    
    bool intensityProbsFlag; ///< indicates if the intensity probability function is loaded
    
    bool textureProbsFlag; ///< indicates if the texture probability function is loaded
    
    bool probabilityImageFlag; ///< indicates if the probability image has been computed
    
    bool seedPointFlag; ///< indicates if the seed piont has been selected
    
    ImageType::IndexType seedPoint;  ///< the region growing seed point

private slots:

    /**
     * \brief load the intensity probability function
     */
    void loadIntensityProbs();

    /**
     * \brief load the texture probability function
     */
    void loadTextureProbs();
    
    /**
     * \brief calls the ImageSegmentation.h function to compute the proability image
     */
    void computeProbability();

    /**
     * \brief calls the ImageSegmentation.h function to segment the image
     */
    void segment();

    /**
     * \brief saves the probability image, region growing image, contour image
     * and the contour coordinates
     */
    void save();
    
    
    /**
     * \brief choose a new seed point
     */
    void newSeed();
    
    /**
    * \brief get the cross point coordinates
    */
    void getCoordinates();
};

#endif // IMAGESEGMENTATIONWIDGET_H
