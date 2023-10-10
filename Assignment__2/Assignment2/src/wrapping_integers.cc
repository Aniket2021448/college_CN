#include "wrapping_integers.hh"
#include <iostream>
#include <cmath>
using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { return isn + uint32_t(n); }

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {

    //As 32 bit number is 2^32, I used math library power function to get get
	const uint64_t _32BitINTrange = (uint64_t)(std::pow(2, 32));
    
    // const uint64_t _64BitINTrange = (_32BitINTrange);

    //calculating the actual offset between the recieved seqNO and the initial seqNO (isn)
    uint32_t actualOffset = n - isn;
    // const uint64_t actualOffset = static_cast<uint64_t>(Offset);

    if ((checkpoint < actualOffset)) {
        //WRAPPED AROUND SECTION

        //checkpoint is basically the last recieved seqNO and actualOffset stores the gap between
        //the first sent and the latest seqNo. 
        //to check whether our seqNo has been wrapped around, we check this
        //Here, in this case it is not wrapped.
        //so, just return the actual difference between them.

        return actualOffset;      
    }

    else
    {   //NOT WRAPPED AROUND SECTION

        //This is done to take into account the possibility that the seqNo has been
        //wrapped around atleast once.
        // it is possilbe that checkpoint is less than actualOffset that's why we add the _32bit integer/2,
        //to make sure that it is wrapped atleast once
        uint64_t actualCheckpoint = (checkpoint - actualOffset) + (_32BitINTrange / 2);

        //To calculate actually how many times the seqNo has been wrapped around in real
        uint64_t wrapGivenNum = actualCheckpoint / _32BitINTrange;

        //To calculate the original seqNo, removing wrapping
        //number of times it has been wrapped x the 32bitInteger give me the complete rounds
        //and to get the precise seqNo, we add the actualOffset to it.
        uint64_t actualSeqNo = wrapGivenNum * _32BitINTrange + actualOffset;
        return actualSeqNo;
    }
}
