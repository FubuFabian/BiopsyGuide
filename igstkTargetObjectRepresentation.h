#ifndef __igstkTargetObjectRepresentation_h
#define __igstkTargetObjectRepresentation_h


#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkTargetObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class TargetObjectRepresentation
 *
 * \brief This class represents a Target object.
 * The representation of the geometric model of the Target is done using
 * VTK implicit functions combined with the Marching Cube algorithm.
 * Boolean operators are also used on the implicit functions to obtain
 * a nice looking shape.
 *

 */

class TargetObjectRepresentation
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TargetObjectRepresentation,
                                 ObjectRepresentation )

public:

  /** Typedefs */
  typedef TargetObject  TargetSpatialObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetTargetObject( const
                   TargetSpatialObjectType * TargetObject );

protected:

  TargetObjectRepresentation( void );
  virtual ~TargetObjectRepresentation( void );

  TargetObjectRepresentation(const Self&); //purposely not implemented
  void operator=(const Self&);                      //purposely not implemented


  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  TargetSpatialObjectType::ConstPointer
                                               m_TargetSpatialObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetTargetObjectProcessing();

  /** Method for performing a null operation during a
   *  State Machine transition */
  void NoProcessing();

private:


  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidTargetObject );
  igstkDeclareInputMacro( NullTargetObject );

  /** States for the State Machine */
  igstkDeclareStateMacro( NullTargetObject );
  igstkDeclareStateMacro( ValidTargetObject );

  TargetSpatialObjectType::ConstPointer m_TargetObjectToAdd;

};


} // end namespace igstk

#endif // TARGETOBJECTREPRESENTATION_H
