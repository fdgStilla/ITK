/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkMorphologyImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef __itkMorphologyImageFilter_txx
#define __itkMorphologyImageFilter_txx

#include <limits.h>

#include "itkConstantBoundaryCondition.h"
#include "itkNumericTraits.h"
#include "itkMorphologyImageFilter.h"

namespace itk {

template<class TInputImage, class TOutputImage, class TKernel>
MorphologyImageFilter<TInputImage, TOutputImage, TKernel>
::MorphologyImageFilter()
{
}
  
template <class TInputImage, class TOutputImage, class TKernel>
void 
MorphologyImageFilter<TInputImage, TOutputImage, TKernel>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the input and output
  InputImagePointer  inputPtr = this->GetInput();
  OutputImagePointer outputPtr = this->GetOutput();
  
  if ( !inputPtr || !outputPtr )
    {
    return;
    }
  
  // we need to compute the input requested region (size and start index)
  int i;
  const typename TOutputImage::SizeType& outputRequestedRegionSize
    = outputPtr->GetRequestedRegion().GetSize();
  const typename TOutputImage::IndexType& outputRequestedRegionStartIndex
    = outputPtr->GetRequestedRegion().GetIndex();
  
  typename TInputImage::SizeType  inputRequestedRegionSize;
  typename TInputImage::IndexType inputRequestedRegionStartIndex;

  const typename TInputImage::SizeType  inputLargestPossibleRegionSize
    = inputPtr->GetLargestPossibleRegion().GetSize();
  const typename TInputImage::IndexType inputLargestPossibleRegionStartIndex
    = inputPtr->GetLargestPossibleRegion().GetIndex();

  long crop=0;
  for (i = 0; i < TInputImage::ImageDimension; i++)
    {
    inputRequestedRegionSize[i]
      = outputRequestedRegionSize[i] + 2 * m_Kernel.GetRadius(i);
    inputRequestedRegionStartIndex[i]
      = outputRequestedRegionStartIndex[i] - m_Kernel.GetRadius(i);

    // crop the requested region to the largest possible region
    //

    // first check the start index
    if (inputRequestedRegionStartIndex[i]
        < inputLargestPossibleRegionStartIndex[i])
      {
      // how much do we need to adjust
      crop = inputLargestPossibleRegionStartIndex[i]
        - inputRequestedRegionStartIndex[i];

      // adjust the start index and the size of the requested region
      inputRequestedRegionStartIndex[i] += crop;
      inputRequestedRegionSize[i] -= static_cast<unsigned long>(crop);
      }
    // now check the final size
    if (inputRequestedRegionStartIndex[i] + inputRequestedRegionSize[i] 
        > inputLargestPossibleRegionStartIndex[i]
        + static_cast<long>(inputLargestPossibleRegionSize[i]))
      {
      // how much do we need to adjust
      crop = inputRequestedRegionStartIndex[i]
        + static_cast<long>(inputRequestedRegionSize[i]) 
        - inputLargestPossibleRegionStartIndex[i]
        - static_cast<long>(inputLargestPossibleRegionSize[i]);

      // adjust the size
      inputRequestedRegionSize[i] -= static_cast<unsigned long>(crop);
      }
    }
  
  typename TInputImage::RegionType inputRequestedRegion;
  inputRequestedRegion.SetSize( inputRequestedRegionSize );
  inputRequestedRegion.SetIndex( inputRequestedRegionStartIndex );
  
  inputPtr->SetRequestedRegion( inputRequestedRegion );
}


template<class TInputImage, class TOutputImage, class TKernel>
void
MorphologyImageFilter<TInputImage, TOutputImage, TKernel>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId) 
{
  // Is this the right boundary condition for all morphology operations?
  ConstantBoundaryCondition<TInputImage> BC;
  BC.SetConstant( NumericTraits<PixelType>::Zero );

  SmartNeighborhoodIteratorType n_iter(m_Kernel->GetRadius(), 
                                       this->GetInput(),
                                       outputRegionForThread);
  n_iter.OverrideBoundaryCondition(&BC);

  ImageRegionIterator<TOutputImage>
    o_iter(this->GetOutput(), outputRegionForThread) ;

  n_iter.GoToBegin();
  o_iter.GoToBegin() ;
  KernelIteratorType kernelFirst = m_Kernel->Begin() ;
  while ( ! n_iter.IsAtEnd() )
    {
      o_iter.Set ( this->Evaluate(n_iter, m_Kernel) );
      ++n_iter ;
      ++o_iter ;
    }
}



template<class TInputImage, class TOutputImage, class TKernel>
void
MorphologyImageFilter<TInputImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Kernel: " << m_Kernel << std::endl;
}

}// end namespace itk
#endif
