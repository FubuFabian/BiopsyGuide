/* 
 * File:   itkScalarToTextureHUImageFilter.txx
 * Author: Fabian
 *
 * Created on July 29, 2013, 4:09 PM
 */

#ifndef ITKSCALARTOTEXTUREHUIMAGEFILTER_TXX
#define	ITKSCALARTOTEXTUREHUIMAGEFILTER_TXX

#include "itkScalarToTextureHUImageFilter.h"

#include <itkMirrorPadImageFilter.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkImage.h>
#include <itkSmartPointer.h>
#include <itkImageMomentsCalculator.h>

#include <vector>
#include <math.h>
#include <iostream>

namespace itk {

template <class TInputImage, class TMaskImage, class TOutputImage>
ScalarToTextureHUImageFilter<TInputImage, TMaskImage, TOutputImage>
::ScalarToTextureHUImageFilter()
{
    this->m_RegionSize.Fill( 5 );
    this->m_MaskImage = NULL;
    
}

template<class TInputImage, class TMaskImage, class TOutputImage>
void
ScalarToTextureHUImageFilter<TInputImage, TMaskImage, TOutputImage>
::GenerateData()
{
    
  this->AllocateOutputs();
  
  ProgressReporter progress( this, 0, 
        this->GetInput()->GetRequestedRegion().GetNumberOfPixels(), 100 );
  
  typedef MirrorPadImageFilter<InputImageType,InputImageType> PadFilterType;
  typename PadFilterType::Pointer padFilter = PadFilterType::New();
  
  int boundSize = floor((double)this->m_RegionSize[1]/2);
  
  RegionSizeType bound;
  bound.Fill(boundSize);
  
  padFilter->SetPadBound(bound);
  padFilter->SetInput(this->GetInput());
  padFilter->Update();
  
  typename InputImageType::Pointer padImage = padFilter->GetOutput();   
  
  typedef RegionOfInterestImageFilter<InputImageType,InputImageType> ExtractFilterType;
  typename ExtractFilterType::Pointer extractFilter = ExtractFilterType::New();
  
  typename InputImageType::RegionType region;
  typename InputImageType::IndexType regionIndex;
    
  region.SetSize(this->m_RegionSize);
  
  extractFilter->SetInput(padImage);
  
  ImageRegionIteratorType itPad(padImage, padImage->GetLargestPossibleRegion());
  itPad.GoToBegin();

  ImageRegionIteratorType itOut(this->GetOutput(), 
          this->GetOutput()->GetLargestPossibleRegion());
  itOut.GoToBegin();

  typedef ImageMomentsCalculator<InputImageType> MomentsCalculatorType;
  typename MomentsCalculatorType::Pointer momentsCalculator = MomentsCalculatorType::New();
  
  typename MomentsCalculatorType::MatrixType centralMoments;
  typename InputImageType::PixelType huMoment;
  
  
  while(!itPad.IsAtEnd()){

        regionIndex = itPad.GetIndex();
        region.SetIndex(regionIndex);
        
        if(padImage->GetLargestPossibleRegion().IsInside(region)){ 
            
            extractFilter->SetRegionOfInterest(region);
            extractFilter->UpdateLargestPossibleRegion();
            
            momentsCalculator->SetImage(extractFilter->GetOutput());
            momentsCalculator->Compute();
            
            centralMoments = momentsCalculator->GetCentralMoments();
            
            huMoment = centralMoments[0][0] + centralMoments[1][1];
                
            this->GetOutput()->SetPixel(itOut.GetIndex(),huMoment);
            
            ++itOut;
            
            progress.CompletedPixel();
        }
        
        ++itPad;
  }
  
}

template<class TInputImage, class TMaskImage, class TOutputImage>
void
ScalarToTextureHUImageFilter<TInputImage, TMaskImage, TOutputImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Region size: "
    << this->m_RegionSize << std::endl;
  
}

}// end namespace itk

#endif	/* ITKSCALARTOTEXTUREHUIMAGEFILTER_TXX */

