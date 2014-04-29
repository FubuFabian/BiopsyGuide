/* 
 * File:   VolumeReconstruction.h
 * Author: Fabian
 *
 * Created on April 23, 2014, 4:56 PM
 */

#ifndef VOLUMERECONSTRUCTION_H
#define	VOLUMERECONSTRUCTION_H

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>

// Other includes 
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

#include <vector>

class VolumeReconstruction
{
    
public:
    
    /**
     * \brief Constructor
     */
   static VolumeReconstruction *New()
   {
	return new VolumeReconstruction;
   } 
   
   /**
     * \brief Set the size of the volume data
     */
   void setVolumeSize(vnl_vector<double> volumeSize)
   {
       this->volumeSize = volumeSize;
   }
    
   /**
     * \brief Set the volume data orgin in the 3D scene
     */
   void setVolumeOrigin(vnl_vector<double> volumeOrigin)
   {
        this->volumeOrigin = volumeOrigin;
   }
   
   /**
     * \brief Set image data stack to generate the volume
     */
   void setVolumeImageStack(std::vector< vtkSmartPointer< vtkImageData> > volumeImageStack)
   {
        this->volumeImageStack = volumeImageStack;
   }
    
   /**
     * \brief Set the transformation for each image used in the reconstruction
     */
   void setTransformStack(std::vector< vnl_matrix<double> > transformStack)
   {
       this-> transformStack = transformStack;
   }
   
   /**
     * \brief Set the scale of the images
     */
   void setScale(vnl_vector<double> scale)
   {
       this->scale = scale;
   }
   
   /**
     * \brief Set the resolution of the volume
     */
   void setResolution(int resolution)
   {
       this->resolution = resolution;
   }
  
   
   /**
     * \brief Returns the new volume data with the method
     */
   vtkSmartPointer<vtkImageData> generateVolume();
   
   /**
     * \brief Returns the filled volume data 
     */
   vtkSmartPointer<vtkImageData> fillVolume(int);   
    
protected:
    
    /** Reconstruction Volume */
    vtkSmartPointer<vtkImageData> volumeData;
    
    /** Volume with filled voxels */
    vtkSmartPointer<vtkImageData> filledVolume;
    
   /** Size of the volume */
    vnl_vector<double> volumeSize;

    /** Where the volume data begins in the 3D scene */
    vnl_vector<double> volumeOrigin; 
    
    /** The stack of images data */
    std::vector< vtkSmartPointer< vtkImageData> > volumeImageStack;

    /** Contains the transformation for each image */
    std::vector< vnl_matrix<double> > transformStack;

    /** scale of the images */
    vnl_vector<double> scale;
    
    /** The resolution of 
     * the volume*/
    int resolution;
    
    /**
     * Fill the nearest voxel with pixel information
     * @param volume
     * @param imageWidth
     * @param imageHeight
     * @param imageSize
     * @param xScale
     * @param yScale
     * @param zScale
     * @param volumeScale
     */
    void BinFilling();
    
    /**
     * \brief Computes the maximun distance in the volume
     */
    double calcMaxDistance();
    
};

#endif	/* VOLUMERECONSTRUCTION_H */

