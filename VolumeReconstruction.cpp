#include "VolumeReconstruction.h"

#include "time.h"

vtkSmartPointer<vtkImageData> VolumeReconstruction::fillVolume(int maxKernelSize)
{
    if(maxKernelSize%2 == 0)
        maxKernelSize -= 1;
    
    double maxDistance = calcMaxDistance();
    std::cout<<"Filling Empty Voxels with a Kernel of "<<maxKernelSize<<"x"<<maxKernelSize<<std::endl;
    
    clock_t begin = clock();

    // create an interpolation buffer
    vtkSmartPointer<vtkImageData> InterpolationVolume = vtkSmartPointer<vtkImageData>::New();
    InterpolationVolume->SetScalarTypeToUnsignedChar();
    InterpolationVolume->SetNumberOfScalarComponents(1);
    InterpolationVolume->SetOrigin(volumeOrigin[0],volumeOrigin[1],volumeOrigin[2]);
    InterpolationVolume->SetDimensions(volumeSize[0],volumeSize[1],volumeSize[2]);
    InterpolationVolume->SetSpacing(scale[0]*resolution,scale[1]*resolution,scale[1]*resolution);
    InterpolationVolume->AllocateScalars();   


    // create volume pointers
    unsigned char * voxel;
    unsigned char * filledVoxel;
    unsigned char * innerVoxel;
    unsigned char * interpVoxel;
    
    vnl_vector<double> centralVoxel;
    centralVoxel.set_size(3);
    
    vnl_vector<double> foundVoxel;
    foundVoxel.set_size(3);

    for(int x=0; x<volumeSize[0]; x++){
        
	std::cout<<"."<<std::flush;

	for(int y=0; y<volumeSize[1]; y++){
		for(int z=0; z<volumeSize[2]; z++){

                filledVoxel = static_cast<unsigned char *>
                        (filledVolume->GetScalarPointer(x, y, z));

                if (filledVoxel[0] == 0)
                {
                    // center window point
                    centralVoxel[0] = x;
                    centralVoxel[1] = y;
                    centralVoxel[2] = z;

                    // initial window size
                    int kernelSize = 3;
                    int windowStep = 1;
                    float acc = 0;
                    int idx = 0;

                    while ((idx == 0) && (kernelSize <= maxKernelSize))
                    {
                        
                        // create subvolume extent
                        int subX1 = ((x - windowStep) < 0) ? 0 : x - windowStep;
                        int subX2 = ((x + windowStep) >= volumeSize[0]) ? volumeSize[0]-1 : x + windowStep;
                        int subY1 = ((y - windowStep) < 0) ? 0 : y - windowStep;
                        int subY2 = ((y + windowStep) >= volumeSize[1]) ? volumeSize[1]-1 : y + windowStep;
                        int subZ1 = ((z - windowStep) < 0) ? 0 : z - windowStep;
                        int subZ2 = ((z + windowStep) >= volumeSize[2]) ? volumeSize[2]-1 : z + windowStep;

                        for (int i = subX1; i <= subX2; i++){
                            for (int j = subY1; j <= subY2; j++){
                                for (int k = subZ1; k < subZ2; k++){
                                    
                                    if (x == i && y == j && z == k)
                                        continue;

                                    innerVoxel = static_cast<unsigned char *> (
                                            volumeData->GetScalarPointer(i, j, k));

                                    if (innerVoxel[0] != 0)
                                    {
                                        foundVoxel[0] = i;
                                        foundVoxel[1] = j;
                                        foundVoxel[2] = k;
                                        
                                        double distance = vnl_vector_ssd(centralVoxel,foundVoxel);
                                        
                                        double w = exp(-0.5*pow(distance/maxDistance,2.0)); //Gaus
                                        //double w = 1 - distance/maxDistance; //Lineal

                                        acc += w*innerVoxel[0];

                                        idx++;
                                    }
                                }
                            }
                        }

                        if (idx != 0)
                        {
                            acc /= idx;

                            interpVoxel = static_cast<unsigned char*>
                                    (InterpolationVolume->GetScalarPointer(x, y, z));
                            interpVoxel[0] = acc;
                        }
                        else
                        {
                            windowStep++;
                            kernelSize += 2;
                        }
                    }
                }
            }
        }
    }


    for(int x=0; x<volumeSize[0]; x++){
	for(int y=0; y<volumeSize[1]; y++){
		for(int z=0; z<volumeSize[2]; z++){
                    
                    
                interpVoxel = static_cast<unsigned char*>
                        (InterpolationVolume->GetScalarPointer(x, y, z));
                
                if (interpVoxel[0] != 0)
                {
                    voxel = static_cast<unsigned char *>
                            (volumeData->GetScalarPointer(x, y, z));



                    voxel[0] = interpVoxel[0];
                }
            }
        }
    }
    
    clock_t end = clock();
    double diffticks = end - begin;
    double diffms = (diffticks * 10) / CLOCKS_PER_SEC;
    std::cout<<std::endl<<"Time elapsed filling volume: "<< double(diffms)<<" ms" <<std::endl;
    
    return volumeData;
}

double VolumeReconstruction::calcMaxDistance()
{
        double x = volumeSize[0]*scale[0]*resolution; 
        double y = volumeSize[1]*scale[1]*resolution; 
        double z = volumeSize[2]*scale[1]*resolution; 
        
        double maxDistance = sqrt(x*x + y*y + z*z);
	std::cout<<std::endl<<"Maximum distance in the volume: "<<maxDistance<<std::endl;

	return maxDistance;
}
