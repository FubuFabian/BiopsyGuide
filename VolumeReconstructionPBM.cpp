#include "VolumeReconstructionPBM.h"

// vtk includes
#include <vtkNew.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageReslice.h>

// other includes
#include "time.h"
#include "float.h"
#include "math.h"
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_3x3.h>
#include <vector>

vtkSmartPointer<vtkImageData> VolumeReconstructionPBM::generateVolume()
{
    
    
    std::cout<<"Generating Volume Data"<<std::endl;
    
    volumeData = vtkSmartPointer<vtkImageData>::New();
    volumeData->SetNumberOfScalarComponents(1);
    volumeData->SetScalarType(VTK_UNSIGNED_CHAR);
    volumeData->SetOrigin(volumeOrigin[0],volumeOrigin[1],volumeOrigin[2]);
    volumeData->SetDimensions(volumeSize[0],volumeSize[1],volumeSize[2]);
    volumeData->SetSpacing(scale[0]*resolution,scale[1]*resolution,scale[1]*resolution);
    volumeData->AllocateScalars();
        
    /** Pointer to voxels who have been filled in the bin filling stage*/
    filledVolume = vtkSmartPointer<vtkImageData>::New();
    filledVolume->SetScalarTypeToUnsignedChar();
    filledVolume->SetNumberOfScalarComponents(1);
    filledVolume->SetOrigin(volumeOrigin[0],volumeOrigin[1],volumeOrigin[2]);
    filledVolume->SetDimensions(volumeSize[0],volumeSize[1],volumeSize[2]);
    filledVolume->SetSpacing(scale[0]*resolution,scale[1]*resolution,scale[1]*resolution);
    filledVolume->AllocateScalars();   

    /////////////////////////////////
    this->BinFilling();
  
    return volumeData;

}

void VolumeReconstructionPBM::
BinFilling()
{
     //create accumulation buffer
    vtkSmartPointer<vtkImageData> AccDataVolume = vtkSmartPointer<vtkImageData>::New();
    AccDataVolume->SetScalarTypeToUnsignedChar();
    AccDataVolume->SetNumberOfScalarComponents(1);
    AccDataVolume->SetOrigin(volumeOrigin[0],volumeOrigin[1],volumeOrigin[2]);
    AccDataVolume->SetDimensions(volumeSize[0],volumeSize[1],volumeSize[2]);
    AccDataVolume->SetSpacing(scale[0]*resolution,scale[1]*resolution,scale[1]*resolution);
    AccDataVolume->AllocateScalars();
    
    std::cout<<"Calculating voxel values"<<std::flush;
    clock_t begin = clock();

    for (int i = 0; i < volumeImageStack.size(); i++)
    {
        int * imageSize = volumeImageStack.at(i)->GetDimensions();
        std::cout<<"."<<std::flush;
        
        for (int x = 0; x<imageSize[0]; x++){
            for (int y = 0; y<imageSize[1]; y++){
                
                int voxel[3];
                voxel[0] = (pixelCoordsXStack.at(i)[x][y] - volumeOrigin[0])/(scale[0]*resolution);
                voxel[1] = (pixelCoordsYStack.at(i)[x][y] - volumeOrigin[1])/(scale[1]*resolution);
                voxel[2] = (pixelCoordsZStack.at(i)[x][y] - volumeOrigin[2])/(scale[1]*resolution);                
                
                if (voxel[0] < volumeSize[0] && voxel[0] > 0 && 
                        voxel[1] < volumeSize[1] && voxel[1] > 0 &&
                        voxel[2] < volumeSize[2] && voxel[2] > 0)
                {                                                    
                    unsigned char * imagePixel = static_cast<unsigned char *> (
                            volumeImageStack.at(i)->GetScalarPointer(x, y, 0));
                    

                    // get pointer to the current volume voxel 
                    unsigned char * volumeVoxel = static_cast<unsigned char *> (
                            volumeData->GetScalarPointer(voxel[0], voxel[1], voxel[2]));

                    // get pointer to the current accumator volume voxel 
                    unsigned char * accDataVoxel = static_cast<unsigned char *> (
                            AccDataVolume->GetScalarPointer(voxel[0], voxel[1], voxel[2]));


                    // get pointer to the current fill volume voxel 
                    unsigned char * fillVoxel = static_cast<unsigned char *> (
                            filledVolume->GetScalarPointer(voxel[0], voxel[1], voxel[2]));


                    // set voxel value with the corresponding pixel value
                    if (fillVoxel[0] == 0)
                    {
                        volumeVoxel[0] = imagePixel[0];
                        accDataVoxel[0] = 1;
                        fillVoxel[0] = 1;
                    }
                    else
                    {
                        float temp = ((volumeVoxel[0] * accDataVoxel[0]) + imagePixel[0]) /
                                (accDataVoxel[0] + 1);
                        volumeVoxel[0] = (unsigned char) temp;
                        accDataVoxel[0]++;
                    }      
                
                }
          }
        }                
    
     }

    clock_t end = clock();
    double diffticks = end - begin;
    double diffms = (diffticks * 10) / CLOCKS_PER_SEC;
    std::cout<<std::endl<<"Time elapsed reconstructing volume: "<< double(diffms)<<" ms" <<std::endl;

}

void VolumeReconstructionPBM::setPixelCoordsStack(std::vector< vnl_matrix<double> > pixelCoordsXStack
                                               , std::vector< vnl_matrix<double> > pixelCoordsYStack
                                               , std::vector< vnl_matrix<double> > pixelCoordsZStack)
{
    this->pixelCoordsXStack = pixelCoordsXStack;
    this->pixelCoordsYStack = pixelCoordsYStack;
    this->pixelCoordsZStack = pixelCoordsZStack;
}
