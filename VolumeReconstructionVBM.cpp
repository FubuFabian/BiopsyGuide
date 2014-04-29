#include "VolumeReconstructionVBM.h"

#include <vtkMath.h>
#include <vtkMetaImageWriter.h>
#include <vnl/vnl_inverse.h>
#include <exception>
#include <time.h>
#include <map>
#include <float.h>

#include <QString>
#include <QFileDialog>

vtkSmartPointer<vtkImageData> VolumeReconstructionVBM::generateVolume()
{
    
    
    std::cout<<"Generating Volume Data"<<std::endl;
	
    volumeData = vtkSmartPointer<vtkImageData>::New();
    volumeData->SetNumberOfScalarComponents(1);
    volumeData->SetScalarType(VTK_UNSIGNED_CHAR);
    volumeData->SetOrigin(volumeOrigin[0],volumeOrigin[1],volumeOrigin[2]);
    volumeData->SetDimensions(volumeSize[0],volumeSize[1],volumeSize[2]);
    volumeData->SetSpacing(scale[0]*resolution,scale[1]*resolution,scale[1]*resolution);
    volumeData->AllocateScalars();
    
    calcImagePlane();
    maxDistance = this->calcMaxDistance();
        
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

void VolumeReconstructionVBM::
BinFilling()
{
    std::cout<<"Calculating voxel values"<<std::flush;
    clock_t begin = clock();
    
    for(int i=0; i<volumeSize[0]; i++){
        
	std::cout<<"."<<std::flush;

	for(int j=0; j<volumeSize[1]; j++){
		for(int k=0; k<volumeSize[2]; k++){

			double voxel[3];
			voxel[0] = i*scale[0]*resolution + volumeOrigin[0];
			voxel[1] = j*scale[1]*resolution + volumeOrigin[1];
			voxel[2] = k*scale[1]*resolution + volumeOrigin[2];

                        std::vector<double> distances;
                        distances.resize(2);
                        std::fill(distances.begin(),distances.end(),maxDistance);
                                
                        std::vector<int> planes;
                        planes.resize(2);
                                
                        std::vector<double>::iterator itDistances;
                        std::vector<int>::iterator itPlanes;

			for(int plane=0; plane<volumeImageStack.size(); plane++){				
					
				double d = imagePlaneStack.at(plane)->DistanceToPlane(voxel);
					
                                itDistances = std::lower_bound(distances.begin(),distances.end(),d);
                                itPlanes = planes.begin() + (int)(itDistances-distances.begin());
                                        
                                distances.insert(itDistances,d);
                                distances.pop_back();
                                        
                                planes.insert(itPlanes,plane);
                                planes.pop_back();
                             
			}

                                
                        // get pointer to the current volume voxel 
                        unsigned char * volumeVoxel = static_cast<unsigned char *> (
                                                volumeData->GetScalarPointer(i, j, k));
                        unsigned char * filledVoxel = static_cast<unsigned char *> (
                                                filledVolume->GetScalarPointer(i, j, k));
                          
                        std::vector<double> voxelsValue = calcVoxelValue(distances,planes,voxel);
                                
                        volumeVoxel[0] = voxelsValue[0];    
                        filledVoxel[0] = voxelsValue[1];
		}


          }

	}
    
    clock_t end = clock();
    double diffticks = end - begin;
    double diffms = (diffticks * 10) / CLOCKS_PER_SEC;

    std::cout<<std::endl<<"Time elapsed reconstructing volume: "<< double(diffms)<<" ms" <<std::endl;
}

 
std::vector<double> VolumeReconstructionVBM::calcVoxelValue(std::vector< double> distances, std::vector<int> planes, double voxel[3]) //Closests
{										
   
    double voxelValue = 0;
    double filled = 0;
    
    std::vector<double> voxelsValue;
    voxelsValue.reserve(2);
    
    int numberOfIntersectedPlanes = 0;
    int intersectionPlanes = 2;
	
    
    for(int i = 0; i<distances.size(); i++){              
        
        vnl_vector<double> imgCoords = this->calcIntersectionPoint(planes[i],voxel);

	if(!imgCoords.empty()){    
            
            filled = 1;

            numberOfIntersectedPlanes++;               
            
            unsigned char * imagePixel1 = static_cast<unsigned char *> (
                            volumeImageStack.at(planes[i])->GetScalarPointer(imgCoords[0],imgCoords[1]-1, 0));
            unsigned char * imagePixel2 = static_cast<unsigned char *> (
                            volumeImageStack.at(planes[i])->GetScalarPointer(imgCoords[0]-1,imgCoords[1], 0));
            unsigned char * imagePixel3 = static_cast<unsigned char *> (
                            volumeImageStack.at(planes[i])->GetScalarPointer(imgCoords[0],imgCoords[1], 0));
            unsigned char * imagePixel4 = static_cast<unsigned char *> (
                            volumeImageStack.at(planes[i])->GetScalarPointer(imgCoords[0]+1,imgCoords[1], 0));
            unsigned char * imagePixel5 = static_cast<unsigned char *> (
                            volumeImageStack.at(planes[i])->GetScalarPointer(imgCoords[0],imgCoords[1]+1, 0));
                    
            double w = exp(-0.5*pow(distances[i]/maxDistance,2.0)); //Gaus
            //double w = 1 - distances[i]/maxDistance; //Lineal
            
            double pixelValue = w*(imagePixel1[0]+imagePixel2[0]+imagePixel3[0]+imagePixel4[0]+imagePixel5[0])/5;
				
            voxelValue += pixelValue;
                        
            if(numberOfIntersectedPlanes==intersectionPlanes)
                break;

	}
    }
    
    voxelValue /= numberOfIntersectedPlanes;
    
    voxelsValue.push_back(voxelValue);
    voxelsValue.push_back(filled);

    return voxelsValue;

}

vnl_vector<double> VolumeReconstructionVBM::calcIntersectionPoint(int plane, double voxel[])
{
 
    double crossPoint[3];

    imagePlaneStack.at(plane)->ProjectPoint(voxel,crossPoint);
                                        
    vnl_vector<double> pointCoords;
    pointCoords.set_size(4);
    pointCoords.put(0,crossPoint[0]);
    pointCoords.put(1,crossPoint[1]);
    pointCoords.put(2,crossPoint[2]);
    pointCoords.put(3,1);
    
    vnl_matrix<double> inverseTransform = vnl_inverse(transformStack.at(plane));

    vnl_vector<double> imgCoord = inverseTransform*pointCoords;
                                        
    imgCoord.put(0,imgCoord[0]/scale[0]);
    imgCoord.put(1,imgCoord[1]/scale[1]);

    int * imgSize = volumeImageStack.at(plane)->GetDimensions();
                                        
    if(imgCoord[0]>=1 && imgCoord[1]>=1 ){
        if(imgCoord[0]<imgSize[0]-1 && imgCoord[1]<imgSize[1]-1){   
            return imgCoord;
        }
    }
    
    imgCoord.clear();
    return imgCoord;
            
}

void VolumeReconstructionVBM::calcImagePlane()
{
	std::cout<<std::endl;
	std::cout<<"Calculating images planes"<<std::endl<<std::endl;

	for(int i=0; i<volumeImageStack.size(); i++){

		vnl_vector<double> xBounds = imageBoundsXStack.at(i);
		vnl_vector<double> yBounds = imageBoundsYStack.at(i);
		vnl_vector<double> zBounds = imageBoundsZStack.at(i);

		vnl_vector<double> AB;
		AB.set_size(3);

		AB[0] = xBounds[1] - xBounds[0];
		AB[1] = yBounds[1] - yBounds[0];
		AB[2] = zBounds[1] - zBounds[0];

		vnl_vector<double> AC;
		AC.set_size(3);

		AC[0] = xBounds[2] - xBounds[0];
		AC[1] = yBounds[2] - yBounds[0];
		AC[2] = zBounds[2] - zBounds[0];

		double a;
		double b;
		double c;

		a = (AB[1]*AC[2]) - (AC[1]*AB[2]);
		b = (AC[0]*AB[2]) - (AB[0]*AC[2]);
		c = (AB[0]*AC[1]) - (AC[0]*AB[1]);

		double planeMagnitud = sqrt(a*a + b*b + c*c);
		
		a = a/planeMagnitud;
		b = b/planeMagnitud;
		c = c/planeMagnitud;

		vtkSmartPointer<vtkPlane> imagePlane = vtkSmartPointer<vtkPlane>::New();

		imagePlane->SetOrigin(xBounds[0],yBounds[0],zBounds[0]);
		imagePlane->SetNormal(a,b,c);

		imagePlaneStack.push_back(imagePlane);
	}
}

void VolumeReconstructionVBM::setImageBoundsStack(std::vector< vnl_vector<double> > imageBoundsXStack
                                               , std::vector< vnl_vector<double> > imageBoundsYStack
                                               , std::vector< vnl_vector<double> > imageBoundsZStack)
{
    this->imageBoundsXStack = imageBoundsXStack;
    this->imageBoundsYStack = imageBoundsYStack;
    this->imageBoundsZStack = imageBoundsZStack;
}

