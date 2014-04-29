/* 
 * File:   VolumeReconstructionPBM.h
 * Author: Fabian
 *
 * Created on April 7, 2014, 5:22 PM
 */

#ifndef VOLUMERECONSTRUCTIONPBM_H
#define	VOLUMERECONSTRUCTIONPBM_H


#include "VolumeReconstruction.h"

class VolumeReconstructionPBM: public VolumeReconstruction
{
    
public:
    
    
    /**
     * \brief Constructor
     */
   static VolumeReconstructionPBM *New()
   {
	return new VolumeReconstructionPBM;
   } 
   
   /**
     * \brief Set the image bounds
     */
   void setPixelCoordsStack(std::vector< vnl_matrix<double> >, std::vector< vnl_matrix<double> >,
                              std::vector< vnl_matrix<double> >);

   /**
     * \brief Returns the new volume data with the pixel based method
     */
   vtkSmartPointer<vtkImageData> generateVolume();
   
   
private:    

    /** Stacks for the image Bounds in x */
    std::vector< vnl_matrix<double> > pixelCoordsXStack;
    /** Stacks for the image Bounds in Y */
    std::vector< vnl_matrix<double> > pixelCoordsYStack;
    /** Stacks for the image Bounds in Z */
    std::vector< vnl_matrix<double> > pixelCoordsZStack;
    
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

    
};


#endif	/* VOLUMERECONSTRUCTIONPBM_H */

