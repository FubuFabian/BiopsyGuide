/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTargetObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTargetObjectRepresentation.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkImplicitBoolean.h>
#include <vtkSampleFunction.h>
#include <vtkMarchingContourFilter.h>
#include <vtkCylinderSource.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>

namespace igstk
{

/** Constructor */
TargetObjectRepresentation
::TargetObjectRepresentation():m_StateMachine(this)
{
  // We create the ellipse spatial object
  m_TargetSpatialObject = NULL;
  this->RequestSetSpatialObject( m_TargetSpatialObject );

  igstkAddInputMacro( ValidTargetObject );
  igstkAddInputMacro( NullTargetObject  );

  igstkAddStateMacro( NullTargetObject   );
  igstkAddStateMacro( ValidTargetObject  );

  igstkAddTransitionMacro( NullTargetObject,
                           NullTargetObject,
                           NullTargetObject,  No );
  igstkAddTransitionMacro( NullTargetObject,
                           ValidTargetObject,
                           ValidTargetObject,
                           SetTargetObject );
  igstkAddTransitionMacro( ValidTargetObject,
                           NullTargetObject,
                           NullTargetObject,  No );
  igstkAddTransitionMacro( ValidTargetObject,
                           ValidTargetObject,
                           ValidTargetObject,  No );

  igstkSetInitialStateMacro( NullTargetObject );

  m_StateMachine.SetReadyToRun();
}

/** Destructor */
TargetObjectRepresentation::~TargetObjectRepresentation()
{
  this->DeleteActors();
}

/** Set the Target Spatial Object */
void TargetObjectRepresentation
::RequestSetTargetObject( const TargetSpatialObjectType *
                                                            Target )
{
  m_TargetObjectToAdd = Target;
  if( !m_TargetObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullTargetObjectInput );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidTargetObjectInput );
    m_StateMachine.ProcessInputs();
    }
}

/** Set the Target Spatial Object */
void TargetObjectRepresentation::NoProcessing()
{
}

/** Set the Target Spatial Object */
void TargetObjectRepresentation::SetTargetObjectProcessing()
{
  m_TargetSpatialObject = m_TargetObjectToAdd;
  this->RequestSetSpatialObject( m_TargetSpatialObject );
}

/** Print Self function */
void TargetObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Update the visual representation in response to changes in the geometric
 * object */
void TargetObjectRepresentation::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "UpdateRepresentationProcessing called ....\n");
}

/** Create the vtk Actors */
void TargetObjectRepresentation::CreateActors()
{
    this->DeleteActors();
    
    vtkPolyDataMapper* redLineMapper = vtkPolyDataMapper::New();
    
    vtkActor* redLineActor = vtkActor::New();
    redLineActor->GetProperty()->SetColor(1.0,0.0,0.0);
    redLineActor->GetProperty()->SetOpacity(this->GetOpacity());
    redLineActor->SetOrigin(0,0,0);
    redLineActor->SetOrientation(0,0,90);
    
    vtkCylinderSource* redLine = vtkCylinderSource::New();
    redLine->SetRadius(0.3);
    redLine->SetHeight(60);
    redLine->SetCenter(0,0,0);
    
    redLineMapper->SetInputConnection(redLine->GetOutputPort());
    redLineActor->SetMapper(redLineMapper);
    
    this->AddActor(redLineActor);
    redLineMapper->Delete();
    redLine->Delete();
    
    vtkPolyDataMapper* greenLineMapper = vtkPolyDataMapper::New();
    
    vtkActor* greenLineActor = vtkActor::New();
    greenLineActor->GetProperty()->SetColor(0.0,1.0,0.0);
    greenLineActor->GetProperty()->SetOpacity(this->GetOpacity());
    
    vtkCylinderSource* greenLine = vtkCylinderSource::New();
    greenLine->SetRadius(0.3);
    greenLine->SetHeight(60);
    greenLine->SetCenter(0,0,0);
    
    greenLineMapper->SetInputConnection(greenLine->GetOutputPort());
    greenLineActor->SetMapper(greenLineMapper);
    
    this->AddActor(greenLineActor);
    greenLineMapper->Delete();
    greenLine->Delete();
    
    vtkPolyDataMapper* blueLineMapper = vtkPolyDataMapper::New();
    
    vtkActor* blueLineActor = vtkActor::New();
    blueLineActor->GetProperty()->SetColor(0.0,0.0,1.0);
    blueLineActor->GetProperty()->SetOpacity(this->GetOpacity());
    blueLineActor->SetOrigin(0,0,0);
    blueLineActor->SetOrientation(90,0,0);
    
    vtkCylinderSource* blueLine = vtkCylinderSource::New();
    blueLine->SetRadius(0.3);
    blueLine->SetHeight(60);
    blueLine->SetCenter(0,0,0);
    
    blueLineMapper->SetInputConnection(blueLine->GetOutputPort());
    blueLineActor->SetMapper(blueLineMapper);
    
    this->AddActor(blueLineActor);
    blueLineMapper->Delete();
    blueLine->Delete();
    
    

}

/** Create a copy of the current object representation */
TargetObjectRepresentation::Pointer
TargetObjectRepresentation::Copy() const
{
  Pointer newOR = TargetObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetTargetObject(m_TargetSpatialObject);

  return newOR;
}


} // end namespace igstk


