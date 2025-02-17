/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkStreamingProcessObject_h
#define itkStreamingProcessObject_h

#include "itkProcessObject.h"

namespace itk
{

/** \class StreamingProcessObject
 * \brief Base class interface to process data on multiple requested input chunks.
 *
 * Streaming allows the data to be split into chunks and processed
 * separately. The StreamingProcessObject class extends functionally
 * to execute the primary input's pipeline multiple times over
 * different requested regions. After each requested region is
 * generated by the upstream pipeline the StreamedGenerateData method
 * is called.
 *
 * \ingroup ITKSystemObjects
 * \ingroup DataProcessing
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT StreamingProcessObject : public ProcessObject
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(StreamingProcessObject);

  /** Standard class types. */
  using Self = StreamingProcessObject;
  using Superclass = ProcessObject;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Run-time type information (and related methods). */
  itkOverrideGetNameOfClassMacro(StreamingProcessObject);

  /** Override PropagateRequestedRegion from ProcessObject
   *  Since inside UpdateOutputData we iterate over streaming pieces
   *  we don't need to propagate up the pipeline
   */
  void
  PropagateRequestedRegion(DataObject * output) override;

  void
  GenerateData() override;

  /** Override UpdateOutputData() from ProcessObject to divide upstream
   * updates into pieces. */
  void
  UpdateOutputData(DataObject * output) override;

  /** The current requested region number during execution.  The value
   * -1, is used when the pipeline is not currently being updated.  */
  virtual int
  GetCurrentRequestNumber() const;

  void
  ResetPipeline() override;

protected:
  StreamingProcessObject();
  ~StreamingProcessObject() override;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /** \brief Return the actual number of regions to request upstream.
   *
   * This method can be overloaded to return one, when a derived
   * filter is unable stream.
   */
  virtual unsigned int
  GetNumberOfInputRequestedRegions() = 0;


  /** \brief For each streamed region, propagate request to all inputs.
   *
   * Derived classes should overload this method to compute regions
   * splits and propagate to the particular DataObject types used for
   * the input.
   */
  virtual void
  GenerateNthInputRequestedRegion(unsigned int inputRequestedRegionNumber) = 0;

  /** This method will be called multiple times for each requested
   * region generated by the input. */
  virtual void
  StreamedGenerateData(unsigned int inputRequestedRegionNumber) = 0;

  /** Called before the input's first requested region is set or
   * updated.
   */
  virtual void
  BeforeStreamedGenerateData();

  /** Called after all requested regions have been process.
   */
  virtual void
  AfterStreamedGenerateData();

private:
  int m_CurrentRequestNumber{ -1 };
};

} // end namespace itk

#endif // itkStreamingProcessObject_h
