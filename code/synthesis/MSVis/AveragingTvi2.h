#if ! defined (Msvis_AveragingTvi2_H_121211_1236)
#define Msvis_AveragingTvi2_H_121211_1236

#include <casa/aips.h>
#include <synthesis/MSVis/TransformingVi2.h>

namespace casa {

namespace vi {

class WeightFunction;

namespace avg {

class VbSet;

}

class AveragingTvi2 : public TransformingVi2 {

public:

    AveragingTvi2 (ViImplementation2 * inputVii, Double averagingInterval,
                   Int nAveragesPerChunk, WeightFunction * weightFunction);
    ~AveragingTvi2 ();

    /////////////////////////////////////////////////////////////////////////
    //
    // Chunk/Subchunk structure in the AveragingTvi2
    //
    // The averaging interval, in seconds, is specified at construction time.
    // The interval (i.e., the size of the chunk in time) is also specified
    // at creation time; the interval must be an integer multiple of the
    // averaging interval.
    //
    // The input VI's setting must be compatible with those of the
    // AveragingTvi2.  This means that the chunk size of the input VI must
    // have the same duration as the averaging VI.  Although the input VI
    // and the averaging VI have the same duration, the averaging VI will
    // have fewer subchunks since N input subchunks will be averaged down
    // to create a single output subchunk.
    //
    // The input VI will also define the averaging boundaries by its
    // definition of a chunk.  For example, if the input VI allows data with
    // different scans to be in the same chunk, then they will potentially
    // be averaged together.
    //
    // The input VI must use the data description ID as a sort column so that
    // a chunk will only contain data from a single DDID setting.

    void originChunks ();
    void nextChunk ();
    Bool moreChunks () const;

    void origin ();
    void next ();
    Bool more () const;

protected:

    void advanceInputVii ();
    Int determineDdidToUse () const;
    void produceSubchunk ();
    void processInputSubchunk (const VisBuffer2 *);
    Bool reachedAveragingBoundary();
    bool subchunksReady () const;
    void validateInputVi (ViImplementation2 *);

private:

    const Double averagingInterval_p; // averaging interval in seconds
    Int ddidLastUsed_p; // ddId last used to produce a subchunk.
    Bool inputViiAdvanced_p; // true if input VII was advanced but data not used
    const Int nAveragesPerChunk_p; // number of subchunks per chunk on output
    Bool subchunkExists_p;
    avg::VbSet * vbSet_p;
};

} // end namespace vi

} // end namespace casa

#endif // ! defined (Msvis_AveragingTvi2_H_121211_1236)
