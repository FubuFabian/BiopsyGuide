/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTargetObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkTargetObject.h"

namespace igstk
{

/** Constructor */
TargetObject::TargetObject():m_StateMachine(this)
{
  m_TargetSpatialObject = TargetSpatialObjectType::New();
  this->RequestSetInternalSpatialObject( m_TargetSpatialObject );
}

/** Destructor */
TargetObject::~TargetObject()
{
}

/** Print object information */
void TargetObject
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk

