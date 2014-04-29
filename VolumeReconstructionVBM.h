#ifndef VOLUMERECONSTRUCTIONVBM_H
#define VOLUMERECONSTRUCTIONVBM_H

//#include <vtkSmartPointer.h>
//#include <vtkImageData.h>
#include <vtkPlane.h>
//
//#include <vnl/vnl_matrix.h>
//#include <vnl/vnl_vector.h>
//
//#include <math.h>
//#include <vector>
//#include <map>

#include "VolumeReconstruction.h"

#endif // VOLUMERECONSTRUCTION_H
 
using namespace std;

//!Generate a new volume
/*!
  This class generate a new volume data using a voxel based method with the previously loaded data.
  It requiers the images data, the tracker data and the estimated parameters from a calibration.
  The method implemented a nearest pixel interpolation.
*/
class VolumeReconstructionVBM: public VolumeReconstruction
{

public:

    /**
     * \brief Constructor
     */
    static VolumeReconstructionVBM *New()
    {
		return new VolumeReconstructionVBM;
    } 


    /**
     * \brief Set the image bounds
     */
    void setImageBoundsStack(std::vector< vnl_vector<double> >, std::vector< vnl_vector<double> >,
                              std::vector< vnl_vector<double> >);

    /**
     * \brief Returns the new volume data with the voxel based method
     */
    vtkSmartPointer<vtkImageData> generateVolume();

private:

    /** Stacks for the image Bounds in x */
    std::vector< vnl_vector<double> > imageBoundsXStack;
    /** Stacks for the image Bounds in Y */
    std::vector< vnl_vector<double> > imageBoundsYStack;
    /** Stacks for the image Bounds in Z */
    std::vector< vnl_vector<double> > imageBoundsZStack;

    /** The plane equation for each image */
    std::vector< vtkSmartPointer<vtkPlane> > imagePlaneStack;
    
    /** the maximun distance found in the volume */
    double maxDistance;

    vnl_vector<double> calcIntersectionPoint(int plane, double voxel[]);
        
    /**
     * \brief Compute the plane equation for each image
     */
    void calcImagePlane();

    /**
     * \brief Computes the voxel value using three lineal interpolation
     */
    std::vector<double> calcVoxelValue(std::vector<double>, std::vector<int>, double[3]); //Closests
    
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
